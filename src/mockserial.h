/*
 *  Copyright (C) 2018  Digital Incubation & Growth GmbH
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  This software is dual-licensed. For commercial licensing options, please
 *  contact the authors (see README).
 */

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