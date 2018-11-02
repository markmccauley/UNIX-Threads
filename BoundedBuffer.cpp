#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {
	pthread_mutex_init(&mutex, NULL); // initialize mutex and condition variables
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&empty, NULL);
	capacity = _cap;
}

BoundedBuffer::~BoundedBuffer() {
	pthread_mutex_destroy(&mutex); // properly destroy mutex and condition variables
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);
}

int BoundedBuffer::size() {
	pthread_mutex_lock(&mutex); // create thread-safe size function
	int s = q.size();
	pthread_mutex_unlock(&mutex);
	return s;
}

void BoundedBuffer::push(string str) {
	pthread_mutex_lock(&mutex);
	while (q.size() == capacity + 1) { // wait on the first condition
		pthread_cond_wait(&full, &mutex);
	}
	q.push (str); // push item 
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&empty); // signal second condition data is available
}

string BoundedBuffer::pop() {
	pthread_mutex_lock(&mutex);
	while (q.size() == 0) { // wait on the second condition
		pthread_cond_wait(&empty, &mutex);
	}
	string s = q.front();
	q.pop(); // pop item
	pthread_cond_signal(&full); // signal first condition 
	pthread_mutex_unlock(&mutex);
	return s;
}
