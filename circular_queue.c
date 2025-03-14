#include "circular_queue.h"

static struct queue *circ_queue;
static DEFINE_MUTEX(queue_mutex);

int SET_SIZE_OF_QUEUE(int size) {
    mutex_lock(&queue_mutex);
    
    circ_queue = kmalloc(sizeof(struct queue), GFP_KERNEL);

    circ_queue->capacity = size;
    circ_queue->size = 0;
    if (circ_queue == NULL) {
        mutex_unlock(&queue_mutex);
        return -1;
    }
    circ_queue->ptr = kmalloc(size * sizeof(struct data), GFP_KERNEL);
    circ_queue->start = 0;
    circ_queue->end = 0;
    
    mutex_unlock(&queue_mutex);
    return 0;
}

int PUSH_DATA(struct data *data) {
    mutex_lock(&queue_mutex);
    if (circ_queue->size == circ_queue->capacity) {
        mutex_unlock(&queue_mutex);
        return -1;
    }
    
    circ_queue->ptr[circ_queue->start] = *data;
    circ_queue->size++;
    circ_queue->start = (circ_queue->start + 1) % circ_queue->capacity;
    
    mutex_unlock(&queue_mutex);
    return 0;
}

int POP_DATA(struct data *data) {
    mutex_lock(&queue_mutex);
    if (circ_queue->size == 0) {
        mutex_unlock(&queue_mutex);
        return -1;
    }
    
    *data = circ_queue->ptr[circ_queue->end];
    circ_queue->size--;
    circ_queue->end = (circ_queue->end + 1) % circ_queue->capacity;
    
    mutex_unlock(&queue_mutex);
    return 0;
}

long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ret = 0;
    struct data data;

    mutex_lock(&queue_mutex);
    
    if (cmd == SET_SIZE_OF_QUEUE) {
        int size;
        copy_from_user(&size, (int __user *)arg, sizeof(int));
        if(size <= 0) ret = -1;
        else ret = SET_SIZE_OF_QUEUE(size);
    } 
    else if (cmd == PUSH_DATA) {
        if (copy_from_user(&data, (struct data __user *)arg, sizeof(struct data))) ret = -1;
        else ret = PUSH_DATA(&data);
    } 
    else if (cmd == POP_DATA) {
        if (POP_DATA(&data) == 0) {
            if (copy_to_user((struct data __user *)arg, &data, sizeof(struct data))) ret = -1;
        } else ret = -1;
    } 
    else ret = -1;
    
    mutex_unlock(&queue_mutex);
    return ret;
}

static struct file_operations fops = {
    .unlocked_ioctl: device_ioctl
};


static int __init circular_queue_init(void) {
    return 0;
}

static void __exit circular_queue_exit(void) {
    if (circ_queue != NULL) {
        kfree(circ_queue->ptr);
        kfree(circ_queue);
    }
}


module_init(circular_queue_init);
module_exit(circular_queue_exit);
MODULE_LICENSE("GPL");
