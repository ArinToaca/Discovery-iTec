void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial3.available())
  {
    Serial.println("BT recived");
    delay(50);
    char * s=malloc(sizeof(char)*20);
    uint8_t t = 0;
    while(Serial3.available())
    {
      s[t++]=(char)Serial3.read();
      if(t==19)
        break;
    }
    s[t] = '\0';
    Serial.println("Code read: "+String(s));
  }
}
