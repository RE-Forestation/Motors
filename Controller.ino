#include <SPI.h>
#include <WiFi.h>

char ssid[] = "Ten Million";        // your network SSID (name)
char pass[] = "10000000";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
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
  printWifiStatus();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
    if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected()) {   // loop while the client's connected
      delayMicroseconds(10);       // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {    // if there's bytes to read from the client,
        char c = client.read();    // read a byte, then
        Serial.write(c);           // print it out the serial monitor
        if (c == '\n')           // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (!currentLine.length()) {
            client.println("HTTP/1.1 200 OK"); // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            client.println("Content-type:text/html"); // and a content-type so the client knows what's coming, then a blank line
            client.println();
            client.println("SERVER ON");
            client.println();// The HTTP response ends with another blank line
            break; // break out of the while loop
          } else currentLine = "";// if you got a newline, then clear currentLine
        else if (c != '\r') currentLine += c;      // if you got anything else but a carriage return character add it to the end of the currentLine

        //These are for the app
        if(currentLine.length() > 7 && currentLine.substring(0, currentLine.length() - 7).endsWith("GET /")){ //extract and compare
          if (currentLine.endsWith("prVerse")) { digitalWrite(LED_RED, LOW); digitalWrite(LED_BLUE, LOW); } 
          if (currentLine.endsWith("rrVerse")) { digitalWrite(LED_RED, HIGH); digitalWrite(LED_BLUE, HIGH); }
          if (currentLine.endsWith("prWards"))   digitalWrite(LED_BLUE, LOW);
          if (currentLine.endsWith("rrWards"))   digitalWrite(LED_BLUE, HIGH);
          if (currentLine.endsWith("plRever")) { digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, LOW); } 
          if (currentLine.endsWith("rlRever")) { digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, HIGH); }
          if (currentLine.endsWith("plForwa"))   digitalWrite(LED_GREEN, LOW);  
          if (currentLine.endsWith("rlForwa"))   digitalWrite(LED_GREEN, HIGH);
        }
        if(currentLine.length() > 16 && currentLine.substring(0, currentLine.length() - 16).endsWith("GET /")){
          setrighttwo()
        }
        
      }
    }
    client.stop();// close the connection:
    Serial.println("client disconnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
