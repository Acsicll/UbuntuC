#ifndef __CONST_H__
#define __CONST_H__
#include <iostream>
#include <tuple>
#include <vector>
#include <cstring>
#include <string>
#include "const.h"

class Buffer{
    public:
        virtual ~Buffer() = default;
        virtual void reset() = 0;
        virtual void write(const char* data, unsigned int length) = 0;
        virtual void read(char* data,unsigned int maxLenght) = 0;
        virtual void print() const = 0;
        virtual tuple<char*,unsigned int> getRawData();
};

class ConcreateBuffer : public Buffer{
    protected:
        std::vector<char>_buffer;
        unsigned int _length;
    public:
        ConcreateBuffer(unsigned int size = BUFFER_LENGTH) : _buffer(size , 0),_length(0) {}
        void reset() override {
            memset(_buffer.data(),0,_buffer.size());
            _length = 0;
        }
        void write(const char* data, unsigned int len) override {
            if( len <= _buffer.size()){
                memcpy(_buffer.data(),data,len);
                _length = len;
            }else{
                cerr << "< ConcreateBuffer overflow error! >"<<endl;
            }
        }
        void read(char* data, unsigned int maxLength) override {
            unsigned int readLength = std::min(maxLength,_length);
            memcpy(data,_buffer.data(),readLength);
            data[readLength] = '\0';
        }
        void print() const override {
            cout << " [ ConcreateBuffer Data : "<< string(_buffer.data(),_length)<<" ]"<<endl;
        }
        tuple<char*,unsigned int> getRawData(){
            if(_buffer.size()==0){
                return {nullptr , 0};
            }
            return {_buffer.data(),_length};
        }
};

class BufferDecorator : public Buffer {
    protected:
        Buffer* decoratedBuffer;
    public:
        BufferDecorator(Buffer* buffer) : decoratedBuffer(buffer) {}
        void reset() override {
            decoratedBuffer->reset();
        }
        void write(const char* data, unsigned int len) override {
            decoratedBuffer->write(data,len);
        }
        void read(char* data, unsigned int maxLength) override {
            decoratedBuffer->read(data,maxLength);
        }
        void print() const override {
            decoratedBuffer->print();
        }
        tuple<char*,unsigned int> getRawData(){
            return decoratedBuffer->getRawData();
        }
};

class SendBuffer : public BufferDecorator {
    public:
        SendBuffer(Buffer* buffer) : BufferDecorator(buffer) {}
        void print() const override {
            cout<< " [ SendBuffer Data : ";
            BufferDecorator::print();
            cout<<" ]"<<endl;
        }
        void WriteToNetWork(int(*WriteToNetWorkCallback)(int,char*,int),int fd){
            tuple<char*,unsigned int> tmp = getRawData();
            char* buffer = get<0>(tmp);
            int len = get<1>(tmp);
            if(buffer == nullptr || len == 0){
                return;
            }
            WriteToNetWorkCallback(fd,buffer,len);
        }

};

class RecvBuffer : public BufferDecorator {
    public:
        RecvBuffer(Buffer* buffer) : BufferDecorator(buffer) {}
        void print() const override {
            std::cout << "[ RecvBuffer Data : ";
            BufferDecorator::print();
            cout<<" ]"<<endl;
        }
        void receiveFromNetwork(int(*RecvFromNetworkCallback)(int,char*,int),int fd){
            tuple<char*,unsigned int> tmp = getRawData();
            char* buffer = get<0>(tmp);
            int len = get<1>(tmp);
            if(buffer == nullptr || len == 0){
                return;
            }
            RecvFromNetworkCallback(fd,buffer,len);
        }        
};
#endif