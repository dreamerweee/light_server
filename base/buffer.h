#ifndef BUFFER_H
#define BUFFER_H


typedef struct {
    char *data;
    int max_size;   // buffer 大小
    int read_idx;   // 当前读的位置
    int write_idx;  // 当前写的位置
} Buffer;

#endif  // BUFFER_H