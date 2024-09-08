#include "NetWorkBuffer.h"
#include "Logger.h"
#include <cstring>
#include <arpa/inet.h>
std::string NetWorkBuffer::retrivevAsString(size_t n)
{
    if(readAbleBytes() < n)
        return "";

    auto str = std::string(peek(),n);
    retrieveN(n);
    return str;
}

std::string NetWorkBuffer::retrivevAllString()
{
    return retrivevAsString(readAbleBytes()); 
}

uint16_t NetWorkBuffer::revrieveUInt16()
{
    uint16_t num;
    if(readAbleBytes() < 2)
        return 0;
    
    ::memcpy(&num,peek(),2);
    retrieveN(2);
    return ntohs(num); 
}

void NetWorkBuffer::writeString(const std::string &str)
{
    write(str.c_str(),str.size());
}

void NetWorkBuffer::write(const void *data, size_t len)
{
    ensureWriteSafe(len);
    ::memcpy( buffer_ + write_index_, data, len);
    write_index_ += len;
}

void NetWorkBuffer::prepareUInt16(uint16_t n)
{
    uint16_t tmp = htons(n);
    write(&tmp,sizeof(n));
}

void NetWorkBuffer::ensureWriteSafe(size_t len)
{
    if(len <= writeAbleBytes())
        return;
    
    if(read_index_ + writeAbleBytes() >= len){
        ::memcpy(buffer_,peek(),readAbleBytes());
        write_index_ = readAbleBytes();
        read_index_ = 0;
        return;
    }

    char *newBuffer = new char[size_ + len];
    ::memcpy(newBuffer,buffer_,size_);
    delete[] buffer_;
    buffer_ = newBuffer;
    size_ += len;
}
