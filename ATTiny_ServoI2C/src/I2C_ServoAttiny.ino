/* Sweep
 by Abdullah Saei
 Create Servo I2C peripheral using ATTiny85.

    RESET -<----->- 5V
    RX    -|     |- PIN 2
    TX    -|     |- Servo PWM PIN 1
    GND   -<----->- PWM PIN 0

  TX and RX are enabled only in debug mode
*/
#define DEBUG 0

#include <Adafruit_SoftServo.h>

Adafruit_SoftServo myservo;  // create servo object to control a servo
volatile uint8_t pos = 84;    // variable to store the servo position
volatile uint8_t counter = 0;

#define I2C_SLAVE_ADDRESS 0x16 // replace address if needed
#include <TinyWireS.h>

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#if DEBUG
#include <SoftwareSerial.h>
#define rxPin 3
#define txPin 4
SoftwareSerial serial(rxPin, txPin);
#endif


void receiveEvent(uint8_t howMany) {
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }
    if (howMany > TWI_RX_BUFFER_SIZE)
    {
        // Also insane number
        return;
    }

  while (0 < TinyWireS.available()) { // loop through all but the last
    pos = TinyWireS.receive(); // receive byte as a character
  }
}


void setup() {
  OCR0A = 0xAF;     
  TIMSK |= _BV(OCIE0A);

#if DEBUG
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);  
  serial.begin(115200);
  serial.print("I2C address: ");
  serial.print(I2C_SLAVE_ADDRESS, HEX);
  serial.println(" -> DEBUG MODE");
#endif

  myservo.attach(1);

  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onReceive(receiveEvent);
}

void loop() {
  TinyWireS_stop_check();
  if (pos != 0){
#if DEBUG
    serial.print("pos: ");
    serial.println(pos);
#endif
    myservo.write(pos);
    pos = 0;
  } else if(pos == 84){
#if DEBUG
//    serial.print("No initialization yet");
#endif
    
    } 
  // tell servo to go to position in variable 'pos'
  tws_delay(15);                       // waits 15ms for the servo to reach the position
}


// We'll take advantage of the built in millis() timer that goes off
// to keep track of time, and refresh the servo every 20 milliseconds
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, refresh the servos!
  if (counter >= 20) {
    counter = 0;
    myservo.refresh();
  }
}
