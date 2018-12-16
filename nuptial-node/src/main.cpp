#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <elapsedMillis.h>

void Blink(byte, int);
int getRSSI(byte);

#define NODEID      14
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
// #define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
// #define LED         9  //motieno
#define LED         8 //DIY Boards, LOW is OFF
// #define LED         3 //dendro's, LOW in ON
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define NODE_ARRAY_LENGTH 20
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network


RFM69 radio;
uint32_t interval = 30000;
elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs
uint16_t collectionTimeoutInterval = 100;
elapsedMillis collectionTimeout; //timeout for collecting PING responses
int8_t rssiThreshold = -100;

typedef struct {
  uint8_t id;
  uint8_t bv;
  int8_t rssi;
} nodeData;
nodeData NODE;

typedef struct {
  uint8_t thisNodeID = NODEID; //store this nodeId
  nodeData node[NODE_ARRAY_LENGTH]; //other nodes in proximity
} Payload;
Payload toGateway;

typedef struct {
  int8_t val;
} rssi;
rssi fakeRSSI;
rssi rcvRSSI;

void setup() {
  randomSeed(analogRead(0));
  delay(random(500,2000));
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.promiscuous(promiscuousMode);
  Serial.print(NETWORKID); Serial.print("."); Serial.print(NODEID);
  Serial.println(" initialized at 433MHz");
  for(uint8_t i = 0; i < NODE_ARRAY_LENGTH; i++) {
    toGateway.node[i] = NODE;
    toGateway.node[i].id = 0;
    toGateway.node[i].bv = random(0,255);
    toGateway.node[i].rssi = 0;
  }
}

uint8_t lastRequesterNodeID;
uint8_t nodesIndex;

void loop() {
  if (timeElapsed > interval) {
    //Send collected data to Gateway
    if(toGateway.node[0].id > 0) { //if there is data....don't send if there isn't
      Serial.print("[");Serial.print(NODEID, DEC);Serial.print("]->[Gateway]");
      if (radio.sendWithRetry(GATEWAYID, (const void*)(&toGateway), sizeof(toGateway))) {
        Serial.println(" ok!");
        //once the data is sent, clear array for next time
        for(uint8_t i = 0; i < NODE_ARRAY_LENGTH; i++) {
          toGateway.node[i].id = 0;
          toGateway.node[i].bv = 0;
          toGateway.node[i].rssi = 0;
        }
      }
    }
      //send out ping
    Serial.println("*** ping ***");
		radio.send(255, "p", 1);
    Blink(LED,10);
    Blink(LED,10);
    timeElapsed = 0;              // reset the counter to 0 so the counting starts over...
    nodesIndex = 0;
  }

  //handle ping
  bool pingRequested = false;
  bool pingResponse = false;

  if (radio.receiveDone()) {
    lastRequesterNodeID = radio.SENDERID;
    //handle messages
    if(radio.DATALEN == 1 && radio.DATA[0] == 'p' && radio.TARGETID == 255)
      pingRequested = true;
    if(radio.DATALEN == sizeof(fakeRSSI) && radio.TARGETID == NODEID) {
      pingResponse = true;
      rcvRSSI = *(rssi*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      //***PRINTS***//
      Serial.print("[");Serial.print(radio.SENDERID, DEC);Serial.print("]"); 
      Serial.print("->["); Serial.print(radio.TARGETID, DEC);Serial.print("] ");
      // Serial.print(" real rssi: "); Serial.print(radio.readRSSI());
      Serial.print(" fake rssi: "); Serial.print(rcvRSSI.val);
    }
    //respond to ACK
    if (radio.ACKRequested()) {
      radio.sendACK();
      if(radio.TARGETID != 255)
        Serial.println(" - ACK sent");
    } else {
      if(radio.TARGETID != 255)
        Serial.println(" - NO ACK!");
    }
  }
  //handles a ping request. Sends Fake RSSI Value to Requester
	if(pingRequested) { 
    Serial.print(">> Ping Requested <<");
    Serial.println();
    fakeRSSI.val = random(20, 120) * -1;
    delay(random(1,25)); //keeps responses from overlapping
		if(radio.sendWithRetry(lastRequesterNodeID, (const void*)(&fakeRSSI), sizeof(fakeRSSI))) {
      //***PRINTS***//
      Serial.print("[");Serial.print(NODEID, DEC);Serial.print("]"); 
      Serial.print("->[");Serial.print(lastRequesterNodeID, DEC);Serial.print("] : ");
      Serial.print("fake rssi:");Serial.println(fakeRSSI.val);
		}
	}
  //handles responses from other nodes. Saves value to Array to Send to Gateway
  if(pingResponse) {
    toGateway.node[nodesIndex].id = lastRequesterNodeID;
    toGateway.node[nodesIndex].bv = random(0,255);
    toGateway.node[nodesIndex].rssi = rcvRSSI.val;
    nodesIndex++;
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}