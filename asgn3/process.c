#include "process.h"

void processURI(Request *R) {
    if (R->URI[0] == '/') {
        char currentURI[BUF_SIZE];
        memcpy(currentURI, R->URI + 1, R->URI_len - 1);
        memset(R->URI, '\0', R->URI_len);
        memcpy(R->URI, currentURI, R->URI_len - 1);
        R->URI_len -= 1;
    }
}

void processGET(int connfd, Request *R) {
    int fd = -1;
    errno = 0;
    fd = open(R->URI, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            processReply(connfd, R, 404);
            R->state = E;
            return;
        } else {
            processReply(connfd, R, 500);
            R->state = E;
            return;
        }
    }
    struct stat file_stats;

    if (fstat(fd, &file_stats) != 0) {
        processReply(connfd, R, 500);
        R->state = E;
        return;
    }

    char rbuffer[BUF_SIZE];
    int read_size = 0;
    int total_bytes_read = 0;
    int filesize = file_stats.st_size;
    dprintf(connfd, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", filesize);

    do {
        read_size = read(fd, rbuffer, BUF_SIZE);
        total_bytes_read += read_size;
        //printf("\nfile size:%d read size: %d total read:%d\n",filesize, read_size, total_bytes_read);
        write(connfd, rbuffer, read_size);
    } while (total_bytes_read < filesize);

    processLog(R, 200);
    close(fd);
    return;
}

void processPUT(int connfd, Request *R, char *remaining_buffer, int remaining_buf_size) {
    int fd = -1;
    int created = 0;
    errno = 0;
    fd = open(R->URI, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        fd = open(R->URI, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        if (fd == -1) {
            processReply(connfd, R, 500);
            return;
        }
        created = 1;
    }

    struct stat file_stats;

    if (fstat(fd, &file_stats) != 0) {

        processReply(connfd, R, 500);
        R->state = done;
        return;
    }

    int bytes_written;
    char rbuffer[BUF_SIZE];
    int read_size = 1;
    //int too_big = 0;

    //write out the part of the message body that was taken in along with
    int counter = 0;

    if (remaining_buf_size < R->content_length) {
        write(fd, remaining_buffer, remaining_buf_size);
        bytes_written = remaining_buf_size;
    } else {
        while (counter < R->content_length) {
            write(fd, remaining_buffer + counter, 1);
            counter++;
        }
        bytes_written = counter;
    }

    if (bytes_written < R->content_length) {

        while (read_size > 0 && bytes_written < R->content_length) {
            read_size = read(connfd, rbuffer, BUF_SIZE);
            if ((bytes_written + read_size) > R->content_length) {
                counter = 0;
                while (counter + bytes_written < R->content_length) {
                    write(fd, rbuffer + counter, 1);
                    counter++;
                }
                //too_big = 1;
                break;

            } else {
                bytes_written += read_size;
            }

            write(fd, rbuffer, read_size);
        }
    }

    if (created == 0) {
        processReply(connfd, R, 200);
    } else {
        processReply(connfd, R, 201);
    }

    close(fd);
    R->state = done;
    return;
}

void processAPPEND(int connfd, Request *R, char *remaining_buffer, int remaining_buf_size) {
    int fd = -1;
    errno = 0;
    fd = open(R->URI, O_WRONLY | O_APPEND);
    //dprintf(STDOUT_FILENO, )
    if (fd == -1) {
        if (errno == ENOENT) {
            processReply(connfd, R, 404);
            R->state = E;
            return;
        } else {
            processReply(connfd, R, 500);
            R->state = E;
            return;
        }
    }

    struct stat file_stats;

    if (fstat(fd, &file_stats) != 0) {
        processReply(connfd, R, 500);
        return;
    }

    //write out the part of the message body that was taken in along with
    write(fd, remaining_buffer, remaining_buf_size);

    int bytes_written = remaining_buf_size;
    char rbuffer[BUF_SIZE];
    int read_size = 1;
    if (bytes_written < R->content_length) {

        while (read_size > 0 && bytes_written < R->content_length) {
            read_size = read(connfd, rbuffer, BUF_SIZE);
            write(fd, rbuffer, read_size);
            bytes_written += read_size;
        }
    }

    processReply(connfd, R, 200);
    close(fd);
    R->state = done;
    return;
}

void processLog(Request *R, int statuscode) {
    char *method;
    switch (R->method) {
    case (get): method = "GET"; break;
    case (put): method = "PUT"; break;
    case (append): method = "APPEND"; break;
    }
    fprintf(R->logfile, "%s,/%s,%d,%d\n", method, R->URI, statuscode, R->id);
    return;
}

void processReply(int connfd, Request *R, int statuscode) {
    switch (statuscode) {
    case (200): dprintf(connfd, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n"); break;
    case (201):
        dprintf(connfd, "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n");
        break;
    case (404):
        dprintf(connfd, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n");
        break;
    case (500):
        dprintf(connfd, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: "
                        "22\r\n\r\nInternal Server Error\n");
        break;
    }
    processLog(R, statuscode);
}