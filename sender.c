#include "headers.h"
#include "declarations.h"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char message[MSG_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if(mq == (mqd_t) -1) {
        perror("mq_open");
        exit(1);
    }

    printf("Send a message: ");
    fgets(message, MSG_SIZE, stdin);
    
    if(mq_send(mq, message, strlen(message)+1, PRIORITY) == -1) {
        perror("mq_send");
        exit(1);
    }

    mq_close(mq);
    return 0;
}
