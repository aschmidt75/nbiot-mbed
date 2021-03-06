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

#include "commandadapter.h"
#include "controls.h"
#include <string>

namespace Narrowband {

class NarrowbandCore {
public:
    NarrowbandCore(CommandAdapterBase&);

    // checks if modem is ready
    bool ready();    

    // reboots module
    void reboot();

    // turns echo on or off
    OnOffControl echo();

    // turns UE error reporting on or off 
    OnOffControl reportError();

    // returns module info
    list<string> getModuleInfo();

    // returns model identification
    StringControl modelIdentification();

    // returns manufacturer identification
    StringControl manufacturerIdentification();

    // returns an IMSI control
    StringControl IMSI();

    // returns an IMEI control
    StringControl IMEI();

    // turns module on or off
    OnOffControl moduleFunctionality();

    SignalQualityControl signalQuality();

    // 
    OperatorSelectionControl operatorSelection();

    // get/set/activate PDP Contexts
    PDPContextControl PDPContexts();

    // get/set active bands
    BandControl bands() const;

    // get/set configuration
    NConfigControl nconfig() const;

    ConnectionStatusControl connectionStatus() const;

    NetworkRegistrationStatusControl networkRegistrationStatus() const;

    AttachmentControl attachment() const;

    UDPSocketControl udp() const;

protected:
    CommandAdapterBase&    _ca;

};

}