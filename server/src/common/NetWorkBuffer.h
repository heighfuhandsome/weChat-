#pragma once
#include <vector>
#include <iostream>
#include <string>
class NetWorkBuffer{
public:
    NetWorkBuffer(size_t size = 1024):read_index_(0),write_index_(0),size_(size){ buffer_ = new char[size]; }
    ~NetWorkBuffer(){ delete[] buffer_; }
    inline size_t readAbleBytes() const { return write_index_-read_index_; }
    inline size_t writeAbleBytes() const { return size_ - write_index_; }

    inline void retrieveN(size_t n){ if(n>readAbleBytes()){return; } read_index_+=n; }
    std::string retrivevAsString(size_t n);
    std::string retrivevAllString();
    uint16_t revrieveUInt16();

    void writeString(const std::string &str);
    void write(const void *data,size_t len);
    void prepareUInt16(uint16_t n);
    inline char* peek(){ return buffer_+read_index_; }
private:
    void ensureWriteSafe(size_t len);

    char* buffer_;
    size_t read_index_;
    size_t write_index_;
    size_t size_;
};