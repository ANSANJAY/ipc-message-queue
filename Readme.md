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

----------------------------

# Closing a MsgQ
Message queues, being kernel resources, don't have a strict "close" mechanism like files. Instead, a process can detach itself from the queue. The queue itself remains in the system until explicitly removed.

## `mq_close`

## Synopsis

`mq_close` is a system call that closes a message queue descriptor. When a process no longer requires access to a message queue, it should close the descriptor to free up system resources.

## Arguments

- `mqdes`: This is the message queue descriptor. It's obtained from a previous call to `mq_open`.

## Return Value

Upon successful completion, `mq_close` returns 0. Otherwise, -1 is returned, and `errno` is set to indicate the error.

## Error Handling

Errors related to `mq_close` might include:
- `EBADF`: The `mqdes` argument is not a valid message queue descriptor.
- Other errors may be set by the underlying system calls that `mq_close` might invoke.

Always check the return value of `mq_close` and consult `errno` for detailed error information.

## Kernel Resource

Message queues are a kernel resource, meaning they exist and are managed by the kernel rather than user space. This ensures the necessary isolation, consistency, and synchronization guarantees for inter-process communication.

## Reference Counting

Every time a message queue is opened with `mq_open`, a reference count associated with that message queue is incremented. This count keeps track of how many processes or threads currently have the message queue opened.

The `mq_close` function decrements this reference count. The use of reference counting ensures that the message queue's resources aren't prematurely released while processes are still using it.

## How `mq_close` Works with Reference Counting

When `mq_close` is called, the reference count for the message queue is decremented. If the reference count drops to zero, it indicates that no processes or threads are using this message queue anymore. However, the message queue's actual destruction will occur only if all processes using it have closed it and a call to `mq_unlink` has been made.

## Note on Message Queue Destruction

A message queue will be destroyed only if:
1. All processes using it have called `mq_close`, bringing the reference count to zero.
2. A call to `mq_unlink` is made to remove the message queue.

This ensures that a message queue isn't removed while still in use, preventing potential data loss or unexpected behavior.

## Common Use Cases

- **Graceful Termination**: Before a process exits, it's a good practice to close any message queues it opened to ensure proper cleanup and avoid resource leaks.
- **Resource Management**: In systems with limited resources, consistently closing unused message queues can help maintain system performance and stability.


---------------------
# `mq_send` 

## Table of Contents

- [FIFO Principle](#fifo-first-in-first-out-principle)
- [`mq_send` Synopsis and Arguments](#mq_send-synopsis-and-arguments)
- [Blocking Behavior](#blocking-behavior)
- [Non-blocking Behavior with O_NONBLOCK](#non-blocking-behavior-with-ononblock)
- [Additional Details](#additional-details)

---

## FIFO (First In, First Out) Principle

A message queue follows the FIFO principle, which means the first message sent to the queue will be the first to be received. It works just like a real-world queue, such as a line at a checkout counter. The message that has been in the queue the longest is the one that gets delivered first. This ordering ensures a fair and predictable processing order for messages.

---

## `mq_send` Synopsis and Arguments

`mq_send` is used to send a message to a message queue. Its function signature is:

```c
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
```

Arguments:
- `mqdes`: The message queue descriptor returned by `mq_open`.
- `msg_ptr`: A pointer to the message to be sent.
- `msg_len`: The length of the message in bytes.
- `msg_prio`: The priority of the message. Messages with higher priority will be delivered before messages with lower priority.

---

## Blocking Behavior

By default, `mq_send` will block if the message queue is full. It will wait until space becomes available in the queue. This is useful if you want to ensure that a message is sent as soon as there is room in the queue.

---

## Non-blocking Behavior with `O_NONBLOCK`

If the message queue was opened with the `O_NONBLOCK` flag, `mq_send` will not block. Instead, if the queue is full, the function will return immediately with a value of -1, and `errno` will be set to `EAGAIN`. This behavior allows your program to continue executing and retry the send operation later or take appropriate action based on the full queue.

```c
mqd_t mqdes = mq_open("/myqueue", O_WRONLY | O_NONBLOCK);
if (mqdes == -1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
}
if (mq_send(mqdes, "Hello, world!", 13, 0) == -1) {
    if (errno == EAGAIN) {
        printf("Queue is full, try again later. n");
    } else {
        perror("mq_send");
    }
}
```

---

## Additional Details

`mq_send` is a crucial function for sending messages between processes via message queues. Understanding its behavior and options allows for more robust and responsive inter-process communication.

Remember to check the return value of `mq_send` to detect any errors and handle them appropriately. Be mindful of the blocking/non-blocking behavior when designing your application to ensure that it performs optimally and can handle situations where the message queue is full.

---

# Deque a Msg from a MsgQ with  `mq_receive` 

## Table of Contents

- [`mq_receive` Synopsis and Arguments](#mq_receive-synopsis-and-arguments)
- [Blocking Behavior](#blocking-behavior)
- [Non-blocking Behavior with `O_NONBLOCK`](#non-blocking-behavior-with-ononblock)
- [Return Value](#return-value)
- [Additional Details](#additional-details)

---

## `mq_receive` Synopsis and Arguments

`mq_receive` is a system call used to receive a message from a message queue. Its function signature is:

```c
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
```

Arguments:
- `mqdes`: The message queue descriptor, which is obtained from a previous call to `mq_open`.
- `msg_ptr`: A pointer to an empty buffer where the received message will be stored.
- `msg_len`: The size of the buffer pointed to by `msg_ptr`. It should be greater than or equal to the maximum message size as defined by the message queue attributes.
- `msg_prio` (optional): If not NULL, it points to an unsigned integer where the priority of the received message will be stored.

---

## Blocking Behavior

If the message queue is empty, `mq_receive` behaves as a blocking call by default. This means the calling process will wait until a message becomes available in the queue.

---

## Non-blocking Behavior with `O_NONBLOCK`

If the message queue was opened with the `O_NONBLOCK` flag, and the queue is empty, `mq_receive` will not block. Instead, it will return immediately with a value of -1, and `errno` will be set to `EAGAIN`. 

```c
mqd_t mqdes = mq_open("/myqueue", O_RDONLY | O_NONBLOCK);
char buffer[128];
if (mq_receive(mqdes, buffer, sizeof(buffer), NULL) == -1) {
    if (errno == EAGAIN) {
        printf("Queue is empty, try again later. n");
    } else {
        perror("mq_receive");
    }
}
```

---

## Return Value

The `mq_receive` function returns the number of bytes read from the queue. In case of an error, it returns -1, and `errno` is set appropriately.

---

## Additional Details

- Remember to allocate a buffer (`msg_ptr`) that is sufficiently large to accommodate the maximum message size for the message queue. If the buffer is too small, `mq_receive` will fail with `EMSGSIZE`.
- The priority (if the `msg_prio` argument is provided) gives the receiver a mechanism to process messages based on their urgency. Messages with higher priority will be delivered before those with lower priority.
- Always verify the return value and handle errors gracefully for robust inter-process communication.

---

# Unlinking a Message Queue with `mq_unlink`

## Table of Contents

- [Synopsis and Description](#synopsis-and-description)
- [Usage and Behavior](#usage-and-behavior)
- [Considerations and Caveats](#considerations-and-caveats)
- [Example](#example)
- [Additional Details](#additional-details)

---

## Synopsis and Description

The `mq_unlink` system call is used to remove a named message queue. Once the message queue is unlinked, it's name is removed and future attempts to open the queue with `mq_open` will fail unless the queue is recreated.

Function signature:

```c
int mq_unlink(const char *name);
```

Argument:
- `name`: The name of the message queue to be removed.

---

## Usage and Behavior

Using `mq_unlink` removes the message queue name but doesn't necessarily destroy the queue immediately. The queue is destroyed only after all references to it (from open descriptors) are closed. 

---

## Considerations and Caveats

- If processes still have the queue open when `mq_unlink` is called, they can continue to use it. The queue is destroyed only after the last descriptor referencing it is closed.
- Unlinking a queue that doesn't exist will result in an error.
- It's good practice to unlink queues when they are no longer needed to free up system resources.

---

## Example

```c
mqd_t mqdes = mq_open("/myqueue", O_RDONLY | O_CREAT);
// ... use the message queue ...
mq_close(mqdes);
mq_unlink("/myqueue");
```

In the above code, after finishing operations with the message queue, it is closed and then unlinked.

---

## Additional Details

- Always check the return value of `mq_unlink` for potential errors.
- `mq_unlink` is analogous to the `unlink` system call for files. It removes the name, making the queue inaccessible, but the underlying resources are released only after all references to it are gone.

---

# Using Message Queues (MsgQ) as an IPC Mechanism

Message queues provide an effective IPC mechanism, allowing processes to communicate without shared memory or being directly connected.

## Table of Contents

- [Overview of MsgQ as IPC](#overview-of-msgq-as-ipc)
- [Multiple Senders, Single Receiver Concept](#multiple-senders-single-receiver-concept)
- [Examples and Usage](#examples-and-usage)
- [Additional Information](#additional-information)

---


# Message Queues (MsgQ) as an IPC Mechanism in Unix-like Systems

Message queues offer a robust mechanism for inter-process communication (IPC) in Unix-like systems. They allow for asynchronous communication between processes without necessitating direct connections or shared memory.

## Table of Contents

- [Introduction to MsgQ](#introduction-to-msgq)
- [Setting Up a Message Queue](#setting-up-a-message-queue)
- [Sending and Receiving Messages](#sending-and-receiving-messages)
- [Closing, Unlinking, and Cleaning Up](#closing-unlinking-and-cleaning-up)
- [Multiple Senders and a Single Receiver Concept](#multiple-senders-and-a-single-receiver-concept)
- [Best Practices and Recommendations](#best-practices-and-recommendations)

---

```sql
Process 1            Process 2            Process 3
+--------+          +--------+          +--------+
| Sender |          | Sender |          | Sender |
|        |          |        |          |        |
|mq_open |          |mq_open |          |mq_open |
|mq_send |          |mq_send |          |mq_send |
|mq_close|          |mq_close|          |mq_close|
+--------+          +--------+          +--------+
    |                  |                  |
    v                  v                  v
               Kernel Resource
                +-------------------------+
                |  Message Queue (MsgQ)   |
                |                         |
                |    1. Packet from P1    |
                |    2. Packet from P2    |
                |    3. Packet from P3    |
                |        ... (FIFO)       |
                +-------------------------+
                           |
                           |
                           v
                      +--------+
                      |Receiver|
                      |        |
                      |mq_open |
                      |mq_recv |
                      +--------+

* Only one receiver can access and read a message from the MsgQ at any given time.
  This ensures that a message is not processed multiple times by different receivers.


```

## Introduction to MsgQ

Message queues provide kernel-supported queue mechanisms, where messages are stored until another process retrieves them. This FIFO (First In, First Out) structure guarantees sequential order, enabling streamlined communication between processes.

---

## Setting Up a Message Queue

To initiate a message queue:

```c
#include <mqueue.h>

mqd_t mqdes;
struct mq_attr attr;

attr.mq_flags = 0;
attr.mq_maxmsg = 10;
attr.mq_msgsize = 512;
attr.mq_curmsgs = 0;

mqdes = mq_open("/myqueue", O_CREAT | O_RDWR, 0644, &attr);
if (mqdes == (mqd_t) -1) {
    perror("mq_open");
}
```

Here, `/myqueue` is created with a capacity for 10 messages, each up to 512 bytes.

---

## Sending and Receiving Messages

**Sending Messages**:

```c
const char *message = "Hello, IPC!";
if (mq_send(mqdes, message, strlen(message) + 1, 0) == -1) {
    perror("mq_send");
}
```

**Receiving Messages**:

```c
char buffer[512];
ssize_t bytesRead;

bytesRead = mq_receive(mqdes, buffer, sizeof(buffer), NULL);
if (bytesRead == -1) {
    perror("mq_receive");
} else {
    printf("Received: %s n", buffer);
}
```

---


## Additional Information

For more details, always refer to the man pages and other documentation related to the specific message queue implementation you're working with. Remember that while message queues are powerful, you should choose the IPC mechanism that best suits your application's needs.

---

If you need more comprehensive examples or have other questions regarding IPC or any other topic, always refer to the official documentation or expert sources.


## Code Walk and Implementation
For a detailed walk-through and practical implementation of using MsgQs for IPC, refer to the examples folder in this repository. We'll go step-by-step through creating, sending, and receiving messages in different processes.

 ## Compilation Instructions

 ### Prerequisites

 - Ensure you have  `gcc ` (or relevant compiler/tool) installed. If not, install it:

   ```
  sudo apt-get install gcc  # For Debian/Ubuntu-based systems
   ```

 
 ### Steps

1.  **Clone the Repository **:
   
   Use git to clone the repository to your local machine:
    ```
   git clone https://github.com/ANSANJAY/ipc-message-queue.git
    ```


## Compilation Instructions

To compile the server, run:

```
make server
```

To compile the receiver, run:

```
make receiver
```

4.  **Run the Compiled Code **:

   After successfully compiling, run the code reciver and sender in separate terminal windows
    
    ```bash
   ./reciever 
    ```

    ```bash
  ./sender
  ```

  Expected output:

![Expected Output Screenshot](./Screenshot%20from%202023-08-22%2001-04-11.png)