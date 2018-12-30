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


#include "narrowbandcore.h"

namespace Narrowband {

NarrowbandCore::NarrowbandCore(CommandAdapterBase& ca) : _ca(ca) {

}

bool NarrowbandCore::ready() {
    Narrowband::ModemResponse r;
    if (_ca.send("AT", r, 100)) {
        return r.isOk();
    }
    return false;
}    

void NarrowbandCore::reboot() {
    Narrowband::ModemResponse r;
    _ca.send("AT+NRB", r, 10000);
}

bool NarrowbandCore::setEcho(bool bEcho) {
    char buf[32];
    snprintf(buf, sizeof(buf), "ATE=%d", bEcho);
    return d(buf);
}

bool NarrowbandCore::setReportError(bool bEnable) {
    char buf[32];
    snprintf(buf, sizeof(buf), "AT+CMEE=%d", bEnable);
    return d(buf);
}


list<string> NarrowbandCore::getModuleInfo() {
    ModemResponse r;
    if (_ca.send("ATI", r, 1000)) {
        return r.getResponses();
    }
}

string NarrowbandCore::getModelIdentification() {
    return e("AT+CGMM");
}
string NarrowbandCore::getManufacturerIdentification() {
    return e("AT+CGMI");
}

string NarrowbandCore::getIMEI() {
    return e("AT+CGSN");
}

string NarrowbandCore::getIMSI() {
    return e("AT+CIMI");
}

bool NarrowbandCore::getModuleFunctionality(bool& fullFunctionality) {
    string v = f("AT+CFUN?", "+CFUN");
    if (v.length() != 1) {
        return false;
    }
    fullFunctionality = (v == "1");
    return true;
}

bool NarrowbandCore::setModuleFunctionality(bool fullFunctionality) {
    char buf[32];
    snprintf(buf, sizeof(buf), "AT+CFUN=%d", fullFunctionality);
    return d(buf, 5000);
    
}


bool NarrowbandCore::d( const string & cmd, unsigned int timeout) {
    ModemResponse r;
    if (_ca.send(cmd.c_str(), r, timeout)) {
        return r.isOk();
    }
    return false;
}


// executes cmd with a default timeout (TODO),
// takes the first non-echo line from result responses as
// return. returns empty string in case of error or
// empty responses.
string NarrowbandCore::e( const string & cmd, unsigned int timeout) {
    ModemResponse r;
    if (_ca.send(cmd.c_str(), r, timeout)) {
        if ( r.isOk()) {
            if ( r.getResponses().size() > 0) {
                string s = r.getResponses().front();
                // check for echo enabled, remove echo
                if ( s == cmd) {
                    r.getResponses().pop_front();
                }
                s = r.getResponses().front();
                return s;
            }
        };
    }
    return "";
}

string NarrowbandCore::f( const string & cmd, const string & key, unsigned int timeout) {
    ModemResponse r;
    string res = "";
    if (_ca.send(cmd.c_str(), r, timeout)) {
        if ( r.isOk()) {
            (void)r.getCommandResponse(key, res);
        };
    }
    return res;
}

}
