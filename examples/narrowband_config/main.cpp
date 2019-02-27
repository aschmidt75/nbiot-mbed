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
#include <mbed.h>
#include "narrowband.h"
#include "controls.h"

// connect serials to USB (pc), set default baud rate
Serial pc(USBTX, USBRX, 115200); 

// Check your board's spec which pins map to the modem shield
// and make sure these pins do not interfere with other serials (!)
// baud rate 9600 default for BC68, BC95
RawSerial modem(PA_0, PA_1, 9600);

// CommandAdapter on top of the raw serial modem
Narrowband::CommandAdapter<mbed::RawSerial> mca(modem);

Narrowband::NarrowbandCore nbc(mca);
Narrowband::Narrowband nb(nbc);

int main() {
    wait(1);

    printf("Sample board info query. Compile with -D __NBIOT_MBED_DEBUG_1 -D __NBIOT_MBED_DEBUG_0\n");
    printf("to see modem debug I/O.\n\n");

    while(nbc.ready() == false) {
        wait(2);
    }

    nbc.reportError().set(true);
    wait(1);

    // NarrowbandConfig object, all wanted values need to be enabled
    Narrowband::NarrowbandConfig cfg;
    cfg.bands.enable();
    cfg.ops_mode.enable();
    cfg.ops_name.enable();

    // queries the current configuration for all wanted values
    nb.currentConfiguration(cfg);

    // modify an item and update
    list<int>& activeBands = cfg.bands.get();
    activeBands.push_back(8);
    nb.configure(cfg);


    //
    Narrowband::PDPContextControl pcc = nbc.PDPContexts();
    pcc.get();

    const Narrowband::PDPContextList& pcl = pcc.contexts();
    for ( std::map<int, Narrowband::PDPContext>::const_iterator pcl_it = pcl.begin(); pcl_it != pcl.end(); ++pcl_it) {
        Narrowband::PDPContext c = pcl_it->second;

        printf("Contexts: %d, %d/%s/%s -> %sactive\n", pcl_it->first, 
            c.cid,c.type.c_str(),c.apn.c_str(),
            pcc.isActive(c)?"is ":"is not " );
/*
        if ( some_condition...) {
            wait(1);
            pcc.activate(c);
            wait(1);
        }
        */
    }

    Narrowband::OperatorSelectionControl osc = nbc.operatorSelection();
    if (osc.get()) {
        printf("OperatorSelection: mode=%s, operator=%s\n", (osc.mode() == 0?"Automatic":(osc.mode()==1?"Manual":(osc.mode()==2?"Deregister":"Unknown"))), osc.operatorName().c_str());
    }

    pc.printf("DONE>\n");
}
