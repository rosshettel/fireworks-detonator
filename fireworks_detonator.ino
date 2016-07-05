String tempBuffer = "";
String payload = "";
bool foundSMS = false;
bool ledOn = false;

#include <SPI.h>

#define SPI_SELECT()    (SPI.beginTransaction(L07))
#define SPI_DESELECT()  (SPI.endTransaction())
#define SPI_WRITE(data) SPI_SELECT(); SPI.transfer(data); SPI_DESELECT()
#define RELAYSHIELD_Close_ALL() SPI_WRITE(0b01010101)
#define RELAYSHIELD_Open_ALL()  SPI_WRITE(0b10101010)
#define RELAYSHIELD_Close_K1()  SPI_WRITE(0b00000001)
#define RELAYSHIELD_Open_K1()   SPI_WRITE(0b00000010)
#define RELAYSHIELD_Close_K2()  SPI_WRITE(0b00000100)
#define RELAYSHIELD_Open_K2()   SPI_WRITE(0b00001000)
#define RELAYSHIELD_Close_K3()  SPI_WRITE(0b00010000)
#define RELAYSHIELD_Open_K3()   SPI_WRITE(0b00100000)
#define RELAYSHIELD_Close_K4()  SPI_WRITE(0b01000000)
#define RELAYSHIELD_Open_K4()   SPI_WRITE(0b10000000)

void setup() {
  SerialUSB.begin(9600);
  Serial2.begin(9600);
  SerialCloud.begin(115200);
  SPI.begin();

  delay(4000);
  SerialUSB.println("Remote detonator warming up....");

  //close is relay on, open is relay off
  RELAYSHIELD_Open_ALL();

  Dash.begin();
  Dash.pulseLED(100,3000);
}

void loop() {
  char currChar;

  while(SerialCloud.available()) {
    currChar = (char)SerialCloud.read();

    //check if current buffer hits SMSRCVD code
    if (!foundSMS) {
      if (tempBuffer == "SMSRCVD") {
        foundSMS = true;
        ledOn = true;
      }
    } else if (currChar == '\n') {
      payload = stripOffLengthNumber(payload);
      payload.trim();

      Dash.pulseLED(50, 150);
      launchFireworks(payload);
      Dash.pulseLED(100,3000);

      foundSMS = false;
      payload = "";
    } else {
      payload.concat(currChar);
    }

    //only keep sliding buffer of length 7
    if (tempBuffer.length() >= 7) {
      tempBuffer.remove(0, 1);
    }

    tempBuffer.concat(currChar);

    SerialUSB.write(currChar);
  }
}

void launchFireworks(String payload) {
    if (payload == "boom" || payload == "boom" || payload == "1") {
      RELAYSHIELD_Close_K1();
    } else if (payload == "red" || payload == "Red" || payload == "2") {
      RELAYSHIELD_Close_K2();
    } else if (payload == "white" || payload == "White" || payload == "3") {
      RELAYSHIELD_Close_K3();
    } else if (payload == "blue" || payload == "Blue" || payload == "4") {
      RELAYSHIELD_Close_K4();
    } else {
      SerialUSB.println("Unrecognized trigger word, ignoring...");
      return;
    }

    
    SerialUSB.println(".........BOOM!!!!!!");
    delay(1000);
    RELAYSHIELD_Open_ALL();
}

String stripOffLengthNumber(String payload) {
    SerialUSB.println(payload);
  int index = 0;
  while(payload[index] == ',') {
    ++index;
  }
  while(payload[index] != ',') {
    ++index;
  }
  ++index;

  payload.remove(0, index);
  return payload;
}
