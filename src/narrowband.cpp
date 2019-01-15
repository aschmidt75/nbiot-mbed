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
}

bool Narrowband::configure(const NarrowbandConfig& c) {
    if ( c.echo_on) {
        _core.echo().set(c.echo_on.get());
    }
    if ( c.bands) {
        list<int> v = c.bands.get();
        _core.bands().set(v);
    }
    return false;
}


bool Narrowband::attach() {
    return false;
}

void Narrowband::attach(Thread& t) {
    t.start(this, &Narrowband::attach_);
}

void Narrowband::attach_(void) {
}

bool Narrowband::isAttached() const {
    return false;
}


}
