#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int find_tours(int id,int nrow,int ncol,int vis_sqrs,int spaces[nrow*ncol],int hist[nrow*ncol]) {
 
  // add current id to history
  hist[vis_sqrs] = id;
  int moves[8],i,l,k;
  static int tours = 0;
  #pragma omp threadprivate(tours)
  if (vis_sqrs == (nrow*ncol-1)) {
    tours = tours + 1;
    return 0;
  }
 
  // generate possible moves, with conditions
  moves[0] = id - 2*ncol - 1;
  moves[1] = id - 2*ncol + 1;
  moves[2] = id + 2*ncol - 1;
  moves[3] = id + 2*ncol + 1;
  moves[4] = id - 2 - ncol;
  moves[5] = id - 2 + ncol;
  moves[6] = id + 2 - ncol;
  moves[7] = id + 2 + ncol;
  if (id <= 2*ncol) {
    moves[0] = 0;
    moves[1] = 0;
  }
  if (id <= ncol) {
    moves[4] = 0;
    moves[6] = 0;
  }
  if (id > ncol*(nrow-2)) {
    moves[2] = 0;
    moves[3] = 0;
  }
  if (id > ncol*(nrow-1)) {
    moves[5] = 0;
    moves[7] = 0;
  }
  if (id % ncol == 2 | id % ncol == 1) {
    moves[4] = 0;
    moves[5] = 0;
  }
  if (id % ncol == 1) {
    moves[0] = 0;
    moves[2] = 0;
  }
  if (id % ncol == 0 | id % ncol == (ncol-1)) {
    moves[6] = 0;
    moves[7] = 0;
  }
  if (id % ncol == 0) {
    moves[1] = 0;
    moves[3] = 0;
  }
  for (l=0;l<8;l++) {
    for (i=0;i<=vis_sqrs;i++) {
      if (moves[l] == hist[i]) {
        moves[l] = 0;
      }
    }
  }

  // recursive call 
  for (k=0;k<8;k++) {
    if (moves[k] != 0) {
      find_tours(moves[k],nrow,ncol,vis_sqrs+1,spaces,hist);
    }
  }
  return tours;  
}
 
int main (int argc,char *argv[]) {
  int m = 3;
  int n = 4;
  int i,j;

  // create board
  int board[m*n];
  for (i=0;i<m*n;i++) {
    board[i] = i+1;
  }
  int hist[m*n];
  int q,p,w;
  int vis_sqrs = 0;
  double tstart, tend, elapsed;

  // parallelize and time
  tstart = omp_get_wtime();
  int tot = 0;

  #pragma omp parallel num_threads(m*n)
  {
    #pragma omp for private(q,p,vis_sqrs,hist) schedule(static)
    for (q=1;q<(m*n+1);q++) {
      p = find_tours(q,m,n,vis_sqrs,board,hist);
      tot = tot + p;
    }
  }
  tend = omp_get_wtime();
  elapsed = tend-tstart;
  printf("Found %d tours\n",tot);
  printf("Took %f seconds\n",elapsed/100);
  return 0;
}