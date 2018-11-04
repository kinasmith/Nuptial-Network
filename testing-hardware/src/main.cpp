#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <elapsedMillis.h>

void Blink(byte, int);
int getRSSI(byte);

#define NODEID      2
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
// #define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9  //motieno
// #define LED         3 //diy
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!

RFM69 radio;
unsigned int interval = 10000;
elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs
int rssiThreshold = -100;


typedef struct {
  unsigned long uptime; //uptime in ms
} Payload;
Payload theData;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.promiscuous(false);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  delay(random(500,2000));
}

byte ackCount=0;
byte lastRequesterNodeID = NODEID;

void loop() {
  //listening and responding to other nodes
  bool ping = false;
  bool response = false;
  if (radio.receiveDone())
  {
    lastRequesterNodeID = radio.SENDERID;
    Serial.print("from ["); Serial.print(radio.SENDERID, DEC); Serial.print("] ");
    Serial.print("sig: "); Serial.print(radio.readRSSI());
    Serial.print(" ->"); Serial.print(radio.TARGETID, DEC);
    Serial.println();

    if(radio.DATALEN == 1 && radio.DATA[0] == 'p' && radio.TARGETID == 255) {
      // if(radio.readRSSI() > rssiThreshold) {
        ping = true;
        Serial.println("<- ping");
      // }
    }

    if(radio.DATALEN == 1 && radio.DATA[0] == 'r' && radio.TARGETID == NODEID) {
      // if(radio.readRSSI() > rssiThreshold) {
        response = true;
        Serial.print("<-"); Serial.println(radio.SENDERID);
      // } else Serial.println(radio.readRSSI());
    }
    
    if (radio.ACKRequested()) {
      radio.sendACK();
      // Serial.println(" - ACK sent");
      delay(10);
    }
    Blink(LED,3);
  }

	if(ping) {
    delay(random(4,25));
		// Serial.print("to ["); Serial.print(lastRequesterNodeID); Serial.print("] ");
		if(radio.sendWithRetry(lastRequesterNodeID, "r", 1)) {
			Serial.print("->"); Serial.println(lastRequesterNodeID);
		} else {
			// Serial.println("...nothing");
		}
    // Serial.println("**********");
	}

  if (timeElapsed > interval) {
    Serial.println("---> ping");
		radio.send(255, "p", 1);
    Blink(LED,3);
    timeElapsed = 0;              // reset the counter to 0 so the counting starts over...

  }
}

int getRSSI(byte ID) {
  return 7;
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
