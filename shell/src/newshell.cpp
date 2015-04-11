#include <iostream>
#include <time.h>
#include <string.h>
#include <unistd.h> //allows use of unix system calls, like execl(). Rather important.
#include <stdlib.h>

using namespace std;

void error(string);
string parsecommand(string);

int main(){
      string prompt="mysh";
      string command;
      do
      {
        cout << prompt << "> ";
        getline(cin,command);
        quickcompile(command);
        //command=parsecommand(command);    commented out because this function currently breaks the exit command.
        
      if (command == "shaprompt")
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
        compile="echo -n ' '; g++";
        compile=compile + " " + command;
        system(compile.c_str());
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
        compile="echo -n ' '; gcc";
        compile=compile + " " + command;
        system(compile.c_str());
        command.clear();
    }
    else
        return;
    
}
