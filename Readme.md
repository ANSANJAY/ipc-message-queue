# Message Queue (MsgQ) as an IPC Technique

Welcome to this repository which delves into the nuances of using Message Queues (MsgQ) as a method for Inter-process Communication (IPC). This content is crafted based on my 10 years of experience as a network engineer.

## Table of Contents
1. [Introduction](#introduction)
2. [MsgQ as a Kernel Resource](#msgq-as-a-kernel-resource)
3. [Opening and Creating a MsgQ](#opening-and-creating-a-msgq)
4. [Closing a MsgQ](#closing-a-msgq)
5. [Enque A Msg into MsgQ](#enque-a-msg-into-msgq)
6. [Deque a Msg from a MsgQ](#deque-a-msg-from-a-msgq)
7. [Unlink a MsgQ](#unlink-a-msgq)
8. [How to Use a MsgQ as an IPC](#how-to-use-a-msgq-as-an-ipc)
9. [Code Walk and Implementation](#code-walk-and-implementation)

## Introduction
Message Queues are a classical IPC mechanism which allow processes to communicate asynchronously through messages. They are especially useful when processes operate at different rates or are not synchronized.

## MsgQ as a Kernel Resource
Message queues are kernel resources. This means they exist independently of any process. Even if the process that created the queue exits, the queue remains until it's explicitly removed. The kernel ensures the messages sent are kept safe until they are read or until the queue is destroyed.

## Opening and Creating a MsgQ
To use a message queue, it first needs to be created or opened using system calls like `msgget()`. 

```c
int msgget(key_t key, int msgflg);
```

## Closing a MsgQ
Message queues, being kernel resources, don't have a strict "close" mechanism like files. Instead, a process can detach itself from the queue. The queue itself remains in the system until explicitly removed.

## Enque A Msg into MsgQ
To enqueue (or send) a message to the queue, one can use `msgsnd()`.

```c
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

## Deque a Msg from a MsgQ
Reading (or dequeuing) a message from the queue can be done using `msgrcv()`.

```c
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
```

## Unlink a MsgQ
To destroy a message queue, the `msgctl()` function is used with the `IPC_RMID` command.

```c
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

## How to Use a MsgQ as an IPC
Message queues are particularly effective in scenarios where:
- Processes are unsynchronized and need a mechanism to avoid race conditions.
- You require a simple, one-way communication method without establishing a connection.
- You wish to communicate asynchronously, where the sender and receiver don't block each other.

## Code Walk and Implementation
For a detailed walk-through and practical implementation of using MsgQs for IPC, refer to the examples folder in this repository. We'll go step-by-step through creating, sending, and receiving messages in different processes.
