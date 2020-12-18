#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
using namespace std;

int main()
{

    //Load A
	ifstream fin1("data_mtx/A0_dense_float.mtx");
	//ifstream fin("test2.mtx");
	//while (fin.peek() == '%') fin.ignore(2048, '\n');

	int M_A, K_A, L_A;
	fin1 >> M_A >> K_A >> L_A;

	//B as a normal Matrix
	cout << "start A as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_A[M_A][K_A] = {0};
	for (int i = 0; i < L_A; i++) {
		int row_A, col_A;
		double value_A;
		fin1 >> row_A >> col_A >> value_A;
		V_A[row_A][col_A] = value_A;
	}
	cout << "A matrix construction finished\n";
	fin1.close();

    //Loading B
    ifstream fin("data_mtx/B_sparse90.mtx");
	//ifstream fin("test2.mtx");
	while (fin.peek() == '%') fin.ignore(2048, '\n');
	int K_B, N_B, L_B;
	fin >> K_B >> N_B >> L_B;

	//A SpMM Matrix Validation Check
	if (K_A != K_B){
		cout<< "Matrix A and Matrix B can not be multipled together";
		return 1;
	}
	//B as a normal Matrix
	cout << "start B as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_B[K_B][N_B] = {0};
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B;
		double value_B;
		fin >> row_B >> col_B >> value_B;
		V_B[row_B][col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin.close();

    double V_C[M_A][N_B] = {0};
    // Multiplying matrix a and b and storing in array mult.
    int bsize = 64;
    int en = bsize*(K_B/bsize);
	double sum;
	cout << "2d blocking: start multiplcation" << endl;
	auto begin = chrono::high_resolution_clock::now();
    for (int kk =0; kk<en; kk+=bsize){
        for(int jj=0;jj<en; jj+=bsize){
            for(int i=0;i<M_A;i++){
				for(int j=jj; j<jj+bsize;j++){
                	sum = V_C[i][j];
					for (int k=kk;k<kk+bsize;k++){
						sum+=V_A[i][k] * V_B[k][j];
					}
					V_C[i][j] = sum;
				}

            }
        }
    }
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span= chrono::duration_cast<chrono::duration<double>>(end-begin);
	cout << "2d blocking: It took me " << time_span.count() << " seconds.";
    return 0;
}