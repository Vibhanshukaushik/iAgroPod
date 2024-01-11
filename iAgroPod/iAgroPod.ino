#include<SoftwareSerial.h>
#include<Wire.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
//#include<LiquidCrystal_I2C.h>
//#include<dht.h>

#define FIREBASE_HOST "iagropod-default-rtdb.firebaseio.com"     //example.firebaseio.com
#define FIREBASE_AUTH "owX97k8GmUa4blhLzrnGfqPvNY46fWOUOP5kveu4"  //token_or_secret
#define WIFI_SSID "OnePlus 9R"                 //SSID
#define WIFI_PASSWORD "1246850pnp"         //PASSWORD
// defining NPK pin through TTL communication
#define RE 8
#define DE 9
// defining EC through TTL communication
#define RE1 13
#define DE1 12


// defining DHT pin
//dht DHT;
//#define DHT11 7

//initalize the NPK
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11]; 

//initalize the EC
const byte ec[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE};
const byte salinity[] = {0x01, 0x03, 0x00, 0x14, 0x00, 0x01, 0xC4, 0x0E};

byte values1[8];

SoftwareSerial npk(11,10);
SoftwareSerial ecs(15,14);

int air,water;


void setup(){
  Serial.begin(9600);
   // checking weather it is connnected to firebase or not!
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
   Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(16,OUTPUT);  
  
   npk.begin(9600);
   ecs.begin(9600);
  
   pinMode(RE, OUTPUT);
   pinMode(DE, OUTPUT);
   delay(2000);
   pinMode(RE1, OUTPUT);
   pinMode(DE1, OUTPUT);
   delay(2000);
   
}
void loop(){
  air=Firebase.getInt("Air Pump");
  if(air==1){
      Serial.println("AIr_SW ON");
    }else{
        Serial.println("Air_sw OFF");
      }
  water=Firebase.getInt("Water Pump");
  if(air==1){
      Serial.println("Water_SW ON");
    }else{
        Serial.println("Water_sw OFF");
      }
  

  digitalWrite(16,LOW);
  
  // reading NPK values
  byte val_N, val_P, val_K;
  val_N = nitrogen();
  delay(250);
  val_P = phosphorous();
  delay(250);
  val_K = potassium();
  delay(250);
  // reading EC values
  int soil_ec= ec_data();
  delay(250);
  int soil_salinity= salinity_data();
  delay(250);

  // printing and uploading the values of NPK and EC
  
  Firebase.setFloat("/user/Nitrogen", val_N);
   if (Firebase.failed()) {
      Serial.print("setting /val_N failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);

  Firebase.setFloat("/user/Phosphorous", val_P);
   if (Firebase.failed()) {
      Serial.print("setting /val_P failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);
  
  Firebase.setFloat("/user/Potassium", val_K);
   if (Firebase.failed()) {
      Serial.print("setting /val_K failed:");
      Serial.println(Firebase.error());  
      return;
  }

  Firebase.setFloat("/user/Electrical Conductivity", soil_ec);
   if (Firebase.failed()) {
      Serial.print("setting /soil_ec failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);

  Firebase.setFloat("/user/Salinity", soil_salinity);
   if (Firebase.failed()) {
      Serial.print("setting /soil_salinity failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);


  
  
  /*float a= DHT.read11(DHT11);
  float temperature= DHT.temperature;
  float humidity= DHT.humidity;
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\n");
  Firebase.setFloat("Temperature", temperature);
  if (Firebase.failed()) {
      Serial.print("setting /temperature failed:");
      Serial.println(Firebase.error());  
      return;
  }*/
  /*Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("\n");
  Firebase.setFloat("Humidity", humidity);
  if (Firebase.failed()) {
      Serial.print("setting /humidity failed:");
      Serial.println(Firebase.error());  
      return;
  }*/
  delay(2000);
}

// retriving the NPK values
byte nitrogen(){
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(npk.write(nitro,sizeof(nitro))==8){
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for( byte i=1; i<7; i++){
      values[i]= npk.read();
    }
     
    }
    return values[4];
 }
 
 byte phosphorous(){
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(npk.write(phos,sizeof(phos))==8){
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for( byte i=1; i<7; i++){
      values[i]= npk.read();
    }
     
    }
    return values[4];
 }
 byte potassium(){
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(npk.write(pota,sizeof(pota))==8){
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for( byte i=1; i<7; i++){
      values[i]= npk.read();

    }
     
    }
    return values[4];
 }

 byte ec_data(){
  digitalWrite(DE1, HIGH);
  digitalWrite(RE1, HIGH);
  delay(10);

  if( ecs.write(ec, sizeof(ec)) == 8)
  {
    digitalWrite(DE1,LOW);
    digitalWrite(RE1,LOW);

    for( byte i=0; i<7; i++){
      values1[i] = ecs.read();
      
    }
    
  }
  return (int(values1[3] << 8| values1[4]));
}

byte salinity_data(){
  digitalWrite(DE1, HIGH);
  digitalWrite(RE1, HIGH);
  delay(10);

  if( ecs.write(salinity, sizeof(salinity)) == 8)
  {
    digitalWrite(DE1,LOW);
    digitalWrite(RE1,LOW);

    for( byte i=0; i<7; i++){
      values1[i] = ecs.read();
      
    }
    
  }
  return(int(values1[3] << 8| values1[4]));
}
