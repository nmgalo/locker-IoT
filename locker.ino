#include <Servo.h>
#include <Adafruit_Fingerprint.h>

Servo servo;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial serial(2, 3);
#else

#define serial Serial1
#endif

#define OPEN_LOCKER_POSITION 90

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serial);
int servoPosition = 0; 

void setup() {

  Serial.begin(9600);
  while (!Serial);
  servo.attach(9);
  
  delay(100);
  finger.begin(57600);

  servo.write(servoPosition);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("System error, Did not find fingerprint sensor");
    while (1) { delay(1); }
  }
  
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
}

void loop() {
  getFingerprintID();
  delay(50);
}

// void getFingerPrintId() {
//   uint8_t p = finger.getImage();
//   p = finger.image2Tz();
//   p = finger.fingerSearch();
  
//   switch (p) {
//     case FINGERPRINT_OK:
//         unlock(); 
//         delay(2000);
//         lock();
//         Serial.println("GR8");
//       break;
//     case FINGERPRINT_PACKETRECIEVEERR: Serial.println("PACKETRECIEVEERR"); return p;
//     case FINGERPRINT_NOTFOUND: Serial.println("NOTFOUND"); return p;
//     case FINGERPRINT_INVALIDIMAGE: Serial.println("INVALIDIMAGE"); return p;
//     case FINGERPRINT_FEATUREFAIL: Serial.println("FEATUREFAIL"); return p;
//     case FINGERPRINT_NOFINGER: Serial.println("NOFINGER"); return p;
//     case FINGERPRINT_IMAGEFAIL: Serial.println("IMAGEFAIL"); return p;
//     default:
//         unlock(); 
//         delay(2000);
//         lock();
//       return p;
//   }
// }

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    unlock();
    delay(2000);
    lock();
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}


void unlock() {
  for (servoPosition = 0; servoPosition <= OPEN_LOCKER_POSITION; servoPosition += 1) {
    servo.write(servoPosition);
    delay(15);
  }
}

void lock() {
  for (servoPosition = OPEN_LOCKER_POSITION; servoPosition >= 0; servoPosition -= 1) {
    servo.write(servoPosition);
    delay(15);
  }
}
