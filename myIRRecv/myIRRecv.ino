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

volatile int count,bitCount,spaceTimeCount,timeLength[100];

int stateCountLeg[4];
typedef struct x{
  char *symbol;
  int val;
} mapping;
#define NR_DATE 20
mapping date[NR_DATE];

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

  TIMSK1 &= ~(1 << OCIE1A);//disable time 1
  
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

  TIMSK1 |= (1 << OCIE1A); //enable timer 1
}

ISR(TIMER1_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
    bool m = (PINE & (0x1 << 4))==0;
    
    switch(STATE)
    {
      case ST_START:
        if(m)
        {
          bitCount=0;
          STATE = ST_MARK;
        }
      break;
  
      case ST_MARK:
        if(!m)
        {
          STATE = ST_SPACE;
          spaceTimeCount=0;
        }
      break;
  
      case ST_SPACE:
        spaceTimeCount++;
        if(spaceTimeCount>45)
        {
          STATE = ST_STOP;
          break;
        }
        if(m)
        {
          timeLength[bitCount++] = spaceTimeCount;
          STATE = ST_MARK;
        }
      break;
  
      case ST_STOP:
        STATE = ST_START;

        int i;
        uint8_t final_output = 0;

        uint8_t addr,negAddr,cmd,negCmd;

        /*Serial.println("Received");
        for(i=0;i<bitCount;i++)
        {
          Serial.println("Length: "+String(timeLength[i]));
        }*/

        if(bitCount<5)
        {
          if(timeLength[0]>20&&timeLength[0]<24)
            Serial.println("Repeat");
          break;
        }
    
        for(i=1;i<bitCount;i++)
        {
           final_output <<= 1;
           if(timeLength[i] > 10)
            final_output ^= 1; 

          switch(i)
          {
            case 8:
              addr = final_output;
            break;
    
            case 16:
              negAddr = final_output;
            break;

            case 24:
              cmd = final_output;
            break;

            case 32:
              negCmd = final_output;
            break;
          }

          if(i%8==0)
          {
            //Serial.println(final_output,BIN);
            final_output = 0;
          }
        }
        int j;
        if((addr&negAddr)==0 && (cmd&negCmd)==0)
        {
          //Serial.println("Data ok");
          for(j = 0; j < NR_DATE; j++){
            if(cmd == date[j].val){
              Serial.println(date[j].symbol);
              break;
            }
          }
        }
        else
        {
          if((addr&negAddr)==0)
            Serial.println("Data almost so ok");
          else
            Serial.println("Data not so ok");
        } 
      break;
    }
}
void setup() {
  pinMode(3, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),InterruptArin,RISING);
 
  date[0].symbol = "1";
  date[0].val = 0x88;
  date[1].symbol = "2";
  date[1].val = 0x48;
  date[2].symbol = "3";
  date[2].val = 0xC8;
  date[3].symbol = "4";
  date[3].val = 0x28;
  date[4].symbol = "5";
  date[4].val = 0xA8;
  date[5].symbol = "6";
  date[5].val = 0x68;
  date[6].symbol = "7";
  date[6].val = 0xE8;
  date[7].symbol = "8";
  date[7].val = 0x18;
  date[8].symbol = "9";
  date[8].val = 0x98;
  date[9].symbol = "0"; 
  date[9].val = 0x8; 
  date[10].symbol = "UP"; 
  date[10].val = 0x2; 
  date[11].symbol = "OK"; 
  date[11].val = 0x22; 
  date[12].symbol = "DOWN"; 
  date[12].val = 0x82; 
  date[13].symbol = "POWER"; 
  date[13].val = 0x10; 
  date[14].symbol = "VOL_UP"; 
  date[14].val = 0x40; 
  date[15].symbol = "VOL_DOWN";
  date[15].val = 0xC0; 
  date[16].symbol = "PR_UP"; 
  date[16].val = 0x0; 
  date[17].symbol = "PR_DOWN"; 
  date[17].val = 0x80;
  date[18].symbol = "LEFT"; 
  date[18].val = 0xE0; 
  date[19].symbol = "RIGHT"; 
  date[19].val = 0x60;
  
  Serial.begin(9600);
  // put your setup code here, to run once:
  // initialize Timer1
  count=0;
  STATE = ST_START;
  pinMode(2,INPUT);
  
  noInterrupts(); // disable all interrupts

  //timer 1 is used for RF reading
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  OCR1A = 200; // compare match register 16MHz/256/2Hz
  TCCR1B = _BV(WGM12) | _BV(CS11);
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

  count=0;

  //timer 2 is used for RF writing

    TIMSK2 = 0;

  const uint8_t pwmval = 16000000 / 2000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = pwmval;
  OCR2B = pwmval / 3;

  sendSpace(0);
  
  interrupts(); // enable all interrupts
}

void InterruptArin(){
  static unsigned long last_irq = 0;
   unsigned long interrupt_time = millis();
   
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_irq > 200) 
   {
     Serial.println("Blup in data mea!");
     sendData(0x88);
   }
   last_irq = interrupt_time;
}
void loop() {
  //sendData();
  delay(1000);
}
