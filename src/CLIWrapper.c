#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int fd[2]; //used to store both ends of pipe (file descriptor)
    pid_t pid;
    char buffer[512];
    size_t total_len = 0;

    for (int i = 1; i < argc; i++) {
        total_len += strlen(argv[i]);
    }
    char *result = malloc(total_len);
    if (!result) return 1;
    result[0] = '\0'; //placed infront so strcat can start writing

    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]); 
        if (i < argc - 1) strcat(result, " ");
    }

    //creating pipe 
    if(pipe(fd)==-1){
        perror("failed to create pipe");
        exit(1);
    }
    pid = fork();
    if(pid ==-1){
        perror("failed to create child process");
        exit(1);
    }
\
    if(pid==0){
        close(fd[0]); //since the child is a writer close the read end
        
        //moving the fd for stderr into the write end
        if (dup2(fd[1],STDERR_FILENO)==-1){
            perror("error moving the stderr into the pipe");
            exit(1);
        }
        close(fd[1]);
        execlp("/home/m-ammar/Documents/GitHub/Slim/LLM_Talker.pysh", "sh", "-c", result, NULL); //run the command
    }
    else{
        close(fd[1]); //parent is reader close its write end
        //will output 0 if no error occurs.
        int read_status = read(fd[0],buffer,sizeof(buffer)-1); //leave a space for the null terminator
        if (read_status<0){
            perror("problem reading the error");
            exit(1);
        }
        else if(read_status>=1){
            buffer[read_status]='\0';
            FILE *python_pipe = popen("python /home/m-ammar/Documents/GitHub/home/m-ammar/Documents/GitHub/Slim/LLM_Talker.py/Slim/LLM_Talker.py", "w");
            if(python_pipe==NULL){
                perror("error reaching the python file\n");
                exit(1);
            }
            fprintf(python_pipe,"%s",buffer);
            pclose(python_pipe);
        }
        wait(NULL);
        free(result);
    }
}