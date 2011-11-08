// tro9.2.client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(linker, "/stack:134217728")

//#define DEBUG 1

using namespace std;

char server[256];
int port, serverPort;

WSADATA wsaData;
WORD version;
int error;

//server
int ss;
struct sockaddr_in client_sin, server_sin;
struct in_addr server_address;


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
	/*================================================================*/
	class Matrix{
	private:
		Vector** m;
        int _size;

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
    };
	Matrix::Matrix(int size){
		
    	m = new Vector* [size];
        for (int i = 0; i < size; i++) {
        	m[i] = new Vector (size);
        }
		
        _size = size;
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
	/*================================================================*/
	Vector* vectorSum(Vector* A, Vector* B) {
		int size = A->getLength();
		Vector* ret = new Vector(size);

		for (int i = 0; i < size; i++) {
			ret->setElem(i, A->getElem(i) + B->getElem(i));
            }
        return ret;
	}
	Vector* vectorMulConst(Vector *A, int b) {
		int size = A->getLength();
		Vector* ret = new Vector(size);

		for (int i = 0; i < size; i++) {
			ret->setElem(i, A->getElem(i) * b);
		}
		return ret;
	}
	Matrix* matrixSum(Matrix* MA, Matrix* MB) {
		int size = MA->getLength();
		Matrix* ret = new Matrix(size);

		for (int i = 0; i < size; i++){
			ret->setRow(i, vectorSum(MA->getRow(i), MB->getRow(i)));
		}
		return ret;
	}
	int vectorMul(Vector* A, Vector* B) {
		int size = A->getLength();
		int ret = 0;

		for (int i = 0; i < size; i++) {
			ret += A->getElem(i) * B->getElem(i);
		}

		return ret;
	}
	Matrix* matrixMul(Matrix *MA, Matrix *MB) {
		int size = MA->getLength();
		Matrix* ret = new Matrix(size);

		for (int i = 0; i < size; i++){
			for (int j = 0; j < size; j++){
				ret->setElem(i, j, vectorMul(MA->getRow(i), MB->getColumn(i)));
			}
		}
		return ret;
	}
	Matrix* transMatrix(Matrix* MA){
		int size = MA->getLength();
		Matrix* ret = new Matrix(size);
		for (int i = 0; i < size; i++){
			for (int j = 0; j < size; j++){
				ret->setElem(i, j, MA->getElem(j, i));
			}
		}
		return ret;
	}
	Vector* vectorMulMatrix(Vector* V, Matrix* MA){
		int size = V->getLength();
		Vector* ret = new Vector(size);

		for (int i = 0; i < size; i++){
			ret->setElem(i, vectorMul(V, MA->getColumn(i)));
		}
		return ret;
	}
/*================================================================*/

void receive(int s, char* buffer, int buffer_size) {
	char* to_receive_buffer = buffer;
	int to_receive_size = buffer_size;
	while (to_receive_size > 0)
	{
		to_receive_size -= recv(s, to_receive_buffer, to_receive_size, 0);
		to_receive_buffer = buffer + buffer_size - to_receive_size;
	}
}

int sendData(SOCKET ClientSocket, char* buf, int length) {
	auto result = send(ClientSocket, buf, length, 0);

#ifdef DEBUG 
	if (result > 0)
 		cout << "Bytes sent: " << result << endl;
 	else if (result == 0)
 		cout << "Connection closed" << endl;
 	else
 		cout << "sent failed: " << WSAGetLastError() << endl;
#endif DEBUG

 	return 0;
}

//getting information about cores of computation node
int getCoresNumber (void) {
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	return sinfo.dwNumberOfProcessors;
}




int _tmain(int argc, _TCHAR* argv[])
{
//*
	cout << "Welcome to distributing computing with Win32.Sockets." << endl;
	cout << "Student: Sergii Khomenko." << endl;
	cout << "Client application with task: a=max(MB*MC+MO+ME+MR)" << endl;
		
	cout << "Please enter ip address of the server: " << endl;
	cin >> server;	
	cout << "Please enter port of the server: " << endl;
	cin >> serverPort;

	server_address.S_un.S_addr = inet_addr(server);
	int coresNumber = getCoresNumber();	

	//initiates use of ws2.dll by a process.
	version = MAKEWORD(2, 0);
	error = WSAStartup(version, &wsaData);	
	if ( error != 0 )
	{
		cout << "Program can't create socket." << endl;
		return 0;
	}		
	
	ss = socket(AF_INET, SOCK_STREAM, 0);	
	
	memset(&client_sin, 0, sizeof(client_sin));
	client_sin.sin_family = AF_INET;
	client_sin.sin_addr.s_addr = INADDR_ANY;	
	//client_sin.sin_port = htons(port);

	if (SOCKET_ERROR == bind(ss, (struct sockaddr*) &client_sin, sizeof(client_sin))) {
		cout << "[-] Couldn't start socket for sending information about cores." << endl;
		cin.sync();
		cout << cin.get();
		return 0;
	}	

	
	memset(&server_sin, 0, sizeof(server_sin));
	server_sin.sin_family = AF_INET;
	server_sin.sin_addr.s_addr = inet_addr(server);
	server_sin.sin_port = htons(serverPort);
	connect(ss, (struct sockaddr*) &server_sin, sizeof(server_sin));
	cout << "[*] Connected to the server." << endl;

	cout << "[*] Sending information about cores." << endl;
	int buffer_length = sizeof(int);
	char* buffer = new char[buffer_length];
	*((int*) buffer) = coresNumber;	
	sendData(ss, buffer, buffer_length);

	delete buffer;
	buffer_length=sizeof(int);
	buffer = new char[buffer_length];
	receive(ss, buffer, buffer_length);
	cout << "N = " << (int)(*(int*)buffer) << endl;
	int N = (int)(*(int*)buffer);
	receive(ss, buffer, buffer_length);
	int H = (int)(*(int*)buffer);
	cout << "H = " << (int)(*(int*)buffer) << endl;
	cout << "  [>] Input data for computations have been received" << endl;

	char *bbuffer = new char[sizeof(int)*N*N];
	receive(ss, bbuffer, sizeof(int)*N*N);

	cout << "  [>] Big matrix has been received also" << endl;
	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++)
			cout << ((int*)bbuffer)[i*N+j] <<" ";
		cout << endl;
	}

	char *mrh_b = new char[sizeof(int)*N*H*coresNumber];
	receive(ss, mrh_b, sizeof(int)*N*H*coresNumber);
	cout << "  [>] Part of matrix MR has been received also" << endl;
	for (int i=0; i<H*coresNumber; i++) {
		for (int j=0; j<N; j++)
			cout << ((int*)mrh_b)[i*N+j] <<" ";
		cout << endl;
	}


	shutdown(ss, 0);

	cin.sync();
	cout << cin.get();	
	return 0;
}

