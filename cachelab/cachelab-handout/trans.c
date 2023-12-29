/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#define bsize 8

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

char transpose_test_1[] = "test 4 64X64 with tile 4X4 to solve mapping conflict";
void transpose_4regs(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    int tmp0, tmp1, tmp2, tmp3;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j+=4) {
            tmp0 = A[i][j];
            tmp1 = A[i][j+1];
            tmp2 = A[i][j+2];
            tmp3 = A[i][j+3];

            B[j][i] = tmp0;
            B[j+1][i] = tmp1;
            B[j+2][i] = tmp2;
            B[j+3][i] = tmp3;
        }
    }    
}

char transpose_test_4[] = "test 4 for 64*64";
void transpose_prefetch(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tile_i;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i+=8) {
        for (j = 0; j < M; j+=8) {
            tmp0 = A[i+0][j];
            tmp1 = A[i+1][j];
            tmp2 = A[i+2][j];
            tmp3 = A[i+3][j];
            tmp4 = A[i+4][j];
            tmp5 = A[i+5][j];
            tmp6 = A[i+6][j];
            tmp7 = A[i+7][j];
            for(tile_i=0; tile_i<8; tile_i++){
                tmp0 = A[i+tile_i][j];
                tmp1 = A[i+tile_i][j+1];
                tmp2 = A[i+tile_i][j+2];
                tmp3 = A[i+tile_i][j+3];
                tmp4 = A[i+tile_i][j+4];
                tmp5 = A[i+tile_i][j+5];
                tmp6 = A[i+tile_i][j+6];
                tmp7 = A[i+tile_i][j+7];

                B[j][i+tile_i] = tmp0;
                B[j+1][i+tile_i] = tmp1;
                B[j+2][i+tile_i] = tmp2;
                B[j+3][i+tile_i] = tmp3;
                B[j+4][i+tile_i] = tmp4;
                B[j+5][i+tile_i] = tmp5;
                B[j+6][i+tile_i] = tmp6;
                B[j+7][i+tile_i] = tmp7;
            }
            tile_i = 0;
        }
    }    
}

char transpose_test_3[] = "32X32 solution";
void transpose_8X8tile_32X32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tile_i;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (j = 0; j < N; j+=8) {
        for (i = 0; i < M; i+=8) {
            for(tile_i=0; tile_i<8; tile_i++){
                tmp0 = A[i+tile_i][j];
                tmp1 = A[i+tile_i][j+1];
                tmp2 = A[i+tile_i][j+2];
                tmp3 = A[i+tile_i][j+3];
                tmp4 = A[i+tile_i][j+4];
                tmp5 = A[i+tile_i][j+5];
                tmp6 = A[i+tile_i][j+6];
                tmp7 = A[i+tile_i][j+7];

                B[j][i+tile_i] = tmp0;
                B[j+1][i+tile_i] = tmp1;
                B[j+2][i+tile_i] = tmp2;
                B[j+3][i+tile_i] = tmp3;
                B[j+4][i+tile_i] = tmp4;
                B[j+5][i+tile_i] = tmp5;
                B[j+6][i+tile_i] = tmp6;
                B[j+7][i+tile_i] = tmp7;
            }
            tile_i = 0;
        }
    }    
}

char transpose_test_5[] = "test 5 solution for 64*64 using 4*4tile";
void transpose_4X4tile_64X64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tile_i;
    int tmp0, tmp1, tmp2, tmp3;

    for (j = 0; j < N; j+=4) {
        for (i = 0; i < M; i+=4) {
            for(tile_i=0; tile_i<4; tile_i++){
                tmp0 = A[i+tile_i][j];
                tmp1 = A[i+tile_i][j+1];
                tmp2 = A[i+tile_i][j+2];
                tmp3 = A[i+tile_i][j+3];

                B[j][i+tile_i] = tmp0;
                B[j+1][i+tile_i] = tmp1;
                B[j+2][i+tile_i] = tmp2;
                B[j+3][i+tile_i] = tmp3;
            }
            tile_i = 0;
        }
    }    
}

char transpose_test_7[] = "64X64 solution with magic";
void transpose_8X8tile_64X64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tile_i;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (j = 0; j < N; j+=8) {
        for (i = 0; i < M; i+=8) {
            for(tile_i=0; tile_i<4; tile_i++){
                tmp0 = A[i+tile_i][j];
                tmp1 = A[i+tile_i][j+1];
                tmp2 = A[i+tile_i][j+2];
                tmp3 = A[i+tile_i][j+3];
                tmp4 = A[i+tile_i][j+4];
                tmp5 = A[i+tile_i][j+5];
                tmp6 = A[i+tile_i][j+6];
                tmp7 = A[i+tile_i][j+7];

                B[j][i+tile_i] = tmp0;
                B[j+1][i+tile_i] = tmp1;
                B[j+2][i+tile_i] = tmp2;
                B[j+3][i+tile_i] = tmp3;
                B[j+0][i+tile_i+4] = tmp4;
                B[j+1][i+tile_i+4] = tmp5;
                B[j+2][i+tile_i+4] = tmp6;
                B[j+3][i+tile_i+4] = tmp7;
            }
            tile_i = 0;
            for(tile_i=0; tile_i<4; tile_i++){
                tmp0 = A[i+tile_i+4][j];
                tmp1 = A[i+tile_i+4][j+1];
                tmp2 = A[i+tile_i+4][j+2];
                tmp3 = A[i+tile_i+4][j+3];
                tmp4 = B[j+0][i+tile_i+4];
                tmp5 = B[j+1][i+tile_i+4];
                tmp6 = B[j+2][i+tile_i+4];
                tmp7 = B[j+3][i+tile_i+4];

                B[j+0][i+4+tile_i] = tmp0;
                B[j+1][i+4+tile_i] = tmp1;
                B[j+2][i+4+tile_i] = tmp2;
                B[j+3][i+4+tile_i] = tmp3;
                B[j+4+tile_i][i+0] = tmp4;
                B[j+4+tile_i][i+1] = tmp5;
                B[j+4+tile_i][i+2] = tmp6;
                B[j+4+tile_i][i+3] = tmp7;
            }
            tile_i = 0;
            for(tile_i=0; tile_i<4; tile_i++){
                tmp0 = A[i+4+tile_i][j+4+0];
                tmp1 = A[i+4+tile_i][j+4+1];
                tmp2 = A[i+4+tile_i][j+4+2];
                tmp3 = A[i+4+tile_i][j+4+3];
               
                B[j+4+0][i+4+tile_i] = tmp0;
                B[j+4+1][i+4+tile_i] = tmp1;
                B[j+4+2][i+4+tile_i] = tmp2;
                B[j+4+3][i+4+tile_i] = tmp3;

            }
            tile_i = 0;
        }
    }    
}


char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tile_i;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i+=8) {
        for (j = 0; j < M; j+=8) {
            for(tile_i=0; tile_i<8; tile_i++){
                tmp0 = A[i+tile_i][j];
                tmp1 = A[i+tile_i][j+1];
                tmp2 = A[i+tile_i][j+2];
                tmp3 = A[i+tile_i][j+3];
                tmp4 = A[i+tile_i][j+4];
                tmp5 = A[i+tile_i][j+5];
                tmp6 = A[i+tile_i][j+6];
                tmp7 = A[i+tile_i][j+7];

                B[j][i+tile_i] = tmp0;
                B[j+1][i+tile_i] = tmp1;
                B[j+2][i+tile_i] = tmp2;
                B[j+3][i+tile_i] = tmp3;
                B[j+4][i+tile_i] = tmp4;
                B[j+5][i+tile_i] = tmp5;
                B[j+6][i+tile_i] = tmp6;
                B[j+7][i+tile_i] = tmp7;
            }
            tile_i = 0;
        }
    }    
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    // 可以在这里注册很多个函数，只要记得住名称
    // registerTransFunction(transpose_4X4tile, transpose_test_2); 
    registerTransFunction(transpose_8X8tile_64X64, transpose_test_7); 

    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

