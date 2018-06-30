#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define FAILED_TO_FORK 1
#define FAILED_TO_EXECUTE_PROGRAM 2
#define FAILED_TO_MAKE_PIPE 3

// > родительский процесс умирает раньше чем потомки, как это исправить?
// Запускать каждую программу в отдельном процессе, делать для каждой пары
// таких процессов pipe и ждать, пока все процессы завершаться с помощью wait

// > parent process finishes before children, how to fix it?
// Launch every program in separate forked process, making pipe for every
// pair of such processes and wait until all processes finish using wait()

int main(int argc, char* argv[]) {
  int isParent;
  int prevReadEnd;
  int p[2];
  for (int i = 1; i < argc; i++) {
    if (pipe(p) < 0) {
      perror("Pipe creation failed!");
      exit(FAILED_TO_MAKE_PIPE);
    }
    isParent = fork();
    if (isParent < 0) {
      perror("Fork failed!");
      exit(FAILED_TO_FORK);
    }
    if (!isParent) {
      close(p[0]);
      dup2(prevReadEnd, STDIN_FILENO);
      if (i != argc - 1) {
        dup2(p[1], STDOUT_FILENO);
      }
      execlp(argv[i], argv[i], NULL);
      perror("Program execution failed!\n");
      exit(FAILED_TO_EXECUTE_PROGRAM);
    } else {
      close(prevReadEnd);
      prevReadEnd = p[0];
      close(p[1]);
    }
  }

  for (int i = 1; i < argc; i++) {
    wait(NULL);
  }

  return 0;
}