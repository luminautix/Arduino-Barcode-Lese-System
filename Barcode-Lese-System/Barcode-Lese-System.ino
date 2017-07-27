#include <SPI.h>        
#include <Ethernet2.h>
#include <EthernetUdp2.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xC0, 0x87}; // MAC des Ethernet2-Arduino-Controller
IPAddress ip(192, 9, 200, 42);            // Adresse des Ethernet2-Arduino-Controller

unsigned int localPort = 50456;             // Port zum auf Leitung hören
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //Puffer für eingehende Pakete,

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int triggerChannel = 4;

EthernetUDP Udp;          // Eine EthernetUDP Instanz zum Senden und Empfangen von Paketen über UDP

void setup() {
  Ethernet.begin(mac,ip); // startet Ethernet mit angegebener MAC und IP
  Udp.begin(localPort); // startet UDP auf angegebenen Port

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  pinMode(triggerChannel, OUTPUT);
  digitalWrite(triggerChannel, LOW);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    digitalWrite(triggerChannel, HIGH); // setze Barcodetrigger auf 5V
    // Serial.println(inputString);

 
    Udp.beginPacket("192.9.200.255", 51456);  // öffne einen Port Broadcast -> ...255
    for(int i = 0; i < inputString.length(); i++){
      if (isDigit(inputString[i])){
        Udp.write(inputString[i]);
      }
    }
    Udp.endPacket();

    
    // clear the string:
    inputString = "";
    stringComplete = false;
    digitalWrite(triggerChannel, LOW);
  }

  /* 
  //Antworte gezielt einer Anfrage eines Clienten
  int packetSize = Udp.parsePacket();         // gibt die Größe eines existierenden Paketes bekannt
  if (packetSize)                             // wenn packetSize > 0
  {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);       // lies das Packet in die Variable 'packetBuffer'
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());    // Beginne eine Antwort an die IP-Adresse und den Port, der das Paket geschickt hat
    sendDataWithUDP(poti);                                // Sende die Antwort
  }
  */
}

/* Funktionen Bereich --------------------------------------------------------------------------------------------- */
/*

  
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
