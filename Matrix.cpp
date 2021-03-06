#include "Matrix.h"
using namespace std;

MATRIX::MATRIX() : row(0), column(0), data(NULL) {}

MATRIX::MATRIX(int row, int column, double *data) : row(row), column(column){
	this->data = new double[row * column];
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			this->data[i * column + j] = data[i * column + j];
}

MATRIX::MATRIX(const MATRIX& b) : row(b.row), column(b.column){
	this->data = new double[row * column];
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			this->data[i * column + j] = b.data[i * column + j];
}

MATRIX::~MATRIX(){
	delete[] data;
}

void MATRIX::print() const{
	for(int i = 0; i < row; ++i){
		for(int j = 0; j < column; ++j)
			cout << data[i * column + j] << "	";
		cout << endl;
	}
	cout << endl;
}

MATRIX& MATRIX::operator=(MATRIX& b){
	row = b.row, column = b.column;
	delete[] data;
	this->data = new double[row * column];
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			this->data[i * column + j] = b.data[i * column + j];
	return *this;
}

MATRIX MATRIX::add(MATRIX b){
	if(b.row != row || b.column != column)
		throw "Matrix dimension mismatch error";
	double c[row * column];
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			c[i * column + j] = data[i * column + j] + b.data[i * column + j];
	return MATRIX(row, column, c);
}

MATRIX MATRIX::subtract(MATRIX b){
	if(b.row != row || b.column != column)
		throw "Matrix dimension mismatch error";
	double c[row * column];
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			c[i * column + j] = data[i * column + j] - b.data[i * column + j];
	return MATRIX(row, column, c);
}

MATRIX MATRIX::multiply(MATRIX b){
	if(column != b.row)
		throw "Matrix dimension mismatch error";
	double c[row * b.column] = {};
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < b.column; ++j)
			for(int k = 0; k < column; ++k)
				c[i * b.column + j] += data[i * column + k] * b.data[k * b.column + j];
	return MATRIX(row, b.column, c);
}

MATRIX MATRIX::rightDivide(MATRIX b){
	if(column != b.column)
		throw "Matrix dimension mismatch error";
	if(b.determinant() == 0)
		throw "Singular matrix";
	//X = A/b <==> X = A*inv(b)
	return this->multiply(b.inverse());
}

MATRIX MATRIX::leftDivide(MATRIX b){
	if(row != b.row)
		throw "Matrix dimension mismatch error";
	if(this->determinant() == 0)
		throw "Singular matrix";
	double a[row * column], bb[b.row], c[column * b.column];
	for(int i = 0; i < b.column; ++i){//work out the ith column of X
		//copy A.data to a
		for(int j = 0; j < row; ++j)
			for(int k = 0; k < column; ++k)
				a[j * column + k] = data[j * column + k];
		//copy the jth column of b.data to bb
		for(int j = 0; j < b.row; ++j)
			bb[j] = b.data[j * b.column + i];
		//Gauss elimination method
		for(int j = 0; j < row; ++j){
			//select the main element in jth column
			int index = 0; double maxx = 0;
			for(int k = j; k < row; ++k)
				if(fabs(a[k * column + j]) > maxx){
					index = k;
					maxx = fabs(a[k * column + j]);
				}
			for(int k = j; k < column; ++k)
				swap(a[j * column + k], a[index * column + k]);
			swap(bb[j], bb[index]);
			//Eliminate downward in jth column from jth row
			for(int k = j + 1; k < row; ++k){
				double ratio = a[k * column + j] / a[j * column + j];
				for(int l = j; l < column; ++l)
					a[k * column + l] -= ratio * a[j * column + l];
				bb[k] -= ratio * bb[j];
			}
		}
		//work out the last unknown in current column
		c[(column - 1) * b.column + i] = bb[b.row - 1] / a[row * column - 1];
		//work out the others upward
		for(int j = column - 2; j >= 0; --j){
			double dividend = bb[j];
			for(int k = j + 1; k < column; ++k)
				dividend -= a[j * column + k] * c[k * b.column + i];
			c[j * b.column + i] = dividend / a[j * column + j];
		}
	}
	return MATRIX(column, b.column, c);
}

MATRIX MATRIX::transpose(){
	double c[column * row] = {};
	for(int i = 0; i < row; ++i)
		for(int j = 0; j < column; ++j)
			c[j * row + i] = data[i * column + j];
	return MATRIX(column, row, c);
}

MATRIX MATRIX::inverse(){
	if(this->determinant() == 0)
		throw "Singular matrix";
	//create an identity matrix
	double c[row * column] = {};
	for(int i = 0; i < row; ++i)
		c[i * column + i] = 1;
	MATRIX b(row, column, c);
	return this->leftDivide(b);
}

MATRIX MATRIX::adjoint(){
	if(row != column)
		throw "Matrix dimension mismatch error";
	double det = this->determinant();
	//when A is nonsingular, A* = |A| * inv(A)
	if(det != 0){
		double c[row * column] = {};
		for(int i = 0; i < row; ++i)
			c[i * column + i] = det;
		MATRIX b(row, column, c);
		return this->leftDivide(b);
	}
	else{
		double c[row * column] = {};
		double cofactor[(row - 1) * (column - 1)] = {};
		double sign;
		for(int i = 0; i < row; ++i)
			for(int j = 0; j < column; ++j){
				//work out the current cofactor
				for(int k = 0; k < row - 1; ++k)
					for(int l = 0; l < column - 1; ++l){
						if(k < i && l < j) cofactor[k * (column - 1) + l] = data[k * column + l];
						if(k >= i && l < j) cofactor[k * (column - 1) + l] = data[(k + 1) * column + l];
						if(k < i && l >= j) cofactor[k * (column - 1) + l] = data[k * column + l + 1];
						if(k >= i && l >= j) cofactor[k * (column - 1) + l] = data[(k + 1) * column + l + 1];
					}
				MATRIX cof(row - 1, column - 1, cofactor);
				sign = ((i + j) % 2 == 0 ? 1 : -1);
				c[j * column + i] = sign * cof.determinant();//work out the current algebraic cofactor
			}
		return MATRIX(row, column, c);
	}
}

double MATRIX::determinant(){
	if(row != column) return 0;
	if(row == 1) return data[0];//recursive termination condition
	double ans = 0, sign = 1;
	double c[(row - 1) * (column - 1)] = {};
	//Expand along the first row
	for(int i = 0; i < column; ++i){
		//work out the current cofactor
		for(int j = 0; j < row - 1; ++j)
			for(int k = 0; k < column - 1; ++k){
				if(k < i) c[j * (column - 1) + k] = data[(j + 1) * column + k];
				else c[j * (column - 1) + k] = data[(j + 1) * column + k + 1];
			}
		MATRIX b(row - 1, column - 1, c);
		ans += sign * data[i] * b.determinant();
		sign = -sign;
	}
	return ans;
}

double MATRIX::trace(){
	if(row != column)
		throw "Matrix dimension mismatch error";
	double ans = 0;
	for(int i = 0; i < row; ++i)
		ans += data[i * column + i];
	return ans;
}

int main()
{
	double a[9]={1,2,3,4,5,6,8,9,10},b[9]={1.1,3.2,5.3,2.4,10.5,8.6,6.7,7.8,9.9};
	MATRIX A(3,3,a),B(3,3,b);
	A.print();
	B.print();
	try
		{B.adjoint().print();}
	catch(const char* error)
		{cout << error;}
	return 0;
}
