#include <iostream>
#include <time.h>
#include <string.h>
#include <unistd.h> //allows use of unix system calls, like execl(). Rather important.
#include <stdlib.h>
#include <fstream>
#include <cstdio> //I believe this is required for freopen

using namespace std;

void error(string);
string parsecommand(string);
void changeDir(const char*);
void quickcompile(string&);
void output(const char*, const char*)
void outputAppend(const char*, const char*)

int main(){
      string prompt="mysh";
      string command;
      do
      {
        cout << prompt << "> ";
        getline(cin,command);
        quickcompile(command);
        //command=parsecommand(command);    commented out because this function currently breaks the exit command.
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
        execl("/bin/ls", "ls", (char *)0); //this works, BUT it immediately exits to bash and I don't know why.
        				   //likely it needs to fork prior to the call. but that's for Lena to figure out
      }
      else if(command.substr(0, 2) == "cd")
	{
		if(command.length() == 2)
		{
			changeDir("");
		}
		else
		{	
		string dir = command.substr(3);
		cout << dir.find_first_of(" ") ; 
		dir = dir.substr(0, dir.find_first_of(" "));
		//cout << dir << endl;
		changeDir(dir.c_str());
		}
	}
      else
	    error(command);
	    
      }while(command != "exit");
      return 0;
}

void error(string command)
{
    if(command == "exit" || command == "")
        return;
    else
        cout << "wrong\n";
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
        exec=command.substr(0,command.length()-ext);
        
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
        exec=command.substr(0,command.length()-ext);
        
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
	if(strcmp(newDir,"") == 0)
	{
		chdir(getenv("HOME"));
	}
	else
	{
		if(chdir(newDir) == -1)
		{
			cout << newDir << ": No such directory.\n";
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
