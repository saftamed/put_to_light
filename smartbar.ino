#define ID1 1

#define ID2 ID1+4
#define ID3 ID2+4


#define L1 16
#define L22 14
#define L3 12

#define C1 2
#define C2 4
#define C3 5

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else
 #include <WiFi.h>
#endif
#include <ModbusTCP.h>

const int leds[] = {L1,L22,L3};
const int caps[] = {C1,C2,C3};
const int idReg = 0; 
const int ackReg = 1; 
IPAddress remote(192, 168, 0, 53); 
ModbusTCP mb; 

uint16_t res;

void setup() {
  Serial.begin(115200);
  pinMode(L1,OUTPUT);
  pinMode(C1,INPUT_PULLUP);

  pinMode(L22,OUTPUT);
  pinMode(C2,INPUT_PULLUP);

  pinMode(L3,OUTPUT);
  pinMode(C3,INPUT_PULLUP);

  checkSysteme();

  WiFi.begin("LIGHT", "1122334455");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  ledsOff();

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  digitalWrite(leds[1],1);

  mb.client();
  mb.connect(remote);  

  while (!mb.isConnected(remote)) {
    mb.connect(remote);
    Serial.print(".");
    delay(100);
  }

  digitalWrite(leds[1],0);
}

void loop() {
  if (mb.isConnected(remote)) {   
    uint16_t trans = mb.readHreg(remote, idReg, &res,3); 
    while(mb.isTransaction(trans)) {  
      mb.task();
      delay(10);
    }
    if(ID1 == res || ID2 == res || ID3 == res ){
      uint16_t* r = &res;
      uint16_t ack = *(r+1);
      if(ack == 1){
        switch(res){
          case ID1 :
           setAction(0);
          break;
          case ID2 :
           setAction(1);
          break;
          case ID3 :
           setAction(2);
          break;
        }  
        res = -1; 
      }
              
    }
  } else {
    Serial.println("not connect"); 
    mb.connect(remote);           
  }
  delay(1000);
}

bool checkIds(){
    if (mb.isConnected(remote)) {   
      uint16_t res2;
    uint16_t trans = mb.readHreg(remote, idReg, &res2,1); 
    while(mb.isTransaction(trans)) {  
      mb.task();
      delay(5);
    }
    Serial.println(res2 != res);
    return (res2 != res);
  
    }
}

void setAction(int pin){
    ledsOff();
     Serial.println(res);  
     digitalWrite(leds[pin],1);  
     Serial.println("led on"); 
       uint16_t trans = mb.writeHreg(remote, ackReg,2);
      while(mb.isTransaction(trans)) {  
        mb.task();
        delay(10); 
      }
      while( digitalRead(caps[pin])){
        delay(100);
        if(checkIds()){
          ledsOff();
          return;
        }
      }
       Serial.println("box out");
        trans = mb.writeHreg(remote, ackReg,3);
      while(mb.isTransaction(trans)) {  
        mb.task();
        delay(10); 
      }
      while( digitalRead(caps[pin])== 0){
        delay(100);
         if(checkIds()){
           ledsOff();
          return;
        }
      }
       Serial.println("box in");
      digitalWrite(leds[pin],0);
      trans = mb.writeHreg(remote, ackReg,(int) 4);
      Serial.println("led off");

}

void checkSysteme(){
  for(int i = 0;i<3;i++){
    digitalWrite(leds[i],1);
    delay(1000);
  }
}
void ledsOff(){
  for(int i = 0;i<3;i++){
    digitalWrite(leds[i],0);
  }
}




