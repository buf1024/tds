/*
 * sock.h
 *
 *  Created on: 2014-1-26
 *      Author: Luo Guochun
 */

#ifndef __SOCK_H__
#define __SOCK_H__

int tcp_svr(const char* addr, int port);
int tcp_connect(const char* addr, int port, int flag);

int set_opt(int fd, int opt);

int tcp_accept(int fd, char* ip, int* port);

#endif /* __SOCK_H__ */
