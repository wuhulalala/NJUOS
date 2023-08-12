#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"
#include "thread-sync.h"

//#define DEBUG 0
//#define TEST 
#define MAXN 10000
#define MAX_THREAD 16
#define nround(x) ((int)(x))
#define INT2DOUBLE(x) (double)(x)
int T, N, M;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN];
int result;
int finished_threads = 0;
int average_missions = 0;
int last_missions = 0;
int nround = 0;
int wake_threads = 0;
int start = 0;
int missions = 0;
int unfinished[MAX_THREAD];
// to do
#define CAN_WORK(cond) 

#define DP(x, y) (((x) >= 0 && (y) >= 0) ? dp[x][y] : 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)
#define UP ((M < N) ? 0 : 1)
mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();
cond_t master = COND_INIT();

void Tworker(int id) {
  #ifndef TEST
  for (int k = 0; k < M + N - 1; k++) {
    //before_prepare();
    mutex_lock(&lk);
    #ifdef DEBUG
    printf("thread %d : this is line 38 round %d\n", id, k + 1);
    #endif
    while (!(finished_threads < wake_threads && id <= missions && unfinished[id - 1] && start)) {
      if (dp[N - 1][M - 1] != -1) {
        mutex_unlock(&lk);
        return;
      }
      #ifdef DEBUG
      printf("thread %d go to sleep\n", id);
      #endif
      cond_wait(&cv, &lk);
    }
    mutex_unlock(&lk);
    int my_missions = 0;
    if (id > missions) my_missions = 0;
    else if (id <= T){
      my_missions = average_missions;
      #ifdef DEBUG
      assert(my_missions >= 1);
      #endif
    } else {
      my_missions = last_missions;
      #ifdef DEBUG
      printf("%d\n", missions);
      assert(my_missions >= 1);
      #endif
    }
    int starti = 0, startj = 0; 
    if (!UP) {
      starti = ((nround <= MIN(M, N)) ? 0 : (nround - MIN(M, N))) + (id - 1) * average_missions;
      #ifdef DEBUG
      assert(0 <= starti && starti < N);
      #endif
      startj = ((nround >= MIN(M, N)) ? M - 1 : (nround - 1)) - (id - 1) * average_missions;
      #ifdef DEBUG
      assert(0 <= startj && startj < M);
      #endif
    } else if (UP){
      starti = ((nround <= MIN(M, N)) ? (nround - 1) : N - 1) - (id - 1) * average_missions;
      #ifdef DEBUG
      assert(0 <= starti && starti < N);
      #endif
      startj = ((nround >= MIN(M, N)) ? (nround - MIN(M, N)) : 0) + (id - 1) * average_missions;
      #ifdef DEBUG
      assert(0 <= startj && startj < M);
      #endif
    }
    int *temp = (int*)malloc(sizeof(int) * my_missions);
    for (int i = starti, j = startj, count = 0; count < my_missions; ((UP) ? (i--, j++) : (i++, j--)), count++) {

      #ifdef DEBUG
      assert(0 <= i && i < N);
      assert(0 <= j && j < M);
      #endif
      int skip_a = DP(i - 1, j);
      int skip_b = DP(i, j - 1);
      int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
      #ifdef DEBUG
      assert(0 <= count && count < my_missions);
      #endif
      temp[count] = MAX3(skip_a, skip_b, take_both);
    }

    mutex_lock(&lk);
    for (int i = starti, j = startj, count = 0; count < my_missions; ((UP) ? (i--, j++) : (i++, j--)), count++) {

      #ifdef DEBUG
      assert(0 <= i && i < N);
      assert(0 <= j && j < M);
      assert(0 <= count && count < my_missions);
      #endif
      dp[i][j] = temp[count];
      printf("%d %d thread id is %d\n", i, j, id);
      //printf("my_missions : %d\n", my_missions);
    }
    finished_threads++;
    unfinished[id - 1] = 0;
    if (finished_threads == wake_threads)
      cond_broadcast(&master);
    mutex_unlock(&lk);
    #ifdef DEBUG
    printf("thread %d : Here is the problem? round %d\n", id, k + 1);
    #endif
    free(temp);

    
}






  //for (int i = 0; i < N; i++) {
    //for (int j = 0; j < M; j++) {
      //// Always try to make DP code more readable
      //int skip_a = DP(i - 1, j);
      //int skip_b = DP(i, j - 1);
      //int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
      //dp[i][j] = MAX3(skip_a, skip_b, take_both);
      //printf("%d ", dp[i][j]);
    //}
    //printf("\n");
  //}

  //result = dp[N - 1][M - 1];
  #endif
}

int main(int argc, char *argv[]) {
  // No need to change
  assert(scanf("%s%s", A, B) == 2);
  N = strlen(A);
  M = strlen(B);
  T = !argv[1] ? 1 : atoi(argv[1]);
  dp[N - 1][M - 1] = -1;

  //setbuf(stdout, NULL);
  // Add preprocessing code here
  for (int i = 0; i < MAX_THREAD; i++) {
    unfinished[i] = 1;
  }
  for (int i = 0; i < T; i++) {
    create(Tworker);
  }
  for (int i = 1; i <= M + N - 1; i++) {
    finished_threads = 0;
    nround = i;
    if (nround <= MIN(M, N)) missions = nround;
    else missions = MIN((MAX(M, N) - (nround - MIN(M, N))), MIN(M, N));
    #ifdef DEBUG
    assert(1 <= missions && missions <= MIN(M, N));
    #endif
    if (missions <= T) average_missions = 1;
    else {
      average_missions = (int)((double)missions / T);
      last_missions = missions - T * average_missions;
    }
    //printf("average missions : %d\n", average_missions);
    #ifdef DEBUG
    assert(1 <= average_missions && average_missions <= missions);
    #endif
    if (missions <= T) wake_threads = missions;
    else wake_threads = T;
    #ifdef DEBUG
    assert(1 <= wake_threads && wake_threads <= T);
    #endif
    mutex_lock(&lk);
    start = 1;
    cond_broadcast(&cv);
    #ifdef DEBUG
    printf("this is main\n");
    #endif
    while (finished_threads < wake_threads)
      cond_wait(&master, &lk); 
    #ifdef DEBUG
    printf("round %d finished\n", nround);
    #endif
    start = 0;
    mutex_unlock(&lk);
    for (int j = 0; j < wake_threads; j++) {
      unfinished[j] = 1;
    }
  }
  cond_broadcast(&cv);
  join();  // Wait for all workers
  printf("%d\n", dp[N - 1][M - 1]);
}
