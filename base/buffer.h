#ifndef BUFFER_H
#define BUFFER_H


typedef struct {
    char *data;
    int max_size;   // buffer ��С
    int read_idx;   // ��ǰ����λ��
    int write_idx;  // ��ǰд��λ��
} Buffer;

#endif  // BUFFER_H