#include <Arduino.h>

#define ST_START 1
#define ST_MARK 2
#define ST_SPACE 3
#define ST_STOP 4

int STATE;

#ifdef F_CPU
  #define SYSCLOCK F_CPU     // main Arduino clock
#else
  #define SYSCLOCK 16000000  // main Arduino clock
#endif

volatile int count,bitCount,spaceTimeCount,timeLength[100],pushed;


void sendBit(int micro)
{
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(micro);
}

void sendSpace(int micro)
{
  TCCR2A &= ~(_BV(COM2B1));
  delayMicroseconds(micro);
}

void sendData(uint8_t cmd)
{
  noInterrupts(); // disable all interrupts

  TIMSK2 = 0;

  const uint8_t pwmval = 16000000 / 2000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = pwmval;
  OCR2B = pwmval / 3;
  
  interrupts(); // enable all interrupts

  uint8_t negCmd = ~cmd;

  uint32_t sent = cmd;
  sent <<= 8;
  sent += 0x20DF0000 +negCmd;

  //uint32_t sent = 0x20DF+(cmd<<8)+;
  Serial.println("Sending");
  Serial.println(sent,BIN);

  sendBit(9000);
  sendSpace(4500);

  int i;
  //Serial.println("Bits:");
  for(i=31;i>=0;i--)
  {
    sendBit(562);
    if(sent >= 0x80000000)
    {
      //Serial.print("1");
      sendSpace(1687);
    }
    else
    {
      //Serial.print("0");
      sendSpace(562);
    }

    sent <<=1;
  }

  sendBit(562);
  sendSpace(0);
}

void setup() {
  pinMode(9, OUTPUT);
  
  Serial.begin(9600);
}


void loop() {
  if (Serial.read() != -1) {
    sendData(0x88);
    delay(40);
  }
}
