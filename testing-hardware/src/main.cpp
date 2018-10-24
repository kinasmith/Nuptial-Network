#include <Arduino.h>
/****************************************************************************/
/***********************    DON'T FORGET TO SET ME    ***********************/
/****************************************************************************/
#define NODEID    9 //Node Address
#define NETWORKID 100 //Network to communicate on
/****************************************************************************/

#define GATEWAYID 0 //Address of datalogger/reciever
#define FREQUENCY RF69_433MHZ
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define LED 3 //LED pin for UAF Boards
#define LED 8 //LED pin for DIY Boards
#define SERIAL_BAUD 9600 //Serial communication rate

/*==============|| Easy way of commenting out Serial Prints, etc for deployment ||==============*/
#define SERIAL_EN //Comment this out to remove Serial comms and save a few kb's of space
#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif

ISR(WDT_vect){ Sleepy::watchdogEvent();} // set watchdog for Sleepy

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Declare radio (using Automatic Power Adjustments)

/*==============|| UTIL ||==============*/
uint16_t count = 0; //measurement number

//Data Structure for transmitting data packets to datalogger
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	uint32_t sense;
	double brd_tmp;
	double bat_v;
	double ref_v;
};
Payload thePayload;

void setup() {
    #ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD); //only start Serial if DEBUG is on
	#endif
	Wire.begin(); // Begin i2c
	randomSeed(analogRead(0)); //set random seed
	pinMode(LED, OUTPUT);      // Set LED Mode
	//Setup Radio
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI);
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	// Ping the datalogger. If it is alive, it will respond with a 1, and latch to this node until it recieves an "r"
}

void loop() {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
}