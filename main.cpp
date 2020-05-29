#include "mbed.h"
#include "Dht11.h"
#include "ESP01.h"
#include "MFRC522.h"
#include <string>

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
Mutex printingMutex;
Timeout timeoutDoors;

// Communication
ESP01 esp(D14, D15, 9600);

// Outputs
DigitalOut doors(D7);

// Sensors
Dht11 temperature(D2);
MFRC522    RfChip(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS, D8);

void closeDoors()
{
    doors = 0;
}

// Periodical functions
void readDht(void)
{
    temperature.read();
}

void readCard()
{
    static const uint8_t card[] = {0xD9, 0xE5, 0x1D, 0xDA};
    // Look for new cards
    if (! RfChip.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if (! RfChip.PICC_ReadCardSerial()) {
        return;
    }

    for (uint8_t i = 0; i < RfChip.uid.size; i++) {
        if (RfChip.uid.uidByte[i] != card[i]) {
            return;
        }
    }
    doors = 1;
    timeoutDoors.attach(&closeDoors, 5.0);
}

int main()
{
    printf("Starting in context %p\r\n", ThisThread::get_id());
    RfChip.PCD_Init();

    // Fill up the event queue
    queue.call_every(500, readCard);
    queue.call_every(5000, readDht);

    // Start the event queue
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    // Connect ESP to wifi
    /*
    if (esp.connect("WIFINAME", "WIFIPASSWORD")) {
        printf("CONNECTED\r\n");
    } else {
        printf("ERROR ESP DIDN'T CONNECT\r\n");
    }*/

    while (1) {
        /*
        ThisThread::sleep_for(2000);
        printingMutex.lock();
        printf("temperature: %d, humidity: %d\r\n", temperature.getCelsius(), temperature.getHumidity());
        printingMutex.unlock();

        ThisThread::sleep_for(5000);
        char wilczes[] = "wilczes\r\n";
        esp.sendBytes(wilczes, sizeof(wilczes));
        */
        ThisThread::sleep_for(5000);
    }
}