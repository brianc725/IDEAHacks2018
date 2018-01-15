#include <Servo.h>
#include "ardruino_secrets.h"
#include <SPI.h>
#include <WiFi101.h>

/*Code for Ultrasonic Position Sensor modified with permission from Bruce Allen 23/07/09
 *
 * Code for Wifi server  
 modified with permission from:
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
Servo myservo; //create servo object
 
int inputPin = 3;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int ledPin = 4;
const int pwPin = 6;          //reading in pulsewidth from max sonar on 6

int pos = 0; //servo position

long pulse, inches, cm;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);
 
void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(LED_BUILTIN, OUTPUT); //built in led
  pinMode(ledPin, OUTPUT); //"full" led
  pinMode(pwPin, INPUT);  //maxsonar 
  myservo.attach(9); //servo on digital 9

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();
  
}
 
void loop(){

        // listen for incoming clients
  WiFiClient client = server.available();
  String message = "";

   //Used to read in the pulse that is being sent by the MaxSonar device.
   //Pulse Width representation with a scale factor of 147 uS per Inch.

  pulse = pulseIn(pwPin, HIGH);
  
  //147uS per inch
  inches = pulse / 147;

  //change inches to centimetres
  cm = inches * 2.54;

  //end of parsing maxsonar
  
  //begin if else for motion
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      myservo.write(80);
      delay(580);
      myservo.write(90);
      pirState = HIGH;
    }
  //  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  } 
  else {
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      myservo.write(100);
      delay(900);
      myservo.write(90);
      pirState = LOW;
    }                     
  // digitalWrite(LED_BUILTIN, LOW);   
  } 
  //end of if else for motion
  

  //begin if else for full
   if (inches <= 6) {
    digitalWrite(ledPin, HIGH);
    message="I am full!";
   }
   else {
    digitalWrite(ledPin, LOW);
    message="I am not full.";
   } 

  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
       // Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println("\n<!DOCTYPE HTML>\n<html>\n<h1>"+message+"</h1>\n</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
 
}

void changeSite(String message, WiFiClient client) {
     if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
         // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println("\n<!DOCTYPE HTML>\n<html>\n<h1>"+message+"</h1>\n</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

}
