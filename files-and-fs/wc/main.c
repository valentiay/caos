#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <bits/wctype-wchar.h>
#include <stdlib.h>
#include <memory.h>

typedef int bool;
#define true 1
#define false 0

#define NULL_POINTER 1
#define NO_SUCH_FILE 2

size_t getWC(char *buff, size_t bytes, FILE *file, wchar_t *wc) {
  size_t left_bytes = bytes - 1;
  size_t bytes_read = fread(buff + 1, sizeof(char), left_bytes, file);
  mbstate_t state;
  memset(&state, 0, sizeof state);
  mbrtowc(wc, buff, bytes, &state);
  return bytes_read;
}

int processFile(FILE* file, size_t *lines, size_t *words, size_t *chars, size_t *bytes, bool *binary) {
  if (file == NULL) {
    return NO_SUCH_FILE;
  }
  if (lines == NULL || words == NULL || chars == NULL || bytes == NULL || binary == NULL) {
    return NULL_POINTER;
  }

  *binary = false;
  bool prev_whitespace = false;
  wchar_t wc;
  while(!feof(file)) {
    char buff[4];
    *bytes += fread(&buff, sizeof(char), 1, file);
    if (*binary) {
      continue;
    }
    if (((buff[0] >> 7) & 0b1) == 0b0) {
      *bytes +=  getWC(buff, 1, file, &wc);;
    } else if (((buff[0] >> 5) & 0b111) == 0b110) {
      *bytes += getWC(buff, 2, file, &wc);
    } else if (((buff[0] >> 4) & 0b1111) == 0b1110) {
      *bytes += getWC(buff, 3, file, &wc);
    } else if (((buff[0] >> 3) & 0b11111) == 0b11110) {
      *bytes += getWC(buff, 4, file, &wc);
    } else {
      *binary = true;
      continue;
    }

    if (wc == L'\n') {
      (*lines)++;
    }

    if (iswspace(wc)) {
      if (prev_whitespace == false) {
        (*words)++;
      }
      prev_whitespace = true;
    } else {
      prev_whitespace = false;
    }

    (*chars)++;
  }
  if (!prev_whitespace) {
    (*words)++;
  }

  if (wc == L'\n') {
    (*lines)--;
  }
  (*chars)--;
  if (*binary) {
    *lines = 0;
    *words = 0;
    *chars = 0;
  }
}

int main(int argc, char* argv[]) {
  const char* mode = "rb";
  setlocale(LC_ALL, "");

  size_t total_chars = 0;
  size_t total_words = 0;
  size_t total_lines = 0;
  size_t total_bytes = 0;

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      size_t chars = 0;
      size_t words = 0;
      size_t lines = 0;
      size_t bytes = 0;

      bool binary = false;

      FILE *file = fopen(argv[i], mode);
      if (processFile(file, &lines, &words, &chars, &bytes, &binary) == NO_SUCH_FILE) {
        wprintf(L"wc: %s: No such file\n", argv[i]);
      } else {
        fclose(file);

        if (binary) {
          wprintf(L"%14lu %s\n", bytes, argv[i]);
        } else {
          wprintf(L"%4lu %4lu %4lu %s\n", lines, words, chars, argv[i]);
        }

        total_chars += chars;
        total_words += words;
        total_lines += lines;
        total_bytes += bytes;
      }
    }
  } else {
    FILE* file = stdin;
    bool binary;
    processFile(file, &total_lines, &total_words, &total_chars, &total_bytes, &binary);

    if (binary) {
      wprintf(L"%14lu\n", total_bytes);
    } else {
      wprintf(L"%4lu %4lu %4lu\n", total_lines, total_words, total_chars);
    }
  }

  if (argc > 2) {
    wprintf(L"%4lu %4lu %4lu total\n", total_lines, total_words, total_chars);
  }
  return 0;
}