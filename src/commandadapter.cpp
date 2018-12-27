/*
 *  Copyright (C) 2018  Digital Incubation & Growth GmbH
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. *
 *
 *  This software is dual-licensed. For commercial licensing options, please
 *  contact the authors (see README).
 */
#include <mbed.h>
#include <cstdlib>
#include "modemresponse.h"
#include "commandadapter.h"


namespace Narrowband {

void debug_0_impl(const char *p, const size_t sz, char prefix) {
#ifdef __NBIOT_MBED_DEBUG_0
    if ( sz <= 0 || p == 0) {
        return;
    }

    size_t s = 16;
    int rem_sz = (int)sz;
    char *pp1 = (char*)p;
    char *pp2 = (char*)p;
    while ( rem_sz > 0) {
        printf("%c [ ", prefix);
    
        for ( size_t i = 0; i < s; i++) {
            if ( (int)i < rem_sz) {
                printf("%02x ", *pp1++);
            } else {
                printf("   ");
            }   
        }

        printf("] [");
        for ( size_t i = 0; i < s; i++) {
            if ( (int)i < rem_sz) {
                printf("%c", (*pp2 >= 32)?*pp2:'.');
                pp2++;
            } else {
                printf(" ");
            }
        }
        printf("]\n");

        rem_sz -= s;
    }
#endif
}


template <typename T> 
CommandAdapter<T>::CommandAdapter(T& modem) : CommandAdapterBase(), _state(idle), _modem(modem), _cur_response(NULL) {
    reset_buf();
    _modem.attach(callback(this, &CommandAdapter<T>::recv_cb), RawSerial::RxIrq);
    _thread.start(callback(this, &CommandAdapter<T>::thread_cb));
}

template <typename T>
CommandAdapter<T>::~CommandAdapter() {
    _thread.terminate();
    if ( _cur_response != NULL) {
        ModemResponse_delete(_cur_response);
        _mail.free(_cur_response);
    }
}

template <typename T>
void CommandAdapter<T>::reset_buf() {
    _buf.reset();
}

template <typename T>
ModemResponseAlloc* CommandAdapter<T>::get_current_response() {
    if (_cur_response == NULL) {
        _cur_response = _mail.calloc();
        ModemResponse_init(_cur_response);
    }
    return _cur_response;
}

template <typename T>
void CommandAdapter<T>::recv_cb() {   
    if (get_state() == receiving_response) {
        // this is part of the response we're waiting for.
    } else {
        if (get_state() == idle) {
            // this is unsolicited.
            set_state(receiving_unsolicited_response);
        }
    }

    int c = _modem.getc();
    if ( !_buf.full()) {
        _buf.push(c);
    }

    // check EOL
    if ( (_buf.size() >= 2 && c == '\n') || _buf.full()) {
        
        // shovel into a string
        string *line = new string();
        while(!_buf.empty()) {
            char d;
            if (_buf.pop(d)) {
                line->push_back(d);
            }
        }

        // strip ws
        line->erase(line->find_last_not_of("\t\n\v\f\r ") + 1);
        // put into queue for thread to pick it up.
        _queue.put(line);

        reset_buf();

        // if this line was unsolicited, go to idle again
        if (get_state() == receiving_unsolicited_response) {
            set_state(idle);
        }
    }
}

template <typename T>
void CommandAdapter<T>::thread_cb() {
     while (true) {
        osEvent evt = _queue.get();
        if (evt.status == osEventMessage) {
            string *line = (string*)evt.value.p;

            ModemResponse *r = get_current_response()->obj;

            if ( line->length() > 0) {
                // store infos in _cur_response

                debug_0(line->c_str(), line->length(), '<');

                bool b = false;
                if (line->find("OK") == 0) {
                    b = true;
                    r->b_ok = true;
                } 
                if (line->find("ERROR") == 0) {
                    b = true;
                    r->b_error = true;
                }
                std::size_t cme_error_pos = line->find("+CME ERROR: ");
                if (cme_error_pos != std::string::npos) {
                    b = true;
                    r->b_error = true;

                    string code = line->substr(cme_error_pos+12);
                    r->errcode = atoi(code.c_str());
                }
                if ( !r->b_error && (*line)[0] == '+') {
                    b = true;
                    // split urc
                    int pos = line->find(':');
                    if ( pos >= 0) {
                        string key = line->substr(0,pos);
                        string value = line->substr(pos+1, line->length());
                        r->cmdresponses.insert(pair<string,string>(key,value));
                    } else {
                        // unable to parse? add to others
                        r->responses.push_back(*line);
                    }
                } 

                if ( !b) {
                    r->responses.push_back(*line);
                }
        
                
            }

            if ( get_state() == receiving_unsolicited_response) {
                // TODO deliver to urc callbacks.

                // remove. Next message goes into new response struct.
                ModemResponse_delete(_cur_response);
                _cur_response = NULL;
            } else {
                // deliver to mailbox if either flag is set
                if ( (r->isOk() || r->hasError()) ) {
                    // either ..
                    // - we finished reading one block of response that came from a command
                    // - or this is a single line of unsolicited stuff

                    // off to mailbox
                    _mail.put(get_current_response());

                    // forget _cur_response, so next message allocates a new one
                    _cur_response = NULL;

                    // we're done with this message.
                    set_state(idle);
                }
            }

            delete line;
        }
     }
}


template <typename T>
bool CommandAdapter<T>::ensure_state(ModemCommandState s, unsigned long timeout) {
    while( _state != s);
    return true;
}

template <typename T>
bool CommandAdapter<T>::send(const char *p_cmd, ModemResponse& r, unsigned long timeout) {
    if (p_cmd == NULL || strlen(p_cmd) < 2 || !(p_cmd[0]=='A' && p_cmd[1]=='T') ) {
        return false;
    }

    // wait for adapter to become idle..
    if (ensure_state(idle, timeout)) {
        size_t l = strlen(p_cmd);
        debug_0(p_cmd, l, '>' );

        set_state(sending_command);
        _modem.puts(p_cmd);
        _modem.putc('\r');
        _modem.putc('\n');
        set_state(receiving_response);

        // wait for response.
        osEvent evt = _mail.get(timeout);
        if (evt.status == osEventMail) {
            ModemResponseAlloc* p_m = (ModemResponseAlloc*)evt.value.p;
            r = *(p_m->obj);

            debug_1(&r);

            // free the allocator wrapper
            _mail.free(p_m);
            return true;
        }

    }

    return false;
}

template <typename T>
bool CommandAdapter<T>::send(const char *p_cmd, Callback<void(ModemResponse&)> cb, unsigned long timeout) {
    if (p_cmd == NULL || strlen(p_cmd) < 2 || !(p_cmd[0]=='A' && p_cmd[1]=='T') ) {
        return false;
    }

    // wait for adapter to become idle..
    if (ensure_state(idle, timeout)) {
        size_t l = strlen(p_cmd);
        debug_0(p_cmd, l, '>' );

        set_state(sending_command);
        _modem.puts(p_cmd);
        _modem.putc('\r');
        _modem.putc('\n');
        set_state(receiving_response);

        // wait for response.
        osEvent evt = _mail.get(timeout);
        if (evt.status == osEventMail) {
            ModemResponseAlloc* p_m = (ModemResponseAlloc*)evt.value.p;

            debug_1(p_m->obj);
            // call back
            cb(*(p_m->obj));

            _mail.free(p_m);
            return true;
        }
    }

    return false;
}


template class CommandAdapter<mbed::RawSerial>;

}

#include "mockserial.h"
template class Narrowband::CommandAdapter<MockSerial>;

