/*
The following program performs Matrix multiplication: C = A * B where:
A and B can have different dimensions as long as the numCol(A) is same as numRum(B)
A is M by N and B is N by K

M = row1 = numRow(A)
N = col1 = numCol(A)
N = row2 = numRow(B)
K = col2 = numCol(B)

Performing C = A*B for 2D matricies

Note:
int** ary = new int[sizeY][sizeX]

Can be: 
int *ary = new int[sizeX*sizeY];

Then ary[i][j] is rewritten as:
ary[i*sizeY+j]
*/

#include <iostream>
using namespace std;

// Number of rows for matrix A
#define row1 64

// Number of cols for matrix A
#define col1 8

// Number of rows for matrix B
#define row2 8

// Number of cols for matrix B
#define col2 64

/* CPU Matrix Multiply 
C[i][j] += A[i][k] * B[k][j];
C[i*z+j] += A[i*y+k] * B[k*z+j];
Non-blocked
Single Threaded
*/ 
void matrixMultiplyCPU(int *A, int *B, int *C, int x, int y, int z)
{
    // CPU Matrix Multiplication
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < z; j++)
        {
            for (int k = 0; k < y; k++)
            {
                C[i*z+j] += A[i*y+k] * B[k*z+j];
                //C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Function to print 2D Matrix
void printMatrix(int *A, int numRow, int numCol)
{
    for (int i = 0; i < numRow; i++) 
    {
        for (int j = 0; j < numCol; j++) 
        {
            cout << A[i*numCol+j] << " ";
        }
        cout << endl;
    }
}

int main()
{

    // Read data for Sparse matrix A from CSV file, 
    // Then we will have row1 and col1

    
    // Allocate memory for A, B and C
    int *A = new int[row1*col1];
    int *B = new int[row2*col2];
    int *C = new int[row1*col2];

    // Load A with data
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col1; j++) {
            //A[i][j] = i+1;
            A[i*col1+j] = i+1;
        }
    }

    // Print A
    //printMatrix(A, row1, row2);
   
    // Load B with data
    for (int i = 0; i < row2; i++) {
        for (int j = 0; j < col2; j++) {
            // B[i][j] = j+1;
            B[i*col2+j] = j+1;
        }
    }

    // Print B
    //printMatrix(B, row2, col2);

    // Load C with data
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            //C[i][j] = 0;
            C[i*col2+j] = 0;
        }
    }

    // CPU Matrix Multiplication
    matrixMultiplyCPU(A, B, C, row1, row2, col2);

    // Print C
    printMatrix(C, row1, col2);

    // Free Memory
    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
