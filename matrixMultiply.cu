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

__global__ void matrixMultiplyShared(int* a, int *b, int *c)
{
    int x = blockIdx.x;
    int y = blockIdx.y;
    int k = threadIdx.x; 
    __shared__ int p[col1];

    c[col2*y+x] = 0;
    p[k] = a[col1*y+k]*b[col2*k+x];

    __syncthreads();

    for(int i = 0; i < col1; i++)
    {
        c[col2*y+x] = c[col2*y+x]+p[i];
    }
}

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
    int *A = new int[row1*col1];
    int *B = new int[row2*col2];
    int *C = new int[row1*col2];
    int *res = new int[row1*col2];
    int *a_d, *b_d, *c_d;

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

    // Print C
    //printMatrix(C, row1, col2);
    
    
    cudaMalloc((void **) &a_d, row1*col1*sizeof(int));
    cudaMalloc((void **) &b_d, row2*col2*sizeof(int));
    cudaMalloc((void **) &c_d, row1*col2*sizeof(int));

    cudaMemcpy(a_d, A, row1*col1*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(b_d, B, row2*col2*sizeof(int), cudaMemcpyHostToDevice);

    dim3 grid(col2, row1);
	
    // Time start
    float gpuTimeTaken = 0.0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    
    // Call Kernel Function
    matrixMultiplyShared<<<grid, col1>>>(a_d, b_d, c_d);
    cudaMemcpy(res, c_d, row1*col2*sizeof(int), cudaMemcpyDeviceToHost);
    
    // Time end
    cudaThreadSynchronize();
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&gpuTimeTaken, start, stop);
    cout << "Time taken by GPU Matrix Multiplication in ms: " << gpuTimeTaken << endl;
   
    // CPU Matrix Multiplication
    matrixMultiplyCPU(A, B, C, row1, row2, col2);

    // Compare CPU Matrix Multiply with GPU Matrix Multiply    
    bool correct = false;
    for (int i = 0; i < row1; i++)
    {
	for (int j = 0; j < col2; j++)
	{
	    if (C[i*col2+j] == res[i*col2+j])
	    {
		correct = true;
	    }
	}
    }		
    if (correct)
    {
	cout << "CPU Matrix Multiplication is same as GPU Matrix Multiplication" << endl;
    }
    else
    {
	cout << "Results not the same" << endl;	
    }

    // Free Memory
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] res;
    cudaFree(a_d);
    cudaFree(b_d);
    cudaFree(c_d);

    return 0;
}
