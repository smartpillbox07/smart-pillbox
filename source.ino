/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled 2"
  https://create.arduino.cc/cloud/things/7aa986f6-d4cb-4ac7-80eb-8cc541a4a996

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  CloudSchedule pill1;
  CloudSchedule pill2;
  CloudSchedule pill3;
  bool triggerPill1;
  bool triggerPill2;
  bool triggerPill3;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"

#include <ESP32Servo.h>
#include <Time.h>
#include <NewPing.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <WiFi.h>
#include <HTTPClient.h>
// #include "HTTPSRedirect.h"
#include <WiFiClientSecureBearSSL.h>

const char* wifi_ssid = "HOMEWiFiBR";
const char* wifi_password = "PLDTWiFiBRTorinoFamily_2023";
String requestName = "https://script.google.com/macros/s/AKfycbzUigA_4Lku71Xcwj1bIo5QqDBuLPZjbzys90cKQkwBYCMq6dtXaJ3a3yxF3rj97golig/exec?";
String medName = "";
String compNum = "";
// const char *GScriptId = “AKfycbxD75rbn5gBMBx5AKK_wttlbJtXRxRUr4TYZ1fG0iIPd_ZuCIVgkMmnoQclq71v53hMSA”;

// const char* host = "script.google.com";
// const char* googleRedirHost = "script.googleusercontent.com";

// const int httpsPort =  443;
// HTTPSRedirect client(httpsPort);

// String url = String(“/macros/s/”) + GScriptId + “/exec?”;

// Servo PINS and setup
#define MED_1 19 // ESP32 pin GPIO19 connected to servo motor
#define MED_2 18 // ESP32 pin GPIO18 connected to servo motor
#define MED_3 5  // ESP32 pin GPIO05 connected to servo motor

Servo servoMed1;
Servo servoMed2;
Servo servoMed3;

// I2C Display
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// IR Obstacle
int inputPin = 36;

// Buzzer for Alarm
int buzzerPin = 2;

void setup() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  servoMed1.attach(MED_1);  // attaches the servo on ESP32 pin
  servoMed2.attach(MED_2);  // attaches the servo on ESP32 pin
  servoMed3.attach(MED_3);  // attaches the servo on ESP32 pin
  initializeAllServo();

  display.begin(i2c_Address, true); // Address 0x3C default

  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // IR Pin
  pinMode(inputPin, INPUT);

  // Buzzer Pin
  pinMode(buzzerPin, OUTPUT);

  // Initialize Trigger
  triggerPill1 = LOW;
  triggerPill2 = LOW;
  triggerPill3 = LOW;
    //Initialize Wifi Connection
    WiFi.begin(wifi_ssid, wifi_password);
}

void loop() {
  ArduinoCloud.update();

  // Your code here
  checkMedSchedule();

}

void initializeAllServo()
{
  servoMed1.write(0);
  delay(50);
  servoMed2.write(0);
  delay(50);
  servoMed3.write(0);
  delay(50);
}

// checking the schedule of each MedPill
// set the duration on the widget for 1 second
// each pill active, print in OLED (TBC)
void checkMedSchedule(void)
{
  HTTPClient http;
  if (pill1.isActive())
  {
    digitalWrite(buzzerPin, HIGH);

    triggerPill1 = HIGH; // Activate Trigger to send push notification and email

    display.display();
    display.clearDisplay();

    Schedule localPill1Schedule = pill1.getValue();

    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 30);
    // Display static text
    display.print("Clopidogrel");
    display.print(hour(localPill1Schedule.frm));
    display.print(":");
    display.print(minute(localPill1Schedule.frm));
    display.print(":");
    display.print(second(localPill1Schedule.frm));
    display.println("");
    display.display();

    Serial.print("It is time to take Clopidogrel now: ");
    Serial.print(hour(localPill1Schedule.frm));
    Serial.print(":");
    Serial.print(minute(localPill1Schedule.frm));
    Serial.print(":");
    Serial.print(second(localPill1Schedule.frm));
    Serial.println("");

   while((digitalRead(inputPin) == HIGH)) // lock the med if there is no hands
    {
      Serial.println("IR not triggered");
    }                                        // Set the Schedule Duration for 10mins
    Serial.println("IR Triggered hand detected");

    openMed1();

    delay(5000);

    closeMed1();
    display.setCursor(0, 40);
    // Display static text
    display.print("Clopidogrel taken ");
      int httpcode = http.GET();
      if(httpcode > 0) {
        Serial.println("Request okay");
      } else {
        Serial.println("Request failed");
        Serial.println(http.errorToString(httpcode).c_str());
      }
      medName = "Clopidogrel";
      compNum = "1";
      
      String requestPath = requestName + "medicinename=" + medName + "&compartmentnumber=" + compNum;
      http.begin(requestPath.c_str());
      http.end();
      Serial.println(requestPath.c_str());
      
    //   if (!client.connected()){
    //         Serial.println(“Connecting to client again…”);
    //         client.connect(requestPath.c_str(), httpsPort);
    // }
    // client.printRedir(requestPath.c_str(), host, googleRedirHost);
  }

  if (pill2.isActive())
  {
    digitalWrite(buzzerPin, HIGH);

    triggerPill2 = HIGH; // Activate Trigger to send push notification and email

    display.display();
    display.clearDisplay();

    Schedule localPill2Schedule = pill2.getValue();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 30);
    // Display static text
    display.print("Rosuvastatin");
    display.print(hour(localPill2Schedule.frm));
    display.print(":");
    display.print(minute(localPill2Schedule.frm));
    display.print(":");
    display.print(second(localPill2Schedule.frm));
    display.println("");
    display.display();
    Serial.print("It is time to take Rosuvastatin now: ");
    Serial.print(hour(localPill2Schedule.frm));
    Serial.print(":");
    Serial.print(minute(localPill2Schedule.frm));
    Serial.print(":");
    Serial.print(second(localPill2Schedule.frm));
    Serial.println("");

   while((digitalRead(inputPin) == HIGH)) // lock the med if there is no hands
    {
      Serial.println("IR not triggered");
    }                                        // Set the Schedule Duration for 10mins
    Serial.println("IR Triggered hand detected");

    openMed2();
    delay(5000);
    closeMed2();
    display.setCursor(0, 40);
    // Display static text
    display.print("Rosuvastatin taken ");
      medName = "Rosuvastatin";
      compNum = "2";
      
      String requestPath = requestName + "medicinename=" + medName + "&compartmentnumber=" + compNum;
      http.begin(requestPath.c_str());
      http.end();

  }

  if (pill3.isActive())
  {
    digitalWrite(buzzerPin, HIGH);

    triggerPill3 = HIGH; // Activate Trigger to send push notification and email

    display.display();
    display.clearDisplay();

    Schedule localPill3Schedule = pill3.getValue();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 30);
    // Display static text
    display.print("Ranolazine");
    display.print(hour(localPill3Schedule.frm));
    display.print(":");
    display.print(minute(localPill3Schedule.frm));
    display.print(":");
    display.print(second(localPill3Schedule.frm));
    display.println("");
    display.display();
    Serial.print("It is time to take Ranolazine now: ");
    Serial.print(hour(localPill3Schedule.frm));
    Serial.print(":");
    Serial.print(minute(localPill3Schedule.frm));
    Serial.print(":");
    Serial.print(second(localPill3Schedule.frm));
    Serial.println("");

  while((digitalRead(inputPin) == HIGH)) // lock the med if there is no hands
    {
      Serial.println("IR not triggered");
    }                                        // Set the Schedule Duration for 10mins
    Serial.println("IR Triggered hand detected");

    openMed3();
    delay(5000);
    closeMed3();
    display.setCursor(0, 40);
    // Display static text
    display.print("Ranolazine taken");
      medName = "Ranolazine";
      compNum = "3";
      
      String requestPath = requestName + "medicinename=" + medName + "&compartmentnumber=" + compNum;
      http.begin(requestPath.c_str());
      http.end();


  }
}

/*
  Since Scheduler is READ_WRITE variable, onSchedulerChange() is
  executed every time a new value is received from IoT Cloud.
*/

void onSchedulerChange()  {
  // Add your code here to act upon Scheduler change


}


/*
  Since Pill1 is READ_WRITE variable, onPill1Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onPill1Change()  {
  // Add your code here to act upon Pill1 change
  Schedule localPill1ScheduleChange = pill1.getValue();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  // Display static text
  display.println("Pill 1 Schedule : ");
  display.print(hour(localPill1ScheduleChange.frm));
  display.print(":");
  display.print(minute(localPill1ScheduleChange.frm));
  display.print(":");
  display.print(second(localPill1ScheduleChange.frm));
  display.println("");
  display.display();

  Serial.print("Pill 1 Schedule starts: ");
  Serial.print(hour(localPill1ScheduleChange.frm));
  Serial.print(":");
  Serial.print(minute(localPill1ScheduleChange.frm));
  Serial.print(":");
  Serial.print(second(localPill1ScheduleChange.frm));
  Serial.println("");

}

/*
  Since Pill2 is READ_WRITE variable, onPill2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onPill2Change()  {
  // Add your code here to act upon Pill2 change
  Schedule localPill2ScheduleChange = pill2.getValue();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 25);
  // Display static text
  display.println("Pill 2 Schedule : ");
  display.print(hour(localPill2ScheduleChange.frm));
  display.print(":");
  display.print(minute(localPill2ScheduleChange.frm));
  display.print(":");
  display.print(second(localPill2ScheduleChange.frm));
  display.println("");
  display.display();

  Serial.print("Pill 2 Schedule starts: ");
  Serial.print(hour(localPill2ScheduleChange.frm));
  Serial.print(":");
  Serial.print(minute(localPill2ScheduleChange.frm));
  Serial.print(":");
  Serial.print(second(localPill2ScheduleChange.frm));
  Serial.println("");
}


/*
  Since Pill3 is READ_WRITE variable, onPill3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onPill3Change()  {
  // Add your code here to act upon Pill3 change
  Schedule localPill3ScheduleChange = pill3.getValue();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 45);
  // Display static text
  display.println("Pill 3 Schedule : ");
  display.print(hour(localPill3ScheduleChange.frm));
  display.print(":");
  display.print(minute(localPill3ScheduleChange.frm));
  display.print(":");
  display.print(second(localPill3ScheduleChange.frm));
  display.println("");
  display.display();

  Serial.print("Pill 3 Schedule starts: ");
  Serial.print(hour(localPill3ScheduleChange.frm));
  Serial.print(":");
  Serial.print(minute(localPill3ScheduleChange.frm));
  Serial.print(":");
  Serial.print(second(localPill3ScheduleChange.frm));
  Serial.println("");
}

void openMed1(void)
{
  digitalWrite(buzzerPin, LOW);
  // rotates from 0 degrees to 180 degrees
  for (int pos = 0; pos <= 105; pos += 1) {
    // in steps of 1 degree
    servoMed1.write(pos);
    delay(15); // waits 15ms to reach the position
  }
  delay(1000);
}

void closeMed1(void)
{
  // rotates from 180 degrees to 0 degrees
  for (int pos = 105; pos >= 0; pos -= 1) {
    servoMed1.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}

void openMed2(void)
{
  digitalWrite(buzzerPin, LOW);
  // rotates from 0 degrees to 180 degrees
  for (int pos = 0; pos <= 105; pos += 1) {
    // in steps of 1 degree
    servoMed2.write(pos);
    delay(15); // waits 15ms to reach the position
  }
  delay(1000);
}

void closeMed2(void)
{
  // rotates from 180 degrees to 0 degrees
  for (int pos = 105; pos >= 0; pos -= 1) {
    servoMed2.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}

void openMed3(void)
{
  digitalWrite(buzzerPin, LOW);
  // rotates from 0 degrees to 180 degrees
  for (int pos = 0; pos <= 105; pos += 1) {
    // in steps of 1 degree
    servoMed3.write(pos);
    delay(15); // waits 15ms to reach the position
  }
  delay(1000);
}

void closeMed3(void)
{
  // rotates from 180 degrees to 0 degrees
  for (int pos = 105; pos >= 0; pos -= 1) {
    servoMed3.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}

/*
  Since TriggerPill1 is READ_WRITE variable, onTriggerPill1Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onTriggerPill1Change()  {
  // Add your code here to act upon TriggerPill1 change
}

/*
  Since TriggerPill2 is READ_WRITE variable, onTriggerPill2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onTriggerPill2Change()  {
  // Add your code here to act upon TriggerPill2 change
}

/*
  Since TriggerPill3 is READ_WRITE variable, onTriggerPill3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onTriggerPill3Change()  {
  // Add your code here to act upon TriggerPill3 change
}
