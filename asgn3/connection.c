
#include "connection.h"
#include "process.h"

void handle_connection(int connfd, FILE *logfile) {
    //printf("in handle\n");
    // begin parsing request line
    char buffer[BUF_SIZE];
    // buffer used to store data read from connfd
    char complete_buffer[BUF_SIZE * 4];
    // complete buffer should contain the full request line, if not more
    char usable_buffer[BUF_SIZE * 4];
    // usable_buffer will copy complete buffer to be used
    // only doing this cause strtok supposedly changes the
    // char[] that is used
    int inputcheck = 0;
    int buffer_pos = 0; // buffer_pos is for complete buffer
    char *token = "";
    char HTTP[10];
    char method[Method_length];
    ssize_t bytes_read = 0;
    ssize_t total_bytes_read = 0;
    // bytes_read keeps track of the current read
    // total_bytes_read keeps track of
    Request R = newRequest(connfd);
    R.logfile = logfile;

    // get the request Line
    do {
        fprintf(stderr, "getting request line\n");
        bytes_read = read(connfd, buffer, BUF_SIZE);
        memcpy(complete_buffer + total_bytes_read, buffer, bytes_read);
        fprintf(stderr, "%s", complete_buffer);
        total_bytes_read += bytes_read;
        memset(buffer, '\0', bytes_read);
        if (inputcheck == 0 && bytes_read < 1) {
            R.state = E;
            break;
        }
        inputcheck++;
    } while (strstr(complete_buffer, "\r\n\r\n") == NULL);
    fprintf(stderr, "got reeequest line\n");
    if (R.state != E) {
        memcpy(usable_buffer, complete_buffer, total_bytes_read);

        token = strtok(usable_buffer, "\r\n");

        sscanf(usable_buffer, "%s %s %s", method, R.URI, HTTP);
        sscanf(HTTP, "HTTP/%d.%d", &R.v1, &R.v2);
        buffer_pos = strlen(token);
        R.URI_len = strlen(R.URI);

        if ((strcmp(method, "GET") == 0) || (strcmp(method, "get") == 0)) {
            R.method = get;
        } else if ((strcmp(method, "PUT") == 0) || (strcmp(method, "put") == 0)) {
            R.method = put;
        } else if ((strcmp(method, "APPEND") == 0) || (strcmp(method, "append") == 0)) {
            R.method = append;
        }

        R.state = H;
    }
    fprintf(stderr, "start headers\n");
    if (R.state != E) {

        // begin parsing the headers
        char key[BUF_SIZE];
        char value[BUF_SIZE];
        char *compare = "";
        int copy_pos = 0;
        char copy[BUF_SIZE];
        memset(copy, '\0', BUF_SIZE);
        memcpy(copy, complete_buffer + buffer_pos, total_bytes_read);

        copy[BUF_SIZE - 1] = '\0';

        while (R.state == H) {

            token = strstr(copy + copy_pos, "\r\n");
            compare = strstr(copy, "\r\n\r\n");
            if (R.is_content_length_set == 'n') {
                if (R.method == get && token == compare) {
                    R.state = processing;
                    break;
                }
            }

            if (compare == token) {
                R.state = processing;
                copy_pos += 4; // stops before the message body
                break;
            }

            sscanf(copy + copy_pos, "%s %s", key, value);

            //add more header checks here
            if (strcmp(key, "Content-Length:") == 0) {
                if (R.is_content_length_set == 'n') {
                    R.content_length = atoi(value);
                    R.is_content_length_set = 'y';
                }
            }
            if (strcmp(key, "Request-Id:") == 0) {
                R.id = atoi(value);
            }
            copy_pos = copy_pos + strlen(key) + strlen(value) + 3;
        }
        buffer_pos += copy_pos;
    }
    //printf("processing now");
    // process the request
    if (R.state == processing) {
        // check for URI
        processURI(&R);

        if (R.method == get) {
            processGET(connfd, &R);
        } else if (R.method == put) {
            processPUT(connfd, &R, complete_buffer + buffer_pos, total_bytes_read - buffer_pos);
        } else if (R.method == append) {
            processAPPEND(connfd, &R, complete_buffer + buffer_pos, total_bytes_read - buffer_pos);
        }
    }

    //fprintf(stderr, "done handle\n");

    (void) connfd;
}