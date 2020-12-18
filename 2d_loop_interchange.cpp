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
	//B as a normal Matrix
	cout << "start B as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_B[K_B][N_B] = {0};
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B;
		double value_B;
		fin2 >> row_B >> col_B >> value_B;
		V_B[row_B][col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin2.close();

    double V_C[M_A][N_B] = {0};
    // Multiplying matrix a and b and storing in array mult.
	cout << "1d loop interchange: start multiplcation" << endl;
	auto begin = chrono::high_resolution_clock::now();
    for(int i = 0; i < M_A; ++i)
    {
        for(int k = 0; k < K_A; ++k)
        {
            for(int j = 0; j < N_B; ++j)
            {
                V_C[i][j] += V_A[i][k] * V_B[k][j];
            }
        }
    }
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span= chrono::duration_cast<chrono::duration<double>>(end-begin);
	cout << "2d loop interchange: It took me " << time_span.count() << " seconds.";
    return 0;
}