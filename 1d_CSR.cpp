#include<iostream>
#include<fstream>
#include<algorithm>
#include<ctime>
#include<chrono>
#include<ratio>
using namespace std;


int main() { 
	//CSR

	//Loading A sparse matrix
	ifstream fin1("data_mtx/A0_dense_float.mtx");
	//ifstream fin("test2.mtx");
	//while (fin.peek() == '%') fin.ignore(2048, '\n');

	int M_A, K_A, L_A;
	fin1 >> M_A >> K_A >> L_A;


	//B as a normal Matrix
	cout << "start A as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_A[M_A*K_A] = {0};
	for (int i = 0; i < L_A; i++) {
		int row_A, col_A;
		double value_A;
		fin1 >> row_A >> col_A >> value_A;
		//ROW_INDEX_B[i] = row_B;
		//COL_INDEX_B[i] = col_B;
		V_A[row_A * K_A + col_A] = value_A;
	}
	cout << "A matrix construction finished\n";
	fin1.close();
	//matrix construct



	//Loading B Sparse Matrix
	ifstream fin2("data_mtx/B_sparse90.mtx");
	//ifstream fin("test2.mtx");
	//while (fin.peek() == '%') fin.ignore(2048, '\n');
	int K_B, N_B, L_B;
	fin2 >> K_B >> N_B >> L_B;

	//A SpMM Matrix Validation Check
	if (K_A != K_B){
		cout<< "Matrix A and Matrix B can not be multipled together";
		return 1;
	}
	cout << "start B matrix construction\n";
	double* V_B = new double[L_B];
	int* ROW_INDEX_B = new int[L_B];
	int* COL_INDEX_B = new int[L_B];
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B;
		double value_B;
		fin2 >> row_B >> col_B >> value_B;
		ROW_INDEX_B[i] = row_B;
		COL_INDEX_B[i] = col_B;
		V_B[i] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin2.close();
	//analyse

	cout << "start B analyze\n";
	int* colptr_B = new int[N_B + 1]; //record how many columns in this row
	int j_B = 0;
	for (int j = 0; j < N_B; j++) {
		colptr_B[j] = j_B;
		while (COL_INDEX_B[j_B] == j) {
			j_B++;
		}
	}
	colptr_B[N_B] = j_B;
	cout << "analyze B finished\n";


	double V_C[M_A*N_B] = {0};

	cout << "CSR: start multiplcation" << endl;
	auto begin_CSR = chrono::high_resolution_clock::now();
	for(int i=0; i<M_A;i++){
		for (int j = 0; j < N_B; j++) {
			for (int k = colptr_B[j]; k < colptr_B[j + 1]; k++) {
				int row_B = ROW_INDEX_B[k];
				double val_B = V_B[k];
				V_C[i*N_B + j] += V_A[i*K_A + row_B] *val_B;
			}
		}
	}


	auto end_CSR = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span_CSR = chrono::duration_cast<chrono::duration<double>>(end_CSR-begin_CSR);
	cout << "2d CSR: It took me " << time_span_CSR.count() << " seconds.";
	cout << endl;   
}
