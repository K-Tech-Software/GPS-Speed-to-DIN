#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Settings:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const int RXPin = 6, TXPin = 5; //RX and TX Pin Setup for Software Serial
static const uint32_t GPSBaud = 9600; //Baudrate GPS Reciever
static const int LED_Pin = 7; //LED for checking GPS Signal, while blinking GPS is not available
const int Speed_Pin = 8; // pin of Mosfet for sending impuls signals
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);// The serial connection to the GPS device
float gps_speed = 1;
unsigned long currentMillis;
int pulses_per_s = 0;
bool GPS_OK = false;

void setup() {
  pinMode(LED_Pin, OUTPUT);
  pinMode(Speed_Pin, OUTPUT);
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0)  //  This sketch displays information every time a new sentence is correctly encoded.
    if (gps.encode(ss.read())) {
      gps_speed = gps.speed.kmph();
      currentMillis = millis();
      Sendpulses();
      displayInfo();
    }
  if (millis() > 5000 && gps.charsProcessed() < 10) Serial.println(F("No GPS detected: check wiring."));
  if (GPS_OK == false)   GPS_Led();
  if (gps.speed.age() > 1000) {
    GPS_OK = false;
  }
  else  {
    GPS_OK = true;
    digitalWrite(LED_Pin, HIGH);
  }
}

void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else {
    Serial.print(F("Pos INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else {
    Serial.print(F(" Date INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
    Serial.print(" ");
    Serial.print("  Pulses/s : ");
    Serial.print(pulses_per_s);
    Serial.print(" ");
    Serial.print("Location Valid? ");
    Serial.print(gps.location.isValid());
    Serial.print(" ");
    Serial.print("GPS Satellites ");
    Serial.print(gps.satellites.value());
    Serial.print(" ");
    Serial.print("GPS Speed Age ");
    Serial.print(gps.speed.age());
    Serial.print(" ");
    Serial.print("GPS Speed Valid? ");
    Serial.print(gps.speed.isValid());
    Serial.print(" ");
    Serial.print("GPS Speed OK? ");
    Serial.print(GPS_OK);
    Serial.print(" ");
    Serial.print("GPS Speed km/h ");
    Serial.print(gps.speed.kmph());
  }
  else {
    Serial.print(F(" Time INVALID"));
  }
  Serial.println();
}

void Sendpulses() {
  const int pulse_per_meter = 130; // Pulses per meter by tractor signal
  if (gps_speed > 2) {
    pulses_per_s = (gps.speed.mps() * pulse_per_meter);
    tone(Speed_Pin, pulses_per_s);
  }
  else {
    pulses_per_s = 0;
    noTone(Speed_Pin);
    digitalWrite(Speed_Pin, LOW);
  }
}

void blinkLED(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}

void GPS_Led() {

  const int delaytime = 150; // Intervall for blinking of LED
  static unsigned long previousMillis = 0;        // will store last time LED was updated
  if (currentMillis - previousMillis >= delaytime) {
    previousMillis = currentMillis; // save the last time you blinked the LED
    blinkLED(LED_Pin);
  }
}
