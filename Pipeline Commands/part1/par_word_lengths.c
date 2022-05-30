#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_WORD_LEN 25

/*
 * Counts the number of occurrences of words of different lengths in a text file and
 * stores the results in an array.
 * file_name: The name of the text file from which to read words
 * counts: An array of integers storing the number of words of each possible length.
 *         counts[0] is the number of 1-character words, counts [1] is the number
 *         of 2-character words, and so on.
 * max_len: The maximum length of any word, and also the length of 'counts'.
 * Returns 0 on success or 1 on error.
 */

int count_word_lengths(const char *file_name, int *counts, int max_len) 
{
	FILE *file = fopen(file_name, "r");
	if(file == NULL)
	{
		perror("file did not open");
		return 1;
	}
	char buf[MAX_WORD_LEN+1];
	while(fscanf(file, "%s",buf) > 0)
	{
		int i = strlen(buf)-1;
		counts[i]++;
	}
	if(ferror(file))
	{
		perror("perror in fscanf");
		return 1;
	}
	if(fclose(file) == EOF)
	{
		perror("fclose error");
		return 1;
	}
	
    return 0;
}

/*
 * Processes a particular file (counting the number of words of each length)
 * and writes the results to a file descriptor.
 * This function should be called in child processes.
 * file_name: The name of the file to process.
 * out_fd: The file descriptor to which results are written
 * Returns 0 on success or 1 on error
 */
 
int process_file(const char *file_name, int out_fd) 
{
	int counts[MAX_WORD_LEN];
	for(int i = 0; i <= MAX_WORD_LEN; i++)
	{
		counts[i] = 0;
	}
	count_word_lengths(file_name, counts, MAX_WORD_LEN);
	//if(count_word_lengths(file_name, counts, MAX_WORD_LEN) == 1)
	//{
		//printf("count_word_lengths error");
		//return 1;
	//}
	if(write(out_fd, counts, (sizeof(int)*MAX_WORD_LEN)) == -1)
	{
		perror("write error in process_file");
		return 1;
	}
    return 0;
}

int main(int argc, char **argv) 
{
//  Create a pipe for each child process
//  Fork a child to process all specified files (names are argv[1], argv[2], ...)
// Aggregate all the results together by reading from pipes in the parent
    if(argc == 1) 
    {
        // No files to consume, return immediately
        return 0;
    }
    int *my_pipe = malloc(2*(argc-1)* sizeof(int)*MAX_WORD_LEN);
    if(my_pipe == NULL)
    {
		printf("my_pipe is NULL");
		return 1;
	}
	for(int i = 0; i <= argc-1;i++)//ITERATE THROUGH ALL FILES
	{
		if(pipe(my_pipe + 2*i) == -1)
		{
			perror("pipe error in main");
			return 1;
		}
		pid_t child_pid = fork();
		if (child_pid == -1)
		{
			perror("fork");
			if(close(my_pipe[i*2]) == -1)
			{
				perror("close error in child process");
				return 1;
			}
			if(close(my_pipe[2*i + 1]) == -1)
			{
				perror("close error in child process");
				return 1;
			}
			return 1;
		} 
		else if (child_pid == 0) //CHILD PROCESS
		{
			for(int j = 0; j <= i; j++)
			{
				if(close(my_pipe[2*j]) == -1) // Close read end of pipe
				{
					perror("close error in child process");
					return 1;
				}
			}
			if(process_file(argv[i+1],my_pipe[2*i + 1]) == 1)
			{
				printf("error with process_file in main");
				return 1;
			}
			if(close(my_pipe[2*i + 1]) == -1) // Close write end of pipe
			{
				perror("close error in child process");
				return 1;
			}
			return 0;
		} 
	}
	//PARENT PROCESS OUTSIDE OF LOOP
	int aggBuf[MAX_WORD_LEN];
	for(int i = 0; i < MAX_WORD_LEN; i++)
	{
		aggBuf[i] = 0;
	}
	int parentBuf[MAX_WORD_LEN];
	for(int i = 0; i< argc-1; i ++)
	{
		if(close(my_pipe[2*i+1]) == -1)
		{
			perror("close error in parent process");
			return 1;
		}
		if(read(my_pipe[i*2], parentBuf, (sizeof(int)*MAX_WORD_LEN)) == -1)
		{
			perror("read error in parent process");
			return 1;
		}
		if(close(my_pipe[2*i]) == -1)
		{
			perror("close error in parent process");
			return 1;
		}
		for(int i = 0; i < MAX_WORD_LEN; i++)
		{
			aggBuf[i] += parentBuf[i];
		}
	}
    for (int i = 1; i <= MAX_WORD_LEN; i++) 
    {
        printf("%d-Character Words: %d\n", i , aggBuf[i-1]);
    }
    free(my_pipe);
    return 0;
}
