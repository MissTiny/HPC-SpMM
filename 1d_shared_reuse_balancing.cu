#include <iostream>
#include<ctime>
#include<stdio.h>
#include<fstream>
using namespace std;


__global__ void CUDA_matrix_multiplication(double *A, double *B, double *C, int N){
    
    __shared__ double smem_c[64][64];
    __shared__ double smem_a[64][8];
    __shared__ double smem_b[8][64];

    int c = blockIdx.x * 64;
    int r = blockIdx.y *64;
    smem_c[threadIdx.y*2][threadIdx.x*2] =0;
    smem_c[threadIdx.y*2][threadIdx.x*2+1]=0;
    smem_c[threadIdx.y*2+1][threadIdx.x*2]=0;
    smem_c[threadIdx.y*2+1][threadIdx.x*2+1]=0;
    for (int kk=0;kk<N;kk+=8){
        for (int i=threadIdx.x+blockDim.x*threadIdx.y; i<64*8; i+=blockDim.x*blockDim.y){
            int k = kk+i/64;
            int rt = r + i%64;
            int ct = c + i%64;
            smem_a[i%64][i/64] = A[rt*N+k];
            smem_b[i/64][i%64] = B[k*N+ct];
        }
    
        __syncthreads();
        	
        for (int n=0; n<(8);n++){
            smem_c[threadIdx.y*2][threadIdx.x*2] += smem_a[threadIdx.y*2][n] * smem_b[n][threadIdx.x*2];
            smem_c[threadIdx.y*2][threadIdx.x*2+1] += smem_a[threadIdx.y*2][n] * smem_b[n][threadIdx.x*2+1];
            smem_c[threadIdx.y*2+1][threadIdx.x*2] += smem_a[threadIdx.y*2+1][n] * smem_b[n][threadIdx.x*2];
            smem_c[threadIdx.y*2+1][threadIdx.x*2+1] += smem_a[threadIdx.y*2+1][n] * smem_b[n][threadIdx.x*2+1];
        }

        __syncthreads();
    }

    //if ((c+threadIdx.x) < N && (r+threadIdx.y)<N){
        C[((blockIdx.y*64 + threadIdx.y*2)*N) + (blockIdx.x * 64)+threadIdx.x*2] = smem_c[threadIdx.y*2][threadIdx.x*2];
        C[((blockIdx.y*64 + threadIdx.y*2+1)*N) + (blockIdx.x * 64)+threadIdx.x*2] = smem_c[threadIdx.y*2+1][threadIdx.x*2];
        C[((blockIdx.y*64 + threadIdx.y*2)*N) + (blockIdx.x * 64)+threadIdx.x*2+1] = smem_c[threadIdx.y*2][threadIdx.x*2+1];
        C[((blockIdx.y*64 + threadIdx.y*2+1)*N) + (blockIdx.x * 64)+threadIdx.x*2+1] = smem_c[threadIdx.y*2+1][threadIdx.x*2+1];
    //}




}


int main()
{
    double *d_A;
    double *d_B;
    double *d_C;
	//Loading A sparse matrix
	ifstream fin1("data_mtx/A0_dense_float.mtx");
	//ifstream fin("test2.mtx");
	//while (fin.peek() == '%') fin.ignore(2048, '\n');

	int M_A, K_A, L_A;
	fin1 >> M_A >> K_A >> L_A;

    int line_notsame = 12;
    int size_A = (line_notsame+1)*K_A;
    int memSize_A = size_A *sizeof(double);
	//B as a normal Matrix
	cout << "start A as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
    double* V_A = new double[M_A *K_A];
    double* new_A = new double[(line_notsame+1)*K_A];
	for (int i = 0; i < L_A; i++) {
		int row_A, col_A;
		double value_A;
		fin1 >> row_A >> col_A >> value_A;
		//ROW_INDEX_B[i] = row_B;
		//COL_INDEX_B[i] = col_B;
        V_A[row_A * K_A + col_A] = value_A;
        if(row_A<line_notsame){
            new_A[row_A * K_A + col_A] = value_A;
        }
	}
	cout << "A matrix construction finished\n";
	fin1.close();
	//matrix construct
    //Loading B
    ifstream fin2("data_mtx/B_sparse90_loadbalancing.mtx");
	//ifstream fin("test2.mtx");
	//while (fin.peek() == '%') fin.ignore(2048, '\n');
	int K_B, N_B, L_B;
	fin2 >> K_B >> N_B >> L_B;
    int size_B = K_B*N_B;
    int memSize_B = size_B *sizeof(double);
	//A SpMM Matrix Validation Check
	if (K_A != K_B){
		cout<< "Matrix A and Matrix B can not be multipled together";
		return 1;
	}
	//B as a normal Matrix
	cout << "start B as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double* V_B = new double[K_B*N_B];
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B;
		double value_B;
		fin2 >> row_B >> col_B >> value_B;
		V_B[row_B*N_B+ col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin2.close();

    int size_C = (line_notsame+1)*N_B;
    int memSize_C = size_C *sizeof(double);
    double* V_C = new double[(line_notsame+1)*N_B];
    for (int m=0; m<(line_notsame+1);m++){
        for (int n=0;n<N_B;n++){
            V_C[m*N_B+n] = 0;
        }
    }
    cudaMalloc((void **) &d_A, memSize_A);
    cudaMemcpy(d_A, new_A,memSize_A,cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_B, memSize_B);
    cudaMemcpy(d_B, V_B,memSize_B,cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_C,memSize_C);
    cudaMemcpy(d_C, V_C,memSize_C,cudaMemcpyHostToDevice);

    //launch kernel

    dim3 nblocks((M_A+64-1)/64,(N_B+64-1)/64);
    dim3 nthreads(32,32);
    cout << "CUDA Multiplication: Start" <<endl;
    float time = 0.0;
    cudaEvent_t begin,end;
    cudaEventCreate(&begin);
    cudaEventCreate(&end);
    cudaEventRecord(begin,0);
    CUDA_matrix_multiplication<<<nblocks,nthreads>>>(d_A, d_B, d_C,K_A);
    cudaMemcpy(V_C,d_C,memSize_C,cudaMemcpyDeviceToHost);
    cudaThreadSynchronize();
    cudaEventRecord(end,0);
    cudaEventSynchronize(end);
    cudaEventElapsedTime(&time,begin,end);
    cout << "CUDA Multiplication: It took me " << time << " milliseconds." <<endl;


    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
