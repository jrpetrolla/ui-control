#include <ESP8266WiFi.h>

const char* ssid = "Wifi";
const char* password = "Senha do wifi";
const char* server = "ip da ui";

static const int pinoCLK = 14; //PINO DIGITAL (CLK)
static const int pinoDT = 12;  //PINO DIGITAL (DT)
static const int pinoSW = 13; //PINO DIGITAL (SW)

WiFiClient client;
int id = 1;
float value = 0;
int muted = 0;
static const float inc = 0.01;
int pinAstateCurrent;
int pinAstateLast = HIGH;

unsigned long previousButtonMillis=0;
unsigned long currentMillis;

String SetdMute;
String SetdMix;

void setup() {
  Serial.begin (9600);
  pinMode (pinoCLK,INPUT);
  pinMode (pinoDT,INPUT);
  pinMode (pinoSW,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());    
  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    client.println("GET /raw HTTP1.1\n\n");
    client.println("Connection: close");
    client.println();    
  } else {
    Serial.println("connection failed");
  }
  currentMillis = millis();
  SetdMix="SETD^i."+String(id)+".mix^";
  SetdMute="SETD^i."+String(id)+".mute^";
} 
void sendCmd(String str){
  Serial.println(str);
  client.println(str);
}
void setValue(bool inctype){
  value = inctype ? ((value+inc>=1) ? 1 : value+inc) : ((value<=inc) ? 0 : value-inc);
  sendCmd(SetdMix+String(value));
}

void readButtonState() {
  if(digitalRead(pinoSW) == LOW){
    if(currentMillis-previousButtonMillis > 500) {
      setMuted(muted==0 ? 1 : 0);
      sendCmd(SetdMute+String(muted));
    }
    previousButtonMillis = currentMillis;
  }
}
void setMuted(int value){
  muted=value;
  digitalWrite(LED_BUILTIN, muted==0 ? HIGH : LOW);
}

void loop() {
  int len = client.available();
  if (len > 0) {
    if (len > 512) len = 512;
    byte buffer[len];
    client.read(buffer, len);
    //Serial.write(buffer, len);
  }
  pinAstateCurrent = digitalRead(pinoCLK);
  if((pinAstateLast == LOW) && (pinAstateCurrent == HIGH))
    setValue(digitalRead(pinoDT)==LOW);
  pinAstateLast = pinAstateCurrent;

  currentMillis = millis();
  readButtonState();
}
