#include <ESP8266WiFi.h>

const char* ssid = "Wifi";
const char* password = "Senha do wifi";
const char* server = "ip da ui";

static const int pinoCLK = 14; //PINO DIGITAL (CLK)
static const int pinoDT = 12;  //PINO DIGITAL (DT)
static const int pinoSW = 13; //PINO DIGITAL (SW)

WiFiClient client;
int id = 0;
float value = 0;
int muted = 0;
static const float inc = 0.03;
int pinAstateCurrent;
int pinAstateLast = HIGH;

unsigned long previousButtonMillis=0;
unsigned long previousAliveMillis=0;
unsigned long currentMillis;

String SetdMute;
String SetdMix;

ICACHE_RAM_ATTR void changeEncoder(){
  pinAstateCurrent = digitalRead(pinoCLK);
  if((pinAstateLast == LOW) && (pinAstateCurrent == HIGH))
    setValue(digitalRead(pinoDT)==LOW);
  pinAstateLast = pinAstateCurrent;
}
ICACHE_RAM_ATTR void changeSwitch(){
  if(currentMillis-previousButtonMillis > 150) {
    setMuted(muted==0 ? 1 : 0);
    sendCmd(SetdMute+String(muted));
  }
  previousButtonMillis = currentMillis;
}
void setValue(bool inctype){
  value = inctype ? ((value+inc>=1) ? 1 : value+inc) : ((value<=inc) ? 0 : value-inc);
  sendCmd(SetdMix+String(value));
}
void sendCmd(String str){
  //Serial.println(">> " + str);
  client.println(str);
}
void setMuted(int value){
  muted=value;
  digitalWrite(LED_BUILTIN, muted==0 ? HIGH : LOW);
}
void checkValues(String msg){
  int index = msg.indexOf(SetdMute);
  if(index>0){
    index+=SetdMute.length();
    setMuted(msg.substring(index,index+1).toInt());
    //Serial.println("<< " + SetdMute+String(muted));
  }
  index = msg.indexOf(SetdMix);
  if(index>0){
    index+=SetdMix.length();
    int lastindex=msg.indexOf('\n',index);
    value=msg.substring(index,lastindex).toFloat();
    //Serial.println("<< " + SetdMix+String(value));
  }
}
void setup() {
  //Serial.begin (9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");  }
  //Serial.print("\nWiFi connected - IP address: ");
  //Serial.println(WiFi.localIP());

  pinMode(pinoCLK,INPUT);
  pinMode(pinoDT,INPUT);
  pinMode(pinoSW,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(pinoCLK), changeEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinoSW), changeSwitch, HIGH);
  currentMillis = millis();
  previousAliveMillis=currentMillis;
  SetdMix="SETD^i."+String(id)+".mix^";
  SetdMute="SETD^i."+String(id)+".mute^";
  
  if (client.connect(server, 80)) {
    //Serial.print("connected to ");
    //Serial.println(client.remoteIP());
    client.println("GET /raw HTTP1.1\n\nConnection: close\n\n");
  } else {
    //Serial.println("connection failed");
  }
} 

void loop() {
  currentMillis = millis();
  int len = client.available();
  if (len > 0) {
    if (len > 512) len = 512;
    byte buffer[len];
    client.read(buffer, len);
    checkValues(String((char*)buffer));
  }else{
    if(currentMillis-previousAliveMillis > 1000) {
      sendCmd("ALIVE");
      previousAliveMillis = currentMillis;
    }
  }
  /*
  pinAstateCurrent = digitalRead(pinoCLK);
  if((pinAstateLast == LOW) && (pinAstateCurrent == HIGH))
    setValue(digitalRead(pinoDT)==LOW);
  pinAstateLast = pinAstateCurrent;
  readButtonState();
  */
}