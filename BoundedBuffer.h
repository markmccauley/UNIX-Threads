#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
using namespace std;

class BoundedBuffer {
private:
	queue<string> q;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
    int capacity;	
public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push (string);
    string pop();
    int get_cap() {return capacity;}
};

#endif /* BoundedBuffer_ */
