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

#include <mbed.h>
#include "modemresponse.h"

namespace Narrowband {

ModemResponse::ModemResponse() :
    b_ok(false), b_error(false), b_unsolicited(false), errcode(0) { 
    }

ModemResponse::ModemResponse(ModemResponse& r) :
    b_ok(r.b_ok), b_error(r.b_error), b_unsolicited(r.b_unsolicited),
    cmdresponses(r.cmdresponses), responses(r.responses), errcode(0) { }

bool ModemResponse::getCommandResponse(const string& key, string& value) {
    multimap<string,string>::iterator it = cmdresponses.find(key);
    if (it != cmdresponses.end()) {
        value = it->second;
        return true;
    }
    return false;
}

ModemResponse& ModemResponse::operator=(ModemResponse& r)
{
    b_ok = r.b_ok; 
    b_error = r.b_error; 
    b_unsolicited = r.b_unsolicited; 
    errcode = r.errcode;
    cmdresponses = r.cmdresponses;
    responses = r.responses;
    return *this;
}

ModemResponse& ModemResponse::operator=(const ModemResponse& r)
{
    b_ok = r.b_ok; 
    b_error = r.b_error; 
    b_unsolicited = r.b_unsolicited; 
    errcode = r.errcode;
    cmdresponses = r.cmdresponses;
    responses = r.responses;
    return *this;
}


bool ModemResponse::hasResponse(const string& key) {
    for ( list<string>::iterator it = responses.begin(); it != responses.end(); ++it) {
        if ( key == *it) {
            return true;
        }
    }
    return false;
}

ModemResponse* ModemResponse_init(ModemResponseAlloc *m) {
    m->obj = new ModemResponse();
    return m->obj;
}

void ModemResponse_delete(ModemResponseAlloc *m) {
    delete m->obj;
}

void debug_1_impl(ModemResponse *m) {
#ifdef __NBIOT_MBED_DEBUG_1
    if ( m == NULL)
        return;

    printf("(");
    if (m->hasError()) {
        printf("ERR");
        if ( m->getErrCode() != 0) {
            printf("[%d] ", m->getErrCode());
        } else {
            printf(" ");
        }
    }
    if (m->isOk()) {
        printf("OK ");
    }
    printf("RSP={");
    for (multimap<string,string>::iterator it = m->getCommandResponses().begin(); it != m->getCommandResponses().end(); ++it) {
        printf("%s -> %s|", it->first.c_str(), it->second.c_str());
    }
    printf("} MSG={");
    for (list<string>::iterator it = m->getResponses().begin(); it != m->getResponses().end(); ++it) {
        printf("%s|", it->c_str());
    }
    printf("})\n");
#endif
}


}
