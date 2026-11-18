// Four-LRA wrist vibrotactile prototype firmware.
// Motor A: channel 4 dorsal; B: channel 5 radial; C: channel 6 ulnar; D: channel 7 volar.
// USB serial: 115200 baud. Example commands: A,8,1000; SIM,AB,8,12,1000; SEQ,AB,8,12,1000.
// A DONE reply indicates completion of the programmed command, not measured mechanical output.

// I2C communication with the TCA9548A multiplexer and DRV2605L drivers.
#include <Wire.h>
#include <Adafruit_DRV2605.h>

// The same driver object is reused after selecting an individual multiplexer channel.
Adafruit_DRV2605 drv;
const uint8_t MUX_ADDRESS = 0x70;
const uint8_t channels[4] = {4,5,6,7};
const int MAX_RTP = 100;
const unsigned long MAX_DURATION = 2000;

// Select a bitmask of TCA9548A channels using its I2C address.
bool selectMask(uint8_t mask) {
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(mask);
  return Wire.endTransmission() == 0;
}
bool selectOne(int ch) { return selectMask((uint8_t)(1U << ch)); }

// Translate motor letters A-D into zero-based indices.
int motorIndex(char c) {
  if (c >= 'A' && c <= 'D') return c - 'A';
  return -1;
}

// Reject empty or nonnumeric serial command fields.
bool validNumber(const String &s) {
  if (s.length() == 0) return false;
  for (unsigned int i=0;i<s.length();i++) if (!isDigit(s[i])) return false;
  return true;
}

// Select one motor driver and set its digital real-time playback command.
bool setMotor(int index, uint8_t rtp) {
  if (index < 0 || index > 3 || !selectOne(channels[index])) return false;
  drv.setMode(DRV2605_MODE_REALTIME);
  drv.setRealtimeValue(rtp);
  return true;
}

// Stop all four motors individually, then deselect multiplexer channels.
void stopAll() {
  // Address each driver separately to avoid depending on shared-channel broadcast behavior.
  for (int i=0;i<4;i++) setMotor(i,0);
  selectMask(0);
}

// Apply one timed pulse to a selected motor.
bool pulseOne(int a, int rtp, unsigned long duration) {
  if (!setMotor(a,(uint8_t)rtp)) { stopAll(); return false; }
  delay(duration);
  stopAll();
  return true;
}

// Deliver a pair either in sequence or with near-concurrent software starts.
bool pulsePair(int a,int ra,int b,int rb,unsigned long duration,bool sequential) {
  if (a==b || a<0 || b<0) return false;
  if (sequential) {
    if (!pulseOne(a,ra,duration)) return false;
    return pulseOne(b,rb,duration);
  }
  // Start both without an intervening delay; starts are not hardware synchronized.
  if (!setMotor(a,(uint8_t)ra)) { stopAll(); return false; }
  if (!setMotor(b,(uint8_t)rb)) { stopAll(); return false; }
  delay(duration);
  stopAll();
  return true;
}

// Parse single, simultaneous, sequential, and STOP serial commands.
void processCommand(String line) {
  line.trim(); line.toUpperCase();
  if (line == "STOP") { stopAll(); Serial.println("DONE"); return; }
  // Legacy: A,20,1000 or AB,20,1000 (same RTP for both)
  // New: SIM,AB,8,12,1000 or SEQ,AB,8,12,1000
  String fields[5]; int n=0; int start=0;
  for (unsigned int i=0;i<=line.length();i++) {
    if (i==line.length() || line[i]==',') {
      if (n>=5) { Serial.println("ERROR:FORMAT"); return; }
      fields[n++] = line.substring(start,i); start=i+1;
    }
  }
  bool success=false;
  if (n==3) {
    String p=fields[0];
    if ((p.length()!=1 && p.length()!=2) || !validNumber(fields[1]) || !validNumber(fields[2])) { Serial.println("ERROR:FORMAT"); return; }
    int r=fields[1].toInt(); unsigned long d=fields[2].toInt();
    if (r<0 || r>MAX_RTP || d==0 || d>MAX_DURATION) { Serial.println("ERROR:RANGE"); return; }
    int a=motorIndex(p[0]);
    if (p.length()==1) success=pulseOne(a,r,d);
    else success=pulsePair(a,r,motorIndex(p[1]),r,d,false);
  } else if (n==5 && (fields[0]=="SIM" || fields[0]=="SEQ")) {
    String p=fields[1];
    if (p.length()!=2 || !validNumber(fields[2]) || !validNumber(fields[3]) || !validNumber(fields[4])) { Serial.println("ERROR:FORMAT"); return; }
    int ra=fields[2].toInt(), rb=fields[3].toInt(); unsigned long d=fields[4].toInt();
    if (ra<0 || ra>MAX_RTP || rb<0 || rb>MAX_RTP || d==0 || d>MAX_DURATION) { Serial.println("ERROR:RANGE"); return; }
    success=pulsePair(motorIndex(p[0]),ra,motorIndex(p[1]),rb,d,fields[0]=="SEQ");
  } else { Serial.println("ERROR:FORMAT"); return; }
  Serial.println(success ? "DONE" : "ERROR:COMMAND_FAILED");
}

// Initialize serial/I2C communication and configure each driver for LRA RTP mode.
void setup() {
  Serial.begin(115200); Serial.setTimeout(100); Wire.begin(); delay(500);
  bool ok=true;
  for (int i=0;i<4;i++) {
    if (!selectOne(channels[i]) || !drv.begin()) { ok=false; continue; }
    drv.useLRA(); drv.selectLibrary(6); drv.setMode(DRV2605_MODE_REALTIME); drv.setRealtimeValue(0);
  }
  stopAll(); Serial.println(ok ? "READY" : "ERROR:INITIALIZATION");
}

// Process one newline-terminated serial command whenever one is available.
void loop() {
  if (Serial.available()) processCommand(Serial.readStringUntil('\n'));
}
