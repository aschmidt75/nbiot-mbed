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

#include "commandadapter.h"
#include "controls.h"
#include <string>

namespace Narrowband {

class NarrowbandCore {
public:
    NarrowbandCore(CommandAdapterBase&);

    bool ready();    

    void reboot();

    OnOffControl echo();

    OnOffControl reportError();

    list<string> getModuleInfo();

    StringControl modelIdentification();

    StringControl manufacturerIdentification();

    StringControl IMSI();

    StringControl IMEI();

    OnOffControl moduleFunctionality();

    OperatorSelectionControl operatorSelection();

    PDPContextControl PDPContexts();

    BandControl bands() const;

protected:
    CommandAdapterBase&    _ca;

};

}