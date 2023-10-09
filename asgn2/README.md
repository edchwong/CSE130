# Changes to the starter code

Most of the code has just been copied over from asgn1, so in order to keep that code working the same, I edited out the handle_connection function that was given. Rather than using the macro for logging that was given to us, I saved the pointer of the log file into my request struct, and just passed the request struct to my reply and log functions. 

# Changes to Asgn1 code

Besides the new starter code for httpserver.c, I also made a few changes to each of the other files. In request.h, I moved a few new macros from the starter code into request.h and added a new variable to the request struct to hold the logfile. In process, I created two new function to handle sending the http reply and audit logging, as well as removing many error checks since the homework document stated that we only had to handle 200, 201, 404, 500 status codes. 

# New functions:

## processLog() 

ProcessLog() is passed in a request and a status code, and writes a log to the log file.

## processReply()

processReply() is passed in connfd, a request, and a status code, and writes an http response to the client.

