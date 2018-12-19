#include <mbed.h>
#include <modemresponse.h>
#include <modemcommandadapter.h>


Serial pc(USBTX, USBRX); 
RawSerial modem(PA_0, PA_1);




// main

// read chars from USB serial, add to buffer.
// wait for EOL, send to modem via ModemCommandAdapter.

const size_t buf_ui_size = 128;
char buf_ui[buf_ui_size] = "\0";
char *p_buf_ui = &buf_ui[0];
size_t buf_ui_l = 0;

void push_user_char(int c) {
    if (buf_ui_l < buf_ui_size) {
        *p_buf_ui++ = c;
        buf_ui_l++;
    }
}

bool get_functionality(ModemCommandAdapter *mca, bool& enabled) {
    bool res = false;

    ModemResponse *r;
    if (mca->send("AT+CFUN?", r, 1000)) {
        // grab cmdresponses for code.
        multimap<string,string>::iterator it = r->getCommandResponses().find("+CFUN");
        if (it != r->getCommandResponses().end()) {
            if (it->second == "1") {
                enabled = true;
                res = true;
            }
            if (it->second == "0") {
                enabled = false;
                res = true;
            }
        }
    }
    delete r;
    return res;
}

bool get_product_identification(ModemCommandAdapter *mca, string& vendor, string& model, string& revision) {
    bool res = false;

    ModemResponse *r;
    if (mca->send("ATI", r, 1000)) {
        if ( r->getResponses().size() >= 3) {
            list<string>::iterator it = r->getResponses().begin(); 

            vendor = it->c_str();
            ++it; model = it->c_str();
            ++it; revision = it->c_str();
            

            res = true;
        }
    }
    delete r;
    return res;
}

int main() {
    wait(1);

    pc.baud(115200);
    modem.baud(9600);


    ModemCommandAdapter *mca = new ModemCommandAdapter(modem);

    // wait for attention..
    bool b_noat = true;
    while(b_noat) {
        ModemResponse *r;
        if (mca->send("AT", r, 100)) {
            b_noat = false;
        }
        delete r;
        wait(1);
    }

    // 
    string p,m,r;
    if ( get_product_identification(mca, p,m,r)) {
        printf("This is a %s from %s, revision %s\n", m.c_str(), p.c_str(), r.c_str());
    }

    //
    bool enabled;
    if (get_functionality(mca, enabled)) {
        printf("modem is %senabled\n", (enabled?"":"not "));
    }

    pc.printf("READY>\n");

    // read user input from usb, send to modem line-wise
    while(1) {
        if(pc.readable()) {
            int c = pc.getc();
            pc.putc(c);
            push_user_char(c);
            if ( c == '\n') {
                *p_buf_ui++ = '\0';

                ModemResponse *r;
                if ( mca->send(buf_ui, r, 2000)) {
/*                    printf("--- %X\n", r);
                    debug_1(r);
                    printf("---\n");
                    */
                }

                if ( r != NULL) {
                    delete r;
                }

                p_buf_ui = &buf_ui[0];
                buf_ui_l = 0;

                memset(buf_ui, 0, buf_ui_size);

            }
        }
    }
}
