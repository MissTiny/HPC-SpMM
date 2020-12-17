#include <iostream>
#include<ctime>
#include<stdio.h>
using namespace std;


void CPU_matrix_multiplication(int *A, int *B, int *C, int N){
    for(int m=0;m<N;m++){
        for(int n=0;n<N;n++){
            for(int k=0;k<N;k++){
                C[m*N+n] +=A[m*N+k] * B[k*N+n];
            }
        }
    }
}


__global__ void CUDA_matrix_multiplication(int *A, int *B, int *C, int N){
    
    __shared__ int smem_c[64][64];
    __shared__ int smem_a[64][8];
    __shared__ int smem_b[8][64];

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
    //init established
    int N,size, memSize; // M for row. K reduction, N for column
    N=4096;
    size = N*N;
    memSize = size*sizeof(int);

    int *A = new int[size];
    int *B = new int[size];
    int *C_1 = new int[size];
    int *C_2 = new int[size];
    int *d_A;
    int *d_B;
    int *d_C;
    //random init A
    for (int m=0; m<N;m++){
        for(int k=0; k<N; k++){
            A[m*N+k] = m+1;
        }
    }

    //random init B
    for (int k=0; k<N;k++){
        for (int n=0;n<N;n++){
            B[k*N+n] = k+1;
        }
    }

    for (int m=0; m<N;m++){
        for (int n=0;n<N;n++){
            C_1[m*N+n] = 0;
            C_2[m*N+n] = 0;
        }
    }
    cout << "A: "<<N<<" x " <<N <<", B: " <<N<<" x " <<N <<", C: "<<N<<" x " <<N<<endl;
    cout << "CPU Multiplication: Start" <<endl;
    
    CPU_matrix_multiplication(A,B,C_1,N);
    cout<<"CPU Multiplication: Finish"<<endl;
    cudaMalloc((void **) &d_A, memSize);
    cudaMemcpy(d_A, A,memSize,cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_B, memSize);
    cudaMemcpy(d_B, B,memSize,cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_C,memSize);
    cudaMemcpy(d_C, C_2,memSize,cudaMemcpyHostToDevice);

    //launch kernel

    dim3 nblocks((N+64-1)/64,(N+64-1)/64);
    dim3 nthreads(32,32);
    cout << "CUDA Multiplication: Start" <<endl;
    float time = 0.0;
    cudaEvent_t begin,end;
    cudaEventCreate(&begin);
    cudaEventCreate(&end);
    cudaEventRecord(begin,0);
    CUDA_matrix_multiplication<<<nblocks,nthreads>>>(d_A, d_B, d_C,N);
    cudaMemcpy(C_2,d_C,memSize,cudaMemcpyDeviceToHost);
    cudaThreadSynchronize();
    cudaEventRecord(end,0);
    cudaEventSynchronize(end);
    cudaEventElapsedTime(&time,begin,end);
    cout << "CUDA Multiplication: It took me " << time << " seonds." <<endl;
   
    bool error = false;
    for (int i=0; i<size;i++){
	
        if(C_1[i] != C_2[i]){
            error = true;
            cout<<"Error: the cuda is not correct at i= ["<< i <<"] place" <<endl;
        }
	
    }
    if(!error){
        cout<<"Correct: the cuda result is the same as CPU result"<<endl;
    }

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
