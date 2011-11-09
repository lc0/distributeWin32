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

int H, N, hNode, hnSize, nSize, node_max=MININT;
char server[256];
int port, serverPort;

char *mbh_b, *mc_b, *moh_b, *me_b, *mrh_b;

WSADATA wsaData;
WORD version;
int error;

//server
int ss;
struct sockaddr_in client_sin, server_sin;
struct in_addr server_address;

HANDLE *client_comm, *core_calc;
DWORD *client_comm_id, *core_calc_id;
CRITICAL_SECTION comm_cs, result_cs;
HANDLE *core_finished;


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

//parallel computation on cores of distributed nodes
void client_computations(int *node_number) {
	cout << "[*] Computation on the core" << *node_number << " has started" << endl;

	//copying data from global context to local context
	//TODO:check size and offset for core's part of task
	char *mbh_lb = new char[hnSize];
	EnterCriticalSection(&comm_cs);
	memcpy(mbh_lb, mbh_b, hnSize);
	LeaveCriticalSection(&comm_cs);
	char *mc_lb = new char[nSize];
	EnterCriticalSection(&comm_cs);
	memcpy(mc_lb, mc_b, nSize);
	LeaveCriticalSection(&comm_cs);	
	char *moh_lb = new char[hnSize];
	EnterCriticalSection(&comm_cs);
	memcpy(moh_lb, moh_b, hnSize);
	LeaveCriticalSection(&comm_cs);	
	char *me_lb = new char[nSize];
	EnterCriticalSection(&comm_cs);
	memcpy(me_lb, me_b, nSize);
	LeaveCriticalSection(&comm_cs);	
	char *mrh_lb = new char[hnSize];
	memcpy(mrh_lb, mrh_b, hnSize);

	//distributed calculations on client side. Here can be some another useful functionality
	int core_max = MININT;
	for (int i=0; i<H; i++) {
		for (int j=0; j<N; j++) {
			int tval = 0;
			for (int v=0; v<N; v++) 
				tval += ((int*)mbh_lb)[i*N+v]*((int*)mc_lb)[j*N+v] + ((int*)moh_lb)[i*N+v]*((int*)me_lb)[j*N+v];
			tval+=((int*)mrh_lb)[i*N+j];
			if (tval>core_max) core_max = tval;
		}
	}
	//cout << core_max << endl;

	EnterCriticalSection(&result_cs);
	if (core_max > node_max) node_max = core_max; 
	LeaveCriticalSection(&result_cs);
	
	ReleaseSemaphore(core_finished[*node_number], 1, NULL);
	delete(mbh_lb); delete(mc_lb); delete(moh_lb);
	delete(me_lb); delete(mrh_lb);
	
	cout << "[*] Computation on the core" << *node_number << " has finished" << endl;
}

//getting information about cores of computation node
int getCoresNumber (void) {
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	return sinfo.dwNumberOfProcessors;
}


int _tmain(int argc, _TCHAR* argv[])
{
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

	//receiving data for computations
	delete buffer;
	buffer_length=sizeof(int);
	buffer = new char[buffer_length];
	InitializeCriticalSection(&comm_cs);
	InitializeCriticalSection(&result_cs);


	receive(ss, buffer, buffer_length);
	N = (int)(*(int*)buffer);
	cout << "N = " << N << endl;
	receive(ss, buffer, buffer_length);
	H = (int)(*(int*)buffer);
	cout << "H = " << H << endl;	

	hNode = H*coresNumber;
	hnSize = sizeof(int)*N*hNode;
	nSize = sizeof(int)*N*N;

	mbh_b = new char[hnSize];
	receive(ss, mbh_b, hnSize);
	mc_b = new char[nSize];
	receive(ss, mc_b, nSize);
	moh_b = new char[hnSize];
	receive(ss, moh_b, hnSize);
	me_b = new char[nSize];
	receive(ss, me_b, nSize);
	mrh_b = new char[hnSize];
	receive(ss, mrh_b, hnSize);
	cout << "  [>] Input data for computations have been received" << endl;
	
	//threads for calculations with cores of distributed nodes
	core_calc = new HANDLE[coresNumber];
	core_calc_id = new DWORD[coresNumber];
	core_finished = new HANDLE[coresNumber];
	for (int i=0; i<coresNumber; i++)
		core_finished[i]= CreateSemaphore(NULL, 0, 1, NULL);

	for (int i=0; i<coresNumber; i++) {
		int* param = new int;
		*param = i;
		
		core_calc[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) client_computations, 
			param, 0, (DWORD*) ((size_t)core_calc_id + i * sizeof(DWORD)));		
	}

	WaitForMultipleObjects(coresNumber, core_finished, true, INFINITE);

	//sending the result of node's calculations to the server 
	char* result_buf = new char[sizeof(int)];
	*((int*) result_buf) = node_max;	
	sendData(ss, result_buf, sizeof(int));
	cout << "  [<] Result has been sent " << endl;

	shutdown(ss, 0);
	delete(mbh_b); delete(mc_b); delete(moh_b);
	delete(me_b); delete(mrh_b);

	cin.sync();
	cout << cin.get();	
	return 0;
}

