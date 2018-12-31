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


}

