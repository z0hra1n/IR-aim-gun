#include <Wire.h>
#include <BleCombo.h>

BleCombo bleCombo;

#define joyX 34
#define joyY 36

#define triggerPin 25
#define reloadPin 26
#define solenoidPin 27

int IRsensorAddress = 0xB0;
int slaveAddress = IRsensorAddress >> 1;

int threshold = 800;

byte data_buf[16];
int Ix[4];
int Iy[4];
int s;

bool solenoid = false;

void Write_2bytes(byte d1, byte d2){
Wire.beginTransmission(slaveAddress);
Wire.write(d1);
Wire.write(d2);
Wire.endTransmission();
}

void setup() {
Serial.begin(115200);
Wire.begin();

pinMode(triggerPin, INPUT_PULLUP);
pinMode(reloadPin, INPUT_PULLUP);
pinMode(solenoidPin, OUTPUT);

digitalWrite(solenoidPin, LOW);

bleCombo.begin();

Write_2bytes(0x30,0x01); delay(10);
Write_2bytes(0x30,0x08); delay(10);
Write_2bytes(0x06,0x90); delay(10);
Write_2bytes(0x08,0xC0); delay(10);
Write_2bytes(0x1A,0x40); delay(10);
Write_2bytes(0x33,0x33); delay(10);
delay(100);
}

void loop() {

solenoid = digitalRead(triggerPin) == HIGH;
digitalWrite(solenoidPin, solenoid ? HIGH : LOW);

if(bleCombo.isConnected()){
if(solenoid) bleCombo.press(MOUSE_LEFT);
else bleCombo.release(MOUSE_LEFT);
}

if(digitalRead(reloadPin) == HIGH && bleCombo.isConnected()){
bleCombo.press('r');
delay(100);
bleCombo.release('r');
}

int dx = analogRead(joyX);
int dy = analogRead(joyY);

int jx = dx - 2048;
int jy = dy - 2048;

if(abs(jx) < threshold) jx = 0;
if(abs(jy) < threshold) jy = 0;

if(bleCombo.isConnected()){

```
bleCombo.release('w');
bleCombo.release('a');
bleCombo.release('s');
bleCombo.release('d');

if(jy > threshold) bleCombo.press('w');
if(jy < -threshold) bleCombo.press('s');

if(jx > threshold) bleCombo.press('d');
if(jx < -threshold) bleCombo.press('a');
```

}

Wire.beginTransmission(slaveAddress);
Wire.write(0x36);
Wire.endTransmission();
Wire.requestFrom(slaveAddress, 16);

int i = 0;
while(Wire.available() && i < 16){
data_buf[i] = Wire.read();
i++;
}

Ix[0] = data_buf[1];
Iy[0] = data_buf[2];
s = data_buf[3];

Ix[0] += (s & 0x30) << 4;
Iy[0] += (s & 0xC0) << 2;

Serial.print("X=");
Serial.print(Ix[0]);
Serial.print(" Y=");
Serial.println(Iy[0]);

if(bleCombo.isConnected()){
int deltaX = map(Ix[0], 0, 1023, -5, 5);
int deltaY = map(Iy[0], 0, 767, -5, 5);
bleCombo.move(deltaX, deltaY);
}

delay(50);
}
