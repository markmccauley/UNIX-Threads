#include "SafeBuffer.h"
#include <string>
#include <queue>
using namespace std;

SafeBuffer::SafeBuffer() {
	pthread_mutex_init(&mut, NULL); // create a mutex object
}

SafeBuffer::~SafeBuffer() { // destructor for mutex object
	pthread_mutex_destroy(&mut); 
}

int SafeBuffer::size() {
	pthread_mutex_lock(&mut); // add locks to make the function thread-safe
	int s = q.size(); 
	pthread_mutex_unlock(&mut);

    return s;
}

void SafeBuffer::push(string str) {
	pthread_mutex_lock(&mut); // add locks to make function thread-safe
	q.push (str);
	pthread_mutex_unlock(&mut);
}

string SafeBuffer::pop() {
	pthread_mutex_lock(&mut); // add locks to make function thread-safe
	string s = q.front();
	q.pop();
	pthread_mutex_unlock(&mut); 

	return s;
}
