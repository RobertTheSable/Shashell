#include <iostream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <cstdio> //I believe this is required for freopen

// include unix systemheaders
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sysexits.h>

using namespace std;

void error(string);
string parsecommand(string);
void changeDir(const char*);
void quickcompile(string&);
void output(const char*, const char*);
void outputAppend(const char*, const char*);
int runExternalCommand(char**);
int generateArgList(string, char***, int);

int main(){
      string prompt="mysh";
      string command;
      do
      {
	command = "";
        cout << prompt << "> ";
        getline(cin,command);
        //quickcompile(command);
        //command=parsecommand(command);    commented out because this function currently breaks the exit command.
        //*
	if(command.find(">") != string::npos || command.find(">>") != string::npos)
	{
		int pos = command.find(">");
		string part1;//command
		string part2;//filename
		const char* cmd = part1.c_str();
		const char* filename;
		if(command.find(">>") != string::npos)//outputAppend
		{
			part2 = command.substr(pos+2, string::npos);
			filename = part2.c_str();
			outputAppend(cmd, filename);
		}//if
		else//output
		{
			part2 = command.substr(pos+1, string::npos);
			filename = part2.c_str();
			output(cmd, filename);
		}//else
	}//if
      else if (command == "shaprompt")
      {
        cout << "New prompt? ";
        getline(cin, prompt);
        cout << "Prompt set as " << prompt << endl;
      }
      else if (command == "date")
      {
        time_t tim;
        time(&tim);
        cout << ctime(&tim);
      }
      else if (command == "groot")
      {
        cout << "I am Groot\n";
      }
      else if (command == "shamod")
      {
        cout << "I think you meant to type \"chmod\"\n"; //we should probably actually get this functional...
      }
      else if (command == "ls")
      {
		char* cmd[3];
		cmd[0] = const_cast<char*>("ls");
		cmd[1] = const_cast<char*>("--color=auto");
		cmd[2] = NULL;
		runExternalCommand(cmd);
	      //execl("/bin/ls", "ls", (char *)0); //this works, BUT it immediately exits to bash and I don't know why.
        			   //likely it needs to fork prior to the call. but that's for Lena to figure out
      }//*/
      else if(command.length() >= 2 && command.substr(0, 2) == "cd")
	{
		if(command.length() == 2)
		{
			changeDir("");
		}
		else
		{	
			string dir = command.substr(3);
			dir = dir.substr(0, dir.find_first_of(" "));
			changeDir(dir.c_str());
		}
	}
      else
	    error(command);
	    
      
      }while(command != "exit");
      return 0;
}
string parsecommand(string command)
{
    int i=command.find_first_not_of(' ');
    int j=command.find_last_not_of(' ');
    if(i == 0 && j== 0)
        return command;
    else if(j == 0)
    {
        command=command.substr(i, command.length()-i);
        return command;
    }
    else if(i==0 && j != 0)
    {
        command=command.substr(0, command.length()-j);
        return command;
    }
    else
    {
        command=command.substr(i, command.length()-i);
        //command==command.substr(0, command.length()-(j-1));
        //command.erase(j+1);
       //cout << j << endl;
        return command;
    }
}

void quickcompile(string& command) //This is functional, now it just needs to be made pretty
{
    ifstream input;
    string compile;
    /*put in parser(maybe)*/
    if(command.find(".cpp") != string::npos)
    {
        input.open(command.c_str());
        if(!input)
        {
            cout << "error: " << command << " not found\n";
            input.close();
            command.clear();
            return;
        }
        int ext=command.find(".cpp");
        string exec = command.substr(0,command.length()-ext);
        
        compile="echo -n; g++";
        compile=compile + " " + command + " -o " + exec;
        system(compile.c_str());
        exec="./"+exec;
        system(exec.c_str());
        command.clear();
    }
    else if(command.find(".c") != string::npos)
    {
        input.open(command.c_str());
        if(!input)
        {
            cout << "error: " << command << " not found\n";
            input.close();
            command.clear();
            return;
        }
        
        int ext=command.find(".c");
        string exec=command.substr(0,command.length()-ext);
        
        compile="echo -n; gcc";
        compile=compile + " " + command + " -o " + exec;
        system(compile.c_str());
        exec="./"+exec;
        system(exec.c_str());
        command.clear();
    }
    else
        return;
    
}

void changeDir(const char* newDir)
{
	char dir[256] = "";
	if(newDir[0] == '~')
	{
		char* home = const_cast<char*>(getenv("HOME"));
		//strcat(home, newDir+1);
		strcat(dir, home);
		strcat(dir, newDir+1);
		//cout << dir << endl;
	}
	else
	{
		strcat(dir, newDir);
	}
	if(strcmp(dir,"") == 0)
	{
		cout << "Changing to " << getenv("HOME") << endl;
		chdir(getenv("HOME"));
	}
	else
	{
		if(chdir(dir) == -1)
		{
			cout << dir << ": No such directory.\n";
		}
	}
}

void output(const char* cmd, const char* filename)
{
	//redirect output
	freopen(filename, "w", stdout);
	//run commands
	
	//restore output to terminal
	freopen("/dev/tty", "a", stdout); //this only works on Linux
}//output

void outputAppend(const char* cmd, const char* filename)
{
	//redirect output
	freopen(filename, "a", stdout);
	//run commands
	
	//restore output to terminal
	freopen("/dev/tty", "a", stdout); //this only works on Linux
}//outputappend
/*
 * External commands function.
 * Takes an array of c strings as an argument.
 * Array must start with the name of the command to run, followed by the arguments for the function.
 */
int runExternalCommand(char** args)
{
	int pipedescriptors[2];
	int readErr, err;
	pid_t pid;

	/* creates two file descriptors for communication between parent and child
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
		
		// close the reading file descriptor in the child since we don't need it.
		close(pipedescriptors[0]);
		
		//execute command
		execvp(args[0], args );
		
		// write information so parent can read it.
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
		
		// read data from the child.
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
				perror("waitpid");
				return EX_SOFTWARE;
			}
		}
	}
	return err;
}
int generateArgList(string command, char*** argList, int k)
{
	int x,y;
	
	x = 0;
	y = 0;
	
	while(y < k)
	{
		strcpy((*argList)[y], (command.substr( x, command.find_first_of(' ', x) - x )).c_str());
		 
		
		x = command.find_first_of(' ', x) + 1;
		
		y++;
		
	}
	return k;
	
}
void error(string command)
{
    char** arguments;
    if(command == "exit" || command == "")
    {
        return;
    }
    
    else
    {
	
	//cout << command << endl;
	int k = 0;
	for(int idx = 0; idx < command.size(); idx++)
	{
		if(command[idx] == ' ')
		{
			k++;
		}
	}
	k++;
	arguments = (char**)malloc((256) * sizeof(char*));
	for(int idx = 0; idx < 256; idx++)
	{
		arguments[idx] = (char*)malloc(256);
	}
	arguments[k] = NULL;
	int num = generateArgList(command, &arguments, k);
	
	runExternalCommand(arguments);
        //cout << "wrong\n";
	//*
    }
}