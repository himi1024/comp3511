/*
    COMP3511 Spring 2022 
    PA1: Simplified Linux Shell (Multi-level Pipe)

    Your name: CHAN FOO HIM
    Your ITSC email: fhchanaa          @connect.ust.hk 

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks. 

*/

// Note: Necessary header files are included
// Do not add extra header files
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 8 pipe segments
#define MAX_PIPE_SEGMENTS 8

// Assume that we have at most 8 arguments for each segment
// We also need to add an extra NULL item to be used in execvp
// Thus: 8 + 1 = 9
//
// Example: 
//   echo a1 a2 a3 a4 a5 a6 a7 
//
// The execvp system call needs to store an extra NULL to represent the end of the parameter list
//
//   char *arguments[MAX_ARGUMENTS_PER_SEGMENT]; 
//
// For the above example, 9 strings are stored: echo a1 a2 a3 a4 a5 a6 a7 NULL
//
#define MAX_ARGUMENTS_PER_SEGMENT 9

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// The pipe character
#define PIPE_CHAR "|"

// Define the  Standard file descriptors here
#define STDIN_FILENO    0       // Standard input
#define STDOUT_FILENO   1       // Standard output 
#define STDERR_FILENO   2       // Standard error output 

// Helper: parse_tokens function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// cmdline // the input argument of the process_cmd function
//
// Sample usage:
//
//  parse_tokens(pipe_segments, cmdline, &num_pipe_segments, PIPE_CHAR);
//
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc ;   
}

// The function proitotype of process_cmd, to be implemented below
void process_cmd(char *cmdline);

/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);
    process_cmd(cmdline);
    return 0;
}

// TODO: implementation of process_cmd
void process_cmd(char *cmdline)
{
    // You can try to write: printf("%s\n", cmdline); to check the content of cmdline
    //printf("%s\n", cmdline);
    char* pipe_segments[MAX_PIPE_SEGMENTS];
    int num_pipe_segments = 0;   
    
    parse_tokens(pipe_segments, cmdline, &num_pipe_segments, PIPE_CHAR);
    
    char* argv[MAX_ARGUMENTS_PER_SEGMENT];
    
    
    int num_argv = 0;

    //parse_tokens(argv, pipe_segments, &num_argv, SPACE_CHARS); 
    // execvp(pipe_segments[0], pipe_segments);
    
    int data, pfds[2];   
    data = STDIN_FILENO;
 
    for(int i = 0; i < num_pipe_segments; i++){
	//printf("%s\n", pipe_segments[i]);
	parse_tokens(argv, pipe_segments[i], &num_argv, SPACE_CHARS);
	argv[num_argv] = '\0';
	//printf("%s\n", pipe_segments[i]);
	
	if(num_pipe_segments == 1)
		execvp(argv[0], argv);
	
	else if(i+1 != num_pipe_segments){
  		pipe(pfds);
		pid_t pid = fork();

		//Child Process
		if( pid==0 ){
			if(data != STDIN_FILENO ){
				dup2(data, STDIN_FILENO);
				//close(data);
			}
			dup2(pfds[1], STDOUT_FILENO);
			close(pfds[1]); //dup to STDOUT
			close(pfds[0]); //no need
			execvp(argv[0],argv);
		        break;
		}

		//Parent Process
		else{  
			close(0); //no need	
			close(pfds[1]);
			//prev = pfds[0];
			dup2(pfds[0], data);
		
		}	
         }
	else{ //Last process
	       if(data != STDIN_FILENO){
              		dup2(data, STDIN_FILENO);
	   		close(data);
	               		 
	 	}
     	 	execvp(argv[0],argv);
		
	}
	
	//printf("%s", argv);    
    }
}
