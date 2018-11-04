#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

void Blink(byte, int);

#define NODEID      1
#define NETWORKID   100
// #define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

int TRANSMITPERIOD = 1000; //transmit a packet to gateway so often (in ms)
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;

typedef struct {
  unsigned long uptime; //uptime in ms
} Payload;
Payload theData;

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.promiscuous(true);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
}

long lastPeriod = -1;

void loop() {
  //check for any received packets
  if (radio.receiveDone()) {
    Serial.print(NETWORKID); Serial.print("."); Serial.print(radio.SENDERID, DEC);
    Serial.print(" RSSI-"); Serial.print(radio.readRSSI());

    if (radio.ACKRequested()) {
      radio.sendACK();
      Serial.print(" - ACK sent");
      delay(10);
    }
    Blink(LED,5);
    Serial.println();
  }
  
  int currPeriod = millis()/TRANSMITPERIOD;

  if (currPeriod != lastPeriod) {
    Serial.print("Sending ping");
		radio.send(255, "p", 1);
    Serial.println();
    Blink(LED,3);
    lastPeriod = currPeriod;
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
