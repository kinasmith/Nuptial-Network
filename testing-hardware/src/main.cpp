#include <Arduino.h>
#include <RFM69_ATC.h>//https://www.github.com/lowpowerlab/rfm69
#include <elapsedMillis.h>
#include "Sleepy.h" 

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
//#define LED 3 //LED pin for UAF Boards
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

/*==============|| Functions ||==============*/
void Blink(byte, int);
ISR(WDT_vect){ Sleepy::watchdogEvent();} // set watchdog for Sleepy

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Declare Radio
byte lastRequesterNodeID = NODEID; //Last Sensor to communicate with this device
int8_t NodeID_latch; //Listen only to this Sensor #

/*==============|| UTIL ||==============*/
uint16_t count = 0; //measurement number

//Data Structure for transmitting data packets to datalogger
struct Payload {
	uint16_t count;
	double bat_v;
};
Payload thePayload;

void setup() {
    #ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD); //only start Serial if DEBUG is on
	#endif
	randomSeed(analogRead(0)); //set random seed
	pinMode(LED, OUTPUT);      // Set LED Mode
	//Setup Radio
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	// radio.enableAutoPower(ATC_RSSI);
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
}

elapsedMillis trigger_time;

void loop() {
	// bool ping = false;
	// if (radio.receiveDone()) { //if recieve packets from sensor...
	// 	DEBUG("rcv < "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] ");
	// 	// lastRequesterNodeID = radio.SENDERID; //SENDERID is the Node ID of the device that sent the packet of data
		// if(radio.DATALEN == 1 && radio.DATA[0] == 'p') {
		// 	DEBUGln("p");
		// 	ping = true;
		// }
		// if(radio.DATALEN == 1 && radio.DATA[0] == 'r') {
		// 	DEBUGln("r");
		// }
		// Blink(LED,5); //blink the LED really fast
	// 	if (radio.ACKRequested()) radio.sendACK(); //send acknoledgement of reciept
	// }
	// //sends a response back to confirm that the datalogger is alive
	// if(ping) {
	// 	DEBUG("snd > "); DEBUG('['); DEBUG(lastRequesterNodeID); DEBUG("] ");
	// 	if(radio.sendWithRetry(lastRequesterNodeID, "r", 1)) {
	// 		DEBUGln("r");
	// 	} else {
	// 		DEBUGln("Failed . . . no ack");
	// 	}
	// }

	if(trigger_time > 10000) {
		if(radio.sendWithRetry(1, "p", 1)) {
			DEBUG("snd > "); DEBUG('['); DEBUG("255"); DEBUGln("]");
		} else {
			DEBUGln("Failed . . . no ack");
		}
		trigger_time = 0;
	}
}

/**
 * Blinks an LED
 * @param PIN      Pin to Blink
 * @param DELAY_MS Time to Delay
 */
void Blink(byte PIN, int DELAY_MS) {
	digitalWrite(PIN,HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN,LOW);
	delay(DELAY_MS);
}