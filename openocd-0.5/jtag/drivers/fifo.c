/*
 * fifo-server.c
 *
 *  Created on: Apr 19, 2012
 *      Author: yangyy
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/time.h>
#include "fifo.h"
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <log.h>

#define FIFO_OPEN_TIMEOUT 100000 // micro second
#define FIFO_OPEN_REPEAT  -1	// negetive means unlimited

static char mapu_fifo_dir[64];
static const char* fifo_name[2] = {"TEMPORARY-FIFO-0", "TEMPORARY-FIFO-1"};
// server read  <--- fifo0 <---- client write
// server write  ---> fifo1 ----> client read

// fifo_path = mapu_fifo_dir + fifo_name
static char fifo_path[2][128];

static int fd_for_read = -1;
static int fd_for_write = -1;

static int
set_mapu_dir(void)
{
	char*home = getenv("HOME");
	int res, i;
	struct stat st;

	strcpy(mapu_fifo_dir, home);
	strcat(mapu_fifo_dir, "/.mapu.d/");

	res = stat(mapu_fifo_dir, &st);
	if(res == 0)    // dir already exists
	assert(S_ISDIR(st.st_mode));
	else {
		res = mkdir(mapu_fifo_dir, S_IRWXU | S_IRWXG | S_IRWXO);    // a dir MUST be X mode to create a file into it.
		res = chmod(mapu_fifo_dir, S_IRWXU | S_IRWXG | S_IRWXO);
	}

	for(i = 0; i < 2; i++) {
		strcpy(fifo_path[i], mapu_fifo_dir);
		strcat(fifo_path[i], fifo_name[i]);

		res = stat(fifo_path[i], &st);
		if(res == 0) { // file already exists
			if( !S_ISFIFO(st.st_mode) )
				res = remove(fifo_path[i]);
			else  {
				res = chmod(fifo_path[i], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				continue;
			}
		}
		res = mkfifo(fifo_path[i], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		res = chmod(fifo_path[i], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	}
	return 0;
}

static int
open_fifo(int flags/*, int timeout*/)
{
	char op0[] = "read", op1[] = "write";
	char obj0[] = "simulator", obj1[] = "openocd";

	int ret, mode;
	char * path, *op, *obj;

	switch(flags & 0x3) {
		case (SERVER_SIDE | READ_PORT):
			mode = (O_RDONLY | O_NONBLOCK);
			path = fifo_path[0];
			op = op0;
			obj = obj0;
			break;
		case (CLIENT_SIDE | WRITE_PORT):
			mode = (O_WRONLY | O_NONBLOCK);
			path = fifo_path[0];
			op = op1;
			obj = obj1;
			break;
		case (SERVER_SIDE | WRITE_PORT):
			mode = (O_WRONLY | O_NONBLOCK);
			path = fifo_path[1];
			op = op1;
			obj = obj0;
			break;
		case (CLIENT_SIDE | READ_PORT):
			mode = (O_RDONLY | O_NONBLOCK);
			path = fifo_path[1];
			op = op0;
			obj = obj1;
			break;
		default:
			break;
	}
	int open_cnt = 0;
	do {
		ret = open(path, mode);
		if(ret < 0) {
			open_cnt++;
			if(open_cnt % 100 == 0) LOG_INFO("%d seconds has elapsed when %s tries to open fifo's %s port...",
											FIFO_OPEN_TIMEOUT * 100/1000000, obj, op);

			usleep(FIFO_OPEN_TIMEOUT);
		}
		else break;

		if(FIFO_OPEN_REPEAT < 0)
			continue;
		else if(open_cnt > FIFO_OPEN_REPEAT)
			break;
	} while(1);

	return ret;
}

int
read_fifo_timeout(char *buf, int size, int timeout, int repeat)
{
	struct timeval tv;
	fd_set readfds;
	int res, ret;

	int len = -100;    // arbitrary negtive other than -1
	int count;
	if(repeat > 0) count = repeat;

	// perform at least one read.
	// @a repeat is 0 means no repeat, 1 means repeat once(sums up to twice read), ...
	// 	  		   -1 means read loop forever until succeded.
	do {
		// initialization should be done every time, since data objects have been modified
		// when select() returns.
		FD_ZERO(&readfds);
		FD_SET(fd_for_read, &readfds);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		res = select(fd_for_read + 2, &readfds, NULL, NULL, &tv);    // blocking!

		if(res == -1) {
			perror("select");
			ret = -1;
		}
		else if(res == 0) {
			LOG_INFO("%d seconds has elapsed when trying to read from fifo.", timeout);
			ret = -1;
		}

		if(FD_ISSET(fd_for_read, &readfds)) {
			len = read(fd_for_read, buf, size);

			if(len == -1) {
				perror("read");
				ret = -1;
			}
			else if(len > 0) {
				LOG_DEBUG("read %d bytes (expected %d bytes) from fifo:", len, size);
				for(res = 0; res < len; res++) {
					LOG_DEBUG("(%02d)%02X ", res, (unsigned char) buf[res]);
					/*if((res>0) && (res % 10 == 0))
					 LOG_DEBUG("\n");*/
				}
				// LOG_DEBUG("\n");
				ret = len;
				break;    // read with a success
			}
		}

		if(repeat != -1) {
			count--;
			if(count < 0) break;    // repeat run out
		}
	} while(1);

	return ret;
}

int
write_fifo(const char *buf, int size)
{
	int ret = write(fd_for_write, buf, size);
	int res;
	if(ret != size) LOG_INFO("write to fifo error, expect to write %d bytes other than %d bytes", size, ret);
	else {
		LOG_DEBUG("write %d bytes (expected %d bytes) to fifo: ", ret, size);
		for(res = 0; res < ret; res++) {
			LOG_DEBUG("(%02d)%02X", res, (unsigned char) buf[res]);
			/*if((res + 1) % 10 == 0)
			 LOG_DEBUG("\n");*/
		}
//		LOG_DEBUG("\n");
	}

	return ret;
}

int
init_fifo_client_ext(const char* path)
{
	set_mapu_dir();

	int res;
	if(path != NULL) {
		int pid = fork();
		if(pid == 0) {
			LOG_INFO("---- openocd invoke the simulator at %s...", path);
			res = execl("/usr/bin/gnome-terminal", "gnome-terminal", "--window", "--geometry=145x25+20+0", "-e", path, NULL);
		}
	}

	char lock[128];
	const char suffix[] = ".client.lock";
	struct stat st;

	strcpy(lock, fifo_path[0]);
	strcat(lock, suffix);
	res = stat(lock, &st);

//	if(res != 0) { // usually lock does not exist
//		res = mkfifo(lock, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		fd_for_write = open_fifo(CLIENT_SIDE | WRITE_PORT);
//	}
//	else {
//		fprintf(stdout, "----!!!! fifo is in use by other openocd process, please check !!!!----\n");
//		exit(-1);
//	}

	if(fd_for_write >= 0) {
		LOG_INFO("---- openocd opened fifo's write port successfully.");

		strcpy(lock, fifo_path[1]);
		strcat(lock, suffix);
		res = stat(lock, &st);

//		if(res != 0) { // usually lock does not exist
//			res = mkfifo(lock, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			fd_for_read = open_fifo(CLIENT_SIDE | READ_PORT);
//		}
//		else {
//			fprintf(stdout, "----!!!! fifo is in use by other openocd process, please check !!!!----\n");
//			exit(-1);
//		}

		if(fd_for_read >= 0) {
			LOG_INFO("---- openocd opened fifo's read port successfully.");
			LOG_INFO("---- openocd connected to a simulator.");
			return 0;
		}
		else {
			LOG_INFO("---- openocd failed to open fifo's read port.");
			return -1;
		}
	}
	else {
		LOG_INFO("---- openocd failed to open fifo's write port.");
		return -1;
	}
}

int
init_fifo_server(void)
{
	set_mapu_dir();

	int res;
	char lock[128];
	const char suffix[] = ".server.lock";
	struct stat st;

	strcpy(lock, fifo_path[0]);
	strcat(lock, suffix);
	res = stat(lock, &st);

//	if(res != 0) { // usually lock does not exist
//		res = mkfifo(lock, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		fd_for_read = open_fifo(SERVER_SIDE | READ_PORT);
//	}
//	else {
//		fprintf(stdout, "----!!!! fifo is in use by other simulator process, please check !!!!----\n");
//		exit(-1);
//	}

	if(fd_for_read >= 0) {
		LOG_INFO("---- simulator opened fifo's read port successfully.");

		strcpy(lock, fifo_path[1]);
		strcat(lock, suffix);
		res = stat(lock, &st);

//		if(res != 0) { // usually lock does not exist
//			res = mkfifo(lock, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			fd_for_write = open_fifo(SERVER_SIDE | WRITE_PORT);
//		}
//		else {
//			fprintf(stdout, "----!!!! fifo is in use by other simulator process, please check !!!!----\n");
//			exit(-1);
//		}

		if(fd_for_write >= 0) {
			char str[] = "---- simulator opened fifo's write port successfully."
						 "---- simulator accepted connection from openocd.";
			LOG_INFO(str);
			return 0;
		}
		else {
			LOG_INFO("---- simulator failed to open fifo's write port.");
			return -1;
		}
	}
	else {
		LOG_INFO("---- simulator failed to open fifo's read port.");
		return -1;
	}
}

int
deinit_fifo_client(void)
{
	int res;
	char lock[128];
	const char suffix[] = ".client.lock";
	struct stat st;
	int i;
	for(i = 0; i < 2; i++) {
		strcpy(lock, fifo_path[i]);
		strcat(lock, suffix);

		res = stat(lock, &st);

		// usually lock does exist
		if(res == 0) res = remove(lock);
	}

	return 0;
}

int
deinit_fifo_server(void) {
	int res;
	char lock[128];
	const char suffix[] = ".server.lock";
	struct stat st;
	int i;
	for(i = 0; i < 2; i++) {
		strcpy(lock, fifo_path[i]);
		strcat(lock, suffix);

		res = stat(lock, &st);

		// usually lock does exist
		if(res == 0) res = remove(lock);
	}

	return 0;
}
