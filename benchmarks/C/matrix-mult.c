#include <stdio.h>
#include <stdlib.h>

double **mat_alloc(int n_row, int n_col)
{
	double **mat, *a;
	int i;
	a = (double*)calloc(n_row * n_col, sizeof(double));
	mat = (double**)malloc(n_row * sizeof(void*));
	for (i = 0; i < n_row; ++i)
		mat[i] = &a[i * n_col];
	return mat;
}

void mat_free(double **mat)
{
	free(mat[0]); free(mat);
}

double **mat_gen(int n)
{
	double **a, tmp = 1.0 / n / n;
	int i, j;
	a = mat_alloc(n, n);
	for (i = 0; i < n; ++i)
		for (j = 0; j < n; ++j)
			a[i][j] = tmp * (i - j) * (i + j);
	return a;
}

double **mat_mul(int n, int p, double **a, int m, double **b)
{
	double **c;
	int i, j, k;
	c = mat_alloc(n, m);
	for (i = 0; i < n; ++i)
		for (k = 0; k < p; ++k)
			for (j = 0; j < m; ++j)
				c[i][j] += a[i][k] * b[k][j];
	return c;
}

int benchmark(int arg)
{
  int n = arg;
  double **a, **b, **c;
	
  a = mat_gen(n);
  b = mat_gen(n);
  c = mat_mul(n, n, a, n, b);
  //printf("%f\n", c[n>>1][n>>1]);
  mat_free(c); mat_free(b); mat_free(a);
  return 0;
}
  

// BENCHMARKING:
#include <time.h>

int main()
{
  clock_t start_time, end_time;
  double execution_time;
  
  for (int i = 0; i < 500; i++)
    {
      start_time = clock();
      for (int iter = 0; iter < 100; iter++){
	benchmark(10);	
      }
      end_time = clock();
      
      execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
      
      printf("%f\n", execution_time*1000);
    }

  return 0;
}

