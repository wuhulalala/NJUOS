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

#define SIZEOF_INT 4
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
  int vertex[INITIAL_TREE_WIDTH];
  char name[MAX_PROCESS_NAME];
  int pid;

}node;

void get_arguments(int argc, char *argv[], Arguments *arg);

void arguments_initial(Arguments *arg);

size_t get_process_data(pro_info **pros);

void free_processes(pro_info **pros, size_t counts);

void build_pstree(pro_info **pros, size_t counts, int *graph[]); 
  //to do

void print_tree(pro_info **pros, size_t counts);

void initial_tree(int *graph[]);

void free_tree(int *graph[]);
int main(int argc, char *argv[]) {
  //for (int i = 0; i < argc; i++) {
    //assert(argv[i]);
    //printf("argv[%d] = %s\n", i, argv[i]);
  //}
  //assert(!argv[argc]);
  Arguments arg;
  pro_info *processes[INITIAL_MAX_PROCESS_NUM];
  int *graph[INITIAL_MAX_PROCESS_NUM];
  
  get_arguments(argc, argv, &arg);
  //printf("%ds SHOW_PIDS\n%ds NUMERIC_SORT argument is %s\n%ds VERSION\n", 
        //arg.SHOW_PIDS, arg.NUMERIC_SORT, arg.arg, arg.VERSION));




  // use unix tools to get the information about pid(the message is stored at file /proc/[pid]/stat), storing the info in a temp file data
  system("find /proc/ -maxdepth 2 -mindepth 2 -type f -name \"stat\" | xargs -d '\n' awk '{print $1, $2, $4, $20, $22}' 2>&1 \
              | grep -v No | sed 's/^\\([^()]*\\) (\\(.*\\)) \\(.*\\)$/\\1 \\2 \\3/' > data");

  size_t counts = get_process_data(processes);
  //for (size_t i = 0; i < counts; i += 1) {
    //printf("pid is %d, ppid is %d, process name is %s, number of threads: %d, fork time: %s\n", 
          //processes[i] -> pid, processes[i] -> ppid, processes[i]->name, 
          //processes[i] -> threads, processes[i] -> fork_time);
  //}
  initial_tree(graph);
  build_pstree(processes, counts, graph);
  free_processes(processes, counts);
  free_tree(graph);
  system("rm data");
  return 0;
}



void initial_tree(int *graph[]) {
  for (size_t i = 0; i < INITIAL_MAX_PROCESS_NUM; i += 1) {
    graph[i] = (int *) malloc(INITIAL_LENGTH * sizeof(int));
    graph[i][INDEX] = 2;
    graph[i][LENGTH] = INITIAL_LENGTH;
  }
}




void print_dash(int count) {
  printf("----");
}

void print_pipe() {
  //to do
}




void build_pstree(pro_info **pros, size_t counts, int *graph[]) {
  for (size_t i = 0; i < counts; i += 1) {
    int pid = pros[i] -> pid;
    int ppid = pros[i] -> ppid;
    int index = graph[ppid][INDEX];
    if (index < graph[ppid][LENGTH]) {
      graph[ppid][index] = pid;
      index += 1;
      graph[ppid][INDEX] = index;
    } else {
      int *temp = (int *) malloc(graph[ppid][LENGTH] * SIZEOF_INT);
      size_t size = graph[ppid][LENGTH] * SIZEOF_INT;
      memmove(temp, graph[ppid], size);
      graph[ppid] = (int *) realloc(graph[ppid], graph[ppid][LENGTH] * FACTOR * SIZEOF_INT);
      memmove(graph[ppid], temp, size);
      i -= 1;
      free(temp);
      continue;
    }
  } 

}


void free_tree(int *graph[]) {
  for (size_t i = 0; i < INITIAL_MAX_PROCESS_NUM; i += 1) {
    free(graph[i]);
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