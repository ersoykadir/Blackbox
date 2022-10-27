#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <string.h>
#include <stdlib.h>

#define READ_END	0 // fd[0]
#define WRITE_END	1 // fd[1]

#include "part_b.h"


/**
 * This is a RPC function that the clients can call to execute.
 * Callee should provide an executable path.
 * Then it should input two numbers and will be provided the result of the RPC function back.
 * 
 * RPC function creates a child process that executes the given blackbox.
 * 	It reads two numbers from client and gives it to blackbox as input and reads the output of the blackbox, giving it back to client. 
 **/
outputData *
part_b_1_svc(inputData *argp, struct svc_req *rqstp)
{
	/**
	struct outputData{
		int result;
		int error;
		string error_message<>;
	};
	**/
	static outputData  result;

	//printf("Rpc function is called with path %s and output_file %s. \n", argp->path, argp->outfile_name);

	/* initializing the error binary to false(0)*/
	result.error = 0;
	result.error_message = "";

	/* ID of child process */
	pid_t pid;
	/* Pipes between parent and child, one for each (stdin,stdout,stderr) of child */
	int first_pipe[2];	/*	input pipe 	*/
	int second_pipe[2]; /* 	result pipe */
	int third_pipe[2]; 	/*	error pipe 	*/


	/*Message from parent to child*/
	char p_msg[10000] = "";
	/*Buffer that parent uses to read messages from child*/
	char buffer[10000] ="";

	char *prog_name ;
	char *outfile_name;
	FILE *output_file;
	outfile_name = argp->outfile_name;
	prog_name = argp->path;

	/* Parent sends input numbers to child from first_pipe. */
	if(pipe(first_pipe) == -1){
		fprintf(stderr, "First pipe failed.\n");
		exit(1);
	}
	/* Child sends result to parent from second_pipe. */
	if(pipe(second_pipe) == -1){
		fprintf(stderr, "Second pipe failed.\n");
		exit(1);
	}
	/* Child sends error messages to parent from third_pipe. */
	if(pipe(third_pipe) == -1){
		fprintf(stderr, "Third pipe failed.\n");
		exit(1);
	}

	pid = fork();
	if(pid < 0){
		fprintf(stderr, "Fork failed.\n");
		exit(2);
	}

	if(pid > 0)	 // parent process
	{
		/* close unnecessary pipes*/
		close(first_pipe[READ_END]);
		close(second_pipe[WRITE_END]);
		close(third_pipe[WRITE_END]);

		/* pack input numbers sent from client into a char array and send to child*/
		sprintf(p_msg,"%d %d", argp->a, argp->b);
		//printf("Parent read: %s\n", p_msg);
		write(first_pipe[WRITE_END], p_msg, strlen(p_msg)+1 );
		close(first_pipe[WRITE_END]);

		int errorbytes = read(third_pipe[READ_END], buffer, sizeof(buffer));
		close(third_pipe[READ_END]);

		if(errorbytes != 0)/* child had an error, write the error to output text*/
		{
			//printf("Error: %s\n",buffer);
			/* Set error binary to true to inform client about error*/
			result.error = 1;
			result.error_message = buffer;
			return &result;
			exit(3);
		}
		else	/* child executed properly, set the output data according to result*/
		{
			result.error = 0;
			result.error_message = "";

			/* reading the output pipe*/
			read(second_pipe[READ_END], buffer, sizeof(buffer));
			close(second_pipe[READ_END]);
			
			result.result = atoi(buffer);/* setting outputData.result*/

			//printf("Result of the blackbox is:  >%d<\n", result.result);
			return &result;
		}
	}
	else	 // child process
	{
		/* close unnecessary pipes*/
		close(first_pipe[WRITE_END]);
		close(second_pipe[READ_END]);
		close(third_pipe[READ_END]);
		/* redirecting  */
		int dp = dup2(first_pipe[READ_END], 0); //blackbox stdin is first pipe
		int dp2 = dup2(second_pipe[WRITE_END], 1);//blackbox stdout is second pipe
		int dp3 = dup2(third_pipe[WRITE_END], 2);//blackbox stderr is third pipe
		
		execl(prog_name, "", NULL);
		//printf("%s\n",prog_name);
		perror("Child failed to exec blackbox !\n");
		exit(4);
	}
	
}
