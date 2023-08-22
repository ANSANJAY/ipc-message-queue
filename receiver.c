#include "headers.h"
#include "declarations.h"

int main() {
mqd_t mq;
struct mq_attr attr;
char* buffer;

mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, NULL);
if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
}

mq_getattr(mq, &attr);
buffer = malloc(attr.mq_msgsize);
if (!buffer) {
    perror("malloc");
    exit(1);
}

if (mq_receive(mq, buffer, attr.mq_msgsize, NULL) == -1) {
    perror("mq_receive");
    free(buffer);
    exit(1);
}

printf("Received message: %s", buffer);
free(buffer);


    mq_close(mq);
    mq_unlink(QUEUE_NAME);  // Remove the message queue
    return 0;
}
