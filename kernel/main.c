#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 256
#define STRACE_ARG_LEN 2
#define NULL_LEN 1
#define EXEC_LEN(argc) (STRACE_ARG_LEN + NULL_LEN + ((argc) - 1))
#define SYSCALL_NUM 512

typedef struct syscall {
  char *name;
  double time;
  double ratio;
}syscalls;

typedef struct ratios {
  double ratio;
  int pos;
}ratios;

int search(syscalls *sys, int len, char *name) {
  int rc = -1;
  for (int i = 0; i < len; i++) {
    if (strcmp(sys[i].name, name) == 0) {
      rc = i;
      break;
    }
  }
  return rc;
}

// Comparison function for sorting ratios
int compare_ratios(const void *a, const void *b) {
    const ratios *ratio_a = (const ratios *)a;
    const ratios *ratio_b = (const ratios *)b;
    
    if (ratio_a->ratio < ratio_b->ratio) return -1;
    if (ratio_a->ratio > ratio_b->ratio) return 1;
    return 0;
}


void get_sorted_positions(ratios *arr, int size, int *sorted_positions) {
    // Create a copy of the array and sort it based on ratios
    ratios *sorted_array = malloc(size * sizeof(ratios));
    for (int i = 0; i < size; i++) {
        sorted_array[i] = arr[i];
    }
    qsort(sorted_array, size, sizeof(ratios), compare_ratios);
    
    // Create a mapping between ratios and positions
    for (int i = 0; i < size; i++) {
        sorted_positions[i] = sorted_array[i].pos;
    }
    
    // Free memory
    free(sorted_array);
}

int main(int argc, char *argv[]) {

  char buffer[BUFFER_SIZE * 2];
  size_t bytes_read = 0;

  syscalls syscalls[SYSCALL_NUM];
  int index = 0;

  char *line_start = buffer + BUFFER_SIZE;
  char *line_end;
  while ((bytes_read = read(0, buffer + BUFFER_SIZE, sizeof(buffer) - BUFFER_SIZE)) > 0) {
      line_end = buffer + BUFFER_SIZE;

      while ((line_end = strchr(line_start, '\n')) != NULL) {
          *line_end = '\0'; // Null-terminate the line
          char syscall_name[256];
          char time[256];
          int time_len = 0, name_len = 0;
          int name_get = 0;
          for (char *p = line_start; *p; p++) {
            while (*p != '(' && !name_get) {
              syscall_name[name_len++] = *p++;
            }
            name_get = 1;
            if (*p == '<') {
              p++;
              while (*p != '>') {
                time[time_len++] = *p++;
              }
            }
          }
          time[time_len] = '\0';
          syscall_name[name_len] = '\0';
          int idx;
          if ((idx = search(syscalls, index, syscall_name)) == -1) {

            syscalls[index].name = (char *)malloc(name_len + 1);
            strcpy(syscalls[index].name, syscall_name);
            syscalls[index].time = strtod(time, NULL);
            index++;

          } else {
            syscalls[idx].time += strtod(time, NULL);
          }

          line_start = line_end + 1; // Move to the next line
      }

      // Move remaining data to the beginning of the buffer
      size_t bytes_left = bytes_read - (line_start - (buffer + BUFFER_SIZE));
      size_t bytes_cost = (line_start - (buffer + BUFFER_SIZE));
      memmove(buffer + bytes_cost, line_start, bytes_left);
      line_start = buffer + bytes_cost;
  }

  double total_time = 0.0;
  for (int i = 0; i < index; i++) {
    total_time += syscalls[i].time;
  }

  free(syscalls[index - 2].name);
  free(syscalls[index - 1].name);

  int size = index - 2;
  ratios ratio[size];
  for (int i = 0; i < size; i++) {
    syscalls[i].ratio = syscalls[i].time / total_time;
    ratio[i].ratio = syscalls[i].ratio;
    ratio[i].pos = i;
  }


  int sorted_positions[size];

  get_sorted_positions(ratio, size, sorted_positions);

  for (int i = size - 1; i >= 0; i--) {
    printf("%s (%f%%)\n", syscalls[sorted_positions[i]].name, syscalls[sorted_positions[i]].ratio * 100);
    free(syscalls[sorted_positions[i]].name);

  }


  exit(EXIT_SUCCESS);
}
