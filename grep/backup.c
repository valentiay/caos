#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Первый раз отправлено 2018-05-10
// Первый раз проверено  2018-05-17
// Вторая попытка

#define MAX_STR_LEN 1024
#define MAX_TMP_NAME_LEN 10

int thread_num;
char* file_name;
char* pattern;

int** pipes;
char buff[MAX_STR_LEN];

void execute_grep(int thread_no) {
  pipe(pipes[thread_no]);
  if (fork() == 0) {
    close(pipes[thread_no][0]);
    dup2(pipes[thread_no][1], STDOUT_FILENO);
    close(pipes[thread_no][1]);
    execlp("grep", "grep", pattern, buff, NULL);
    printf("Grep error\n");
  } else {
    close(pipes[thread_no][1]);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("im here\n");
    perror("Wrong parameter number. Use ./grep <pattern> <file> <thread_count>");
    exit(1);
  }

  pattern = argv[1];
  file_name = argv[2];
  thread_num = atoi(argv[3]);

  FILE* file = fopen(file_name, "r");
  if (file == NULL) {
    perror("No such file");
    exit(2);
  }

  pipes = malloc(sizeof(int*) * thread_num);
  for (int i = 0; i < thread_num; i++) {
    pipes[i] = malloc(sizeof(int) * 2);
  }

  for (int i = 0; i < thread_num; i++) {
    if (fgets(buff, MAX_STR_LEN, file) == NULL) {
      break;
    }
    buff[strlen(buff) - 1] = '\0';
    execute_grep(i);
  }

  char tmp_file_name[MAX_TMP_NAME_LEN] = "aXXXXXX";
  FILE* tmp = fdopen(mkstemp(tmp_file_name), "w");
  int file_ended = 0;
  while(!file_ended) {
    for(int i = 0; i < thread_num; i++) {
      wait(NULL);
    }

    for (int i = 0; i < thread_num; i++) {
      ssize_t got_chars = 0;
      while ((got_chars = read(pipes[i][0], buff, MAX_TMP_NAME_LEN)) > 0) {
        buff[got_chars] = 0;
        fprintf(tmp, buff);
      }
      close(pipes[i][0]);
    }

    for (int i = 0; i < thread_num; i++) {
      if (fgets(buff, MAX_STR_LEN, file) == NULL) {
        file_ended = 1;
        break;
      }

      buff[strlen(buff) - 1] = '\0';
      execute_grep(i);
    }
  }
  fclose(file);
  fclose(tmp);

  for (int i = 0; i < thread_num; i++) {
    free(pipes[i]);
  }
  free(pipes);

  pid_t pid = fork();
  if (pid == 0) {
    execlp("sort", "sort", "-u", tmp_file_name, NULL);
  } else {
    waitpid(pid, NULL, 0);
    unlink(tmp_file_name);
  }
}