#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <elapsedMillis.h>

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         3 
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define NODE_ARRAY_LENGTH 20

void Blink(byte, int);


bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

RFM69 radio;



typedef struct {
  uint8_t id;
  uint8_t bv;
  int8_t rssi;
} nodeData;
nodeData NODE;

typedef struct {
  uint8_t rcvNodeID; //store this nodeId
  nodeData node[NODE_ARRAY_LENGTH]; //other nodes in proximity
} Payload;
Payload fromNode;


// typedef struct {
//   uint8_t rcvNodeID; //store this nodeId
//   uint8_t nodes[NODE_ARRAY_LENGTH]; //other nodes in proximity
// } Payload;
// Payload theData;


void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  randomSeed(analogRead(0));
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.promiscuous(promiscuousMode);
  // Serial.print(NETWORKID); Serial.print("."); Serial.print(NODEID);
  // Serial.println(" initialized, listening at: 433MHz");
  for(uint8_t i = 0; i < NODE_ARRAY_LENGTH; i++) {
    fromNode.node[i] = NODE;
  }
}

byte lastRequesterNodeID = NODEID;
byte ackCount=0;

void loop() {
  bool printData = false;
  if (radio.receiveDone())
  {
    if(radio.TARGETID == 1) {
      // Serial.print("[");Serial.print(radio.SENDERID, DEC);Serial.print("]"); 
      // Serial.print("->["); Serial.print(radio.TARGETID, DEC);Serial.print("] ");
      // Serial.print(" rssi: "); Serial.print(radio.readRSSI());
      if (radio.DATALEN != sizeof(Payload))
        Serial.print("Invalid payload received, not matching Payload struct!");
      else
      {
        printData = true;
      }
      
      if (radio.ACKRequested())
      {
        radio.sendACK();
        // Serial.print(" - ACK sent.");
      }
      // Serial.println();
      Blink(LED,3);
    }
  }
  if(printData) {
    fromNode = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
    // Serial.print("***** [");
    Serial.print(fromNode.rcvNodeID);
    Serial.print(",");
    // Serial.println("] *****");
    for(uint8_t i=0; i<NODE_ARRAY_LENGTH; i++){
      if(fromNode.node[i].id > 0) {
        Serial.print(fromNode.node[i].id);
        Serial.print(",");
        Serial.print(fromNode.node[i].bv);
        Serial.print(",");        
        Serial.print(fromNode.node[i].rssi);
        Serial.print(",");        

      }
    }
    Serial.println();
  }

}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}