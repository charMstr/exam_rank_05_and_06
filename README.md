# exam rank 06 @42

This solution should answer to the exam rank 06 of school 42.
See subject attached.

This program has remaining call to printf() and sleep() that need to be removed
but have been left for the sake of clarity.

In this exercice, we are writing a server that should allow clients to connect,
and forward any received message from a client, to all the other connected
clients. There is no content-length, or chunk-encoding or likewise
implementation of such a protocol that could allow us to know the length of the
message we are going to receive. The only hint we have is the given\_main.c
given as start to get inspired, and it contains the function extract\_message()
already implemented:

```
int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1); 
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}
```

The school checker seems to connect a few clients, then send messages and do a
sort at the end of the pipe. ```... | sort ``` then all this is "diffed"
The best way to make sure the outputs will remain the same with the checker,in
case some sort of usleep() calls are made between each write, and the binary is
then killed half way, is to use the given function. So we will break down our
calls to write() the same way, even if its completely dumb.

The essential of the logic can be understood in the structure t\_list:

````
typedef struct s_list
{
	int fd;
	int id;
	char *queued_msgs;
	char *extracted_msg;
	struct s_list *next;
	struct s_list *previous;
}			t_list;
````

Each link of type t\_list represents a client connected to the server.
`fd`: It stores the fd of the connexion socket with this specific client.
`id`: It stores the client's id.
`queued_msgs`: It stores the messages to be sent for a specific client. Any new
message is appended to this string.
`extracted_msg`: It is the destination where we extract the next message from
`queued_msgs`. The next message being the next line up to a '\n'.

**NOTE1: if no '\n' is found, we directly atempt to write `queued_msgs`.**

**NOTE2: each time extracted_msg or queued_msgs is fully written it is freed and
reset to NULL. If its not fully sent over the network, we simply shift the bytes
it contains**
