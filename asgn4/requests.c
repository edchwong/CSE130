#include "requests.h"

Request newRequest(int connfd) {
    Request R;
    R.fd = connfd;
    R.URI_len = 0;
    R.v1 = 0;
    R.v2 = 0;
    R.content_length = 0;
    R.is_content_length_set = 'n';
    R.id = 0;

    R.method = none;
    R.state = MUV;
    return R;
}
