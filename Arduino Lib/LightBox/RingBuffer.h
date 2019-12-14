#ifndef LIGHT_RIGHT_BUFFER_H
#define LIGHT_RIGHT_BUFFER_H


template<class T>
class RingBuffer
{
public:
    RingBuffer(uint8_t bufferSZ = 0): bufferSize(bufferSZ), currentOffset(0)
    {
        data = new T[bufferSZ];
    }

    T* buffer()
    {
        return data;
    }

    void set(uint8_t index, T value)
    {
        uint8_t accessIndex = getIndex(max(currentOffset + index, 0));
        data[accessIndex] = value;
    }

    void push(T value)
    {
        data[currentOffset] = value;
        currentOffset = getIndex(currentOffset + 1);
    }

    T get(uint8_t index)
    {
        uint8_t accessIndex = getIndex(max(currentOffset + index, 0));
        return data[accessIndex];
    }

    uint8_t size()
    {
        return bufferSize;
    }

private:

    uint8_t getIndex(uint8_t index)
    {
        return index % bufferSize;
    }

    uint8_t bufferSize;
    T* data;
    uint8_t currentOffset;


};



#endif