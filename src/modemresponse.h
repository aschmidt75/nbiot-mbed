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

class ModemCommandAdapter;

class ModemResponse {
friend class ModemCommandAdapter;

public:
    ModemResponse();
    ModemResponse(ModemResponse& r);

    bool isOk() const { return b_ok; };
    bool hasError() const { return b_error; };
    bool isUnsolicited() const { return b_unsolicited; };

    string getCommandResponse(const string& key);
    multimap<string,string>& getCommandResponses() { return cmdresponses; }
    list<string>& getResponses() { return responses; }

protected:    
    bool b_ok;              // modem returned with "OK"
    bool b_error;           // modem returned with "ERROR"
    bool b_unsolicited;     // true if message is unsolicited
    multimap<string,string> cmdresponses;      // contains responses of the type +CMD:<params>
    list<string>  responses;                   // contains responses of type <params>
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
