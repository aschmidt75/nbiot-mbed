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

OnOffControl NarrowbandCore::echo() {
    return OnOffControl(_ca, "", "ATE=", "", false, true);
}

OnOffControl NarrowbandCore::reportError() {
    return OnOffControl(_ca, "AT+CMEE", "+CMEE", true, true);
}

list<string> NarrowbandCore::getModuleInfo() {
    ModemResponse r;
    if (_ca.send("ATI", r, 1000)) {
        return r.getResponses();
    }
    return list<string>();
}

StringControl NarrowbandCore::modelIdentification() {
    return StringControl(_ca, "AT+CGMM", "", true, false);
}

StringControl NarrowbandCore::manufacturerIdentification() {
    return StringControl(_ca, "AT+CGMI", "", true, false);
}

StringControl NarrowbandCore::IMEI() {
    return StringControl(_ca, "AT+CGSN", "", true, false);
}

StringControl NarrowbandCore::IMSI() {
    return StringControl(_ca, "AT+CIMI", "", true, false);
}

OnOffControl NarrowbandCore::moduleFunctionality() {
    OnOffControl c(_ca, "AT+CFUN", "+CFUN", true, true);
    c.read_timeout() = 500;
    c.write_timeout() = 5000;
    return c;
}

OperatorSelectionControl NarrowbandCore::operatorSelection() {
    return OperatorSelectionControl(_ca);
}

PDPContextControl NarrowbandCore::PDPContexts() {
    return PDPContextControl(_ca);
}

BandControl NarrowbandCore::bands() const {
    return BandControl(_ca);
}

NConfigControl NarrowbandCore::nconfig() const {
    return NConfigControl(_ca);
}


}
