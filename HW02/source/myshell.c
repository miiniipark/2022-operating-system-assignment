#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXARGC 20
#define MAXLINE 80
int execute(char**);
char** gettoken(char*);

char* cwd; // initial working directory

int main()
{
    char line[MAXLINE], * token;
    char** args;
    int status = 0;

    printf("My Shell Started.... \n");
    /*if argument buf == NULL,
    getcwd() fuction automatically malloc memory*/
    cwd = getcwd(NULL, 0);

    while (1) {

        printf("MyShell>");
        fgets(line, MAXLINE, stdin);

        if (strncmp(line, "quit", 4) != 0) {
            args = gettoken(line);
            status = execute(args);
            printf("\n");

        }
        else {
            free(cwd); // free memory malloc'd by getcwd()
            exit(1);
        }

    } /* end of while*/

    free(args);

    return(1);

}

char** gettoken(char* line)
{

    int idx = 0;
    char delim[] = " \t\r\n";
    char** tokenList = malloc(MAXLINE);
    char* token;

    if (!tokenList) {
        printf("malloc error \n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, delim);
    while (token != NULL) {
        tokenList[idx] = token;
        idx++;

        if (idx > MAXARGC - 1) {
            printf("Too many args  \n");
            break;
        }
        token = strtok(NULL, delim);
    } // end of while

    tokenList[idx] = NULL; // insert end of string mark
    return tokenList;

} // end of gettoken

int execute(char** args)
{
    pid_t pid;
    int status;
    int i = 1;
    char* path; // directory for custom commands

    if (args[0] == NULL) { // empty command
        return 1;
    }

    printf(" %s ", args[0]);
    while (args[i] != NULL) {

        printf(" %s ", args[i]);
        i++;

    }
    printf("\n");

    // if command is "cd", no need to create a new process
    if (strcmp(args[0], "cd") == 0) {
        status = chdir(args[1]); // use chdir() fuction to move directory
        free(args); // prepare to receive new commands
        if (status == 0) {
            return 0; // continue working in a shell process
        }
        else {
            perror("");
            return 1; // continue working in a shell process
        }
    }

    // source : https://nomad-programmer.tistory.com/106
    pid = fork(); // create new process
    if (pid < 0) { // failed to create process
        perror("");
    }
    else if (pid == 0) { // child process
        status = execvp(args[0], args); // searching PATH directory
        if (status < 0) {
            // searching mybin directory
            path = malloc(strlen(cwd) + strlen("/mybin/") + strlen(args[0]) + 1);
            strcpy(path, cwd);
            strcat(path, "/mybin/");
            strcat(path, args[0]);
            status = execv(path, args);
            /*If the program doesn't exist,
            clean up resources of child shell processes*/
            if (status < 0) {
                perror("");
                free(path);
                free(args);
                free(cwd);
                exit(EXIT_FAILURE);
            }
        }
    }
    else { // parent process
        // parent will wait for the child to complete
        wait(NULL);
    }

    free(args);
    return (1);
}// end of execute
