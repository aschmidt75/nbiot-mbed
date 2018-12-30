#include <mbed.h>
#include "modemresponse.h"
#include "commandadapter.h"

// connect serials to USB (pc) and modem
// Check your board's spec which pins map to the modem shield
// and make sure these pins do not interfere with other serials (!)
Serial pc(USBTX, USBRX); 
RawSerial modem(PA_0, PA_1);

// uses AT+CFUN? to check if modem is enabled
bool get_functionality(Narrowband::CommandAdapterBase *mca, bool& enabled) {
    bool res = false;

    Narrowband::ModemResponse r;
    if (mca->send("AT+CFUN?", r, 1000)) {
        // grab cmdresponses for code.
        string cfun_value;
        if ( r.getCommandResponse("+CFUN", cfun_value)) {
            res = true;
            enabled = (cfun_value == "1");            
        }
    }
    return res;
}

// use ATI to get the product identification
bool get_product_identification(Narrowband::CommandAdapterBase *mca, string& vendor, string& model, string& revision) {
    bool res = false;

    Narrowband::ModemResponse r;
    if (mca->send("ATI", r, 1000)) {
        // ATI returns information as line responses,
        // vendor, model and revision. They can be accessed
        // by iterating the list.
        if ( r.getResponses().size() >= 3) {
            list<string>::iterator it = r.getResponses().begin(); 

            vendor = it->c_str();
            ++it; model = it->c_str();
            ++it; revision = it->c_str();

            res = true;
        }
    }
    return res;
}

// read chars from USB serial, add to buffer.
// wait for EOL, send to modem via ModemCommandAdapter.
int main() {
    wait(1);

    //
    pc.baud(115200);
    modem.baud(9600);

    Narrowband::CommandAdapter<mbed::RawSerial> *mca = new Narrowband::CommandAdapter<mbed::RawSerial>(modem);

    // wait for attention..
    bool b_noat = true;
    while(b_noat) {
        wait(1);
        Narrowband::ModemResponse r;
        if (mca->send("AT", r, 100)) {
            b_noat = false;
        }
    }

    // 
    string p,m,r;
    if ( get_product_identification(mca, p,m,r)) {
        printf("This is a %s from %s, %s\n", m.c_str(), p.c_str(), r.c_str());
    }

    //
    bool enabled;
    if (get_functionality(mca, enabled)) {
        printf("modem is %senabled\n", (enabled?"":"not "));
    }

    pc.printf("READY>\n");

    // read user input from serial usb, send to modem line-wise
    // display response(s)
    const size_t buf_ui_size = 128;
    char buf_ui[buf_ui_size] = "\0";
    char *p_buf_ui = &buf_ui[0];
    size_t buf_ui_l = 0;

    while(1) {
        if(pc.readable()) {
            int c = pc.getc();
            pc.putc(c);
            if (buf_ui_l < buf_ui_size) {
                *p_buf_ui++ = c;
                buf_ui_l++;
            }

            if ( c == '\n') {
                *p_buf_ui++ = '\0';

                Narrowband::ModemResponse r;
                if ( mca->send(buf_ui, r, 2000)) {
                    debug_1(&r);
                }

                // clear buf
                p_buf_ui = &buf_ui[0];
                buf_ui_l = 0;
                memset(buf_ui, 0, buf_ui_size);

            }
        }
    }
}
