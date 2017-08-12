
// servo section
//include the servo library
#include <Servo.h>  

//create a servo object called servo1 
Servo servo1;  
int current_position;

// ble section
#include <CurieBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedCharCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = 13; // pin to use for the LED


// lcd section
/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*-----( Declare Variables )-----*/
//NONE


void setup()
{
  // ble setup
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");


  // servo setup
  //attach servo1 to pin 9 on the Arduino 101
  servo1.attach(9);
  
  current_position = 0;
  initial(current_position);
  
  // Pause to get it time to move
  delay(1000);          

  // lcd setup

    lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight

    // ------- Quick 3 blinks of backlight  -------------
      for(int i = 0; i< 3; i++)
      {
        lcd.backlight();
        delay(250);
        lcd.noBacklight();
        delay(250);
      }
      lcd.backlight(); // finish with backlight on  
    
    //-------- Write characters on the display ------------------
    // NOTE: Cursor Position: (CHAR, LINE) start at 0  
      lcd.setCursor(0,0); //Start at character 4 on line 0
      lcd.print("Hello, world!");
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("Thi's Smart Ads.");
      delay(8000);  
    

}

void clear() {
  lcd.clear();
  // ------- Quick 3 blinks of backlight  -------------
    for(int i = 0; i< 3; i++)
    {
      lcd.backlight();
      delay(250);
      lcd.noBacklight();
      delay(250);
    }
    lcd.backlight(); // finish with backlight on  
  
}

void moveto(int current, int next) {
  int position;
  if (next > current) {
    // Tell servo to go to 90 degrees, stepping by one degree
    for(position = current; position < next; position += 1)
    { 
      // Move to next position
      servo1.write(position);
      // Short pause to allow it to move
      delay(20);               
    }    
  } else {
    // Tell servo to go to 90 degrees, stepping by one degree
    for(position = current; position >= next; position -= 1)
    { 
      // Move to next position
      servo1.write(position);
      // Short pause to allow it to move
      delay(20);               
    }        
  }

  delay(1000);
  current_position = next;
}

void initial(int position) {
  moveto(position, 90);
}

void launch(int position) {
  moveto(position, 180);
}

void loop()
{ 

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        switch(switchCharacteristic.value()) {
          case 2:
            clear();
            lcd.setCursor(0,0); 
            lcd.print("I Love You!");
            break;
          case 3:
            clear();
            lcd.setCursor(0,0); 
            lcd.print("Halo, IoT World!");
            break;
          default:
            if (switchCharacteristic.value()) {   // any value other than 0
              Serial.println("LED on");
              digitalWrite(ledPin, HIGH);         // will turn the LED on
              launch(current_position);
            } else {                              // a 0 value
              Serial.println(F("LED off"));
              digitalWrite(ledPin, LOW);          // will turn the LED off
              initial(current_position);
            }
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }

}