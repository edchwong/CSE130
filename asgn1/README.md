# Functions 

### newRequest

This function creates a new Request struct which is used to store the request's method, URI, HTTP version, Content Length, and the state of the request.

### handle_connection

Handle_connection is broken up into three sections. The first section deals with getting the request line from the client. It repeatedly reads bytes in from the client, checks if at least one "\r\n" is sent, stores the data in a large buffer, stopping once it has seen at least one "\r\n." The large buffer is then parsed to get the method, URI, and HTTP server. 

The second section of this function is parsing the header. Since this assignment only cares for the header "Content-Length" the function only tries to match that header, and only checks any other header passed in to see if they match the proper "Key: Value" formatting.

The third section is simply checking to see what method was requested, and then sends any relevant data to the respective processing function.

### processURI 

This function is used to make sure that the URI is ready to be used by open(), it checks to see if there is a leading '/' character, and removes it if it does.

### processGET

This function handles GET requests. It starts by opening the file specified by the URI. If it successfully opens the file, it gets the size of the file using fstat(), and then repeatedly reads a chunk of bytes at a time and writes them to the client.

### processPUT

This function handles PUT requests. It tries to open the file and truncates it, if the file doesn't exist, it will be created. This function repeatedly reads from either the message body that was sent in the request, or the file specified by the request, and then writes that data into file specified by the URI

### processAPPEND

This function Handles APPEND requests. It tries to open the file specified by the URI, sets the file cursor to the end of the file, repeatedly reads input from either the client's message body, or the file specified by the client, and then writes the data to the end of the file.

# ERRORS

The errors this program handles are 
1. improperly formatted headers
2. invalid http versions
3. unimplemented methods
4. Non-existent files
5. directories being specified
6. 0 byte requests
7. inmproper file permissions

# Design

I broke this program into different parts. The starter code given to us is left mostly untouched in httpserver.c, with only the handle_connection function being moved to a seperate file. The program is set up so that httpserver.c only interacts with connection.c, and connection.c interacts with process.c. Request.c is only directly used by connection.c, but functions in process.c do use the struct provided by request.c. To keep things from getting too complex, the program is broken down into multiple parts, parsing the request line, parsing the headers, and then processing the actual requests. Errors are dealt with as they occur, and will have a corresponding error status sent as a response.

# High Level Design

The program creates a connection and listens for requests using a socket. Once a request is sent, the program will attempt to read 4096 bytes from the socket into a buffer. The program will start parsing the buffer for the method, URI, and HTTP version, checking to make sure they are valid. The program will then parse the each header that is sent in, checking that it follows the "key: value" format, and saving the appropriate information. Once the headers have been parsed, the method is checked and then its respective process function is called. Get will be simply make read the contents of an existing file and writes it to the client. GET may not work as intended, but it is mostly correct. PUT will read the content of the message body or the contents of a file, and replace the contents of a file designated by the URI. APPEND will read the content of the message body or the contents of a file, and write it to the end of the file designated by URI. Once everything is done processing, it the server waits for another request until it is told to close.
