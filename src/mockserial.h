#pragma once

#include <mbed.h>
#include <string>

class MockSerial {
public:
    MockSerial(unsigned int baud = 9600);

    int putc(int c);

    int puts(const char *str);
    
    int getc();

    void attach(Callback<void()> func, SerialBase::IrqType type = SerialBase::RxIrq);

    void reset();

    void setExpectString(const string &s) { expect_str = s; }

    void setResponse(const char *str);

    void baud(unsigned int b) { _baud = b; }

protected:
    Callback<void()> _func;
    Thread          _thr;

    char            put_buf[1024];
    char            *p_put_buf; 

    string          expect_str;
    bool            thr_flag;
    CircularBuffer<char, 256>   response_buf;

    unsigned int    _baud;

private:
    void    thread_func();
};