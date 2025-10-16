#include <ArduinoRS485.h>

enum Events {
  OPEN_GATE,
  CLOSE_GATE,
  STOP,
  VENTING,
  HALF_OPEN,
  TOGGLE_LIGHT,
  NONE
}

enum Response_Status {
  OPENING,
  CLOSING,
  STOPPED,
  OPEN,
  CLOSED,
  HALF_OPENED,
  VENTING,
  NONE
}

#define MAX_CALLSTACK_SIZE 10
Events callstack[MAX_CALLSTACK_SIZE];
int callstack_count = 0;

Events check_callstack() {
  if (callstack_count == 0) {
    return Events.NONE;
  }

  Events event = callstack[0];

  for (int i = 1; i < callstack_count; i++) {
    callstack[i-1] = callstack[i];
  }

  callstack_count--;

  return event;
}

#define RESPONSE_SIZE 21
byte response_message[RESPONSE_SIZE];

response_message handleEvent(Events event) {
  switch (event) {
    case OPEN_GATE:
      Serial.println("OPEN_GATE triggered");
      break;
    case CLOSE_GATE:
      Serial.println("CLOSE_GATE triggered");
      break;
    case STOP:
      Serial.println("STOP triggered");
      break;
    case VENTING:
      Serial.println("STOP triggered");
      break;
    case HALF_OPEN:
      Serial.println("HALF_OPEN triggered");
      break;
    case TOGGLE_LIGHT:
      Serial.println("TOGGLE_LIGHT triggered");
      break;
    default:
      break;
  }
}

// Calculate CRC-16 (Modbus)
uint16_t calculateCRC(uint8_t *buffer, int length) {
  int i, j;
  uint16_t crc = 0xFFFF;
  uint16_t tmp;

  for (i = 0; i < length; i++) {
    crc = crc ^ buffer[i];
    for (j = 0; j < 8; j++) {
      tmp = crc & 0x0001;
      crc = crc >> 1;
      if (tmp) {
        crc = crc ^ 0xA001;
      }
    }
  }
  return crc;
}

#define FIRST_BLOCK_SIZE 27
byte firstBlock[FIRST_BLOCK_SIZE];

#define SECOND_BLOCK_SIZE 17
byte secondBlock[SECOND_BLOCK_SIZE]

void setup() {
  int counter = 0;

  Serial.begin(57600);
  RS485.setPins(int txPin, int dePin, int rePin);
  RS485.begin(57600);
  RS485.receive();
}

void loop() {

  int index = 0;

  while (RS485.available() > 0 && index < FIRST_BLOCK_SIZE + SECOND_BLOCK_SIZE) {
    byte runtimeData = RS485.read();
    if (index < FIRST_BLOCK_SIZE) {
      firstBlock[index] = runtimeData;
      Serial.write("FirstBlock: " + runtimeData);
    } else {
      secondBlock[index - FIRST_BLOCK_SIZE] = runtimeData;
      Serial.write("SecondBlock: " + runtimeData);
    }
    index++;
  }

  if (index == FIRST_BLOCK_SIZE + SECOND_BLOCK_SIZE) {
    index = 0;
  }

  Events event = check_callstack();
  if (event != Events.NONE) {
    handleEvent();
  }

  RS485.beginTransmission();
  RS485.print("hello ");
  RS485.println(counter);
  RS485.endTransmission();
  counter++
  delay(1000);
}
