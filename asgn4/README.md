# Design

Since this assignment is continuing to build off of the http server built in assignment 1,2, and 3, most of the code has remained the same. In order to acomplish the goal of having the server be multithreaded, the way connections were being accepted needed to change. My main function acted as the dispatcher thread, and also creates the desired number of worker threads. The dispatcher listens for requests throught the socket just like before, but rather than sending the connection to the handle_connection() function like it did before, the dispatcher will enqueue the connection, and then signal that there is something in the queue. A worker thread receives the signal and then dequeues then processes it by calling handle_connection(). Handle_connection keeps trying to read until it gets the full request and all headers. The method of the request is checked and then the respective process function is called.

# High Level Design

To make the server multithreaded, a bounded buffer is used to keep track of how many requests need to be processed. The bounded buffer is dynamically allocated, and also circular so that it wont accidentally try to access unallocated memory. The dispatcher tries to add a connection to the queue, and waits until space is available before enqueuing when the queue is full. A worker tries to dequeue an item, or waits if the queue is empty, and then calls handle_connection() to process it. The server locks whenever a thread enters the critical region, in this case the critical region is whenever a thread tries to access the queue. The server also uses wait and signal to stop the threads from enqueuing when the queue is full, or dequeuing when the thread is empty. To make my requests atomic, I use flock() to ensure that no request can read while a file is being written and vice versa. I have not been able to figure out how to make my server run concurrently, but an idea that was suggested by Euguene was to make each put or append request first create a temporary file, get a lock for it, write to that temp file, and then rename the file to the one in the uri.  



# Data Structure

I implemented the queue structure that Eugene showed us in order to create the shared buffer utilized by the threads. It is circular so that we don't have to worry about accidentally trying to access unallocated memory. The buffer is also bounded to prevent the head or tail of the queue from being overwritten as well as preventing the server from accepting a rediculous number of requests at once and possibly runnining out of memory.

# Functions

## queue_new, enqueue, dequeue, queue_full, queue_empty, queue_print

These are the functions implemented for the queue data structure used for the shared buffer. Queue_new dynamically allocates memory for the buffer, and initializes its values. Enqueue adds a connection to the tail of the queue, and dequeue removes the connection at the head of the queue. Queue_full and queue_empty checks to see if the queue is empty/full. Queue_print is used to print out the queue for debugging purposes.

## worker 

Worker is the function that is passed into pthread_create in order to create the worker threads. Worker aquires a lock, checks if the queue is empty, and waits for a signal if it is. If the queue is not empty, then it will dequeue an item and save the connection stored in it. Releases the lock, and signals that an item was removed, and then calls handle_connection to process the request. Once the request has finished, the connection is closed, and the loop restarts.


## main

The main thread acts as our dispatcher thread, and also creates the worker threads that are needed to process the requests. The main thread runs like normal, accepting connections that it are heard from the socket, aquires the lock, checks if the queue is full, and waits if it is. If the queue is not full, then the connection will be enqueued, it will signal that something has been added to the queue, release the lock, and then listen to the socket again to start the loop over.

## newRequest

This function creates a new Request struct which is used to store the request's method, URI, HTTP version, Content Length, and the state of the request.

## handle_connection

Handle_connection is broken up into three sections. The first section deals with getting the request line from the client. It repeatedly reads bytes in from the client, checks if at least one "\r\n" is sent, stores the data in a large buffer, stopping once it has seen at least one "\r\n." The large buffer is then parsed to get the method, URI, and HTTP server. 

The second section of this function is parsing the header. Since this assignment only cares for the header "Content-Length" the function only tries to match that header, and only checks any other header passed in to see if they match the proper "Key: Value" formatting.

The third section is simply checking to see what method was requested, and then sends any relevant data to the respective processing function.

## processURI 

This function is used to make sure that the URI is ready to be used by open(), it checks to see if there is a leading '/' character, and removes it if it does.

## processGET

This function handles GET requests. It starts by opening the file specified by the URI. If it successfully opens the file, it gets the size of the file using fstat(), tries to aquire a shared lock, and then repeatedly reads a chunk of bytes at a time and writes them to the client.

## processPUT

This function handles PUT requests. It tries to open the file and truncates it, if the file doesn't exist, it will be created. This function will try to aquire an exclusive lock on the outfile before repeatedly reading from either the message body that was sent in the request, or the file specified by the request, and then writes that data into file specified by the URI

## processAPPEND

This function Handles APPEND requests. It tries to open and aquire an exclusive lock on the file specified by the URI, sets the file cursor to the end of the file, repeatedly reads input from either the client's message body, or the file specified by the client, and then writes the data to the end of the file.