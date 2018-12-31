#include "controls.h"
#include "modemresponse.h"

namespace Narrowband {

ControlBase::ControlBase(const ControlBase& rhs) : 
    _cab(rhs._cab), _readable(rhs._readable), _writeable(rhs._writeable),
    _read_timeout(rhs._read_timeout), _write_timeout(rhs._write_timeout) {
}


StringControl::StringControl(CommandAdapterBase& cab, string cmdread, string cmdwrite, bool readable_, bool writeable_) :
    ControlBase(cab,readable_,writeable_), _cmdread(cmdread), _cmdwrite(cmdwrite) {
}

StringControl::StringControl(const StringControl& rhs) :
    ControlBase(rhs._cab, rhs._readable, rhs._writeable), _cmdread(rhs._cmdread), _cmdwrite(rhs._cmdwrite) {

}

string StringControl::get() const {
    string s;
    get(s);
    return s;
}

bool StringControl::get(string &value) const {
    if ( readable()) {
        ModemResponse r;
        if (_cab.send(_cmdread.c_str(), r, _read_timeout)) {
            if ( r.isOk()) {
                if ( r.getResponses().size() > 0) {
                    value = r.getResponses().front();
                    // check for echo enabled, remove echo
                    if ( value == _cmdread) {
                        r.getResponses().pop_front();
                    }
                    value = r.getResponses().front();
                    return true;
                }
            };
        }

    }
    return false;
}

bool StringControl::set(string value) const {
    if ( writeable()) {
        ModemResponse r;
        string lcmd = _cmdwrite + value;
        if (_cab.send(lcmd.c_str(), r, _write_timeout)) {
            return r.isOk();
        }
    }
    return false;
}


OnOffControl::OnOffControl(CommandAdapterBase& cab, string cmd, string key, bool readable_, bool writeable_) :
    ControlBase(cab,readable_,writeable_), _cmdread(cmd+"?"), _cmdwrite(cmd+"="), _key(key) {
}

OnOffControl::OnOffControl(CommandAdapterBase& cab, string cmdread, string cmdwrite, string key, bool readable_, bool writeable_) :
    ControlBase(cab,readable_,writeable_), _cmdread(cmdread), _cmdwrite(cmdwrite), _key(key) {
}

OnOffControl::OnOffControl(const OnOffControl& rhs) :
    ControlBase(rhs._cab, rhs._readable, rhs._writeable), _cmdread(rhs._cmdread), _cmdwrite(rhs._cmdwrite), _key(rhs._key) {

}

bool OnOffControl::get() const {
    bool s;
    get(s);
    return s;
}

bool OnOffControl::get(bool &value) const {
    if ( readable()) {
        ModemResponse r;
        if (_cab.send(_cmdread.c_str(), r, _read_timeout)) {
            if ( r.isOk()) {
                // keyed?
                if (_key.length() > 0) {
                    string v;
                    if (r.getCommandResponse(_key,v)) {
                        value = ( v == "1");
                        return true;
                    }
                } else {
                    // indexed, take first response.
                    if ( r.getResponses().size() > 0) {
                        string v = r.getResponses().front();
                        // check for echo enabled, remove echo
                        if ( v == _cmdread) {
                            r.getResponses().pop_front();
                        }
                        v = r.getResponses().front();
                        value = ( v == "1");
                        return true;
                    }
                }
            };
        }

    }
    return false;
}

bool OnOffControl::set(bool value) const {
    if ( writeable()) {
        ModemResponse r;
        string lcmd = _cmdwrite + (value?"1":"0");
        if (_cab.send(lcmd.c_str(), r, _write_timeout)) {
            return r.isOk();
        }
    }
    return false;
}

}