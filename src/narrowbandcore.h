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
#include <string>

namespace Narrowband {

class NarrowbandCore {
public:
    NarrowbandCore(CommandAdapterBase&);

    bool ready();    

    void reboot();

    bool setEcho(bool bEcho);

    bool setReportError(bool bEnable);

    list<string> getModuleInfo();

    string getModelIdentification();

    string getManufacturerIdentification();

    string getIMSI();

    string getIMEI();

    bool getModuleFunctionality(bool& fullFunctionality);

    bool setModuleFunctionality(bool fullFunctionality);

protected:
    CommandAdapterBase&    _ca;

private:
    bool d(const string & command, unsigned int timeout = 1000);

    string e(const string & command, unsigned int timeout = 1000);

    string f(const string & command, const string & key, unsigned int timeout = 1000);
};

}