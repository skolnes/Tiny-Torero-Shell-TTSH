/*
 * The Tiny Torero Shell (TTSH)
 *
 * Runs a shell that operates like terminal and runs command lines
 * accordingly.
 *
 * This file is part of COMP 280, Project 7
 * Authors:
 * 1. Scott Kolnes (skolnes@sandiego.edu)
 * 2. Michael Korody (mkorody@sandiego.edu)
 *
 *
 */

// NOTE: all new includes should go after the following #define
#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "parse_args.h"
#include "history_queue.h"


// Forward Declarations of functions to be run later 
void runShellCMND(char **argv, int run_foreground);

void runEXECVP(char **argv, int run_foreground);

void child_handler(__attribute__ ((unused)) int sig);

void runNumCommand(char **argv, unsigned int *counter);

//Remeber the print history function is in the history_queue

int main() { 
	//Utilizing code given from hints
	//Handles SIGCHLD call by pointing all child signals to child_handler
	struct sigaction sa;
	sa.sa_handler = child_handler;
	sa.sa_flags = SA_NOCLDSTOP;     // don't call handler on child pause/resume
	sigaction(SIGCHLD, &sa, NULL);
	
	//Keeps a running command line counter
	unsigned int cmndline_counter = 1;

	//REPL loop 
	while(1) {
		// (1) print the shell prompt
		fprintf(stdout, "ttsh> ");  
		fflush(stdout);

		// (2) read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL)
				&& ferror(stdin)) {
			// fgets could be interrupted by a signal.
			// This checks to see if that happened, in which case we simply
			// clear out the error and restart our loop so it re-prompts the
			// user for a command.
			clearerr(stdin);
			continue;
		}

		/*
		 * Check to see if we reached the end "file" for stdin, exiting the
		 * program if that is the case. Note that you can simulate EOF by
		 * typing CTRL-D when it prompts you for input.
		 */
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}

		// (3) make a call to parseArguments function to parse it into its argv
		// format

		//use argv to determine what to execute
		char *argv[MAXARGS];
		//parse the command line arguments and indicate if it will run in the
		//foreground or background
		int run_background = parseArguments(cmdline, argv);
		//If input is simply ENTER then skip over it
		if (strlen(cmdline) <= 1) {
			continue;
		}
		//handles rerun of a given command line number
		if (argv[0][0] == '!') {
			runNumCommand(argv, &cmndline_counter);
			cmndline_counter ++;
			continue;
		}

		//Creates a history entry to be added to the history queue
		HistoryEntry entry;
	   	entry.cmd_num = cmndline_counter;
		//entry.cmdline = cmdline;
		strncpy(entry.cmdline, cmdline, MAXLINE);
		//Adds given entry into the queue and increases the cmndline_counter
		add_queue(entry);
		cmndline_counter ++;

		// (4) Call a function that will determine how to execute the command
		// that the user entered, and then execute it
		
		//Run function to handle shell command 
		runShellCMND(argv, run_background);
		//add_queue(entry);
		
		//End of code since it is in the while loop and will exit when exit is
		//called
	}
	return 0;
}

/**
 * This function handles the forking of processes and which will run in the
 * background or foreground. Runs the given command using execvp. 
 * 
 * @param argv				The given parsed command line
 * @param run_background	The flag if process is to be run in forground or
 * 							background. 0 if foreground else background
 */
void runEXECVP(char **argv, int run_background) {
	//Creates a pid variable
	pid_t pid;
	//fork
	//Checks if given arg is a valid command
	if ((pid = fork()) == 0){	//child process
		int check = execvp(argv[0], argv);
		if (check){
			printf("INVALID COMMAND\n");
			exit(0);
		}
	}
	//If run background is false, run in foreground then
	if (run_background == 0){
		waitpid(pid, NULL, 0);
	}
	//run in background
	else{
		waitpid(pid, NULL, WNOHANG);
	}
}

/**
 * This function first handles the parsed in command line to see if it is a
 * built in function or not and then sends non built in functions to execvp
 *
 * @param argv				The parsed argument line
 * @param run_background	The flag if process is to be run in forground or
 * 			      			background. 0 if foreground else
 * 							background
 */
void runShellCMND(char **argv, int run_background) {
	//First checks if user inputed exit, therefore to exit the shell
	if (strcmp(argv[0], "exit") == 0) {
		printf("Goodbye!\n");
		exit(0);
	}
	//Runs the history command which prints out the history queue
	else if (strcmp(argv[0], "history") == 0){
	print_queue();
	}
	//Checks if user runs CD and handles appropriately
	else if (strcmp(argv[0], "cd") == 0) {
		if (argv[1] == NULL){
			chdir(getenv("HOME"));
		}
		else {
			if (chdir(argv[1]) == -1){
				printf("No such directory exisits!\n");
			}
		}
	}
	else { 
	//Run execvp function
	//At this point we know that it is not a built in function
	//so we will use execvp in order to run the other commands 
	//given by users.
	runEXECVP(argv, run_background);
	}	
	//End of code since it is in the while loop and will exit when exit is
	//called
}

/**
 * Handles the child if sent a SIGCHLD
 *
 * @param sig 		The recieved signal that calls the child handler
 */
void child_handler(__attribute__ ((unused)) int sig) {
	//creates a pid and child status varialble
	pid_t pid;
	int child_status;
	//if the pid is not a child pid 
	while ((pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
		//If it gets here the child has been reaped
		//and the next pid will break out of the while loop
		printf("reaped child status: %d \n", pid);
	}
}

/**
 * For the advanced component, if given a command line to run again then
 * reruns said line from the historical queue
 *
 * @param argv		The non-parsed command line to be run
 * @param counter	The command line counter to find the command line
 * 					associated
 */
void runNumCommand(char **argv, unsigned int *counter) {
	//This function is only used if the command line contains a '!num'
	
	//First we would need to create another char cmdline to hold the command
	//line previously run in history
	char cmdline[strlen(argv[0])];
	//Get rid of the ! by shifting all chars to the left
	int i;
	// since i is not unsigned, cast other argument to an int
	for (i = 0; i < (int)strlen(argv[0]); i++) {
		cmdline[i] = argv[0][i + 1];
	}
	printf("cmdline is : %s\n", cmdline);
	//Turn it into unsigned number
	unsigned int num = strtol(cmdline, NULL, 0);
	//checks if its not the terminating string indicating the command line
	//number is indeed inside the queue
	if (strcmp(giveNumberCommand(num), " ") != 0) {
		// now create new command line parse array in order to parse and then
		// run just line in main
		char *cmnd[MAXARGS];
		int run_background = parseArguments(giveNumberCommand(num), cmnd);    
		HistoryEntry entry;
		entry.cmd_num = *counter;
		strncpy(entry.cmdline, *cmnd, MAXLINE);    
		add_queue(entry);
		runShellCMND(cmnd, run_background);
	}
	else{
		//Since the main will always increase the cmdline_counter, decrease it
		//to handle this case 
		counter --;
	}
}
