/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300/1310 LoRa module.
  This example code is in the public domain.
  MODIFIED BY GOUGH LUI FOR ELEMENT14 SAVE THE BEES CHALLENGE MAR-2023
*/

#include <MKRWAN_v2.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include "ArduinoLowPower.h"
//#include "DFRobot_ECPRO.h"

#define EC_PIN A0
#define TE_PIN A1
#define PH_PIN A2
#define RES2 820.0
#define ECREF 200.0
#define GDIFF (30/1.8)
#define VR0  0.223
#define G0  2
#define I  (1.24 / 10000)



//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3
#define Switch 4

SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);


uint16_t EC_Voltage, TE_Voltage;
float Conductivity, Temp;
float vv_old=0;

LoRaModem modem;

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

int cnt = 0;

void setup() {
pinMode(4, OUTPUT);
pinMode(SDA, INPUT_PULLUP);
pinMode(SCL, INPUT_PULLUP);


 digitalWrite(Switch, HIGH);
 //SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);
 delay(2000);
 analogReadResolution(12);
  Wire.begin();
  Wire.setClock(100000);



  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    //Serial.println("Failed to start module");
    while (1) {}
  };
  //Serial.print("Your module version is: ");
  //Serial.println(modem.version());
  //Serial.print("Your device EUI is: ");
  //Serial.println(modem.deviceEUI());
  // From https://docs.arduino.cc/tutorials/mkr-wan-1310/lorawan-regional-parameters
  //modem.sendMask("ff000000f000ffff00020000");
  int connected = 0;

  while (!connected) {
    //Serial.println("Attempting to join network ...");
    connected = modem.joinOTAA(appEui, appKey);
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
}

void loop() {

  pinMode(4, OUTPUT);
 digitalWrite(Switch, HIGH);
 delay(1000);
 SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);
  Wire.begin();
  Wire.setClock(100000);
  // Serial.begin(115200);
 delay(1000);


  if (distanceSensor.init() == false){  
    //Serial.println("Sensor online!");
    }

  //Serial.println();
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  float d=0;
  for(int i = 0; i < 10; i++) {
   delay(1000);
    d = d + distanceSensor.getDistance()*10; //Get the result of the measurement from the sensor
   }
  distanceSensor.stopRanging();
  float dd=d/100;
  int h=0;
  int vv=0;  
  int ero; 
  float v;
  h=400 - dd;
  if (h <= 0) {
    vv = 0; }
  else {
  v = 0.0290565*dd*dd - 71.101*dd + 23921.67;
  vv = v/10; 
  ero = (vv - vv_old);
}
  if (ero <= 0) {
    ero = 0; }
  else {
    ero = (vv - vv_old);
  }
  vv_old= vv;

  uint16_t EC_Voltage =0, TE_Voltage=0;
  float PH_Voltage=0, Offset=7.0;
   for(int i = 0; i < 16; i++) {
   delay(500);
  EC_Voltage = EC_Voltage + (uint32_t)analogRead(EC_PIN)* 3280 / 4096;
  TE_Voltage = TE_Voltage + (uint32_t)analogRead(TE_PIN)* 3280 / 4096;
  PH_Voltage = PH_Voltage + (float)analogRead(PH_PIN)* 3280 / 4096;
   }
   EC_Voltage = EC_Voltage/16;
   TE_Voltage = TE_Voltage/16;
   PH_Voltage = PH_Voltage/16;

  float Rpt1000 = (TE_Voltage/GDIFF+VR0)/I/G0;
  float Temp = (Rpt1000-1000)/3.85/2000;
  float pHValue = 0.01690*(PH_Voltage - 1500)/3+ Offset;
  int pH = pHValue * 100;
  float ecvalueRaw = 100000 * EC_Voltage / 820.0 / 200.0 * 1.16;
  float value = ecvalueRaw / (1.0 + 0.02 * (Temp - 25.0));
  value = value/10;
//*
  Serial.print("Distance_dd(mm): ");
  Serial.println(dd);
  Serial.print("Distance_d(mm): ");
  Serial.println(d);
  Serial.print("Vol (v): ");
  Serial.println(v);
  Serial.print("EC_Voltage: " + String(EC_Voltage) + " mV\t");
  Serial.print("Conductivity: " + String(value) + " mS/cm\t");
  Serial.print("TE_Voltage: " + String(TE_Voltage) + " mV\t");
  Serial.println("Temp: " + String(Temp) + " ℃");
  Serial.println("pH: " + String(pHValue) + " ");
  Serial.println("R: " + String(Rpt1000) + " Ohm");
  Serial.println("Volume (vv): " + String(vv) + " l");
  Serial.print("Erotus(l): ");
  Serial.println(ero);
//*/
  int ecvalue = (value)*1; //dht.readTemperature() * 100;

  byte payload[8];
  payload[0] = highByte(vv);
  payload[1] = lowByte(vv);
  payload[2] = highByte(ecvalue);
  payload[3] = lowByte(ecvalue);
  payload[4] = highByte(pH);
  payload[5] = lowByte(pH);
  payload[6] = highByte(ero);
  payload[7] = lowByte(ero);
  

  modem.setPort(1);
  modem.beginPacket();
  modem.write(payload, sizeof(payload));
  modem.endPacket(false);
  //delay(100000);

  int err;
 // modem.beginPacket();
 // modem.print(msg);
 // modem.endPacket(false);

  delay(1000);
  if (!modem.available()) {
  //  Serial.println("No downlink message!");
  } else {
    char rcv[64];
    int i = 0;
    while (modem.available()) {
      rcv[i++] = (char)modem.read();
    }
    //Serial.print("Received: ");
    for (unsigned int j = 0; j < i; j++) {
    }
  }
  //Serial.println("Waiting for next Transmit");
  delay(5000); // 2 minute intervals
  Wire.end();
  digitalWrite(Switch, LOW);
  LowPower.sleep(1800000); //30min
  
}