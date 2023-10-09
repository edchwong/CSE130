# Design

Since this assignment is continuing to build off of the http server built in assignment 1 and 2, most of the code has remained the same. In order to acomplish the goal of having the server be multithreaded, the way connections were being accepted needed to change. My main function acted as the dispatcher thread, and also creates the desired number of worker threads. The dispatcher listens for requests throught the socket just like before, but rather than sending the connection to the handle_connection() function like it did before, the dispatcher will enqueue the connection, and then signal that there is something in the queue. A worker thread receives the signal and then dequeues then processes it by calling handle_connection(). The rest of the process remains the same as it did before.

# High Level Design

To make the server multithreaded, a bounded buffer is used to keep track of how many requests need to be processed. The bounded buffer is dynamically allocated, and also circular so that it wont accidentally try to access unallocated memory. The dispatcher tries to add a connection to the queue, and waits until space is available before enqueuing when the queue is full. A worker tries to dequeue an item, or waits if the queue is empty, and then calls handle_connection() to process it. The server locks whenever a thread enters the critical region, in this case the critical region is whenever a thread tries to access the queue. The server also uses wait and signal to stop the threads from enqueuing when the queue is full, or dequeuing when the thread is empty. 

# Data Structure

I implemented the queue structure that Eugene showed us in order to create the shared buffer utilized by the threads. It is circular so that we don't have to worry about accidentally trying to access unallocated memory. The buffer is also bounded to prevent the head or tail of the queue from being overwritten as well as preventing the server from accepting a rediculous number of requests at once and possibly runnining out of memory.

# Functions

## queue_new, enqueue, dequeue, queue_full, queue_empty, queue_print

These are the functions implemented for the queue data structure used for the shared buffer. Queue_new dynamically allocates memory for the buffer, and initializes its values. Enqueue adds a connection to the tail of the queue, and dequeue removes the connection at the head of the queue. Queue_full and queue_empty checks to see if the queue is empty/full. Queue_print is used to print out the queue for debugging purposes.

## worker 

Worker is the function that is passed into pthread_create in order to create the worker threads. Worker aquires a lock, checks if the queue is empty, and waits for a signal if it is. If the queue is not empty, then it will dequeue an item and save the connection stored in it. Releases the lock, and signals that an item was removed, and then calls handle_connection to process the request. Once the request has finished, the connection is closed, and the loop restarts.


## main

The main thread acts as our dispatcher thread, and also creates the worker threads that are needed to process the requests. The main thread runs like normal, accepting connections that it are heard from the socket, aquires the lock, checks if the queue is full, and waits if it is. If the queue is not full, then the connection will be enqueued, it will signal that something has been added to the queue, release the lock, and then listen to the socket again to start the loop over.
