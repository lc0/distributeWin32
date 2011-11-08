#include "stdafx.h"
#include "windows.h"
#include <iostream>
#include <fstream>

using namespace std;

class Vector {
	private:
        int* v;
        int _size;

	public:		
        int manualN;
        int defVal;
		
		Vector();
		Vector(int size);
		int getElem(int pos);
		void setElem(int pos, int val);
		void printVector();
		void printVector(ofstream *stream);
		int getLength();
		void inputVector();
		void inputVector(ifstream *stream);
		void setManualConf(int N, int defaultVal);
		void copy(Vector* tvec);

    };		
	Vector::Vector(int size) {
    	v = new int[size];
		_size = size;

		manualN = 4;
        defVal = 1;
	}
	int Vector::getElem(int pos){
    	return v[pos];
    }
    void Vector::setElem(int pos, int val) {
    	v[pos] = val;
    }
	void Vector::printVector(){    	
        for (int i = 0; i < _size; i++) {
        	cout << " " << v[i];
        }
        cout << "\n";
     }
	void Vector::printVector(ofstream *stream){    	
        for (int i = 0; i < _size; i++) {
        	*stream << " " << v[i];
        }
        *stream << "\n";
     }
	int Vector::getLength() {
    	return _size;
    }
	void Vector::inputVector() {
		int i=0;
		
		while (i<_size && _size<=manualN) {
			cin >> v[i];
			i++;			
		}
		if (_size>manualN) {
			i=0;
			do{
				v[i]=defVal;
			}while(++i<_size);			
		}		
	}
	void Vector::inputVector(ifstream *stream) {
		int i=0;
		
		while (i<_size && _size<=manualN) {
			*stream >> v[i];
			i++;			
		}
		if (_size>manualN) {
			i=0;
			do{
				v[i]=defVal;
			}while(++i<_size);			
		}		
	}
	void Vector::setManualConf(int N, int defaultVal) {
    	manualN = N;
        defVal = defaultVal;
    }
	void Vector::copy(Vector* tvec) {
		for (int i = 0; i < _size; i++){
		 	tvec[i]=v[i];
	 	}
 	}

	/*================================================================*/
	class Matrix{
	private:
		Vector** m;
        int _size;
		int N;

	public:
		Matrix (int size);
		int getElem(int posX, int posY);
		void setElem(int posX, int posY, int val);
		void setRow(int pos, Vector* V);
		Vector* getRow(int pos);
		Vector* getColumn(int pos);		
		int getLength();
		void printMatrix();
        void printMatrix(ofstream *stream);
		void inputMatrix();
		void inputMatrix(ifstream *stream);
		void copy(Matrix* tmat);
    };
	Matrix::Matrix(int size){
		
    	m = new Vector* [size];
        for (int i = 0; i < size; i++) {
        	m[i] = new Vector (size);
        }
		
        _size, N = size;
   	}
	int Matrix::getElem(int posX, int posY){
    	return m[posX]->getElem(posY);
    }
	void Matrix::setElem(int posX, int posY, int val){
    	m[posX]->setElem(posY, val);
    }
	void Matrix::setRow(int pos, Vector* V) {
    	m[pos] = V;
    }
	Vector* Matrix::getRow(int pos) {
    	return m[pos];
    }
	Vector* Matrix::getColumn(int pos) {
    	Vector *ret = new Vector(_size);		

      	for (int i = 0; i < _size; i++){
       		ret->setElem(i, m[i]->getElem(pos));
        }
        return ret;
	}
	int Matrix::getLength() {
    	return _size;
    }
	void Matrix::printMatrix(){
        for (int i = 0; i < _size; i++) {
			m[i]->printVector();
       	}
    }
	void Matrix::printMatrix(ofstream *stream){
        for (int i = 0; i < _size; i++) {
			m[i]->printVector(stream);
       	}
    }
	void Matrix::inputMatrix() {
    	for (int i = 0; i < _size; i++) {                
                m[i]->inputVector();
        }
    }
	void Matrix::inputMatrix(ifstream *stream) {
    	for (int i = 0; i < _size; i++) {                
                m[i]->inputVector(stream);
        }
    }
	void Matrix::copy(Matrix* tmat) {
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				this->setElem(i,j,tmat->getElem(i,j));
			}
	 	}
 	}
	int vectorMul(Vector* A, Vector* B) {
		int size = A->getLength();
		int ret = 0;

		for (int i = 0; i < size; i++) {
			ret += A->getElem(i) * B->getElem(i);
		}

		return ret;
	}