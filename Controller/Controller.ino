#include <Adafruit_PWMServoDriver.h>

#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver(0x41);
char ssid[] = "legalslavery"; // your network SSID (name)
char pass[] = "atleast8char"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
bool e = true;
WiFiServer server(80);

void setleftone(int value)
{
  pca.setPin(0, value);
  pca.setPin(4, value);
  pca.setPin(6, value);
  Serial.println(value);
}

void setlefttwo(int value)
{
  pca.setPin(1, value);
  pca.setPin(5, value);
  pca.setPin(7, value);
  Serial.println(value);
}

void setrighttwo(int value)
{
  pca.setPin(9, value);
  pca.setPin(11, value);
  pca.setPin(3, value);
  Serial.println(value);
}

void setrightone(int value)
{
  Serial.println();
  pca.setPin(8, value);
  pca.setPin(10, value);
  pca.setPin(2, value);
  Serial.println(value);
}

void backward(int value)
{
  pca.setPin(1, value);
  pca.setPin(3, value);
  pca.setPin(5, value);
  pca.setPin(7, value);
  pca.setPin(9, value);
  pca.setPin(11, value);
  Serial.println(value);
}

void forward(int value)
{
  pca.setPin(0, value);
  pca.setPin(2, value);
  pca.setPin(4, value);
  pca.setPin(6, value);
  pca.setPin(8, value);
  pca.setPin(10, value);
  Serial.println(value);
}

void left(int value)
{
  pca.setPin(1, value);
  pca.setPin(2, value);
  pca.setPin(5, value);
  pca.setPin(7, value);
  pca.setPin(8, value);
  pca.setPin(10, value);
  Serial.println(value);
}

void right(int value)
{
  pca.setPin(0, value);
  pca.setPin(3, value);
  pca.setPin(4, value);
  pca.setPin(6, value);
  pca.setPin(9, value);
  pca.setPin(11, value);
  Serial.println(value);
}

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
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

  pca.begin();
  pca.setPWMFreq(500); // GIGA default
  left(0);
  right(0);
}

void loop()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  digitalWrite(LED_BLUE, LOW);
  if (client)
  { // if you get a client,
    digitalWrite(LED_BLUE, HIGH);
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected())
    {                        // loop while the client's connected
      delayMicroseconds(10); // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        if (c == '\n')          // if the byte is a newline character
          if (!currentLine.length())
          {
            client.println("HTTP/1.1 200 OK");        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            client.println("Content-type:text/html"); // and a content-type so the client knows what's coming, then a blank line
            client.println();
            client.println("SERVER ON");
            client.println(); // The HTTP response ends with another blank line
            break;            // break out of the while loop
          }
          else
            currentLine = ""; // if you got a newline, then clear currentLine
        else if (c != '\r')
          currentLine += c; // if you got anything else but a carriage return character add it to the end of the currentLine

        // These are for the app
        if (currentLine.endsWith("GET /Z"))
        {
          (e = !e) ? servo.setPin(0, 120) : servo.setPin(0, 160);
          digitalWrite(LED_GREEN, LOW);
        }
        else if (currentLine.length() > 5 && (currentLine.substring(0, currentLine.length() - 5)).endsWith("GET /A"))
        { // keyboard bindings
          digitalWrite(LED_RED, LOW);
          int value = (currentLine.substring((currentLine.length() - 5), (currentLine.length() - 1)).toInt());
          switch ((currentLine[currentLine.length() - 1]))
          {
          case 'f':
            forward(value);
            break;
          case 'b':
            backward(value);
            break;
          case 'l':
            left(value);
            break;
          case 'r':
            right(value);
            break;
          }
        }
        else if (currentLine.length() > 16 && currentLine.substring(0, currentLine.length() - 16).endsWith("GET /"))
        { // screen bindings
          setrighttwo((currentLine.substring((currentLine.length() - 4), currentLine.length()).toInt()));
          setrightone((currentLine.substring((currentLine.length() - 8), (currentLine.length() - 4)).toInt()));
          setlefttwo((currentLine.substring((currentLine.length() - 12), (currentLine.length() - 8)).toInt()));
          setleftone((currentLine.substring((currentLine.length() - 16), (currentLine.length() - 12)).toInt()));
        }
      }
    }
    client.stop(); // close the connection:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
