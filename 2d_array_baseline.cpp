#include <iostream>
using namespace std;

int main()
{
    int a[10][10], b[10][10], mult[10][10], r1, c1, r2, c2, i, j, k;

    cout << "Enter rows and columns for first matrix: ";
    cin >> r1 >> c1;
    cout << "Enter rows and columns for second matrix: ";
    cin >> r2 >> c2;

    // If column of first matrix in not equal to row of second matrix,
    // ask the user to enter the size of matrix again.
    while (c1!=r2)
    {
        cout << "Error! column of first matrix not equal to row of second.";

        cout << "Enter rows and columns for first matrix: ";
        cin >> r1 >> c1;

        cout << "Enter rows and columns for second matrix: ";
        cin >> r2 >> c2;
    }

    // Storing elements of first matrix.
    cout << endl << "Enter elements of matrix 1:" << endl;
    for(i = 0; i < r1; ++i)
        for(j = 0; j < c1; ++j)
        {
            cout << "Enter element a" << i + 1 << j + 1 << " : ";
            cin >> a[i][j];
        }

    // Storing elements of second matrix.
    cout << endl << "Enter elements of matrix 2:" << endl;
    for(i = 0; i < r2; ++i)
        for(j = 0; j < c2; ++j)
        {
            cout << "Enter element b" << i + 1 << j + 1 << " : ";
            cin >> b[i][j];
        }

    // Initializing elements of matrix mult to 0.
    for(i = 0; i < r1; ++i)
        for(j = 0; j < c2; ++j)
        {
            mult[i][j]=0;
        }

    // Multiplying matrix a and b and storing in array mult.
    for(i = 0; i < r1; ++i)
        for(j = 0; j < c2; ++j)
            for(k = 0; k < c1; ++k)
            {
                mult[i][j] += a[i][k] * b[k][j];
            }

    // Displaying the multiplication of two matrix.
    cout << endl << "Output Matrix: " << endl;
    for(i = 0; i < r1; ++i)
    for(j = 0; j < c2; ++j)
    {
        cout << " " << mult[i][j];
        if(j == c2-1)
            cout << endl;
    }
    //Load A
	ifstream fin("higgs-twitter.mtx");
	//ifstream fin("test2.mtx");
	while (fin.peek() == '%') fin.ignore(2048, '\n');

	int M_A, K_A, L_A;
	fin >> M_A >> K_A >> L_A;

	//B as a normal Matrix
	cout << "start A as normal matrix construction\n";
	//int* V_B = new int[K_B][N_B];
	int V_A[M_A][K_A] = {0};
	for (int i = 0; i < L_B; i++) {
		int row_A, col_A, value_A;
		fin >> row_A >> col_A >> value_A;
		V_A[row_A][col_A] = value_A;
	}
	cout << "A matrix construction finished\n";
	fin.close();

    //Loading B
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
		V_B[row_B][col_B] = value_B;
	}
	cout << "B matrix construction finished\n";
	fin.close();

    int V_C[M_A][N_B] = {0};
    // Multiplying matrix a and b and storing in array mult.
	cout << "Baseline: start multiplcation" << endl;
	auto begin = chrono::high_resolution_clock::now();
    for(i = 0; i < M_A; ++i)
    {
        for(j = 0; j < N_B; ++j)
        {
            for(k = 0; k < K_A1; ++k)
            {
                V_C[i][j] += V_A[i][k] * V_B[k][j];
            }
        }
    }
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span= chrono::duration_cast<chrono::duration<double>>(end-begin);
	cout << "Baseline: It took me" << time_span.count() << "seconds.";
    return 0;
}