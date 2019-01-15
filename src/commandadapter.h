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

#include <string>
#include <list>
#include <map>
#include <cstddef>
#include <mbed.h>

using namespace std;

#include <modemresponse.h>

namespace Narrowband {

#ifdef __NBIOT_MBED_DEBUG_0
void debug_0_impl(const char *p, const size_t sz, char prefix);
#define debug_0(p,sz,prefix) debug_0_impl(p,sz,prefix)
#else
#define debug_0(a,b,c)
#endif

enum ModemCommandState {
    idle = 0,
    sending_command,
    receiving_response,
    receiving_unsolicited_response
};

class CommandAdapterBase {
public:
    // send command to modem, wait up to timeout msecs for response,
    // store in r. return true if response has been received in time. 
    virtual bool send(const char *p_cmd, ModemResponse& r, unsigned long timeout) = 0;

    virtual bool send(const char *p_cmd, Callback<void(ModemResponse&)> cb, unsigned long timeout) = 0;

};

/**
 */
template <typename T>
class CommandAdapter : public CommandAdapterBase {
public:
    CommandAdapter(T& modem);
    ~CommandAdapter();

    bool send(const char *p_cmd, ModemResponse& r, unsigned long timeout);

    bool send(const char *p_cmd, Callback<void(ModemResponse&)> cb, unsigned long timeout);

    ModemCommandState get_state() const { return _state; };

protected:
    void reset_buf();

    // attached to _modem, collects chars from modem in buffer. waits
    // for EOL, forwards lines to _queue.
    void recv_cb();

    // waits on _queue. parses lines into a ModemResponse. Sends
    // response to _mail
    void thread_cb();

    void set_state(ModemCommandState s) { _state = s; }
    bool ensure_state(ModemCommandState s, unsigned long timeout = 0);

    ModemResponseAlloc* get_current_response();

private:
    ModemCommandState               _state;

    T&                              _modem;
    CircularBuffer<char, 256>       _buf;                               // buffer characters from modem
    Queue<string, 16>               _queue;                             // queues string lines coming back from modem
    Thread                          _thread;                            // thread processes lines from _queue
    Mail<ModemResponseAlloc, 8>     _mail;                              // mailbox to receive ModemResponses

    ModemResponseAlloc              *_cur_response;                     // holds the response currently begin read from modem
};

}