#include <mbed.h>

#include "commandadapter.h"
#include "narrowbandcore.h"
#include "mockserial.h"

using namespace Narrowband;

// connect serials to USB (pc) and Mock Serial
Serial pc(USBTX, USBRX); 
MockSerial modem;
CommandAdapter<MockSerial> mca(modem);

size_t ASSERT_NUM_TOTAL = 0;
size_t ASSERT_NUM_OK = 0;
size_t ASSERT_NUM_FAILED = 0;

#define TEST_ASSERT0(condition, line, message)     ASSERT_NUM_TOTAL++; if (condition) { ASSERT_NUM_OK++; } else { ASSERT_NUM_FAILED++; printf("line %lu: %s\n",(unsigned long)(line), (message));}
#define TEST_ASSERT(condition)                     TEST_ASSERT0((condition), __LINE__, " test assert FAILED.")

#define TIMEOUT 1000

void testNonAtCommandIsNotAccepted() {
    ModemResponse r;
    TEST_ASSERT(mca.send("NON-AT-COMMAND", r, TIMEOUT) == false);
    wait(1);
}

void testOkCmdResponse() {
    modem.reset();
    modem.setExpectString("AT+UNITTEST\r\n");
    modem.setResponse("+KEY1:1\r\n+KEY2:2\r\nRESPLINE\r\nOK\r\n");

    ModemResponse r;
    bool res = mca.send("AT+UNITTEST", r, TIMEOUT);
    wait(1);

    TEST_ASSERT(res == true);
    TEST_ASSERT(r.isOk() == true);
    TEST_ASSERT(r.hasError() == false);
    TEST_ASSERT(r.getCommandResponses().size() == 2);

    string v;
    TEST_ASSERT(r.getCommandResponse("+KEY1", v) == true);
    TEST_ASSERT(v == "1");

}

void testErrCode() {
    modem.reset();
    modem.setExpectString("AT+UNITTEST\r\n");
    modem.setResponse("+CME ERROR: 47\r\n");

    ModemResponse r;
    bool res = mca.send("AT+UNITTEST", r, TIMEOUT);
    wait(1);

    TEST_ASSERT(res == true);
    TEST_ASSERT(r.isOk() == false);
    TEST_ASSERT(r.hasError() == true);
    TEST_ASSERT(r.getErrCode() == 47);
}

int main() {
    wait(1);

    //
    pc.baud(115200);

    testNonAtCommandIsNotAccepted();
    testOkCmdResponse();
    testErrCode();

    //

    printf("SUMMARY: %d asserts, %d OK, %d FAILED.\n", ASSERT_NUM_TOTAL, ASSERT_NUM_OK, ASSERT_NUM_FAILED);
}
