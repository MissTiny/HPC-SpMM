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
	ifstream fin("higgs-twitter.mtx");
	//ifstream fin("test2.mtx");
	while (fin.peek() == '%') fin.ignore(2048, '\n');

	int M_A, K_A, L_A;
	fin >> M_A >> K_A >> L_A;

	//matrix construct
	cout << "start A matrix construction\n";
	int* V_A = new int[L_A];
	int* ROW_INDEX_A = new int[L_A];
	int* COL_INDEX_A = new int[L_A];
	for (int i = 0; i < L_A; i++) {
		int row_A, col_A, value_A;
		fin >> row_A >> col_A >> value_A;
		ROW_INDEX_A[i] = row_A;
		COL_INDEX_A[i] = col_A;
		V_A[i] = value_A;
	}
	cout << "A matrix construction finished\n";
	fin.close();
	//analyse

	cout << "start A analyze\n";
	int* rowptr_A = new int[M_A + 1]; //record how many columns in this row
	int j_A = 0;
	for (int i = 0; i < M; i++) {
		rowptr_A[i] = j_A;
		while (ROW_INDEX_A[j_A] == i) {
			j_A++;
		}
	}
	rowptr_A[M_A] = j_A;
	cout << "analyze A finished\n";

	//Loading B Dense Matrix
	//B as a normal Matrix


	ifstream fin("higgs-twitter.mtx");
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
	int V_B[K_B][N_B] = {0};
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B, value_B;
		fin >> row_B >> col_B >> value_B;
		//ROW_INDEX_B[i] = row_B;
		//COL_INDEX_B[i] = col_B;
		V_B[row_B][col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin.close();
	/* B as a sparse matrix
	//matrix construct
	cout << "start B as CSR matrix construction\n";
	int* V_B = new int[L_B];
	int* ROW_INDEX_B = new int[L_B];
	int* COL_INDEX_B = new int[L_B];
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B, value_B;
		fin >> row_B >> col_B >> value_B;
		ROW_INDEX_B[i] = row_B;
		COL_INDEX_B[i] = col_B;
		V_B[i] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin.close();
	//analyse

	cout << "start B analyze\n";
	int* rowptr_B = new int[M_B + 1]; //record how many columns in this row
	int j_B = 0;
	for (int i = 0; i < M; i++) {
		rowptr_B[i] = j_B;
		while (ROW_INDEX_B[j_B] == i) {
			j_B++;
		}
	}
	rowptr_B[M_B] = j_B;
	cout << "analyze B finished\n";
	*/
	/*
	//dense vector construct
	cout << "start construct dense vector\n";
	int* Dense_Vector = new int[N];
	for (int i = 0; i < N; i++) {
		Dense_Vector[i] = 1;
	}
	cout << "dense vector finished" << endl;
	*/
	//result vector init -- without optimize
	//int* result = new int[L];
	/*
	int* V_C = new int[M_A * N_B];
	int* ROW_INDEX_C = new int[M_A * N_B];
	int* COL_INDEX_C = new int[M_A * N_B];

	cout << "CSR: start multiplcation" << endl;
	auto begin_CSR = chrono::high_resolution_clock::now();
	for (int i = 0; i < M_A; i++) {
		V_C[i] = 0.0;
		for (int k = rowptr_A[i]; k < rowptr_A[i + 1]; k++) {
			int column_A = COL_INDEX_A[k];
			int val_A = V_A[k];
			result1[i] += val1 * Dense_Vector[column1];

		}
	}
	*/
	// C generated as normal matrix
	int* V_C[M_A][N_B] = {0};

	cout << "CSR: start multiplcation" << endl;
	auto begin_CSR = chrono::high_resolution_clock::now();
	for (int i = 0; i < M_A; i++) {
		for (int k = rowptr_A[i]; k < rowptr_A[i + 1]; k++) {
			int column_A = COL_INDEX_A[k];
			int val_A = V_A[k];
			V_C[i][column_A] += val_A * V_B[column_A][i];

		}
	}

	auto end_CSR = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span_CSR = chrono::duration_cast<chrono::duration<double>>(end_CSR-begin_CSR);
	cout << "SpMM CSR: It took me" << time_span_CSR.count() << "seconds.";
	cout << endl;
}
