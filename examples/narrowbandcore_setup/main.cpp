#include <mbed.h>
#include "narrowbandcore.h"
#include "controls.h"

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

    Narrowband::PDPContextControl c = nbc.PDPContexts();
    if(c.get()) {
        const Narrowband::PDPContextList& m = c.contexts();
        for ( Narrowband::PDPContextList::const_iterator it = m.begin(); it != m.end(); ++it) {
            Narrowband::PDPContext d = (*it).second;
            printf("%d %s %s\n", d.cid, d.type.c_str(), d.apn.c_str());
        }
    }

    // set (overwrite a context) and SHOW again
    /*
    Narrowband::PDPContext e(1,"IP","what.ever.telco");
    c.set(e);

    if(c.get()) {
        const Narrowband::PDPContextList& m = c.contexts();
        for ( Narrowband::PDPContextList::const_iterator it = m.begin(); it != m.end(); ++it) {
            Narrowband::PDPContext d = (*it).second;
            printf("%d %s %s\n", d.cid, d.type.c_str(), d.apn.c_str());
        }
    }
    */

    // query/set bands
    Narrowband::BandControl bc = nbc.bands();
    // query supported bands
    list<int> bs = bc.supportedBands();
    printf("Supported bands are:\n");
    for ( list<int>::iterator it = bs.begin(); it != bs.end(); ++it) {
        printf("%d, ", *it);
    }
    printf("\n");
    list<int> vv = bc.activeBands();
    printf("Active bands are:\n");
    for ( list<int>::iterator it = vv.begin(); it != vv.end(); ++it) {
        printf("%d, ", *it);
    }
    printf("\n");
    // modify vv here if desired 
    bc.set(vv);

    // set config entry
    Narrowband::NConfigControl ncc = nbc.nconfig();
    ncc.get();
    ncc.set("AUTOCONNECT", "TRUE");
    
    
    pc.printf("DONE>\n");
}
