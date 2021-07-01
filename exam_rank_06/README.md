# Exam rank 06 @42

## Intro:
This solution is my answer to the exam rank 06 of school 42 (See subject
attached).

## Implementation details:
This implementation takes pride in never calling more than once read() or
write() after select() returns for a given filedescriptor. Also it doesn't use
any global variables, or overly large static buffers.

This solution takes in account the fact that messages dont necessarily get sent
successfullly in one shot on the network with a call to write().

The only "shorcut" taken, is calling exit() without freeing the clients list,
and whatever it contains, not even closing open filedescriptors. The OS will
do that for us, and if we wanted to handle all this gracefully we should also
take care of the situation of a signla being sent to our program, which we cant
(signal() and alike functions not allowed...).

## Global architecture

### architecture from a bird's-eye view:

1. set up server. It sets up fd ```int sockfd```.
2. enter infinite while loop:
	1. call to ```select()```.
	2. try to accept new clients trying to connect to ```sockfd```
	3. loop over clients and try to read.
	4. loop over clients and try to write.

### architecture from the "jobs" perpective:

When the server receives a new stream of bytes from a client, it stores it in
the ```char *queue``` variable for that client.

Then this queue variable is broken down in messages ready to be sent (lines
ending with a '\n'). Each  message ready to be sent, aka message, is allocated
once and only once for all the others clients to use it. There is a reference
counter that allows us to know when there is no more client using this message.
Both those variable are stored into a type t_job (see below). Each client will
receive a "job", pushed onto its list of jobs.

```
typedef struct s_job
{
	char *msg;
	int position_in_msg;
	int *nb_references;
	struct s_job *next;
} t_job;
```

## Tips:

The function extract_message() propvided at the start of the exam is reused as
is, except for the call to function ```fatal()``` if it fails.

The function provided and named ```str_join()``` is modified the same way, and
it also doesnt do any free() calls on its parameters.

Do call write() after read(), so it prevents from trying to write in a fd that
actually just got closed.
