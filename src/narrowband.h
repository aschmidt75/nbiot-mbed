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
    bool attach();
    void attach(Thread& t);

    // check if attach to network
    bool isAttached() const;

protected:
    NarrowbandCore&    _core;

private:
    void attach_(void);
};

}