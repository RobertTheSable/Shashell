// This file was created to test the run external commands function.
// Usage (if built as exec.o)
// exec.o [commang to run] [arg and options...]
// Example: exec.o ls -l

#include <cstdio>
#include <cstring>
#include <iostream>

// include unix systemheaders
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sysexits.h>

using namespace std;

int runExternalCommand(char**);

int main(int argc, char** argv) 
{
	runExternalCommand(argv+1);
	return 0;
}
int runExternalCommand(char** args)
{
	int pipedescriptors[2];
	int readErr, err;
	pid_t pid;

	/* creates two file descriptors
	*  the first is for reading
	*  the second is for writing
	*/
	int err1 = pipe(pipedescriptors);
	//set the close on exec flag for the writing file descriptor
	err1 |= fcntl(pipedescriptors[1], F_SETFD, fcntl(pipedescriptors[1], F_GETFD) | FD_CLOEXEC);
	//cout << pipedescriptors[0] << endl << pipedescriptors[1] << endl;
	if(err1 != 0)
	{
		//Exits if there was an error in pipe of fcntl
		cout << "Error while creating file descriptors.\n";
		return EX_OSERR;
	}
	// fork for new process 
	pid = fork();
	if(pid < 0)
	{
		//fork failed
		cout << "Error while forking.\n";
		return EX_OSERR;
	}
	else if(pid == 0)
	{
		// child process.
		
		// close the reading file descriptor since we don't need it.
		close(pipedescriptors[0]);
		
		//execute command
		execvp(args[0], args );
		
		// write to file descriptor.
		write(pipedescriptors[1], &errno, sizeof(int));
		
		//exit normally
		_exit(0);
		// falls through to parent process block.
	}
	else
	{
		// parent process. 
		// close the writing file descriptor since we don't need it anymore
		close(pipedescriptors[1]);
		
		// close(pipedescriptors[0]);
		// read from the file descriptor. should read 0 bytes.
		readErr = read(pipedescriptors[0], &err, sizeof(errno));
		while (readErr == -1) // if there was an error while reading from the file descriptor.
		{
			if (errno != EAGAIN && errno != EINTR) 
			{
				// if the error is not temporary, i.e file does not exist.
				break;
			}
			// if the function call was interrupted or a resource is unavaliable.
			readErr = read(pipedescriptors[0], &err, sizeof(errno));
		}
		if ( readErr != 0 ) 
		{
			// Some error running the command. Usually file does not exist.
			cout << "Error: " << strerror(err) << endl;
			return EX_UNAVAILABLE;
		}
		// close reading descriptor. 
		close(pipedescriptors[0]);
		while (waitpid(pid, &err, 0) == -1) 
		{
			if (errno != EINTR) 
			{
				perror("waitpid error");
				return EX_SOFTWARE;
			}
		}
	}
	return err;
}