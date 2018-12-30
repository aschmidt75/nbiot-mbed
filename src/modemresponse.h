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

#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

namespace Narrowband {

class ModemCommandAdapter;
template <class T> class CommandAdapter;

/**
 * A ModemResponse is a block of data as the modem's response
 * to a command previously sent or received as unsolicited. 
 * It contains status (OK/ERROR), command responses (e.g. +XYZ=1)
 * or plain-text responses (e.g. "rebooted").
 */
class ModemResponse {
friend class ModemCommandAdapter;
template <class T> friend class CommandAdapter;

public:
    ModemResponse();
    ModemResponse(ModemResponse& r);

    ModemResponse& operator=(ModemResponse& r);
    ModemResponse& operator=(const ModemResponse& r);

    bool isOk() const { return b_ok; };
    bool hasError() const { return b_error; };
    bool isUnsolicited() const { return b_unsolicited; };

    // returns modem internal error code (if found)
    unsigned int getErrCode() const { return errcode; };

    // retrieve the command responses as a multimap
    multimap<string,string>& getCommandResponses() { return cmdresponses; }

    // retrieve individual command response by key (first value only)
    bool getCommandResponse(const string& key, string& value);

    // retrieve list of non-command responses
    list<string>& getResponses() { return responses; }

    // check for presence of an individual non-command response
    bool hasResponse(const string& key);

protected:    
    bool b_ok;              // modem returned with "OK"
    bool b_error;           // modem returned with "ERROR"
    bool b_unsolicited;     // true if message is unsolicited
    multimap<string,string> cmdresponses;      // contains responses of the type +CMD:<params>
    list<string>  responses;                   // contains responses of type <params>
    unsigned int errcode;     // if CMEE=1, error code (if found)
};

struct ModemResponseAlloc {
    ModemResponse *obj;
};

ModemResponse* ModemResponse_init(ModemResponseAlloc *m);
void ModemResponse_delete(ModemResponseAlloc *m);


#ifdef __NBIOT_MBED_DEBUG_1
void debug_1_impl(ModemResponse *m);
#define debug_1(m) debug_1_impl(m)
#else
#define debug_1(m)
#endif


}