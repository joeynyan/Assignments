#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

char command[1000];
char parameters[1000];

char * type_prompt() {
  scanf("%s", command);
}

int main(int argc, char const *argv[]) {
  while(1) {
    type_prompt();
    read_command(command, parameters);
    if (fork() != 0) {
      /*Parent Code*/
      waitpid(-1, &status, 0);
    } else {
      /*Child code */
      execve(command, parameters, 0);
    }
  }
  return 0;
}

type_prompt()
