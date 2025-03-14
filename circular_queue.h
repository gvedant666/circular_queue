#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>

#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int *)
#define PUSH_DATA _IOW('a', 'b', struct data *)
#define POP_DATA _IOR('a', 'c', struct data *)

struct data {
    char *data;
    int length;
};


struct queue {
    struct data *ptr;
    int capacity;
    int size;
    int start;
    int end;
};


int SET_SIZE_OF_QUEUE(int size);
int PUSH_DATA(struct data *data);
int POP_DATA(struct data *data);
long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif
