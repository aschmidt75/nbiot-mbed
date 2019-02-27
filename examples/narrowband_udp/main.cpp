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


    if(nb.startAttach()) {
        while(true) {
            wait(2); printf(".");
            if (nb.isAttached()) {
                printf("attached.\n");
                break;
            }
        }
    } else {
        printf("Error attaching to NB network.\n");
    }

    Narrowband::ConnectionStatusControl cs = nbc.connectionStatus();
    printf("modem...\nis %s\n", (cs.isConnected())?"connected":"idle");
    printf("%s registered.\n", nbc.networkRegistrationStatus().isRegistered()?"is":"is not");
    printf("%s attached.\n", nbc.attachment().isAttached()?"is":"is not");
    printf("RSSI: %d\n", nbc.signalQuality().getRSSI());


    nb.sendUDP("10.0.0.1", 9876, "This_is_a_test");

    pc.printf("DONE>\n");
}
