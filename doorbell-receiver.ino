#include <ESP8266WiFi.h>
#include "secrets.h"
#include <WiFiUdp.h>
#include "pitches.h"

WiFiUDP Udp;
char incomingPacket[29];

int noteCountFunkyTown = 23;
// notes in the melody:
int melodyFunkyTown[] = {
  NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_G4, 0, NOTE_G4, NOTE_C5, NOTE_F5, NOTE_E5, NOTE_C5,
  0,
  NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_G4, 0, NOTE_G4, NOTE_C5, NOTE_F5, NOTE_E5, NOTE_C5
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsFunkyTown[] = {
  4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4
};

int noteCountBadGuy = 48;
// notes in the melody:
int melodyBadGuy[] = {
  NOTE_G4, NOTE_D5, NOTE_G4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_AS4,
  NOTE_G4, NOTE_D5, NOTE_G4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_AS4,
  NOTE_G4, NOTE_D5, NOTE_G4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_AS4,
  NOTE_G4, NOTE_D5, NOTE_G4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_AS4,
  NOTE_C5, NOTE_G5, NOTE_C5, NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_DS5,
  NOTE_C5, NOTE_G5, NOTE_C5, NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_DS5,
  NOTE_D5, NOTE_A5, NOTE_D5, NOTE_FS5, NOTE_D5, NOTE_C5
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsBadGuy[] = {
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 4, 4, 4,
  4, 2, 4, 4, 2, 1
};

void setup() {
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  
  WiFi.setAutoReconnect(true);
  WiFi.begin(wifiSsid, wifiPassword);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Udp.begin(receiverPort);
}

unsigned long lastRing = 0;

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(incomingPacket, 28);
    incomingPacket[28] = 0;
    if (strcmp(incomingPacket, receiverSecret) == 0 && (lastRing == 0 || millis() - lastRing > debounce || millis() < lastRing)) {
      lastRing = millis();

      if (melodyIndex == 0) {
        digitalWrite(D1, HIGH);
        delay(1000);
        digitalWrite(D1, LOW);
      } else {
        int noteCount;
        int *melody;
        int *noteDurations;
        if (melodyIndex == 1) {
          noteCount = noteCountFunkyTown;
          melody = melodyFunkyTown;
          noteDurations = noteDurationsFunkyTown;
        } else if (melodyIndex == 2) {
          noteCount = noteCountBadGuy;
          melody = melodyBadGuy;
          noteDurations = noteDurationsBadGuy;
        }
        for (int thisNote = 0; thisNote < noteCount; thisNote++) {
          int noteDuration = 700 / noteDurations[thisNote];
          tone(D1, melody[thisNote], noteDuration);
          int pauseBetweenNotes = noteDuration * 1.30;
          delay(pauseBetweenNotes);
          noTone(D1);
        }
      }
    }
  }
}
