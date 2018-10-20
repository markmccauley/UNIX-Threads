#ifndef SafeBuffer_h
#define SafeBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
using namespace std;

class SafeBuffer {
private:
	queue<string> q;	
	pthread_mutex_t mut;
public:
    SafeBuffer();
	~SafeBuffer();
	int size();
    void push (string str);
    string pop();
};

#endif /* SafeBuffer_ */
