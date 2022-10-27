/**
 * Parent creates a child process that executes the given blackbox.
 * It reads two numbers from user and gives it to blackbox as input and reads the output of the blackbox, writing it to a file. 
 * Parent does these by redirecting the stdin, stdout and stderr of child.
 **/

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <string.h>
#include <stdlib.h>

#define READ_END	0 // fd[0]
#define WRITE_END	1 // fd[1]

int main(int argc, char *argv[])
{
	/* ID of child process */
	pid_t pid;
	/* Pipes between parent and child, one for each (stdin,stdout,stderr) of child */
	int first_pipe[2];	/*	input pipe 	*/
	int second_pipe[2]; /* 	result pipe */
	int third_pipe[2]; 	/*	error pipe 	*/

	int  usr_input1, usr_input2; /*input numbers*/
	int output;

	/*Message from parent to child*/
	char p_msg[10000] = "";
	/*Buffer that parent uses to read messages from child*/
	char buffer[10000] ="";

	char *prog_name;
	char *outfile_name;
	FILE *output_file;
	outfile_name = argv[2];
	prog_name = argv[1];
	output_file = fopen(outfile_name, "a");

	/* Parent sends input numbers to child from first_pipe. */
	if(pipe(first_pipe) == -1){
		fprintf(stderr, "First pipe failed.\n");
		return 1;
	}
	/* Child sends result to parent from second_pipe. */
	if(pipe(second_pipe) == -1){
		fprintf(stderr, "Second pipe failed.\n");
		return 1;
	}
	/* Child sends error messages to parent from third_pipe. */
	if(pipe(third_pipe) == -1){
		fprintf(stderr, "Third pipe failed.\n");
		return 1;
	}

	pid = fork();
	if(pid < 0){
		fprintf(stderr, "Fork failed.\n");
		return 1;
	}

	if(pid > 0) // parent process
	{
		scanf("%d %d", &usr_input1, &usr_input2);	/* reading two integers from user*/

		/* closing unnecessary pipes*/
		close(first_pipe[READ_END]);
		close(second_pipe[WRITE_END]);
		close(third_pipe[WRITE_END]);

		/* pack input numbers into a char array and send to child*/
		sprintf(p_msg,"%d %d", usr_input1, usr_input2);
		//printf("Parent read: %s\n", p_msg);
		write(first_pipe[WRITE_END], p_msg, strlen(p_msg)+1 );
		close(first_pipe[WRITE_END]);

		/* read error pipe to check if child executed properly*/
		int errorbytes = read(third_pipe[READ_END], buffer, sizeof(buffer));
		close(third_pipe[READ_END]);

		if(errorbytes != 0)/* child had an error, write the error to output text*/
		{
			fprintf(output_file, "FAIL:\n%s", buffer);
			//printf("Error: %s",buffer);
			return 1;
		}
		else	/* child executed properly, write the result to output text*/
		{
			/* reading the output pipe*/
			read(second_pipe[READ_END], buffer, sizeof(buffer));
			close(second_pipe[READ_END]);
			output = (atoi(buffer));

			/* writing output to the output text */
			fprintf(output_file, "SUCCESS:\n%d\n", output);
			fclose(output_file);
			//printf("Result of the blackbox is:  >%d<\n", output);
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

		execl(prog_name, "" ,NULL);
		perror("Child failed to exec blackbox !\n");
		return 1;
	}

	return 0;
}