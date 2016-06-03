String tempBuffer = "";
String payload = "";
String launchCode = "BOOM";
bool foundSMS = false;

void setup() {
  SerialUSB.begin(9600);
  SerialCloud.begin(115200);
  delay(4000);
  SerialUSB.println("Remote detonator warming up....");

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
      }
    } else if (currChar == '\n') {
      payload = stripOffLengthNumber(payload);
      payload.trim();
      
      SerialUSB.print("\nSMS received: ");
      SerialUSB.print(payload);

      if (validLaunchCode(payload)) {
        launchFireworks();
      }

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

void launchFireworks() {
  SerialUSB.write("\n.........BOOM!!!!!!\n");
}

bool validLaunchCode(String payload) {
  return payload == launchCode;
}

String stripOffLengthNumber(String payload) {
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

