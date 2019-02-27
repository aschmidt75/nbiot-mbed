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

#include "narrowbandcore.h"
#include <string>

namespace Narrowband {

template <class T>
class config_item {
public:
    config_item() : _cfg(false) { }
    config_item(T t) : _t(t), _cfg(false) { }
    config_item(const config_item<T> & rhs): _t(rhs._t), _cfg(rhs._cfg) { }

    operator bool() const { return _cfg; }
    void enable() { _cfg = true; }
    void disable() { _cfg = false; }

    void set(T t) { _t = t; }
    T get() const { return _t; }
    T& get() { return _t; }

private:
    T       _t;
    bool    _cfg;
};

struct NarrowbandConfig {
    config_item<bool>          echo_on;
    config_item<list<int> >    bands;
    config_item<OperatorSelectMode>     ops_mode;
    config_item<string>                 ops_name;
};

class Narrowband {
public:
    Narrowband(NarrowbandCore&);

    // enable modem
    void begin();

    // turn off modem
    void end();

    // check if modem is enabled and responding
    operator bool();

    // retrieve the current configuration
    void currentConfiguration(NarrowbandConfig& ) const;

    // set/update configuration
    bool configure(const NarrowbandConfig&);

    // trigger network attachment
    bool startAttach();
    bool startDetach();

    // check if attached to network
    bool isAttached() const;

    // one-way send to remote ip/port as UDP datagram
    bool sendUDP(string remoteAddr, unsigned int port, string body);

protected:
    NarrowbandCore&    _core;
};

}