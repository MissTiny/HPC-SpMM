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
	//int M_A = 34;
	//int K_A = 512;
	//int L_A = 17408;
	cout<<M_A <<" "<<K_A<<" " <<L_A<<endl;
	//B as a normal Matrix
	cout << "start A as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_A[M_A*K_A] = {0};
    double last_A[K_A] = {0};
	for (int i = 0; i < L_A; i++) {
		int row_A, col_A;
		double value_A;
		fin1 >> row_A >> col_A >> value_A;
		//if (i==0){
		//cout<<row_A << " "<<col_A<<" " << value_A<<endl;
		//}
        if(i>=(M_A-1)*K_A){
            last_A[i-(M_A-1)*K_A] = value_A;
        }
		V_A[row_A*K_A + col_A] = value_A;
	}
	cout << "A matrix construction finished\n";
	fin1.close();

    //analyze A
    int line_notsame = 0;
    for (int i=0;i<M_A;i++){
        for (int k = 0; k<K_A; k++){
            if(V_A[i*K_A +k] !=last_A[k]){
                line_notsame++;
                break;
            }
        }
    }
    cout<<line_notsame;
    //Loading B
    ifstream fin2("data_mtx/B_sparse90.mtx");
	//ifstream fin("test2.mtx");
	//while (fin2.peek() == '%') fin2.ignore(2048, '\n');
	int K_B, N_B, L_B;
	fin2 >> K_B >> N_B >> L_B;

	//A SpMM Matrix Validation Check
	/*
	if (K_A != K_B){
		cout<<M_A<<" "<< K_A <<" "<<K_B<< " " << N_B <<endl;
		cout<< "Matrix A and Matrix B can not be multipled together";
		return 1;
	}
	*/
	//B as a normal Matrix
	cout << "start B as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	double V_B[K_B*N_B] = {0};
	for (int i = 0; i < L_B; i++) {
		int row_B, col_B;
		double value_B;
		fin2 >> row_B >> col_B >> value_B;
		//cout <<row_B <<" " <<col_B <<" " <<value_B<<endl;;
		V_B[row_B*N_B+ col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin2.close();
	
    double V_C[(line_notsame+1)*N_B] = {0};
    // Multiplying matrix a and b and storing in array mult.
	cout << "1d array Baseline: start multiplcation" << endl;
	auto begin = chrono::high_resolution_clock::now();
    for(int i = 0; i < (line_notsame+1); ++i)
    {
        for(int j = 0; j < N_B; ++j)
        {
            for( int k = 0; k < K_A; ++k)
            {
                V_C[i*N_B+j] += V_A[i*K_B+k] * V_B[k*N_B+j];
            }
        }
    }
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span= chrono::duration_cast<chrono::duration<double>>(end-begin);
	cout << "1d array Baseline: It took me " << time_span.count() << " seconds.";
    
	return 0;
}