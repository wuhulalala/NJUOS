#include <stdio.h>
#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARG_LENGth 128

#define MAX_PROCESS_NAME 256 

#define INITIAL_MAX_PROCESS_NUM 32768

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define FILE_OPEN_FALIURE 1
#define FILE_OPEN_SUCCESS 0

#define PROCESS_FORK_TIME 128

#define MAX_TREE_DEPTH 100

#define INITIAL_TREE_WIDTH 64

#define FACTOR 2

#define INITIAL_LENGTH 3

#define LENGTH 1
#define INDEX 0

#define SIZEOF_NODE sizeof(node)
const struct option longopts[] = {
  {"show-pids", no_argument, NULL, 'p'},
  {"numeric-sort", required_argument, NULL, 'n'},
  {"version", no_argument, NULL, 'V'},
  {NULL, 0, NULL, 0}
};

typedef struct Arguments{
  int flags_count;
  char arg[MAX_ARG_LENGth];
  int SHOW_PIDS;
  int NUMERIC_SORT;
  int VERSION;
}Arguments;

typedef struct pro_info {
  int pid;
  int ppid;
  char name[MAX_PROCESS_NAME];
  char fork_time[PROCESS_FORK_TIME];
  int threads;
}pro_info;

typedef struct node
{
  char name[MAX_PROCESS_NAME];
  int pid;

}node;

void get_arguments(int argc, char *argv[], Arguments *arg);

void arguments_initial(Arguments *arg);

size_t get_process_data(pro_info **pros);

void free_processes(pro_info **pros, size_t counts);

void build_pstree(pro_info **pros, size_t counts, node *graph[]); 

void print_tree(node *graph[], size_t *current_count, size_t counts, int id, int real_count, int mask_count, int real_len, int mask_len, int last_space_len, int flag);

void initial_tree(node *graph[], size_t counts, pro_info **pros);

void free_tree(node *graph[], pro_info **info, size_t counts);
int main(int argc, char *argv[]) {
  //for (int i = 0; i < argc; i++) {
    //assert(argv[i]);
    //printf("argv[%d] = %s\n", i, argv[i]);
  //}
  //assert(!argv[argc]);
  Arguments arg;
  pro_info *processes[INITIAL_MAX_PROCESS_NUM];
  node *graph[INITIAL_MAX_PROCESS_NUM];
  
  get_arguments(argc, argv, &arg);
  //printf("%ds SHOW_PIDS\n%ds NUMERIC_SORT argument is %s\n%ds VERSION\n", 
        //arg.SHOW_PIDS, arg.NUMERIC_SORT, arg.arg, arg.VERSION));




  // use unix tools to get the information about pid(the message is stored at file /proc/[pid]/stat), storing the info in a temp file data
  system("find /proc/ -maxdepth 2 -mindepth 2 -type f -name \"stat\" | xargs -d '\n' awk '{print $1, $2, $4, $20}' 2>&1 \
              | grep -v No | sed 's/^\\([^()]*\\) (\\(.*\\)) \\(.*\\)$/\\1 \\2 \\3/' > data");

  size_t counts = get_process_data(processes);
  for (size_t i = 0; i < counts; i += 1) {
    printf("pid is %d, ppid is %d, process name is %s, number of threads: %d, fork time: %s\n", 
          processes[i] -> pid, processes[i] -> ppid, processes[i]->name, 
          processes[i] -> threads, processes[i] -> fork_time);
  }
  size_t current_count = 0;
  initial_tree(graph, counts, processes);
  build_pstree(processes, counts, graph);
  if (arg.VERSION) {
    fprintf(stderr, "pstree: version 1.0\n");
  } else {
    print_tree(graph, &current_count, counts, 1, -1, 0, 0, strlen(graph[1][INDEX].name), 0, 0);
  }
  free_tree(graph, processes, counts);
  free_processes(processes, counts);
  system("rm data");
  return 0;
}



void initial_tree(node *graph[], size_t counts, pro_info **pros) {
  for (size_t i = 0; i < counts; i += 1) {
    int pid = pros[i] -> pid;
    graph[pid] = (node *) malloc(INITIAL_LENGTH * SIZEOF_NODE);
    graph[pid][INDEX].pid = 2;
    strcpy(graph[pid][INDEX].name, pros[i] -> name);
    graph[pid][LENGTH].pid = INITIAL_LENGTH;
  }
}


void print_tree(node *graph[], size_t *current_count, size_t counts, int id, int real_count, int mask_count, int real_len, int mask_len, int last_space_len, int flag) {
  if (*current_count <= counts) {
    size_t num = graph[id][INDEX].pid;
    *current_count = *current_count + 1;
    if (num > 3) {
      printf("%s----", graph[id][INDEX].name);
      real_count = mask_count;
      real_len = mask_len;
    } else if (num == 2) {
      printf("%s\n", graph[id][INDEX].name);
      if (*current_count < counts) {
        if (flag) {
          for (int i = 0; i < 4 * real_count + 2 + real_len; i += 1) {
            printf(" ");
          } 
          printf("--");
        } else {
          for (int i = 0; i < last_space_len; i += 1) {
            printf(" ");
          } 
          printf("--");
        }
      }
    } else {
      printf("%s----", graph[id][INDEX].name);
    }

    for (size_t i = 2; i < num; i += 1) {
      int id_next = graph[id][i].pid;
      int len = strlen(graph[id_next][INDEX].name);
      if (i == num - 1) 
        print_tree(graph, current_count, counts, id_next, real_count, mask_count + 1, real_len, mask_len + len, 4 * real_count + 2 + real_len, 1);
      else 
        print_tree(graph, current_count, counts, id_next, real_count, mask_count + 1, real_len, mask_len + len, 4 * real_count + 2 + real_len, 0);

    }

  }
}


void build_pstree(pro_info **pros, size_t counts, node *graph[]) {
  for (size_t i = 0; i < counts; i += 1) {
    int pid = pros[i] -> pid;
    if (pid == 1) {
      continue;
    }
    int ppid = pros[i] -> ppid;
    int index = graph[ppid][INDEX].pid;
    if (index < graph[ppid][LENGTH].pid) {
      graph[ppid][index].pid = pid;
      strcpy(graph[ppid][index].name, pros[i] -> name);
      index += 1;
      graph[ppid][INDEX].pid = index;
    } else {
      node *temp = (node *) malloc(graph[ppid][LENGTH].pid * SIZEOF_NODE);
      size_t size = graph[ppid][LENGTH].pid * SIZEOF_NODE;
      memmove(temp, graph[ppid], size);
      graph[ppid] = (node *) realloc(graph[ppid], graph[ppid][LENGTH].pid * FACTOR * SIZEOF_NODE);
      memmove(graph[ppid], temp, size);
      graph[ppid][LENGTH].pid = graph[ppid][LENGTH].pid * 2;
      i -= 1;
      free(temp);
      continue;
    }
  } 

}


void free_tree(node *graph[], pro_info **pros, size_t counts) {
  for (size_t i = 0; i < counts; i += 1) {
    int pid = pros[i] -> pid;
    free(graph[pid]);
  }
}







void get_arguments(int argc, char *argv[], Arguments *arg) {
  int opt;
  int long_index;
  while ((opt = getopt_long(argc, argv, "pn:V", longopts, &long_index)) != -1) {
        switch (opt) {
            case 'p':
                arg -> SHOW_PIDS = 1;
                arg -> flags_count += 1;
                break;
            case 'n':
                arg -> NUMERIC_SORT = 1;
                arg -> flags_count += 1;
                strcpy(arg -> arg, optarg);
                break;
            case 'V':
                arg -> VERSION = 1;
                arg -> flags_count += 1;
                break;
            default:
                //printf("Usage: ./pstree-version [-V --version] \n     [ -p --show-pids arguments] [ -n --numeric-sort ]\n");
                break;
        }
    } 
}

void arguments_initial(Arguments *arg) {
  arg -> SHOW_PIDS = 0, arg -> NUMERIC_SORT = 0, arg -> VERSION = 0;
}

size_t get_process_data(pro_info **pros) {
  char *line = NULL;
  size_t len = 0;
  size_t lines = 0;
  FILE * fp = fopen("data", "r");
  int read;
  if (!fp) {
    return FILE_OPEN_FALIURE;
  }

  while ((read = getline(&line, &len, fp)) != -1) {
        char *token = strtok(line, " ");
        size_t token_index = 0;
        pros[lines] = (pro_info*)malloc(sizeof(pro_info));
        while (token != NULL) {
            if (token_index == 0) {
              pros[lines] -> pid = atoi(token);
            } else if (token_index == 1) {
              strcpy(pros[lines] -> name, token);
            } else if (token_index == 2) {
              pros[lines] -> ppid = atoi(token);
            } else if (token_index == 3) {
              pros[lines] -> threads = atoi(token);
            } else {
              strcpy(pros[lines] -> fork_time, token);
            }
            token = strtok(NULL, " ");
            token_index += 1;
        }
        lines += 1;
  }
  fclose(fp);
  return lines;
}


void free_processes(pro_info **pros, size_t counts) {
  for (size_t i = 0; i < counts; i += 1) {
    free(pros[i]);
  }
}