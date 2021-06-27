/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   given_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/26 20:19:59 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/27 05:51:27 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>

void fatal_args(void)
{
	const char *str = "Wrong number of arguments" "\n";
	write(2, str, strlen(str));
	exit(1);
}

void fatal_error(void)
{
	const char *str = "Fatal error" "\n";
	write(2, str, strlen(str));
	exit(1);
}

/*
** before calling this function, things must look like:
** char *buf; //would be the message we received.
** char *msg; //is the message we want to send.
**
** call would look like this.
** int res;
** if ((res = extract_message(&buf, &msg)) < 0)
** 		fatal_error();
**
** RETURNS: 0: no '\n' met, nothing got extracted, buffer contains the exact same string.
** 			-1: something went wrong --> call the write_error with fatal_error);
** 			1: one '\n' met, *buf contains the remainder, *msg contains the first part of the message to be sent.
*/

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;  //if not already freed, could create a leak...
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1); //should call the fatal error message if -1.
			strcpy(newbuf, *buf + i + 1);
			//*msg contains the initial string, up to (including) the '\n'.
			*msg = *buf;
			(*msg)[i + 1] = 0;
			//*buf contains the remainder of the string
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

char *fd_strdup(const char *str)
{
	char *new;

	if (!(new = malloc (sizeof(char) * (strlen(str) + 1))))
		fatal_error();
	int i = strlen(str);
	new[i] = '\0';
	while (--i >= 0)
		new[i] = str[i];
	return (new);
}

//sets up the socket for the server, return -1 if failure.
int	set_up_server_socket(int port)
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return (-1);
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
	{
		close(sockfd); //add this
		return (-1);
	}
	if (listen(sockfd, 10) != 0)
	{
		close(sockfd); //add this
		return (-1);
	}
	//note: could set socket to non blocking here...
	return (sockfd);
}

typedef struct s_fd_sets
{
	fd_set  fd_set_write;
	fd_set	fd_set_read;
	fd_set	fd_set_excepts;
	int		id_for_next_client;
}			t_fd_sets;

typedef struct s_list
{
	int fd;
	int id;
	char *queued_msgs;		//Any new message is appended to it.
	char *extracted_msg;	//Just before sending a message, the first line up
							//to a '\n' is extracted in this field.
	struct s_list *next;
	struct s_list *previous;
}			t_list;

void	push_front_new_client(t_list **clients, int fd, int id)
{
	t_list *new_client;
	if (!(new_client = malloc(sizeof(t_list))))
		fatal_error();
	//bzero sets previous and queued_msg_to_sent and extracted_msg to NULL;
	bzero(new_client, sizeof(*new_client));
	new_client->fd = fd;
	new_client->id = id;
	new_client->next = *clients;
	*clients = new_client;
}

void	add_new_client_to_fd_set_real(int new_fd, t_fd_sets *real)
{
	FD_SET(new_fd, &real->fd_set_read);
	FD_SET(new_fd, &real->fd_set_write);
	real->id_for_next_client += 1;
}

void	add_msg_to_all_other_clients(char *latest_msg, int fd_to_avoid, t_list *clients)
{
	while (clients)
	{
		if (clients->fd != fd_to_avoid)
		{
			if (!(clients->queued_msgs = str_join(clients->queued_msgs, latest_msg)))
				fatal_error();
		}
		clients = clients->next;
	}
}

//will exit if failure
void try_to_accept_new_clients(int sockfd, t_list **clients, t_fd_sets copy, t_fd_sets *real)
{
	int connfd, len;
	struct sockaddr_in cli;
	t_list *current_client = *clients;
	char intro_buffer[100];

	len = sizeof(cli);
	if (FD_ISSET(sockfd, &copy.fd_set_read))
	{
		connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
		if (connfd < 0)
			fatal_error();
		else
		{
			//sizeof string is 30, including '\0', and without any number
			sprintf(intro_buffer, "server: client %d just arrived\n", real->id_for_next_client);
			add_msg_to_all_other_clients(intro_buffer, connfd, *clients);

			push_front_new_client(clients, connfd, real->id_for_next_client);
			add_new_client_to_fd_set_real(connfd, real);
		}
	}
}

void ft_memmove(char *str, int written_bytes)
{
	int i = written_bytes;
	int j = 0;
	const int size_move = strlen(str) - written_bytes;

	while (i < size_move)
		str[j++] = str[i++];
	str[j] = '\0';
}

char *compose_message_to_send_to_others(int len_buffer, const char *buffer, int id_recipient)
{
	char intro_buffer[100];
	char *msg_composed;
	int i = 0;
	int j = 0;

	sprintf(intro_buffer, "client %d: ", id_recipient);
	const int len_intro = strlen(intro_buffer);

	if (!(msg_composed = malloc(len_intro + len_buffer + 1)))
		fatal_error();
	while (i < len_intro)
	{
		msg_composed[i] = intro_buffer[i];
		i++;
	}
	while (i < len_intro + len_buffer)
	{
		msg_composed[i] = buffer[j];
		j++;
		i++;
	}
	msg_composed[i] = '\0';

	return(msg_composed);
}

void remove_client_from_fd_sets(int fd, t_fd_sets *real_set, t_fd_sets *copy)
{
	//make sure the write function does not attempt to write there.
	FD_CLR(fd, &copy->fd_set_read);
	FD_CLR(fd, &copy->fd_set_write);
	//remove from the real set.
	FD_CLR(fd, &real_set->fd_set_read);
	FD_CLR(fd, &real_set->fd_set_write);
	//could decrease the fd of next client here in real_set->id_of_next_client.
}

// in this function if the client disconnects (reading a '\0') we erase the
//client from the clients list, and we remove it from fd_real_set
void resume_reading_from_clients(t_fd_sets *real_set, t_fd_sets *copy, t_list **clients)
{
	char buffer[65535];
	t_list *current_client = *clients;

	//printf("in resume_reading_from_clients():\n");
	while (current_client)
	{
		if (FD_ISSET(current_client->fd, &copy->fd_set_read))
		{
			int res;
			res = read(current_client->fd, buffer, 65535);
			if (res == -1)
				current_client = current_client->next;
			else if (res == 0) //this means the client disconnected! mother fucker
			{
				remove_client_from_fd_sets(current_client->fd, real_set, copy);

				char intro_buffer[100];
				sprintf(intro_buffer, "server: client %d just left\n", current_client->id);
				add_msg_to_all_other_clients(intro_buffer, current_client->fd, *clients);
				//store next.
				t_list *next_client = current_client->next;
				//need to update the next of previous, if existing, or the list head itself
				if (current_client->previous)
					current_client->previous->next = next_client;
				else
					*clients = next_client;  //the reason why we needed a double pointer here.
				free(current_client->queued_msgs);
				free(current_client->extracted_msg);
				free(current_client);

				current_client = next_client;
			}
			else
			{ //append the read message, to the message to write in each client.
				char *composed_msg;
				composed_msg = compose_message_to_send_to_others(res, buffer, current_client->id);
				add_msg_to_all_other_clients(composed_msg, current_client->fd, *clients);

				current_client = current_client->next;
			}
		}
		else
			current_client = current_client->next;
	}
}

//this function returns the address of the message we are going to send to
//client, then free/set to null OR call fd_memmove() upon.
char **chose_between_queued_msg_and_extracted_msg(t_list *client)
{
	int res;
	char **msg;

	if (client->extracted_msg)
		msg = &client->extracted_msg;
	else
	{
		if ((res = extract_message(&client->queued_msgs, \
						&client->extracted_msg)) < 0)
			fatal_error();
		if (res == 0)
			msg = &client->queued_msgs;
		else
			msg = &client->extracted_msg;
	}

	return(msg);
}

void resume_writing_to_clients(t_fd_sets copy, t_list *clients)
{
	int res;

	//printf("in resume_writing_to_clients():\n");
	while (clients)
	{
		//make sur the fd is set after a select, and that the str pointer is not null
		if (FD_ISSET(clients->fd, &copy.fd_set_write) \
				&& (clients->queued_msgs || clients->extracted_msg))
		{
			//here the message to send needs to be truncated to the first '\n',
			//otherwise the output of our binary and the tester's wont match
			char **msg = chose_between_queued_msg_and_extracted_msg(clients);

			res = write(clients->fd, *msg, strlen(*msg));
			//just skip to next client if pipe was full for example.
			if (res < 0)
				;
			else if ((*msg)[res] == '\0') //quicker than strlen.
			{
				free(*msg);
				*msg = NULL; //the reason why we needed the adress of msg.
			}
			else //do a sort of memmove, no need to realloc.
				ft_memmove(*msg, res);
		}
		clients = clients->next;
	}
}

int main(int argc, char **argv)
{
	int sockfd;

	//DELETE
	int i = 0;

	t_fd_sets fd_sets_real;
	t_fd_sets fd_sets_copy;
	t_list *clients = NULL;

	//nb: subject mentions error only if no argument is given
	if (argc < 2)
		fatal_args();
	if ((sockfd = set_up_server_socket(atoi(argv[1]))) < 0)
		fatal_error();

	//reset and add the sockfd to the set of fd to be monitored for a read.
	bzero(&fd_sets_real, sizeof(fd_sets_real));
	FD_SET(sockfd, &fd_sets_real.fd_set_read);

	while (1)
	{
		//call to select
		fd_sets_copy = fd_sets_real;
		if (select(1024, &(fd_sets_copy.fd_set_read), &(fd_sets_copy.fd_set_write), &(fd_sets_copy.fd_set_excepts), NULL) < 0)
			fatal_error();
		printf("\nexited select: %d\n", i++);
		sleep(1);

		//call to accept
		try_to_accept_new_clients(sockfd, &clients, fd_sets_copy, &fd_sets_real);

		resume_reading_from_clients(&fd_sets_real, &fd_sets_copy, &clients);

		resume_writing_to_clients(fd_sets_copy, clients);
	}
}
