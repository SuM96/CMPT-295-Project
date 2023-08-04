#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * A struct containing properties of a matrix
 * The properties are:
 * matrix: the matrix itself. Usually dynamically allocated
 * rows: the number of rows in the matrix
 * cols: the number of columns in the matrix
*/
typedef struct {
    int* matrix;
    unsigned int size;
} Matrix;

/*
 * This function creates a random array of size SIZE x SIZE
 * The matrix is dynamically allocated, so it must be freed
 * Arguements:
 *      SIZE: The size of a square matrix
 * Returns:
 *      randMatrix: The square matrix containing random integers
*/
Matrix createRandMatrix(const unsigned int SIZE) {
    // Create a random matrix
    Matrix randMatrix;
    randMatrix.matrix = malloc(SIZE * SIZE * sizeof(int));
    // Use a for-loop to fill an array of random values
    for(int i = 0; i < SIZE * SIZE; i++) {
        *(randMatrix.matrix + i) = rand() % 10;
    }
    // Set the size for the square matrix
    randMatrix.size = SIZE;
    return randMatrix;
}

/*
 * This function creates a square identity matrix depending on the size
 * The identity matrix is dynamically allocated so it must be freed.
 * Arguements:
 *      SIZE: size of the square identity matrix
 * Returns:
 *      identity: An idenity matrix of size SIZE x SIZE. The idenity matrix is dynamically allocated, so therefore it must be freed.
*/
Matrix createIdentityMatrix(const unsigned int SIZE) {
    // Create a matrix for the idenity matrix
    Matrix identity;
    identity.matrix = calloc(SIZE * SIZE, sizeof(int));
    // Fill the array with 1s at the (i,i) 
    for(int i = 0; i < SIZE; i++) {
        *(identity.matrix + i * SIZE + i) = 1;
    }
    // Set the size of the idenity matrix
    identity.size = SIZE;
    return identity;
}

/*
 * This function prints the matrix. It prints it row and column wize separated by a space
 * Arguments: 
 *      Matrix: The matrix needed to be printed
 * Returns:
 *      (Nothing)
*/
void printMatrix(Matrix matrix) {
    // This double for-loop seemed to be more easier, however it can be condensed to one for-loop
    for(int i = 0; i < matrix.size; i++) {
        for(int j = 0; j < matrix.size; j++) {
            printf("%d ", *matrix.matrix++);
        }
        printf("\n");
    }
}

/*
 * This function will copy the values from one array to another array.
 * Arguments:
 *      src: The array we are copying from
 *      dst: The array we want the values to be copied to
 *      size: The size of the array for src and dst
 * Returns: 
 *      (Nothing, but dst array will equal to src array after execution)
*/
void copyArray(int* src, int* dst, const int size) {
    // Use this for-loop to copy everything from src to dst
    for(int i = 0; i < size; i++) {
        *(dst + i) = *(src + i);
    }
}

/*
 * This function will compute the dot product of two arrays
 * Arguments: 
 *      arr1: array to be dot product
 *      arr2: array to be dot product
 *      n: Size
 * Return:
 *      dot: dot product of arr1, arr2
*/
int dotProduct(const int* arr_One, const int* arr_Two, const unsigned int n) {
    int dot = 0;
    for(int i = 0; i < n; i++) {
        dot += *(arr_One + i) * *(arr_Two + i);
    }
    return dot;
}

/* 
 * This function will transpose the matrix. The function here needs to be passed by reference since passing by value will cause it to not work
 * Arguements:
 *      matrix: The matrix that will be transposed after calling this function.
 * Returns:
 *      (Nothing, however the original matrix will be transposed after calling this function)
*/
void transpose(Matrix* matrix) {
    for(unsigned int i = 0; i < matrix->size; i++) {
        for(unsigned int j = i; j < matrix->size; j++) {
            // These three lines basically swaps values from matrix(i,j) and matrix(j,i)
            int temp = *(matrix->matrix + i * matrix->size + j);
            *(matrix->matrix + i * matrix->size + j) = *(matrix->matrix + j * matrix->size + i);
            *(matrix->matrix + j * matrix->size + i) = temp;
        }
    }
}

/*
 * This function takes a matrix and returns a vector containing a Matrix, Vector multiplication.
 * Arguments: 
 *      Matrix: The matrix needed for matrix multiplication
 *      arr: This represents the vector
 * Returns: 
 *      result: An array containing the result of matrix vector multiplication. The result is mallocced and needs to be freed.
*/
int* MatrixVectorMulti(Matrix matrix, int* arr) {
    // Allocate an array for the result
    int* result = malloc(matrix.size * sizeof(int));
    // Use the dot product to determine matrix-vector multiplication
    for(unsigned int i = 0; i < matrix.size; i++) {
        *(result + i) = dotProduct(matrix.matrix + (i * matrix.size), arr, matrix.size);
    }
    return result;
}

/*
 * This function will multiply 2 square matrices together and return the result as a matrix. 
 * This method is also not cache efficiant as we will need to access columns of the result and B
 * Arguments:
 *      A: A matrix
 *      B: Another matrix
 * Returns:
 *      result: A matrix consisting of 2 matrices, A and B, multiplied together and the size of the multiplied matrix.
*/
Matrix matrixMultiplyNaive(Matrix A, Matrix B) {
    // Multiply 2 square matrices of the same size result into another square matrix of the same size
    Matrix result;
    result.matrix = malloc(sizeof(int) * A.size * A.size);
    result.size = A.size;
    for(unsigned int i = 0; i < B.size; i++) {
        // Allocate memory for columns of B
        int* BCol = malloc(sizeof(int) * B.size);
        // Copy the ith column and store it into an array called BCol
        for(unsigned int j = 0; j < B.size; j++) {
            *(BCol + j) = *(B.matrix + j * B.size + i);
        }
        // Compute the Matrix-vector multiplication to determine row i
        int* iMatrixVector = MatrixVectorMulti(A, BCol);
        // Copy the result to the columns i
        for(unsigned int j = 0; j < B.size; j++) {
            *(result.matrix + j * B.size + i) = *(iMatrixVector + j); 
        }
        // Free all dynamically allocated memory
        free(BCol);
        free(iMatrixVector);
    }
    return result;
}

/*
 * This function will do a cache friendly matrix multiplication.
 * Arguments:
 *      A: A matrix
 *      B: Another matrix
 * Returns:
 *      result: A matrix consisting of 2 matrices, A and B, multiplied together and the size of the multiplied matrix.
*/
Matrix matrixMultiplyCacheFriendly(Matrix A, Matrix B) {
    Matrix result;
    result.matrix = malloc(sizeof(int) * A.size * A.size);
    result.size = A.size;
    transpose(&B);
    for(int i = 0; i < B.size; i++) {
        int* iMatrixVector = MatrixVectorMulti(A, B.matrix + i * B.size);
        copyArray(iMatrixVector, result.matrix + i * B.size, B.size);
        free(iMatrixVector);
    }
    transpose(&result);
    // Remove the next line during analysis
    transpose(&B);
    return result;
}

/*
 * This function will free memory allocated in M.matrix and set M.size to be 0
 * Arguments:
 *      M: A matrix
 * Returns:
 *      (Nothing, but it will free the matrix M)
*/
void freeMatrix(Matrix *M) {
    free(M->matrix);
    M->size = 0;
}

int main(int argc, char *argv[]) {
    const int sizeMatrix = atoi(argv[1]);
    // There will be only 2 choices.
    // 1: Naive Method
    // 2: Cache Friendly
    const int choice = atoi(argv[2]);
    if(choice != 1 && choice != 2) {
        printf("Please enter either 1 for Cache Friendly Method or 2 for Naive Method.\n");
        return 0;
    }
    // For crummy random number generator
    srand(time(NULL));
    Matrix matrixOne = createRandMatrix(sizeMatrix);
    Matrix matrixTwo = createRandMatrix(sizeMatrix);
    // printf("Matrix 1:\n");
    // printMatrix(matrixOne);
    // printf("Matrix 2:\n");
    // printMatrix(matrixTwo);
    // Cache efficiant
    if(choice == 1) { 
        Matrix multipliedCF = matrixMultiplyCacheFriendly(matrixOne, matrixTwo);
        printf("Matrix Multiplied by Cache Friendly Method:\n");
        // printMatrix(multipliedCF);
        freeMatrix(&multipliedCF);
    }
    // Naive Method
    else { 
        Matrix multipliedNaive = matrixMultiplyNaive(matrixOne, matrixTwo);
        printf("Matrix Multipled by Naive Method:\n");
        // printMatrix(multipliedNaive);
        freeMatrix(&multipliedNaive);
    }
    freeMatrix(&matrixOne);
    freeMatrix(&matrixTwo);
    return 0;
}