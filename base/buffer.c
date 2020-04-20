#include "buffer.h"

static const int kMaxBufferSize = 1024;

// ��ȡ�����д�ռ�
static int GetBackWriteableSize(Buffer *buffer)
{
    return buffer->max_size - buffer->write_idx;
}

// ��ȡǰ���д�ռ�
static int GetFrontWriteableSize(Buffer *buffer)
{
    return buffer->read_idx;
}

// ��ȡ�ɶ�����
static int GetReadableSize(Buffer *buffer)
{
    return buffer->write_idx - buffer->read_idx;
}

// �ƶ��ɶ�����
static void MoveReadableData(Buffer *buffer)
{
    int readable_size = GetReadableSize(buffer);
    // ʹ������������ɣ���Ϊ�������ڴ���򸲸ǵ����������Ҫʹ�� memmove()
    memcpy(buffer->data, buffer->data + buffer->read_idx, readable_size);
    buffer->read_idx = 0;
    buffer->write_idx = readable_size;
}

// ����Ƿ����㹻�ռ��д
static int CheckSize(Buffer *buffer, uint32_t size)
{
    int back_size = GetBackWriteableSize(buffer);
    if (back_size >= size) {
        return 1;
    }
    back_size += GetFrontWriteableSize(buffer);
    if (back_size >= size) {
        MoveReadableData(buffer);
        return 1;
    }
    return 0;
}

// �����ǰ�ռ䲻�㣬����չ�ռ�
// ���� -1 ��ʾ��չʧ��
static int Resize(Buffer *buffer, uint32_t size)
{
    if (CheckSize(buffer, size) == 1) {
        return 0;
    }
    // �ռ䲻������չ
    void *tmp = realloc(buffer->data, buffer->max_size + size);
    if (tmp == NULL) {
        LogError("buffer Resize realloc failed");
        return -1;
    }
    buffer->data = tmp;
    buffer->max_size += size;
    return 0;
}

Buffer* BufferInit()
{
    Buffer *buffer = (Buffer *)malloc(sizeof(buffer));
    if (buffer == NULL) {
        ErrorExit("BufferInit malloc buffer failed");
    }
    buffer->data = malloc(kMaxBufferSize);
    if (buffer->data == NULL) {
        ErrorExit("BufferInit malloc buffer data failed");
    }
    buffer->max_size = kMaxBufferSize;
    buffer->read_idx = 0;
    buffer->write_idx = 0;

    return buffer;
}

void BufferDestroy(Buffer *buffer)
{
    free(buffer->data);
    free(buffer);
}

int BufferWrite(Buffer *buffer, const void *data, uint32_t size)
{
    if (data == NULL) {
        return -1;
    }
    if (Resize(buffer, size) == -1) {
        LogError("buffer BufferWrite failed");
        return -1;
    }
    // ���Ƶ� buffer ��
    memcpy(buffer->data + buffer->write_idx, data, size);
    buffer->write_idx += size;
    return 0;
}

int BufferWriteFromSocket(Buffer *buffer, int fd)
{
    char tmp_buffer[kMaxBufferSize];
    struct iovec iov[2];
    int back_size = GetBackWriteableSize(buffer);
    iov[0].iov_base = buffer->data + buffer->write_idx;
    iov[0].iov_len = back_size;
    iov[1].iov_base = tmp_buffer;
    iov[1].iov_len = kMaxBufferSize;
    int ret = readv(fd, iov, 2);
    if (ret < 0) {
        return -1;
    } else if (ret < back_size) {
        buffer->write_idx += ret;
    } else {
        buffer->write_idx = buffer->max_size;
        BufferWrite(buffer, tmp_buffer, ret - back_size);
    }
    return ret;
}

int BufferRead(Buffer *buffer, void *data, uint32_t size)
{
    
}