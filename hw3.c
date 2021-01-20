#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
/* Citations: 
 * Computer Systems - A Programmer's Perspective 3e pgs: 744, 745, 
 */

void eval(char *cmdline);
int parseline(char *buf, char **argv);

// signal handler for CTRL+C
void ctrlCHandle() {
    char buf[50] = "\ncaught sigint\nCS361 > ";
    write(1, buf, 50);
 }

// signal handler for CTRL+Z
void ctrlZHandle() {
    char buf[50] = "\ncaught sigtstp\nCS361 > ";
    write(1, buf, 50);
}

/* Ref: Computer Systems, a Programmer's Perspective 3e Ch 8.4 (pages 754-756) */
int parseline(char *buf, char **argv) {
  char *delim;
  int argc;

  buf[strlen(buf)-1] = ' ';
  while (*buf && (*buf == ' ')) buf++;
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    if (strcmp(buf, ";") != 0) {
      argv[argc++] = buf;
      *delim = '\0';
    }

    // run the first command if the commandline contains ;
    if (strcmp(buf, ";") == 0){
      argv[argc-1] = NULL; // add a NULL terminator to the array
    pid_t pid;
    // spawn a new child process
    if ((pid = fork()) == 0) {
        if (execve(argv[0], argv, NULL) < 0) {
          printf("%s: Command not found\n", argv[0]);
          exit(0);
        }
      }

  int status;
  if (waitpid(pid, &status, 0) < WUNTRACED) {
     printf("waitpid error\n");
  }
  else {
    printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
  }
  buf = strtok(buf, ";");

  argc = 0;
    }
    buf = delim + 1;
    while (*buf && (*buf == ' '))
      buf++;
  }

  argv[argc] = NULL; // add a NULL terminator to the array

  if (argc == 0) return 1;

  return 0;
}
/* Ref: Computer Systems, a Programmer's Perspective 3e Ch 8.4 (pages 754-756 */
void eval(char *cmdline) {
  char *argv[20];
  char buf[200];
  pid_t pid;

  strcpy(buf, cmdline); // copy cmdline to buf
  parseline(buf, argv);
  if (argv[0] == NULL) return;
  // exit if the user manually typed exit
  if (strcmp(argv[0], "exit") == 0) exit(0);
 
  if ((pid = fork()) == 0) {
    if (execve(argv[0], argv, NULL) < 0) {
      printf("%s: Command not found\n", argv[0]);
      exit(0);
    }
  }

  int status;
  if (waitpid(pid, &status, WUNTRACED) < 0) {
    printf("waitpid error\n");
  }
  else {
    printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
  }
  return;
}


int main(){
    char line[500];

    while (1) {
        printf("CS361 >");

        signal(SIGINT, ctrlCHandle);
        signal(SIGTSTP, ctrlZHandle);

        fgets(line, 500, stdin);  // read the line from the terminal
        eval(line);
    }
  return 0;
  }
