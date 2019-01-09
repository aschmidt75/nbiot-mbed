
#include <mbed.h>
#include "mockserial.h"


MockSerial::MockSerial(unsigned int baud) : _baud(baud) {
    reset();
    _thr.start(callback(this,&MockSerial::thread_func));
}

void MockSerial::reset() {
    memset(put_buf, 0, sizeof(put_buf));
    p_put_buf = put_buf;
    thr_flag = false;
}

void MockSerial::setResponse(const char *str) {
    const char *p = str;
    while (*p && !response_buf.full()) {
        response_buf.push(*p++);
    }    
}

int MockSerial::putc(int c) {
    *p_put_buf++ = c;
    return 0;
}

int MockSerial::puts(const char *str) {
    const char *p = str;
    while (*p) {
        putc(*p++);
    }    
    return 0;
}

int MockSerial::getc() {
    char c;
    if ( response_buf.pop(c)) {
        return c;
    }
    return -1;
}

void MockSerial::attach(Callback<void()> func, SerialBase::IrqType type) {
    _func = func;
}

void MockSerial::thread_func() {
    for(;;) {
        if ( thr_flag == false) {
            if ( expect_str == put_buf) {
               thr_flag = true; 
            }
        } else {
            if ( !response_buf.empty()) {
                _func();

                wait_us((1000L*1000L)/_baud);
            } else {
                thr_flag = false;
            }
        }
    }
}
