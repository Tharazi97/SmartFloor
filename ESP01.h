#include "mbed.h"
#include <string>

class ESP01 {
private:
    Serial *_wifi;
public:
    ESP01(PinName tx, PinName rx, int baud = 9600);
    ~ESP01();
    bool echoFind(string keyword, uint32_t timeout_ms);
    bool sendCommand(string cmd, string ack, uint32_t timeout_ms);
    bool connect(string name, string password);
    bool sendString(string msg, uint32_t timeout_ms = 5000);
    bool sendBytes(const char * msg, uint16_t size, uint32_t timeout_ms = 5000);
    bool readToString(string &msg);
    void flush();
};
