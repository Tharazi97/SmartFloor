#include "mbed.h"
#include "Dht11.h"
#include "ESP01.h"
#include <string>

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
Mutex printingMutex;

// Communication
ESP01 esp(D14, D15, 9600);

// Outputs
DigitalOut led1(LED1);

// Sensors
Dht11 temperature(D2);

// Periodical functions
void changeLED(void)
{
    /*
    printingMutex.lock();
    printf("LED switch on thread: %p\r\n", ThisThread::get_id());
    printingMutex.unlock();*/
    // Toggle LED
    led1 = !led1;
}

void readDht(void)
{
    temperature.read();
}

int main()
{
    printf("Starting in context %p\r\n", ThisThread::get_id());

    // Fill up the event queue
    queue.call_every(500, changeLED);
    queue.call_every(5000, readDht);

    // Start the event queue
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    // Connect ESP to wifi
    if (esp.connect("WIFINAME", "WIFIPASSWORD")) {
        printf("CONNECTED\r\n");
    } else {
        printf("ERROR ESP DIDN'T CONNECT\r\n");
    }

    while (1) {
        /*
        ThisThread::sleep_for(2000);
        printingMutex.lock();
        printf("temperature: %d, humidity: %d\r\n", temperature.getCelsius(), temperature.getHumidity());
        printingMutex.unlock();
        */
        ThisThread::sleep_for(5000);
        char wilczes[] = "wilczes\r\n";
        esp.sendBytes(wilczes, sizeof(wilczes));
    }
}