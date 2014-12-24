/*
 * fifo.h
 *
 *  Created on: Apr 19, 2012
 *      Author: yangyy
 */

#ifndef FIFO_H

#define FIFO_H

/* How to use:
 * (1) call init_fifo_server(void) inside simulator.
 * (2) call init_fifo_client_ext(const char* path) inside openocd, with path to specify
 * 	   simulator's executable file.
 * (3) call write_fifo() to send data; call read_fifo_timeout() to receive data.
 * */

enum _fifo_flag_
{
	SERVER_SIDE = 0, CLIENT_SIDE = 1, READ_PORT = 0, WRITE_PORT = 2
};

int
init_fifo_server(void);

/* use @a path to specify where the simulator's binary image locates.
 * To run openocd, type like "openocd -e path".
 * */
int
init_fifo_client_ext(const char* path);

int
deinit_fifo_client(void);

int
deinit_fifo_server(void);

////////////// before using read_fifo_timeout or write_fifo, make sure init_fifo_xxxx has been called.

/* @a timeout: read_fifo_timeout() will sleep for @a timeout seconds when it fails to read.
 * 			   and it will try @repeat times to read the fifo.
 * */
int
read_fifo_timeout(char * buf, int size, int timeout, int repeat);

int
write_fifo(const char *buf, int size);
#endif // fifo.h
