/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "reqchannel.h"
#include "SafeBuffer.h"
#include "Histogram.h"
using namespace std;

class REQUEST_ARGS {
public :
    int n; // increment this many times
    SafeBuffer *buf; // pointer to data in buffer
    string request; // request for data

    // default constructor
    REQUEST_ARGS(int n, string req, SafeBuffer *buf) : 
        n(n), request(req), buf(buf) {}
};

class WORKER_ARGS {
public:
    SafeBuffer *buf; // pointer to data in buffer
    RequestChannel *workerChannel; // request chan for each thread
    Histogram *hist;
};

void* request_thread_function(void* arg) {
    // cast the arg to our REQUEST_ARGS class
    REQUEST_ARGS * ra = (REQUEST_ARGS *) arg;

    for(int i = 0; i < ra->n; i++) {
        ra->buf->push(ra->request); // add the requests to buffer for num of args
    }
    pthread_exit(NULL);
}

void* worker_thread_function(void* arg) {
    // cast the arg to WORKER_ARGS class 
    WORKER_ARGS * wa = (WORKER_ARGS *) arg;

    while(true) {
        string request = wa->buf->pop();
        wa->workerChannel->cwrite(request);

        if (request == "quit") { // check for quit
            delete wa->workerChannel; // delete RC properly
            break;
        } else {
            string response = wa->workerChannel->cread();
            wa->hist->update (request, response);
        }
    }
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            }
    }

    int pid = fork();
    if (pid == 0){
        execl("dataserver", (char*) NULL);
    }
    else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl << flush;

        SafeBuffer request_buffer;
        Histogram hist;

        // start 3 parallel threads 1 for each person
        pthread_t requests[3];

        REQUEST_ARGS patient_john(n, "data John Smith", &request_buffer);
        REQUEST_ARGS patient_jane(n, "data Jane Smith", &request_buffer);
        REQUEST_ARGS patient_joe(n, "data Joe Smith", &request_buffer);

        pthread_create(&requests[0], NULL, request_thread_function, &patient_john);
        pthread_create(&requests[1], NULL, request_thread_function, &patient_jane);
        pthread_create(&requests[2], NULL, request_thread_function, &patient_joe);

        for (int i = 0; i < 3; i++) { // join threads
            pthread_join(requests[i], NULL);
        }

        cout << "Done populating request buffer" << endl;

        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;

        struct timeval begin, end;

        // start timer
        gettimeofday(&begin, NULL);

        pthread_t workers[w];
        WORKER_ARGS jobs[w];
        string s;

        for (int i = 0; i < w; i++) {
            jobs[i].hist = &hist;
            jobs[i].buf = &request_buffer;
            chan->cwrite("newchannel");
            s = chan->cread ();
            jobs[i].workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
            pthread_create(&workers[i], NULL, worker_thread_function, &jobs[i]);
        }

        for (int i = 0; i < w; i++) { // join threads
            pthread_join(workers[i], NULL); 
        }

        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 

        // End timer
        gettimeofday(&end, NULL);
        int64_t start = begin.tv_sec * 1000000L + begin.tv_usec;
        int64_t finish = end.tv_sec * 1000000L + end.tv_usec;
        int64_t time = finish - start;

        hist.print ();

        cout << "Running time: " << time << endl;
    }
}