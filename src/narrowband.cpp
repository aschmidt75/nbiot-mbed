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

#include "narrowband.h"

namespace Narrowband {

Narrowband::Narrowband(NarrowbandCore& core) : _core(core) {

} 

void Narrowband::begin() {
    _core.moduleFunctionality().on();
}

void Narrowband::end() {
    _core.moduleFunctionality().off();
}

Narrowband::operator bool() {
    if (_core.ready()) {
        return _core.moduleFunctionality().isOn();
    } 
    return false;
}

void Narrowband::currentConfiguration(NarrowbandConfig& c) const {
    if ( c.echo_on) {
        // echo cannot be read
    }
    if ( c.bands) {
        c.bands.set(_core.bands().activeBands());
    }
    if ( c.ops_mode || c.ops_name) {
        OperatorSelectionControl osc = _core.operatorSelection();
        osc.get();

        c.ops_mode.set(osc.mode());
        c.ops_name.set(osc.operatorName());
    }
}

bool Narrowband::configure(const NarrowbandConfig& c) {
    if ( c.echo_on) {
        _core.echo().set(c.echo_on.get());
    }
    if ( c.bands) {
        list<int> v = c.bands.get();
        _core.bands().set(v);
    }
    if ( c.ops_mode) {
        OperatorSelectionControl osc = _core.operatorSelection();
        if ( c.ops_mode.get() == Manual && c.ops_name) {
            osc.mode() = Manual;
            osc.operatorName() = c.ops_name;
            osc.set();
        } else if ( c.ops_mode.get() == Automatic || c.ops_mode.get() == Deregister) {
            osc.mode() = Manual;
            osc.set();
        }
    }
    return false;
}


bool Narrowband::startAttach() {
    _core.connectionStatus().set(0);                // disable unsolicited result codes
    _core.networkRegistrationStatus().set(0);       // same here
    return _core.attachment().attach();
}

bool Narrowband::startDetach() {
    return _core.attachment().detach();
}

bool Narrowband::isAttached() const {
    return _core.attachment().isAttached();
}

bool Narrowband::sendUDP(string remoteAddr, unsigned int port, string body) {
    bool res = false;
    UDPSocketControl sc = _core.udp();
    if (sc.open()) {
        res = sc.sendTo(remoteAddr.c_str(), port, body.length(), (const uint8_t*)body.c_str());
        sc.close();
    }
    return res;
}

}
