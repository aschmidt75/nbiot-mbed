#include <mbed.h>
#include "narrowbandcore.h"

// connect serials to USB (pc), set default baud rate
Serial pc(USBTX, USBRX, 115200); 

// Check your board's spec which pins map to the modem shield
// and make sure these pins do not interfere with other serials (!)
// baud rate 9600 default for BC68, BC95
RawSerial modem(PA_0, PA_1, 9600);

// CommandAdapter on top of the raw serial modem
Narrowband::CommandAdapter<mbed::RawSerial> mca(modem);

// Access to core functionality
Narrowband::NarrowbandCore nbc(mca);


int main() {
    wait(1);

    printf("Sample board info query. Compile with -D __NBIOT_MBED_DEBUG_1 -D __NBIOT_MBED_DEBUG_0\n");
    printf("to see modem debug I/O.\n\n");

    while(nbc.ready() == false) {
        wait(1);
    }

    nbc.setEcho(false);
    nbc.setReportError(true);

    printf("This is a %s from %s\n", 
        nbc.getModelIdentification().c_str(), 
        nbc.getManufacturerIdentification().c_str());

    //
    bool enabled;
    if (nbc.getModuleFunctionality(enabled)) {
        printf("modem is %senabled\n", (enabled?"":"not "));
    }
    if ( !enabled) {
        if ( nbc.setModuleFunctionality(true)) {
            printf("Now enabled.\n");
            enabled = true;
        } else {
            printf("Error enabling modem, make sure SIM is placed correctly.\n");
        }
    }

    printf("IMEI: %s\n", nbc.getIMEI().c_str());
    if (enabled) {
        printf("IMSI: %s\n", nbc.getIMSI().c_str());
    }
}
