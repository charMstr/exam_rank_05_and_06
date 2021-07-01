/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 02:59:01 by charmstr          #+#    #+#             */
/*   Updated: 2021/07/01 04:45:13 by charmstr         ###   ########.fr       */
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

typedef struct s_handle
{
	fd_set real;
	int id_for_next_client;
	int nb_connected;
}		t_handle;

typedef struct s_job
{
	char *msg;
	int position_in_msg;
	int *nb_references;
	struct s_job *next;
} t_job;

typedef struct s_lst_cli
{
	int fd;
	int id;
	char *queue;
	t_job *jobs_list;
	struct s_lst_cli *next;
	struct s_lst_cli *previous;
} t_lst_cli;

void	fatal(void)
{
	write(2, "Fatal error\n", strlen(	"Fatal error\n"));
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
				fatal();
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
		fatal();
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	//free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

int set_up_server(int port)
{
	int sockfd;
	struct sockaddr_in servaddr;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fatal();
	}
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
		close(sockfd);
		fatal();
	}
	if (listen(sockfd, 10) != 0) {
		close(sockfd);
		fatal();
	}
	return(sockfd);
}

void	push_front_new_client(t_lst_cli **clients, int id, int fd)
{
	t_lst_cli *new_client;

	if (!(new_client = malloc(sizeof(t_lst_cli))))
		fatal();
	bzero(new_client, sizeof(t_lst_cli));
	new_client->id = id;
	new_client->fd = fd;
	new_client->next = *clients;
	if (*clients)
		(*clients)->previous = new_client;
	*clients = new_client;
}

int *create_nb_ref(int nb_connected)
{
	int *nb_ref;

	if (!(nb_ref = malloc(sizeof(int))))
		fatal();
	*nb_ref = nb_connected -1;
	return(nb_ref);
}

void	add_job_to_all_other_clients(char *msg, int *nb_ref, t_lst_cli *clients, int id_to_avoid)
{
	t_job *new_job;
	t_job *tmp;

	while (clients)
	{
		if (clients->id == id_to_avoid)
		{
			clients = clients->next;
			continue;
		}
		if (!(new_job = malloc(sizeof(t_job))))
			fatal();
		bzero(new_job, sizeof(t_job));
		new_job->msg = msg;
		new_job->nb_references = nb_ref;
		if (!clients->jobs_list)
			clients->jobs_list = new_job;
		else
		{
			tmp = clients->jobs_list;
			while(tmp && tmp->next)
				tmp = tmp->next;
			tmp->next = new_job;
		}
		clients = clients->next;
	}
}

void	add_intro_job_to_others(char *str, int id, t_lst_cli *clients, int nb_connected)
{
	char intro_buffer[100];
	char *msg;
	int *nb_refs;

	if (nb_connected < 2)
		return ;
	sprintf (intro_buffer, str, id);
	msg = str_join("", intro_buffer);
	nb_refs = create_nb_ref(nb_connected);
	add_job_to_all_other_clients(msg, nb_refs, clients, id);
}

void try_accept_client(int sockfd, t_handle *handle, fd_set copy, t_lst_cli **clients)
{
	int connfd, len;
	struct sockaddr_in  cli;

	if (FD_ISSET(sockfd, &copy))
	{
		len = sizeof(cli);
		connfd = accept(sockfd, (struct sockaddr *)&cli,(socklen_t*)&len);
		if (connfd < 0) {
			fatal();
		}
		FD_SET(connfd, &(handle->real));
		int id = handle->id_for_next_client;
		handle->id_for_next_client++;
		handle->nb_connected++;
		push_front_new_client(clients, id, connfd);
		add_intro_job_to_others("server: client %d just arrived\n", \
			id, *clients, handle->nb_connected);
	}
}

void resume_writing(fd_set copy, t_lst_cli *clients)
{
	int res;
	t_job *job;

	while (clients)
	{
		if (FD_ISSET(clients->fd, &copy) && clients->jobs_list)
		{
			job = clients->jobs_list;
			res = write(clients->fd, job->msg + job->position_in_msg, strlen(job->msg) - job->position_in_msg);
			if (res < 0)
			{
				clients = clients->next;
				continue;
			}
			job->position_in_msg += res;
			if (job->msg[job->position_in_msg] == '\0')
			{
				clients->jobs_list = job->next;
				*(job->nb_references) -= 1;
				if (*(job->nb_references) == 0)
				{
					free(job->msg);
					free(job->nb_references);
				}
				free(job);
			}
		}
		clients = clients->next;
	}
}

void	extract_jobs(t_lst_cli *clients, int nb_connected, int id, char **queue)
{
	int res;
	char *msg;
	char intro[100];
	int *nb_ref;

	sprintf(intro, "client %d: ", id);
	while(1)
	{
		res = extract_message(queue, &msg);
		if (res <= 0)
			break;
		char *tmp = msg;
		msg = str_join(intro, msg);
		free(tmp);
		nb_ref = create_nb_ref(nb_connected);
		add_job_to_all_other_clients(msg, nb_ref, clients, id);
	}
}

void free_job_list(t_job *list)
{
	t_job *to_del;

	while (list)
	{
		to_del = list;
		list = list->next;
		*(to_del->nb_references) -=1;
		if (!(*(to_del->nb_references)))
		{
			free(to_del->msg);
			free(to_del->nb_references);
		}
		free(to_del);
	}
}

void resume_reading(t_handle *handle, fd_set copy, t_lst_cli **clients)
{
	t_lst_cli *cur = *clients;
	int res;
	char buffer[65536];

	while(cur)
	{
		if (FD_ISSET(cur->fd, &copy))
		{
			res = read(cur->fd, buffer, 65535);
			if (res <= 0)
			{
				FD_CLR(cur->fd, &(handle->real));
				close(cur->fd);
				add_intro_job_to_others("server: client %d just left\n", cur->id, *clients, handle->nb_connected);
				handle->nb_connected--;

				t_lst_cli *const next = cur->next;
				if (next)
					next->previous = cur->previous;
				if (cur->previous)
					cur->previous->next = next;
				else
					*clients = next;
				free_job_list(cur->jobs_list);
				free(cur->queue);
				free(cur);
				cur = next;
				continue;
			}
			buffer[res] = '\0';
			if (handle->nb_connected < 2)
			{
				free(cur->queue);
				cur->queue = NULL; //reset the queued since there
				//is no one to listen...
				cur = cur->next;
				continue;
			}
			char *tmp = cur->queue;
			cur->queue = str_join(cur->queue, buffer);
			free(tmp);
			extract_jobs(*clients, handle->nb_connected, cur->id, &cur->queue);
		}
		cur = cur->next;
	}
}


void debug(t_lst_cli *cli)
{
	t_job *job;
	int i;

	while(cli)
	{
		printf("\033[32mclient %d:\n\033[m", cli->id);
		job = cli->jobs_list;
		i = 0;
		while (job)
		{
			printf("\tjob %d: |\033[31m%s\033[m|\n", i, job->msg);
			job = job->next;
		}

		cli = cli->next;
	}

}

int main(int argc, char **argv)
{
	int sockfd;
	fd_set copy_write;
	fd_set copy_read;
	t_handle handle;
	t_lst_cli *clients = NULL;

	if (argc < 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	sockfd = set_up_server(atoi(argv[1]));
	bzero(&handle, sizeof(t_handle));
	FD_SET(sockfd, &handle.real);

	while (1)
	{
		copy_read = handle.real;
		copy_write = handle.real;
		if (select(1024, &copy_read, &copy_write, NULL, NULL) < 0)
			fatal();
		printf("\nEXITING SELECT\n");

		try_accept_client(sockfd, &handle, copy_read, &clients);
		resume_reading(&handle, copy_read, &clients);

		debug(clients);
		resume_writing(copy_write, clients);
	}
}
