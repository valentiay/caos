#include <stdio.h>
#include <string.h>


void printEnd(FILE *arg, char *name) {
  if (!arg) {
    printf("argument %s is invalid\n", name);
    return;
  }
  
  if (name == "") {
    printf("----stdin----\n");
  } else {
    printf("----%s----\n", name);
  }

  char line[1000];
  char strings[10][1000];
  
  int lines = 0;
  int current = 0;

  while (fgets(line, 1000, arg)) {
    strcpy(strings[current], line);
    ++current;
    ++lines;
    current = current % 10;
  }

  if (lines < 10) {
    for (int i = 0; i < lines; ++i) {
      printf("%s", strings[i]);
    }
  } else {
    for (int i = 0; i < 10; ++i) {
      int j = (i + current) % 10;
      printf("%s", strings[j]);
    }
  }

  fclose(arg);
}


int main(int argc, char* argv[]) {

  if (!argv[1]) {
    printEnd(stdin, "");
  } else {
    for (int i = 1; i < argc; ++i) {
      FILE* arg = fopen(argv[i], "r");
      printEnd(arg, argv[i]);
    }
  }

  return 0;
}