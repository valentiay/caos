#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
#include <limits.h>

long n;
long sqrtn;
int th_count;
long chunk_size = 5000000;

char* primes;

long nextPrime(long prev_prime) {
  for (long i = prev_prime + 1; i < sqrtn; i++) {
    if (primes[i] == 1) {
      return i;
    }
  }
  return 0;
}

struct Args {
    char* arr;
    long start;
    long end;
    int thread; // Debug
};

void* eratosthenes(void* a) {
  struct Args* args = (struct Args*)a;
  for (long i = 2; i < sqrtn; i++) {
    if (primes[i] == 1) {
      long j = args->start;
      for (; j % i != 0; j++);
      for (; j < args->end; j += i) {
        args->arr[j - args->start] = 0;
      }
    }
  }
}

int main(int argc, char* argv[]) {
  th_count = atoi(argv[1]);
  if (argc == 2) {
    n = INT_MAX;
  } else {
    n = atol(argv[2]);
  }
  sqrtn = sqrt(n) + 1;
  primes = malloc(sizeof(char)*sqrtn);
  memset(primes, 1, sizeof(char)*sqrtn);

  for (long i = 2; i < sqrtn && i != 0; i = nextPrime(i)) {
    for (long j = 2 * i; j < sqrtn; j += i) {
      primes[j] = 0;
    }
  }

  for (long i = 2; i < sqrtn; i++) {
    if (primes[i] == 1)
    printf("%ld\n", i);
  }

  char** other_primes = malloc(sizeof(char*) * th_count);
  for (int i = 0; i < th_count; i++) {
    other_primes[i] = malloc(sizeof(char) * chunk_size);
  }

  pthread_t* threads = malloc(sizeof(pthread_t) * th_count);
  struct Args* args = malloc(sizeof(struct Args) * th_count);

  for (int i = 0; i < th_count; i++) {
    args[i].arr = other_primes[i];
    args[i].start = sqrtn + i * chunk_size;
    args[i].end = sqrtn + (i + 1) * chunk_size;
    args[i].thread = i;
    if (args[i].start <= n) {
      int status = pthread_create(&threads[i], NULL, eratosthenes, &(args[i]));
    }
  }

  for (long start = sqrtn; start <= n; start += th_count*chunk_size) {
    for (int i = 0; i < th_count; i++) {
      memset(other_primes[i], 1, sizeof(char) * chunk_size);
    }

    for (int i = 0; i < th_count && args[i].start <= n; i++) {
      pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < th_count; i++) {
      for (long j = start + i * chunk_size; j < start + (i + 1) * chunk_size && j <= n; j++) {
        if (other_primes[i][j - (start + i * chunk_size)] == 1) {
          printf("%ld\n", j);
        }
      }

      args[i].arr = other_primes[i];
      args[i].start = start + th_count*chunk_size + i * chunk_size;
      args[i].end = start + th_count*chunk_size + (i + 1) * chunk_size;
      args[i].thread = i;
      if (args[i].start <= n) {
        int status = pthread_create(&threads[i], NULL, eratosthenes, &(args[i]));
      }
    }
  }

  for (int i = 0; i < th_count; i++) {
    free(other_primes[i]);
  }
  free(args);
  free(other_primes);
  free(primes);
}