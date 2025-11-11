#include <stdio.h>
#define SIZE 32

int main() {
    float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    int i, j, k;

    // Initialize matrices
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
            A[i][j] = (float)(i + j + 1);
            B[i][j] = (float)(i - j + 2);
            C[i][j] = 0.0;
        }

    // Matrix multiplication
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            for (k = 0; k < SIZE; k++)
                C[i][j] += A[i][k] * B[k][j];

    // Print one element to prevent optimization away
    printf("C[0][0] = %f\n", C[0][0]);
    return 0;
}

