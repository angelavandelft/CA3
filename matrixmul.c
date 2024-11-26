/*
 * Skeleton code for use with Computer Architecture 2024 assignment 3,
 * LIACS, Leiden University.
 */

#include "timing.h"

#include <immintrin.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <math.h>

/* The following constants are #defined in the makefile. */
#ifndef N_REPEAT
#  error "Need to define N_REPEAT in the Makefile"
#endif

/* Size of the matrix. We use N x N square matrices.
 */
#ifndef N
#  define N 512
#endif

/* The block size. 0 means no blocking.
 * Use this constant when you implement loop blocking.
 */
#if (BLOCK + 0)==0
#  undef BLOCK
#  define BLOCK 0
#endif

static inline int min(int a, int b) {
  return (a < b) ? a : b;
}

static void
matrix_clear(float matrix[N][N])
{
  for (size_t i = 0; i < N; ++i)
    for (size_t j = 0; j < N; ++j)
      matrix[i][j] = 0;
}

static void
matrix_init_identity(float matrix[N][N])
{
  matrix_clear(matrix);

  for (size_t i = 0; i < N; ++i)
    matrix[i][i] = 1.;
}

static void
matrix_init_random(float matrix[N][N])
{
  for (size_t i = 0; i < N; ++i)
    for (size_t j = 0; j < N; ++j)
      matrix[i][j] = rand() % 100;
}

void
matrix_dump(float matrix[N][N])
{
  for (size_t i = 0; i < N; ++i)
    {
      for (size_t j = 0; j < N; ++j)
        printf("%f ", matrix[i][j]);
      printf("\n");
    }
}

bool
matrix_equal(float A[N][N], float B[N][N])
{
  for (size_t i = 0; i < N; ++i)
    for (size_t j = 0; j < N; ++j)
      if (fabsf(A[i][j] - B[i][j]) > 0.000001)
        return false;

  return true;
}

#if BLOCK==0
// non-blocked multiply
static void
matrix_multiply(float C[N][N], const float A[N][N], const float B[N][N])
{
  for (size_t i = 0; i < N; ++i)
    for (size_t j = 0; j < N; ++j)
      {
        float row = 0.0;
        for (size_t k = 0; k < N; k++)
          row += A[i][k] * B[k][j];
        C[i][j] = row;
      }
}

#else
// blocked multiply
static void
matrix_multiply(float C[N][N], const float A[N][N], const float B[N][N])
{
  for (size_t jj = 0; jj < N; jj += BLOCK) {
    for (size_t kk = 0; kk < N; kk += BLOCK) {
      for (size_t i = 0; i < N; i++) {
        for (size_t j = jj; j < min(jj+BLOCK, N); j++) {
          float row = 0.0;
          for (size_t k = kk; k < min(kk+BLOCK, N); k++) {
            row += A[i][k] * B[k][j];
          }
          C[i][j] = row;
        }
      }
    }
  }
}

#endif

int
main(int argc, char **argv)
{
  /* We use this "magic" to allocate real two-dimensional arrays, that
   * are aligned at 64-bytes (cache line size).
   */
  float (*A)[N] = (float (*)[N])_mm_malloc(N * N * sizeof(float), 64);
  float (*B)[N] = (float (*)[N])_mm_malloc(N * N * sizeof(float), 64);
  float (*C)[N] = (float (*)[N])_mm_malloc(N * N * sizeof(float), 64);

  struct timespec init_start_time, init_end_time;
  get_time(&init_start_time);

  matrix_init_identity(A);
  matrix_init_random(B);

  get_time(&init_end_time);

  /* Perform the matrix multiplication */
  struct timespec compute_start_time, compute_end_time;
  get_time(&compute_start_time);

  for (int Z = 0; Z < N_REPEAT; ++Z)
    matrix_multiply(C, A, B);

  get_time(&compute_end_time);


// #ifdef ENABLE_TIMING
  /* Output result in CSV for easy processing */
  /* N,init time,n_repeat,runtime */
  printf("%d,%f,%d,%f\n",
         N,
         get_elapsed_ms(&init_end_time, &init_start_time),
         N_REPEAT,
         get_elapsed_ms(&compute_end_time, &compute_start_time));
// #endif /* ENABLE_TIMING */

#if 0
  /* For debugging */
  if (!matrix_equal(C, B))
    fprintf(stderr, "Matrices are not equal!!\n");
#endif

  _mm_free(A);
  _mm_free(B);
  _mm_free(C);

  return 0;
}