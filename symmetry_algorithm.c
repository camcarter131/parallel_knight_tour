#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>


// functions for converting from single number to row and col indices
int iden(int row,int col,int ncol) {
  int i = (col+1) + (ncol*row);
  return i;
}

int col (int id, int ncols) {
  int c = (id-1)%ncols;
  return c;
}

int row (int id, int ncols) {
  int r = (id-1)/ncols;
  return r;
}

int find_sym_tours(int id,int nrow,int ncol,int vis,int spaces[nrow*ncol],int hist[(nrow*ncol-1)/4],int hist1[(nrow*ncol-1)/4],int hist2[(nrow*ncol-1)/4],int hist3[(nrow*ncol-1)/4]) {
  int moves[8],i,l,k;
  static int tours = 0;

  // shift indices so that (0,0) is central
  int n_mid = ncol/2;
  int r = row(id,ncol)-n_mid;
  int c = col(id,ncol)-n_mid;
  hist[vis] = id;
  
  // rotate 90 degrees
  int r_1 = -1*c;
  int c_1 = r;
  int id_1 = iden(r_1+n_mid,c_1+n_mid,ncol);
  hist1[vis] = id_1;  

  // rotate 180 degrees
  int r_2 = -1*r;
  int c_2 = -1*c;
  int id_2 = iden(r_2+n_mid,c_2+n_mid,ncol);
  hist2[vis] = id_2;  

  // rotate -90 degrees
  int r_3 = c;
  int c_3 = -1*r;
  int id_3 = iden(r_3+n_mid,c_3+n_mid,ncol);    
  hist3[vis] = id_3;

  vis = vis + 1;
  if (vis == ((nrow*ncol-1)/4)) {
    int q;
    int mid = 0;
    int n_mid = 0;

    // specifies that the central square be untouched
    for (q=0;q<vis;q++) {
      if (hist[q] == (ncol*nrow/2)+1) {
        mid = mid + 1;
      }
    }

    // specifies that tour must end adjacent to another
    if (hist[vis-1] != 34) {
      n_mid = 1;
    }

    if (mid == 0 & n_mid == 0) {
      tours = tours + 1;
      int e;
      printf("Sym tour\n");
     
      // prints all four paths at once 
      for (e=0;e<vis;e++) {
        printf("%d:%d:%d:%d\n",hist[e],hist1[e],hist2[e],hist3[e]);
      }
    }
  }
  else{
  
  // generate moves
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
  
  // next move can't be in any of the 4 paths
  for (l=0;l<8;l++) {
    for (i=0;i<vis;i++) {
      if (moves[l] == hist[i]) {
        moves[l] = 0;
      }
      if (moves[l] == hist1[i]) {
        moves[l] = 0;
      }
      if (moves[l] == hist2[i]) {
        moves[l] = 0;  
      }
      if (moves[l] == hist3[i]) {
        moves[l] = 0;
      }
    }
  }
  // unsure of parallelization
  //#pragma omp for private(k)  
  for (k=0;k<8;k++) {
    if (moves[k] != 0) {
      find_sym_tours(moves[k],nrow,ncol,vis,spaces,hist,hist1,hist2,hist3);
    }
  
  }  
  } 
  return tours;
}

int main (int argc,char *argv[]) {
  int m = 9;
  int n = 9;
  int i,j;
  int board[m*n];
  for (i=0;i<m*n;i++) {
      board[i] = i+1;
  }

  int hist[(m*n-1)/4];
  int hist1[(m*n-1)/4];
  int hist2[(m*n-1)/4];
  int hist3[(m*n-1)/4];
  int q,p,w;
  double tstart, tend, elapsed;
  tstart = omp_get_wtime();
  //#pragma omp parallel num_threads(8)
  
  p = find_sym_tours(65,m,n,0,board,hist,hist1,hist2,hist3);
  
  tend = omp_get_wtime();
  elapsed = tend-tstart;
  printf("Found %d tours\n",p);
  printf("Took %f seconds\n",elapsed/100);
  return 0;
}