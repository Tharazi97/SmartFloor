#include "ESP01.h"

ESP01::ESP01(PinName tx, PinName rx, int baud)
{
    _wifi = new Serial(tx, rx, baud);
}

ESP01::~ESP01()
{
    delete _wifi;
}

bool ESP01::echoFind(string keyword, uint32_t timeout_ms)
{
    char current_char = 0;
    char keyword_length = keyword.length();
    Timer tim;
    tim.start();
    while (tim.read_ms() < timeout_ms) {
        if (_wifi->readable()) {
            char ch = _wifi->getc();
            printf("%c", ch);
            if (ch == keyword[current_char]) {
                if (++current_char == keyword_length) {
                    printf("\r\n");
                    return true;
                }
            } else {
                current_char = 0;
            }
        }
    }
    return false; // Timed out
}

bool ESP01::sendCommand(string cmd, string ack, uint32_t timeout_ms)
{
    _wifi->puts(cmd.c_str()); // Send "AT+" command to module
    for (int i = 0; i < cmd.length(); ++i) { // Flush the msg that just was sent
        char ch = _wifi->getc();
        printf("%c", ch);
    }
    if (!echoFind(ack, timeout_ms)) {      // timed out waiting for ack string
        return false;    // ack blank or ack found
    }
    return true;
}

bool ESP01::connect(string name, string password)
{
    if (!sendCommand("AT+RST\r\n", "ready", 5000)) { // Restart esp
        return false;
    }
    if (!sendCommand("AT+CWMODE=1\r\n", "OK", 5000)) { // Change mode to client
        return false;
    }
    if (!sendCommand("AT+CWJAP=\"" + name + "\",\"" + password + "\"\r\n", "OK", 60000)) { // Connect to wifi
        return false;
    }
    if (!sendCommand("AT+CIPMUX=1\r\n", "OK", 5000)) { // Multiple outputs
        return false;
    }
    if (!sendCommand("AT+CIPSERVER=1,80\r\n", "OK", 5000)) { // Start working as a server
        return false;
    }
    return true;
}

bool ESP01::sendString(string msg, uint32_t timeout_ms)
{
    while (_wifi->readable()) { // Read all data on serial
        char ch = _wifi->getc();
        printf("%c", ch);
    }
    // Send command and msg
    if (!sendCommand("AT+CIPSEND=0," + to_string(msg.length()) + "\r\n", ">", timeout_ms)) {
        return false;
    }
    if (!sendCommand(msg + "\r\n", "OK", timeout_ms)) {
        return false;
    }
    if (!sendCommand("AT+CIPCLOSE=0\r\n", "OK", timeout_ms)) {
        return false;
    }
    return true;
}

bool ESP01::sendBytes(const char *msg, uint16_t size, uint32_t timeout_ms)
{
    while (_wifi->readable()) { // Read all data on serial
        char ch = _wifi->getc();
        printf("%c", ch);
    }
    // Send command and msg
    if (!sendCommand("AT+CIPSEND=0," + to_string(size) + "\r\n", ">", timeout_ms)) {
        return false;
    }

    for (int i = 0; i < size; ++i) {
        _wifi->putc(msg[i]);
    }
    _wifi->puts("\r\n");

    if (!echoFind("OK", timeout_ms)) {      // timed out waiting for ack string
        return false;    // ack blank or ack found
    }

    if (!sendCommand("AT+CIPCLOSE=0\r\n", "OK", timeout_ms)) {
        return false;
    }
    return true;
}

bool ESP01::readToString(string &msg)
{
    if (!_wifi->readable()) {
        return false;
    } else {
        while (_wifi->readable()) {
            msg += _wifi->getc();
        }
    }
    return true;
}

void ESP01::flush()
{
    while (_wifi->readable()) { // Flush serial
        _wifi->getc();
    }
}
