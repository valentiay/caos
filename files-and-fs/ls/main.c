#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 512

#define ALLOC_ERROR "Allocator error\n"
#define PATH_ERROR "The path is not correct\n"


int visit(char *dirName) {
  DIR* dir = opendir(dirName);
  printf("%s\n", dirName);
  if (!dir) {
    fprintf(stderr, PATH_ERROR);
    return 1;
  }

  struct stat mystat;
  struct dirent* element;

  int foldersMax = 8;
  int foldersSize = 0;
  char** folders = malloc(foldersMax * sizeof(*folders));

  if (!folders) {
    fprintf(stderr,ALLOC_ERROR);
    return 1;
  }
  for (int j = 0; j < foldersMax; ++j) {
    folders[j] = malloc(512 * sizeof(char));
    if (!folders[j]) {
      fprintf(stderr, ALLOC_ERROR);
    }
  }

  while((element = readdir(dir)) != NULL) {
    char file[MAX_LENGTH];
    char owner[MAX_LENGTH];
    char type;

    sprintf(file, "%s%s", dirName, element->d_name);

    lstat(file, &mystat);

    sprintf(owner, "%s", getpwuid(mystat.st_uid)->pw_name);

    if (S_ISLNK(mystat.st_mode)) {
      type = 'l';
    } else if (S_ISDIR(mystat.st_mode)) {
      type = 'd';
    } else {
      type = '-';
    }

    printf("   %s %li %c %s  \n", element->d_name, mystat.st_size, type, owner);

    if (type == 'd' && strcmp(element->d_name, "..") != 0 && strcmp(element->d_name, ".") != 0) {
      if (foldersMax <= foldersSize) {
        foldersMax *= 2;
        folders = realloc(folders, foldersMax * sizeof(char*));
        for (int i = foldersMax/2; i < foldersMax ; ++i) {
          folders[i] = malloc(sizeof(char) * 512);
          if (!folders[i]) {
            fprintf(stderr, ALLOC_ERROR);
            return 1;
          }
        }

      }
      sprintf(folders[foldersSize++], "%s/", file);
    }
  }


  for (int i = 0; i < foldersSize; ++i) {
    visit(folders[i]);
  }

  for (int i = 0; i < foldersMax; ++i) {
    free(folders[i]);
  }
  free(folders);

  closedir(dir);
  return 0;
}


int main(int argc, char* argv[]) {
  char startDir[MAX_LENGTH];
  if (argc == 1) {
    getcwd(startDir, sizeof(startDir));
    sprintf(startDir, "%s/", startDir);
    visit(startDir);
  } else {
    for (int i = 1; i < argc; ++i) {
      sprintf(startDir, "%s/", argv[i]);
      visit(startDir);
    }
  }

  return 0;
}