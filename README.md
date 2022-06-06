# Pipeline_Commands

This code involves the use of pipes for inter-process communication.

Part 1: Demonstrates a simple system for processing several input files in parallel, by using one process to read and process each of the files. This kind of approach, in contrast to a sequential approach in which each file is read and processed one after another, is becoming increasingly crucial to achieving acceptable performance on modern hardware systems. 

Part 2: Is an extension of the swish_shell to execute program pipelines. This allows one to execute a sequence of programs in which each program consumes input from its predecessor and sends output to its successor. This was one of the key contributions of the original Unix operating system: the ability to compose small, simple tools to achieve a larger goal. This contribution was made possible thanks to the use of pipes in combination with the traditional fork()/dup2()/exec() combination utilized in Swish_Shell.

This code will focus on a few important systems programming topics:

    Text file input and parsing
    Creating pipes with the pipe() system call
    Using pipe file descriptors in combination with read() and write() to coordinate among several processes
    Executing program pipelines using pipe() and dup2() along with the usual fork() and exec()
