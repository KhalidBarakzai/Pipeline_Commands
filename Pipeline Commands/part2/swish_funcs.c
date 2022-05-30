#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "string_vector.h"
#include "swish_funcs.h"

#define MAX_ARGS 10

/*
 * Helper function to close all file descriptors in an array. You are
 * encouraged to use this to simplify your error handling and cleanup code.
 * fds: An array of file descriptor values (e.g., from an array of pipes)
 * n: Number of file descriptors to close
 */
int close_all(int *fds, int n) {
    int ret_val = 0;
    for (int i = 0; i < n; i++) {
        if (close(fds[i]) == -1) {
            perror("close");
            ret_val = 1;
        }
    }
    return ret_val;
}



/*
 * Helper function to run a single command within a pipeline. You should make
 * make use of the provided 'run_command' function here.
 * tokens: String vector containing the tokens representing the command to be
 * executed, possible redirection, and the command's arguments.
 * 
 * pipes: An array of pipe file descriptors.
 * n_pipes: Length of the 'pipes' array
 * 
 * in_idx: Index of the file descriptor in the array from which the programGET /<file> HTTP/1.0\r\n\r\n
 *         should read its input, or -1 if input should not be read from a pipe.
 * out_idx: Index of the file descriptor int he array to which the program
 *          should write its output, or -1 if output should not be written to
 *          a pipe.
 * Returns 0 on success or 1 on error.
 */
int run_piped_command(strvec_t *tokens, int *pipes, int n_pipes, int in_idx, int out_idx)
{
	for(int i = 0; i < n_pipes; i++)//CLOSE all unused ends in pipe
	{
		if( i != in_idx && i != out_idx)
		{
			if(close(pipes[i]) == -1) // TODO Close read end of pipe
			{
				perror("close error in child_pid in_idx\n");
				return 1;
			}
		}

	}
	if(in_idx != -1)
	{
		if(dup2(pipes[in_idx] , STDIN_FILENO) == -1)
		{
			perror("dup2 error in idx\n");
			return 1;
		}
	}
	if(out_idx != -1)
	{
		if(dup2(pipes[out_idx],STDOUT_FILENO) == -1)
			{
				perror("dup2 error in out idx\n");
				return 1;
			}
	}
	if(run_command(tokens) == 1)
	{
		printf("run_command error");
		return 1;
	}
   return 0;
}



int run_pipelined_commands(strvec_t *tokens) 
{
	int n_pipes = strvec_num_occurrences(tokens, "|");
	//{
	//	printf("n_pipes error");
	//	return 1;
	//}
    int *my_pipe = malloc(2*(n_pipes)* sizeof(int));
    if(my_pipe == NULL)
    {
		printf("my_pipe is NULL");
		return 1;
	}
	for(int i = 0; i < n_pipes;i++)//create all the pipes.
	{
		if(pipe(my_pipe + (i*2)) == -1)//PIPE
		{
			perror("pipe error in main\n");
			return 1;
		}
	}
	for(int i = n_pipes; i >= 0;i--)//LOOP: ITERATE THROUGH ALL ARGS BACKWARDS
	{
		strvec_t dest;
		int pipe_index = (strvec_find_last(tokens, "|"));
		//if(pipe_index == -1)
		//{
		//	printf("pipe_index error");
		//	return 1;
		//}
		int input = ((i-1)*2);
		int output = ((2*i)+1);
		if((strvec_slice(tokens, &dest, pipe_index+1 , tokens->length)) == 1)
		{
			printf("strvec slice error");
			return 1;
		}
		strvec_take(tokens, pipe_index);
		pid_t child_pid = fork();//FORK
		if (child_pid == -1)
		{
			perror("fork");
			if(close(my_pipe[(i-1)*2]) == -1)
			{
				perror("close error in child process\n");
				return 1;
			}
			if(close(my_pipe[2*(i + 1)]) == -1)
			{
				perror("close error in child process\n");
				return 1;
			}
			return 1;
		} 
		else if (child_pid == 0) //CHILD PROCESS
		{
			if(i == 0)//check if read end == -1
			{
				if(run_piped_command(&dest, my_pipe , (n_pipes*2), -1, output) == 1)
				{
					printf("run_piped_command error in run_pipeline_command, read = -1\n");
					return 1;
				}
			}
			if(i == n_pipes)//check if write end == -1
			{
				if((run_piped_command(&dest, my_pipe , (n_pipes*2), input, -1)) == 1)
				{
					printf("run_piped_command error in run_pipeline_command, write = -1\n");
					return 1;
				}
			}
			if(run_piped_command(&dest, my_pipe , (n_pipes*2), input , output) == 1)//run piped command normally
			{
				printf("run_piped_command error in run_pipeline_command\n");
				return 1;
			}
			return 0;
		} 
	}
	//PARENT PROCESS
	int status;
	close_all(my_pipe, n_pipes*2);
	for(int i = 0; i < n_pipes+1 ; i ++)//parent waits for all children processes
	{
		
		if (wait(&status) == -1)
		{
			perror("wait error in parent process\n");
			return 1;
		}
	}
	free(my_pipe);
    return 0;
}
