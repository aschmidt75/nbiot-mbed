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
#include "commandadapter.h"

namespace Narrowband {

class ControlBase {
public:
    static unsigned int const default_read_timeout = 500;
    static unsigned int const default_write_timeout = 1000;

    ControlBase(CommandAdapterBase& cab, bool readable = true, bool writeable = true) : 
        _cab(cab), _readable(readable), _writeable(writeable), _read_timeout(default_read_timeout), _write_timeout(default_write_timeout) { };
    ControlBase(const ControlBase& rhs);

    virtual bool supported() const { return true; }
    virtual bool readable() const { return _readable; }
    virtual bool writeable() const { return _writeable; }

    unsigned int& read_timeout() { return _read_timeout; }
    unsigned int& write_timeout() { return _write_timeout; }

protected:
    CommandAdapterBase& _cab;
    bool _readable, _writeable;

    unsigned int _read_timeout;
    unsigned int _write_timeout;

    bool d(const string & command, unsigned int timeout = 1000) const;
    string e(const string & command, unsigned int timeout = 1000) const;
    string f(const string & command, const string & key, unsigned int timeout = 1000) const;
};

class StringControl : public ControlBase {
public:
    StringControl(CommandAdapterBase& cab, string cmdread, string cmdwrite, bool readable = true, bool writeable = true);
    StringControl(const StringControl& rhs);

    virtual string get() const;
    virtual bool get(string &value) const;
    virtual bool set(string value) const;

private:
    string  _cmdread, _cmdwrite;
};

class OnOffControl : public ControlBase {
public:
    OnOffControl(CommandAdapterBase& cab, string cmd, string key, bool readable = true, bool writeable = true);
    OnOffControl(CommandAdapterBase& cab, string cmdread, string cmdwrite, string key, bool readable = true, bool writeable = true);
    OnOffControl(const OnOffControl& rhs);

    virtual bool get() const;
    virtual bool get(bool &value) const;
    virtual bool set(bool value) const;
    virtual bool isOn() const { return get() == true; }
    virtual bool isOff() const { return get() == false; }
    virtual bool on() const { return set(true);}
    virtual bool off() const { return set(false);}

    operator bool () const { return get(); }

private:
    string  _cmdread, _cmdwrite, _key;
};


enum OperatorSelectMode {
    Unknown = -1,
    Automatic = 0,
    Manual = 1,
    Deregister = 2
};

class OperatorSelectionControl : public ControlBase {
public:
    OperatorSelectionControl(CommandAdapterBase& cab);
    OperatorSelectionControl(const OperatorSelectionControl& rhs);

    OperatorSelectMode& mode() { return _mode; }
    string& operatorName() { return _operatorName; }

    virtual bool get();
    virtual bool set();

protected:
    OperatorSelectMode  _mode;
    string              _operatorName;
};

struct PDPContext {
    int             cid;
    string          type;
    string          apn;

    PDPContext(int _cid = -1, string _type = "", string _apn = "") : cid(_cid),type(_type),apn(_apn) { }
    PDPContext(const PDPContext& rhs) : cid(rhs.cid), type(rhs.type), apn(rhs.apn) { }

    bool operator==(const PDPContext& other) const {
        return type == other.type && apn == other.apn;
    };
};

typedef std::map<int, PDPContext>    PDPContextList;

class PDPContextControl : public ControlBase {
public:
    PDPContextControl(CommandAdapterBase& cab);
    PDPContextControl(const PDPContextControl& rhs);

    virtual bool get();
    const PDPContextList& contexts() const { return _contexts; }; 
    PDPContext context(int contextId) const { return _contexts.at(contextId); }; 
    virtual bool set(const PDPContext& );
/*
    bool isActive(const PDPContext& ) const;
    bool setActive(const PDPContext&, bool b_active);
    bool activate(const PDPContext& ctx) { setActive(ctx, true); };
    bool deactivate(const PDPContext& ctx) { setActive(ctx, false); };
*/
    bool hasContextByTypeAndAPN(string type, string apn);

protected:
    PDPContextList    _contexts;
};

class BandControl : public ControlBase {
public:
    BandControl(CommandAdapterBase& cab);
    BandControl(const BandControl& rhs);

    virtual list<int> supportedBands() const;
    virtual list<int> activeBands() const;
    virtual bool set(const list<int>& ) const;

private:
    list<int> csv_to_intlist(string ) const;
};

class NConfigControl : public ControlBase {
public:
    NConfigControl(CommandAdapterBase& cab);
    NConfigControl(const NConfigControl& rhs);

    virtual bool get();
    const std::map<string,string>& get() const { return _entries; };
    const string& valueFor(string key) const { return _entries.at(key); };
    virtual bool set(string key, string value);

private:
    std::map<string,string>     _entries;
};


}

