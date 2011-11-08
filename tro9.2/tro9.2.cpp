// tro9.2.cpp : Defines the entry point for the console application.
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

int port, nodes, N, H, coresCount;

WSADATA wsaData;
WORD version;
int error;

int cs;
struct sockaddr_in client_sin, server_sin;
struct in_addr server_address;

long result_max;

HANDLE *client_comm, *core_calc;
DWORD *client_comm_id, *core_calc_id;

CRITICAL_SECTION comm_cs, result_cs;
HANDLE *calc_finished;



struct node {
	int nodeSocket;
	int coresNumber;	
} *distributed_nodes;

/*================================================================*/
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
		int* getRaw();

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
	int* Vector::getRaw(){
		return v;
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
		int* getRaw();
		int* getRaw(int start, int h);
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
	int* Matrix::getRaw() {
		int *res = new int[_size*_size];
    	for (int i = 0; i < _size; i++) {                
			//memcpy((res+sizeof(int)*_size*i), m[i]->getRaw(), sizeof(int)*_size);			
			memcpy((res+_size*i), m[i]->getRaw(), sizeof(int)*_size);			
        }
		return res;
    }
	int* Matrix::getRaw(int start, int h) {
		int *res = new int[_size*_size];
    	for (int i = 0; i < h; i++) {                
			//memcpy((res+sizeof(int)*_size*i), m[i]->getRaw(), sizeof(int)*_size);			
			memcpy((res+_size*i), m[i+start]->getRaw(), sizeof(int)*_size);			
        }
		return res;
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

Matrix *MB, *MC, *MO, *ME, *MR;

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

//parallel communication with distributed nodes
void client_communications(int *node_number) {
	cout << "[*] Thread for communication with node" << *node_number << " has started" << endl;
	
	cout << "  [>] Sending data to distributed nodes" << endl;
	char* buffer = new char[sizeof(int)];	
	*((int*) buffer) = N;
	sendData(distributed_nodes[*node_number].nodeSocket, buffer, sizeof(int));
	*((int*) buffer) = H;
	sendData(distributed_nodes[*node_number].nodeSocket, buffer, sizeof(int));

	char* bbufer = new char[sizeof(int)*N*N];
	//sending matrix and parts of matrix	
	EnterCriticalSection(&comm_cs);
	memcpy((int*) bbufer, MB->getRaw(), sizeof(int)*N*N);
	LeaveCriticalSection(&comm_cs);
	sendData(distributed_nodes[*node_number].nodeSocket, bbufer, sizeof(int)*N*N);

	EnterCriticalSection(&comm_cs);
	memcpy((int*) bbufer, MR->getRaw(*node_number*H, H*distributed_nodes[*node_number].coresNumber), sizeof(int)*N*H*distributed_nodes[*node_number].coresNumber);
	LeaveCriticalSection(&comm_cs);
	sendData(distributed_nodes[*node_number].nodeSocket, bbufer, sizeof(int)*N*H*distributed_nodes[*node_number].coresNumber);
	


	//waiting for result with blocking thread on receive
	int core_max = MININT;

	EnterCriticalSection(&comm_cs);
	if (core_max > result_max) result_max = core_max; 
	LeaveCriticalSection(&comm_cs);

	//calculation has done
	ReleaseSemaphore(calc_finished[*node_number], 1, NULL);

	cout << "[*] Thread for communication with node" << *node_number << " has finished" << endl;
}

//parallel computation on distributed nodes
void client_computations(int *node_number) {
	cout << "[*] Computation on the core" << *node_number << " has started" << endl;
	

}

//getting information about cores of computation node
int getCoresNumber (void) {
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	return sinfo.dwNumberOfProcessors;
}

int _tmain(int argc, _TCHAR* argv[])
{	
/*
	cout << "Please enter N: " << endl;
	cin >> N;
	MB = new Matrix (N);
	MB->inputMatrix();
	int *t = MB->getRaw();
	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++)
			cout << t[i*N+j] <<" ";
		cout << endl;
	}
*/

	cout << "Welcome to distributing computing with Win32.Sockets." << endl;
	cout << "Student: Sergii Khomenko." << endl;
	cout << "Server application with task: a=max(MB*MC+MO+ME+MR)" << endl;

	cout << "Please enter port of the server: " << endl;
	cin >> port;
	cout << "Please enter number of nodes(with server node): " << endl;
	cin >> nodes;
	cout << "Please enter N: " << endl;
	cin >> N;

	distributed_nodes = new node [nodes];

	//initiates use of ws2.dll by a process.
	version = MAKEWORD(2, 0);
	error = WSAStartup(version, &wsaData);	
	if ( error != 0 )
	{
		cout << "Program can't create socket." << endl;
		return 0;
	}		
	
	cs = socket(AF_INET, SOCK_STREAM, 0);	
	
	memset(&client_sin, 0, sizeof(client_sin));
	client_sin.sin_family = AF_INET;
	client_sin.sin_addr.s_addr = INADDR_ANY;	
	client_sin.sin_port = htons(port);

	if (SOCKET_ERROR == bind(cs, (struct sockaddr*) &client_sin, sizeof(client_sin))) {
		cout << "[-] Couldn't start socket on server side" << endl;
		cin.sync();
		cout << cin.get();
		return 0;
	}	

	if (SOCKET_ERROR == listen(cs, 10)) {
		cout << "[-] Some troubles with starting listening" << endl;		
		return 0;
	}
	cout << "[*] Server is listening." << endl;

	//waiting for connect from distributed nodes
	int nn=0;
	while (nn<nodes) {
		struct sockaddr_in node_sin;
		int size_node_sin = sizeof(node_sin);
		memset(&node_sin, 0, size_node_sin);
		int scs = sizeof(node_sin);

		//todo: work with new kinds of smart pointers
		distributed_nodes[nn].nodeSocket = accept(cs, (struct sockaddr*) &node_sin, &scs);
		int buffer_length = sizeof(int);
		char* buffer = new char[buffer_length];
		receive(distributed_nodes[nn].nodeSocket, buffer, buffer_length);
		distributed_nodes[nn].coresNumber = (int)buffer[0];
		coresCount += distributed_nodes[nn].coresNumber; 

		cout << " [+] Client has connected." << endl;
		cout << "    [*] Node" << distributed_nodes[nn].nodeSocket <<
			" with " << distributed_nodes[nn].coresNumber << " core(s)" << endl;
		nn++;
	}
	cout << "[*] System will work with " << nodes << " node(s) and " << coresCount << " core(s)." << endl;


	
	//initialization data
	MB = new Matrix (N); MC = new Matrix (N); MO = new Matrix (N);
	ME = new Matrix (N); MR = new Matrix (N);

	InitializeCriticalSection(&result_cs);
	InitializeCriticalSection(&comm_cs);	
	calc_finished = new HANDLE[nodes];
	for (int i=0; i<nodes; i++)
		calc_finished[i]= CreateSemaphore(NULL, 0, nodes, NULL);

	MB->inputMatrix(); MC->inputMatrix(); MO->inputMatrix();
	ME->inputMatrix(); MR->inputMatrix();

	H = coresCount<N?N/coresCount:1;
	auto coresNumber = getCoresNumber();
	
	client_comm = new HANDLE[nodes];
	client_comm_id = new DWORD[nodes];
	core_calc = new HANDLE[coresNumber];
	core_calc_id = new DWORD[coresNumber];

	for (int i=0; i<nodes; i++) {
		int* parameter = new int;
		*parameter = i;

		//threads for communication with distributed nodes
		client_comm[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) client_communications, 
			parameter, 0, (DWORD*) ((size_t)client_comm_id + i * sizeof(DWORD)));		
	}
/*	
	for (int i=0; i<coresNumber; i++) {
		int* parameter = new int;
		*parameter = i;

		//threads for calculations on local cores
		core_calc[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) client_computations, 
			parameter, 0, (DWORD*) ((size_t)core_calc_id + i * sizeof(DWORD)));		
	}	
*/	
	//wating for the result from distributed nodes
	WaitForMultipleObjects(nodes,calc_finished, true, INFINITE);

	DeleteCriticalSection(&result_cs);
	DeleteCriticalSection(&comm_cs);

	shutdown(cs, 0);
	
	cin.sync();
	cout << cin.get();
	return 0;	
}

