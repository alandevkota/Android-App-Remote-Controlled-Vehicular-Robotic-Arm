

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define UP 'g'
#define DOWN 'b'
#define LEFT 'l'
#define RIGHT 'r'
#define STOP 's'

#define fistOpen 'o'
#define fistClose 'c'
#define wristLeft '0'
#define wristRight '1'
#define armLeft '2'
#define armRight '3'
#define elbowLeft '4'
#define elbowRight '5'
#define baseOn '6'
#define baseOff '7'

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define fist_motor 2
#define wrist_motor 3
#define arm_motor 4
#define elbow_motor 5

#define FORWARD 1
#define BACKWARD -1

#define rRotate 1
#define lRotate -1

struct MOTOR_PINS {
  int pinIN1;
  int pinIN2;
};

std::vector<MOTOR_PINS> motorPins = {
  { 16, 17 },  //FRONT_RIGHT_MOTOR  //RIGHT_MOTOR
  //{ 18, 19 },  //BACK_RIGHT_MOTOR
  { 13, 12 },  //FRONT_LEFT_MOTOR     //LEFT_MOTOR
  //{25, 33},  //BACK_LEFT_MOTOR

  { 18, 19 },  //fist/grip
  { 14, 27 },  //wrist
  { 26, 25 },  //Arm
  { 21, 22 },  //elbow
  { 33, 32 }   //Base (start and reset)
};

const char* ssid = "Finity";
const char* password = "Zpqmbty1@";

WiFiServer server(80);



void rotateMotor(int motorNumber, int motorDirection) {
  if (motorDirection == FORWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  } else if (motorDirection == BACKWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
  } else {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
}

// void processAuto(char c_) {
//   char temp[50];
//   if (client.available()) {
//     char c_ = client.read();  // read a byte, then
//     Serial.write(c_);
//     processCarMovement(c_);  // print it out the serial monitor
//     for (int i = 0; i < 16; i++) {
//       processCarMovement(c_);
//       temp[i] = c_;
//       delay(2000);
//       processCarMovement(STOP);
//     }
//   }
// }

void processAuto(char c_) {
  char temp[16] = { 'g', 'c', 'c', 'c', 'b', 'r', 'o', 'o', 'o', 'l' };
  for (int i = 0; i < 16; i++) {
    processCarMovement(temp[i]);
    delay(666.67);
    processCarMovement(STOP);
  }
}

void processCarMovement(char inputValue) {
  //Serial.println(inputValue);
  switch (inputValue) {

    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      Serial.println("UP");
      break;

    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      Serial.println("DOWN");
      break;

    case LEFT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      Serial.println("LEFT");
      break;

    case RIGHT:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      Serial.println("RIGHT");
      break;

    case STOP:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      rotateMotor(fist_motor, STOP);
      rotateMotor(wrist_motor, STOP);
      rotateMotor(arm_motor, STOP);
      rotateMotor(elbow_motor, STOP);
      Serial.println("STOP");
      break;

    case fistOpen:
      rotateMotor(fist_motor, FORWARD);
      Serial.println("fistOpen");
      break;

    case fistClose:
      rotateMotor(fist_motor, BACKWARD);
      Serial.println("fistClose");
      break;

    case wristLeft:
      rotateMotor(wrist_motor, FORWARD);
      Serial.println("wristLeft");
      break;

    case wristRight:
      rotateMotor(wrist_motor, BACKWARD);
      Serial.println("wristRight");
      break;

    case armLeft:
      rotateMotor(arm_motor, FORWARD);
      Serial.println("armLeft");
      break;

    case armRight:
      rotateMotor(arm_motor, BACKWARD);
      Serial.println("armRight");
      break;

    case elbowLeft:
      rotateMotor(elbow_motor, FORWARD);
      Serial.println("elbowLeft");
      break;

    case elbowRight:
      rotateMotor(elbow_motor, BACKWARD);
      Serial.println("elbowRight");
      break;

    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      rotateMotor(fist_motor, STOP);
      rotateMotor(wrist_motor, STOP);
      rotateMotor(arm_motor, STOP);
      rotateMotor(elbow_motor, STOP);
      break;
  }
}



void setUpPinModes() {
  for (int i = 0; i < motorPins.size(); i++) {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    rotateMotor(i, STOP);
  }
}


void setup(void) {
  setUpPinModes();
  Serial.begin(115200);
  pinMode(2, OUTPUT);  // set the LED pin mode

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  Serial.println("HTTP server started");

  Serial.println("****************************************************");
  Serial.println("Robot Commands");
  Serial.println("Menu...");
  Serial.println("Enter: 0-7 for robot arm, o/c for fist open and close, g/s/l/r/b for wheels ");
  Serial.println("****************************************************");
}

void loop() {
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c_ = client.read();    // read a byte, then
        Serial.write(c_);           // print it out the serial monitor
        if (c_ == '\n') {           // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c_ != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c_;      // add it to the end of the currentLine
        }

        Serial.println();

        Serial.println("Pressed corresponding character in app: ");
        Serial.println(c_);

        if (c_ == '6') {

          // for (int i = 0; i < 3; i++) {

          //   processAuto(c_);
          // }
          processAuto(c_);
        } else if (c_ == '7') {
          processCarMovement(STOP);
        } else {
          processCarMovement(c_);
          //delay(200);
          //processCarMovement(STOP);
        }
      }
    }
  }
}