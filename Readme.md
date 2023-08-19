# Message Queue (MsgQ) as an IPC Technique

Welcome to this repository which delves into the nuances of using Message Queues (MsgQ) as a method for Inter-process Communication (IPC). 

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

## 1. Opening an Existing Message Queue

```c
mqd_t mq_open(const char *name, int oflag);
```

### Arguments:
- **name:**
  - **Description:** The name of the message queue you want to open. This should start with a `/`. The name serves as an identifier for the IPC (inter-process communication) mechanism.
  - **Example:** `"/my_existing_queue"`
  
- **oflag:**
  - **Description:** Specifies the operational mode of the queue. It's a bitmask that can be set using bitwise OR operations.
    - `O_RDONLY`: Open the queue to receive messages only.
    - `O_WRONLY`: Open the queue to send messages only.
    - `O_RDWR`: Open the queue for both sending and receiving.
    - `O_NONBLOCK`: Messages are sent and received without blocking.
  - **Example:** `O_RDWR | O_NONBLOCK`

## 2. Creating a New Message Queue or Opening an Existing One

```c
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
```

### Arguments:
- **name:**
  - **Description:** Same as above, the identifier for the message queue.
  - **Example:** `"/my_new_or_existing_queue"`
  
- **oflag:**
  - **Description:** A bitmask that defines how the queue should be opened or created.
    - Flags from the simpler version (`O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_NONBLOCK`) are still valid here.
    - `O_CREAT`: Create the message queue if it does not already exist.
    - `O_EXCL`: If used with `O_CREAT`, it ensures exclusive access, thus ensuring the queue is newly created. The function will fail if the queue already exists.
  - **Example:** `O_CREAT | O_RDWR | O_EXCL`

- **mode:**
  - **Description:** Specifies the permissions for the new message queue. It's in standard UNIX permission format and determines who can send/receive messages to/from the queue.
    - `S_IRUSR`: Read permission for the owner.
    - `S_IWUSR`: Write permission for the owner.
    - (and others like `S_IRGRP`, `S_IWGRP`, `S_IROTH`, `S_IWOTH`)
  - **Example:** `S_IRUSR | S_IWUSR`

- **attr:**
  - **Description:** A pointer to a `mq_attr` structure that allows you to specify the attributes for the message queue during creation.
    - `mq_flags`: Message queue flags.
    - `mq_maxmsg`: Maximum number of messages that the queue can hold.
    - `mq_msgsize`: Maximum size of a message (in bytes).
    - `mq_curmsgs`: Number of messages currently in the queue (this is a read-only attribute).
  - **Example:**
  
  ```c
  struct mq_attr attr;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 256;
  ```

The return value for both flavors, if successful, is a message queue descriptor (`mqd_t`) that you can use in subsequent message queue operations. If there's an error, it returns `(mqd_t) -1` and sets the `errno` variable to indicate the nature of the error.

## 1. Opening an Existing Message Queue

```c
mqd_t mq_open(const char *name, int oflag);
```


### Example:

```c
mqd_t queue_descriptor;
queue_descriptor = mq_open("/my_existing_queue", O_RDWR | O_NONBLOCK);
if (queue_descriptor == (mqd_t) -1) {
    perror("mq_open failed");
    exit(1);
}
```

## 2. Creating a New Message Queue or Opening an Existing One

```c
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
```


### Example:

```c
struct mq_attr attr;
attr.mq_maxmsg = 10;
attr.mq_msgsize = 256;

mqd_t queue_descriptor;
queue_descriptor = mq_open("/my_new_or_existing_queue", O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR, &attr);
if (queue_descriptor == (mqd_t) -1) {
    perror("mq_open failed");
    exit(1);
}
```

## Understanding `mode` and the Common `0660` Value

In the context of `mq_open` and many other POSIX functions, the `mode` argument refers to the permissions associated with the new object, in this case, a message queue. It determines who can access the object and how they can access it, using the traditional UNIX file permission notation.

### Breaking Down `0660`:

- `0`: The leading zero indicates the number is in octal (base-8) notation.

- `6`: In binary, this is `110`, which signifies read (`4` in octal) and write (`2` in octal) permissions. This position determines the permissions of the owner (user) of the file or IPC object.
  
- `6`: The second number also indicates read and write permissions. This position is for the group permissions. Any member of the group that owns the file or IPC object has these permissions.
  
- `0`: The third number, being `0`, means no permissions. This is for others (everyone else). So, anyone who isn't the owner or in the owning group has no permissions.

### Summary:
With a mode of `0660`:

- Owner (user): Read and Write
- Group: Read and Write
- Others: No permissions



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
