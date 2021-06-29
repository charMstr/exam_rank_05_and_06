/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/26 20:19:59 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/29 07:46:17 by charmstr         ###   ########.fr       */
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
				fatal_error();
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

int	set_up_server_socket(int port)
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return (-1);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);
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
	char *queued_msgs;
	char *extracted_msg;
	struct s_list *next;
	struct s_list *previous;
}			t_list;

void	push_front_new_client(t_list **clients, int fd, int id)
{
	t_list *new_client;
	if (!(new_client = malloc(sizeof(t_list))))
		fatal_error();
	bzero(new_client, sizeof(*new_client));
	new_client->fd = fd;
	new_client->id = id;
	new_client->next = *clients;
	if (*clients)
		(*clients)->previous = new_client;
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

void try_to_accept_new_clients(t_fd_sets *real, t_fd_sets copy, t_list **clients, int sockfd)
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

void resume_reading_from_clients(t_fd_sets *real_set, t_fd_sets copy, t_list **clients)
{
	char buffer[65535];
	t_list *current_client = *clients;

	while (current_client)
	{
		if (FD_ISSET(current_client->fd, &copy.fd_set_read))
		{
			int res;
			res = read(current_client->fd, buffer, 65535);
			if (res == -1)
				current_client = current_client->next;
			else if (res == 0)
			{
				FD_CLR(current_client->fd, &real_set->fd_set_read);
				FD_CLR(current_client->fd, &real_set->fd_set_write);
				char intro_buffer[100];
				sprintf(intro_buffer, "server: client %d just left\n", current_client->id);
				add_msg_to_all_other_clients(intro_buffer, current_client->fd, *clients);
				t_list * const next = current_client->next;
				if (next)
					next->previous = current_client->previous;
				if (current_client->previous)
					current_client->previous->next = next;
				else
					*clients = next;
				free(current_client->queued_msgs);
				free(current_client->extracted_msg);
				free(current_client);
				current_client = next;
			}
			else
			{
				char *composed_msg;
				composed_msg = compose_message_to_send_to_others(res, buffer, current_client->id);
				add_msg_to_all_other_clients(composed_msg, current_client->fd, *clients);
				free(composed_msg);
				current_client = current_client->next;
			}
		}
		else
			current_client = current_client->next;
	}
}

char **chose_between_queued_msg_and_extracted_msg(t_list *client)
{
	int res;
	char **msg;

	if (client->extracted_msg)
		msg = &client->extracted_msg;
	else
	{
		if (!(res = extract_message(&client->queued_msgs, &client->extracted_msg)))
			msg = &client->queued_msgs;
		else
			msg = &client->extracted_msg;
	}
	return(msg);
}

void resume_writing_to_clients(t_fd_sets copy, t_list *clients)
{
	int res;

	while (clients)
	{
		if (FD_ISSET(clients->fd, &copy.fd_set_write) \
				&& (clients->queued_msgs || clients->extracted_msg))
		{
			char **msg = chose_between_queued_msg_and_extracted_msg(clients);
			res = write(clients->fd, *msg, strlen(*msg));
			if (res < 0)
				;
			else if ((*msg)[res] == '\0')
			{
				free(*msg);
				*msg = NULL;
			}
			else
				ft_memmove(*msg, res);
		}
		clients = clients->next;
	}
}

int main(int argc, char **argv)
{
	int sockfd;
	t_fd_sets fd_sets_real;
	t_fd_sets fd_sets_copy;
	t_list *clients = NULL;

	if (argc < 2)
		fatal_args();
	if ((sockfd = set_up_server_socket(atoi(argv[1]))) < 0)
		fatal_error();
	bzero(&fd_sets_real, sizeof(fd_sets_real));
	FD_SET(sockfd, &fd_sets_real.fd_set_read);
	while (1)
	{
		fd_sets_copy = fd_sets_real;
		if (select(1024, &(fd_sets_copy.fd_set_read), &(fd_sets_copy.fd_set_write), &(fd_sets_copy.fd_set_excepts), NULL) < 0)
			fatal_error();
		try_to_accept_new_clients(&fd_sets_real, fd_sets_copy, &clients, sockfd);
		resume_reading_from_clients(&fd_sets_real, fd_sets_copy, &clients);
		resume_writing_to_clients(fd_sets_copy, clients);
	}
}
