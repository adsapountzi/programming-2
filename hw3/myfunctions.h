#ifndef __MYFUNCTIONS_H__
#define __MYFUNCTIONS_H__

ssize_t my_read(int fd, void *buf, size_t size, int line);
void my_write(int fd, void *buf, size_t size, int line);
void examine(int result,int line);
#endif