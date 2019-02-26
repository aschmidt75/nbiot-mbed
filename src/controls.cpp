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

#include "controls.h"
#include "modemresponse.h"

namespace Narrowband {

ControlBase::ControlBase(const ControlBase& rhs) : 
    _cab(rhs._cab), _readable(rhs._readable), _writeable(rhs._writeable),
    _read_timeout(rhs._read_timeout), _write_timeout(rhs._write_timeout) {
}

bool ControlBase::d( const string & cmd, unsigned int timeout) const {
    ModemResponse r;
    if (_cab.send(cmd.c_str(), r, timeout)) {
        return r.isOk();
    }
    return false;
}

// executes cmd with a default timeout (TODO),
// takes the first non-echo line from result responses as
// return. returns empty string in case of error or
// empty responses.
string ControlBase::e( const string & cmd, unsigned int timeout) const {
    ModemResponse r;
    if (_cab.send(cmd.c_str(), r, timeout)) {
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

string ControlBase::f( const string & cmd, const string & key, unsigned int timeout) const {
    ModemResponse r;
    string res = "";
    if (_cab.send(cmd.c_str(), r, timeout)) {
        if ( r.isOk()) {
            (void)r.getCommandResponse(key, res);
        };
    }
    return res;
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


OperatorSelectionControl::OperatorSelectionControl(CommandAdapterBase& cab) :
    ControlBase(cab) {

}

OperatorSelectionControl::OperatorSelectionControl(const OperatorSelectionControl& rhs) :
    ControlBase(rhs), _mode(rhs._mode), _operatorName(rhs._operatorName) {

}

bool OperatorSelectionControl::get() {
    ModemResponse r;
    if ( _cab.send("AT+COPS?",r, _read_timeout)) {
        if ( r.isOk()) {
            string v;
            if (r.getCommandResponse("+COPS", v)) {
                v += ",";

                // parse line
                string buf;
                int idx = 0;

                for ( string::iterator it = v.begin(); it != v.end(); ++it) {
                    char n = (*it);
                    if ( n != ',' && it != v.end()) {
                        buf += n;
                    } else {
                        if ( idx == 0) {
                            this->_mode = Unknown;
                            if ( buf == "0") { this->_mode = Automatic; }
                            else if ( buf == "1") { this->_mode = Manual; }
                            else if ( buf == "2") { this->_mode = Deregister; }
                        }
                        if ( idx == 1) {
                            // format is fixed to 2;
                        }
                        if ( idx == 2) {
                            size_t n = buf.length()-1;
                            this->_operatorName = buf;
                            size_t n1 = (buf[0] == '\"')?1:0;
                            size_t n2 = (buf[n] == '\"')?n-1:n;
                            string v2 = this->_operatorName.substr(n1,n2);
                            this->_operatorName = v2;
                        }

                        idx++;
                        buf = "";
                    }
                }

                return true;
            }
        }
    }
    return false;
}

bool OperatorSelectionControl::set() {
    char buf[128];
    memset(buf,0,sizeof(buf));

    if ( _mode == Automatic) {
        snprintf(buf,sizeof(buf),"AT+COPS=0");
    }
    if ( _mode == Manual) {
        snprintf(buf,sizeof(buf),"AT+COPS=1,2,\"%s\"",_operatorName.c_str());
    }
    if ( _mode == Deregister) {
        snprintf(buf,sizeof(buf),"AT+COPS=2");
    }

    if ( strlen(buf) > 0) {
        ModemResponse r;
        if (_cab.send(buf, r, _write_timeout)) {
            return r.isOk();
        }
    }

    return false;
}



PDPContextControl::PDPContextControl(CommandAdapterBase& cab) :
    ControlBase(cab) {

}

PDPContextControl::PDPContextControl(const PDPContextControl& rhs) :
    ControlBase(rhs), _contexts (rhs._contexts) {

}

bool PDPContextControl::get() {
    ModemResponse r;
    if (_cab.send("AT+CGDCONT?", r, _read_timeout)) {
        if ( r.isOk()) {
            // parse contexts
            _contexts.clear();

            multimap<string,string>& m = r.getCommandResponses();
            for ( multimap<string,string>::iterator it = m.begin(); it != m.end(); ++it) {
                string line = (*it).second;
                line += ",";
                // parse line
                string buf;
                int idx = 0;

                PDPContext c;

                for ( string::iterator it = line.begin(); it != line.end(); ++it) {
                    char n = (*it);
                    if ( n != ',') {
                        buf += n;
                    } else {
                        if ( idx == 0) {
                            c.cid = atoi(buf.c_str());
                        }
                        if ( idx == 1) {
                            c.type = buf;
                        }
                        if ( idx == 2) {
                            c.apn = buf;
                        }

                        idx++;
                        buf = "";
                    }
                }

                _contexts.insert(pair<int,PDPContext>(c.cid, c));
            }
    
            return true;
        }
    }

    return false;
}

bool PDPContextControl::hasContextByTypeAndAPN(string type, string apn) {
    for ( Narrowband::PDPContextList::const_iterator it = _contexts.begin(); it != _contexts.end(); ++it) {
        Narrowband::PDPContext d = (*it).second;
        if (d.type == type && d.apn == apn) {
            return true;
        }
    }
    return false;
}

bool PDPContextControl::set(const PDPContext& c) {
    char buf[128];
    memset(buf,0,sizeof(buf));

    snprintf(buf,sizeof(buf),"AT+CGDCONT=%d,\"%s\",\"%s\"", 
        c.cid,c.type.c_str(),c.apn.c_str());
    ModemResponse r;
    if (_cab.send(buf, r, _write_timeout)) {
        return r.isOk();
    }

    return false;
}

bool PDPContextControl::isActive(const PDPContext& ctx) const {
    ModemResponse r;
    if ( _cab.send("AT+CGACT?",r, _read_timeout)) {
        if ( r.isOk()) {
            string v;
            if (r.getCommandResponse("+CGACT", v)) {
                v += ",";

                // parse line
                string buf;
                int idx = 0;
                int cid = -1;
                int state = -1;

                for ( string::iterator it = v.begin(); it != v.end(); ++it) {
                    char n = (*it);
                    if ( n != ',' && it != v.end()) {
                        buf += n;
                    } else {
                        if ( idx == 0) {
                            cid = atoi(buf.c_str());
                        }
                        if ( idx == 1) {
                            state = atoi(buf.c_str());
                        }
                        idx++;
                        buf = "";
                    }
                }

                return (ctx.cid == cid && state == 1);
            }
        }
    }
    return false;

}

bool PDPContextControl::setActive(const PDPContext& ctx, bool b_active) {
    char buf[128];
    memset(buf,0,sizeof(buf));

    snprintf(buf,sizeof(buf),"AT+CGACT=%d,%d",b_active,ctx.cid);
    ModemResponse r;
    if (_cab.send(buf, r, _write_timeout)) {
        return r.isOk();
    }

    return false;

}



BandControl::BandControl(CommandAdapterBase& cab) : ControlBase(cab) { }

BandControl::BandControl(const BandControl& rhs) : ControlBase(rhs) { }

list<int> BandControl::supportedBands() const {
    std::string v = f("AT+NBAND=?", "+NBAND", _read_timeout);
    return csv_to_intlist(v);
}

list<int> BandControl::activeBands() const {
    std::string v = f("AT+NBAND?", "+NBAND", _read_timeout);
    return csv_to_intlist(v);
}

bool BandControl::set(const list<int>& b) const {
    string l = "AT+NBAND=";
    for ( list<int>::const_iterator it = b.begin(); it != b.end(); ++it) {
        char buf[16];
        snprintf(buf,sizeof(buf),"%d,",(int)(*it));
        l.append(buf);
    }
    l = l.erase(l.length()-1,1);

    ModemResponse r;
    if( _cab.send(l.c_str(), r, _write_timeout)) {
        return r.isOk();
    }
    return false;
}

list<int> BandControl::csv_to_intlist(string line) const {
    string buf;

    list<int> res;
    for ( string::iterator it = line.begin(); it != line.end(); ++it) {
        char n = (*it);
        if ( n == '(' || n == ')') {
            // ignore
        } else {
            if ( n != ',') {
                buf += n;
            } else {
                res.push_back(atoi(buf.c_str()));
                buf = "";
            }
        }
    }
    if ( buf != "") {
        res.push_back(atoi(buf.c_str()));
    }

    return res;
}

NConfigControl::NConfigControl(CommandAdapterBase& cab) : ControlBase(cab) { 
    read_timeout() = 5000;
}

NConfigControl::NConfigControl(const NConfigControl& rhs) : ControlBase(rhs) { }

bool NConfigControl::get() {
    if ( readable()) {
        ModemResponse r;
        if (_cab.send("AT+NCONFIG?", r, _read_timeout)) {
            if ( r.isOk()) {
                _entries.clear();

                std::multimap<string,string>& m = r.getCommandResponses();
                for ( std::multimap<string,string>::iterator it = m.begin(); it != m.end(); ++it) {
                    string v = it->second;

                    size_t n = v.find(",");
                    if ( n != std::string::npos) {
                        string k = v.substr(0,n);
                        string val = v.substr(n+1,v.length());

                        _entries.insert(pair<string,string>(k,val));
                    }
                }
            }
        }
    }
    return false;
}

bool NConfigControl::set(string key, string value) {
    if ( writeable()) {
        ModemResponse r;
        char buf[256];
        snprintf(buf,sizeof(buf), "AT+NCONFIG=%s,%s", key.c_str(), value.c_str());

        if (_cab.send(buf, r, _write_timeout)) {
            if (r.isOk()) {
                get();
            }
        }
    }
    return false;

}



ConnectionStatusControl::ConnectionStatusControl(CommandAdapterBase& cab) : ControlBase(cab) { }

ConnectionStatusControl::ConnectionStatusControl(const ConnectionStatusControl& rhs) : ControlBase(rhs) { }

pair<bool,int> ConnectionStatusControl::get() const {
    pair<bool,int> res = make_pair<bool,int>(false,-1);

    if ( readable()) {
        ModemResponse r;
        if (_cab.send("AT+CSCON?", r, _read_timeout)) {
            if ( r.isOk()) {
                string v;
                r.getCommandResponse("+CSCON",v);
                if(v.length() > 0) {
                    size_t n = v.find_first_of(",");
                    if ( n > 0 && n != string::npos) {
                        string s1 = v.substr(0,n);
                        string s2 = v.substr(n+1,v.length());

                        res.first = ( s1 == "1")?true:false;
                        res.second = ( s2 == "1")?true:false;
                    }
                }

            }
        }
    }

    return res;
}

bool ConnectionStatusControl::set(bool bUnsolicitedResult) {
    if ( writeable()) {
        ModemResponse r;
        char buf[256];
        snprintf(buf,sizeof(buf), "AT+CSCON=%d", bUnsolicitedResult);

        if (_cab.send(buf, r, _write_timeout)) {
            if (r.isOk()) {
                return true;
            }
        }
    }
    return false;
}




NetworkRegistrationStatusControl::NetworkRegistrationStatusControl(CommandAdapterBase& cab) : ControlBase(cab) { }

//NetworkRegistrationStatusControl::NetworkRegistrationStatusControl(const NetworkRegistrationStatusControl& rhs) : ControlBase(rhs) { }

bool NetworkRegistrationStatusControl::get(int& status) const {
    if ( readable()) {
        ModemResponse r;
        if (_cab.send("AT+CEREG?", r, _read_timeout)) {
            if ( r.isOk()) {
                string v;
                r.getCommandResponse("+CEREG",v);
                if(v.length() > 0) {
                    v += ",";

                    // parse line
                    string buf;
                    int idx = 0;

                    for ( string::iterator it = v.begin(); it != v.end(); ++it) {
                        char n = (*it);
                        if ( n != ',' && it != v.end()) {
                            buf += n;
                        } else {
                            if ( idx == 0) {
                                // unsolicited result code status
                            }
                            if ( idx == 1) {
                                status = atoi(buf.c_str());
                            }

                            idx++;
                            buf = "";
                        }
                    }

                    return true;
                }

            }
        }
    }

    return false;
}

bool NetworkRegistrationStatusControl::set(int mode) {
    if ( writeable()) {
        ModemResponse r;
        char buf[64];
        snprintf(buf,sizeof(buf), "AT+CEREG=%d", mode);

        if (_cab.send(buf, r, _write_timeout)) {
            if (r.isOk()) {
                return true;
            }
        }
    }
    return false;
}

AttachmentControl::AttachmentControl(CommandAdapterBase& cab) : OnOffControl(cab, "AT+CGATT?", "AT+CGATT=", "+CGATT", true, true) 
{ }

AttachmentControl::AttachmentControl(const AttachmentControl& rhs) : OnOffControl(rhs) { }



SocketControl::SocketControl(CommandAdapterBase& cab) : ControlBase(cab) {
    _localPort = -1;
    _socket = 0;
    _bReceiveControl = false;
}

SocketControl::SocketControl(const SocketControl& rhs) : ControlBase(rhs), 
    _localPort(rhs._localPort), _bReceiveControl(rhs._bReceiveControl), _socket(rhs._socket) {

}

SocketControl::~SocketControl() {
    if ( isOpen()) {
        close();
    }
}

long SocketControl::socket_id_ctr = rand()%32767;

bool SocketControl::open() {
    if ( _socket > 0) {
        return false;       // already open
    }

    // figure out a listen port
    _localPort = 32767+next_socket_id();

    ModemResponse r;
    char buf[64];
    snprintf(buf,sizeof(buf), "AT+NSOCR=%s,%d,%d,%d", getType(), getProtocol(), _localPort, _bReceiveControl?1:0);

    if (_cab.send(buf, r, _write_timeout)) {
        if (r.isOk()) {

            string socketID = *(r.getResponses().begin());
            _socket = atoi(socketID.c_str());

            return true;
        } else {
            _localPort = -1;
        }
    }
    return false;
}

bool SocketControl::close() {
    if ( _socket <= 0) {
        return false;
    }

    ModemResponse r;
    char buf[64];
    snprintf(buf,sizeof(buf), "AT+NSOCL=%d", _socket);

    if (_cab.send(buf, r, _write_timeout)) {
        if (r.isOk()) {
            _localPort = -1;
            _socket = 0;
            return true;
        }
    }
    return false;
}



UDPSocketControl::UDPSocketControl(CommandAdapterBase& cab) : SocketControl(cab) {
}

UDPSocketControl::UDPSocketControl(const UDPSocketControl& rhs) : SocketControl(rhs) {
}

bool UDPSocketControl::sendTo(const char *remoteAddr, unsigned int remotePort, size_t length, const uint8_t *p_data) {
    size_t n = 1+length*2;
    char *hexbuf = (char*)malloc(n); 
    memset(hexbuf,0,n);

    char *q = hexbuf;
    const uint8_t *p = p_data;
    for ( size_t i = 0; i < length; i++) {
        sprintf(q, "%.2X", p_data[i]);
        q += 2;
    }

    ModemResponse r;
    char buf[2048];
    snprintf(buf,sizeof(buf), "AT+NSOST=%d,%s,%d,%d,%s", _socket, remoteAddr, remotePort, length, hexbuf);
    free(hexbuf);

    if (_cab.send(buf, r, _write_timeout)) {
        if (r.isOk()) {
            return true;
        }
    }
    return false;
}

bool UDPSocketControl::sendTo(const char *remoteAddr, unsigned int remotePort, string body) {
    return sendTo(remoteAddr, remotePort, body.length(), (const uint8_t*)body.c_str());
}

}