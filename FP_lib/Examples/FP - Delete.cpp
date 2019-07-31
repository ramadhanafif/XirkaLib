#include "FP_lib.h"
#include "CMSDK_CM3.h"
#include <stdio.h>
//#include "uart_stdout.h"
#include "lcd_xirka.h"
#include <stdlib.h>

#define delay delaymilis
#define ID_DELETE 10
#include "XST_UART.h"

Adafruit_Fingerprint finger;
XST_UART Serial(UART1);
void setup(void);
void loop(void);


int main(){
	setup();
	Serial.println(100);
	while(1){
		loop();
		break;
	}
	return 0;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); //Serial.println(p, HEX);
    return p;
  }
}

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nDelete Finger");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
}


void loop()                     // run over and over again
{
  uint8_t id = ID_DELETE;

  if (id == 0) {// ID #0 not allowed, try again!
    return;
  }

  Serial.print("Deleting ID #");
  Serial.println(id);

  deleteFingerprint(id);
}
