#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <DHT.h>

// Update these with values suitable for your network.
const char* ssid = "Movistar-184";
const char* password = "sergio405";
const char* mqtt_server = "192.168.1.37";
#define mqtt_port 1883
#define MQTT_USER "username"
#define MQTT_PASSWORD "password"
#define EEPROM_SIZE 4
#define TOUCH_PIN_1 13
#define TOUCH_PIN_2 13
#define TOUCH_PIN_3 13
#define TOUCH_PIN_4 13
#define DHT_PIN 27
int i = 0;
int ledPin1 = 33;
int ledPin2 = 32;
int ledPin3 = 35;
int ledPin4 = 34;
int pin1PWM = 26;
int pin2PWM = 32;
int pin3PWM = 35;
int pin4PWM = 34;
int ledState1 = LOW;
int ledState2 = LOW;
int ledState3 = LOW;
int ledState4 = LOW;
int touchSensor1 = 100;
int freq = 5000;        // para el fade
int ledChannel0 = 0;     // para el fade
int resolution = 8;     // para el fade
unsigned long debounceTime = 200;
unsigned long debounceWait = 0;
unsigned long debounceTime2 = 6000;
unsigned long debounceWait2 = 0;
char* topic1 = "/home/ESP32/light/";
char* topic2 = "/home/ESP32/dimmer1/";
char* topic3 = "/home/ESP32/sensor1/temperature/";
char* topic4 = "/home/ESP32/sensor1/humidity/";
char* topic5 = "/home/ESP32/sensor1/heatindex/";
DHT dht(DHT_PIN, DHT21);
float t = 25;
float h;
float hic;
//int dutyCycle = 0;


WiFiClient wifiClient;
/*id - GPIO
 * 1 - 4
 * 2 - 5
 * 3 - 16
 * 4 - 17
 */

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      //Once connected, publish an announcement...
      client.publish(topic1, "MQTT esp32 Connected - hello world");
      // ... and resubscribe
      client.subscribe(topic1);
      client.subscribe(topic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  //print recevied messages on the serial console
    Serial.println("-------new message from broker-----");
    Serial.print("Topic:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println("xx");

    String messageTemp;

    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      messageTemp += (char)payload[i];
      }
      
    if (strcmp(topic, topic1) == 0) {    
   
        if (messageTemp == "on1"){
             digitalWrite(ledPin1, HIGH);
             if (HIGH == !ledState1){ledState1 = !ledState1;}
             delay(50);
        }
        if (messageTemp == "on2"){
             digitalWrite(ledPin2, HIGH);
             if (HIGH == !ledState2){ledState2 = !ledState2;}
             delay(50);
        }
        if (messageTemp == "on3"){
             digitalWrite(ledPin3, HIGH);
             if (HIGH == !ledState3){ledState3 = !ledState3;}
             delay(50);
        }
        if (messageTemp == "on4"){
             digitalWrite(ledPin4, HIGH);
             if (HIGH == !ledState4){ledState4 = !ledState4;}
             delay(50);
        }
        if (messageTemp == "off1"){
             digitalWrite(ledPin1, LOW);
             if (LOW == !ledState1){ledState1 = !ledState1;}
             delay(50);
        }
        if (messageTemp == "off2"){
             digitalWrite(ledPin2, LOW);
             if (LOW == !ledState2){ledState2 = !ledState2;}
             delay(50);
        }
        if (messageTemp == "off3"){
             digitalWrite(ledPin3, LOW);
             if (LOW == !ledState3){ledState3 = !ledState3;}
             delay(50);
        }
        if (messageTemp == "off1"){
             digitalWrite(ledPin4, LOW);
             if (LOW == !ledState4){ledState4 = !ledState4;}
             delay(50);
        }
      
    }
    if (strcmp(topic, topic2) == 0) {
        String dutyCycle_str = "";
        for (i = 0; i < length; i++) {
          
          dutyCycle_str += (char)payload[i];
        }
       
        int dutyCycle = (int) dutyCycle_str.toInt();
        Serial.print("el dutyCycle_str es: ");
        Serial.println(dutyCycle_str);
        ledcWrite(ledChannel0, dutyCycle);
        Serial.print("el dutyCycle en int es: ");
        Serial.println(dutyCycle);
        delay(30);  
    }
    
}


void setup() {
  /*
  xTaskCreatePinnedToCore(
                    loop2,      // Function to implement the task 
                    "coreTask", // Name of the task 
                    1000,       // Stack size in words 
                    NULL,       // Task input parameter 
                    0,          // Priority of the task 
                    &task1,     // Task handle. 
                    0);         // Core where the task should run 
 */
  xTaskCreate(switchLamp1, "Switch Lamp", 1024, NULL, 1, NULL);
//  xTaskCreate(sensor1, "sensor1", 30000, NULL, 1, NULL);
//  xTaskCreate(switchLamp2, "Switch Lamp", 1000, NULL, 1, NULL);
//  xTaskCreate(switchLamp3, "Switch Lamp", 1000, NULL, 1, NULL);
//  xTaskCreate(switchLamp4, "Switch Lamp", 1000, NULL, 1, NULL); 
//  xTaskCreate(fadeLamp1, "Fade Lamp", 1000, NULL, 1, NULL);
  Serial.println("Task created...");
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  //pinMode(ledPin3, OUTPUT);
  //pinMode(ledPin4, OUTPUT);
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
  digitalWrite(ledPin3, ledState3);
  digitalWrite(ledPin4, ledState4);
  ledcSetup(ledChannel0, freq, resolution);
  ledcAttachPin(pin1PWM, ledChannel0); 
  dht.begin();
}

void switchLamp1(void * parameter) {
  pinMode(ledPin1, OUTPUT);
  Serial.println("iniciando for switchLamp1");
  for(;;){
     touchSensor1 = touchRead(TOUCH_PIN_1);
     if (touchSensor1 < 35){
       if ((millis() - debounceWait) > debounceTime) {
            vTaskDelay(50);
            if (touchRead(TOUCH_PIN_1) < 35) {
 
              digitalWrite(ledPin1, !ledState1);
              ledState1 = !ledState1;
              if (ledState1 == HIGH){
                client.publish(topic1, "on1");   
              }
              else{
                 client.publish(topic1, "off1");
              }
           
            }
         while (touchRead(TOUCH_PIN_1) < 35){  /* mientras queda presionado el boton: esperar */
           vTaskDelay(30);           
         }
         debounceWait = millis();
       }
       vTaskDelay(30);
     }
  }
}


/*
void sensor1(void * parameter) {
  dht.begin();
  Serial.println("iniciando for sensor1");
  for(;;){

    char bufferT[8];
    char bufferH[5];
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    float hic = dht.computeHeatIndex(t, h, false);
    String strBufferT = String(t, 2);
    String strBufferH = String(h, 2);
    strBufferT.toCharArray(bufferT, 8);
    strBufferH.toCharArray(bufferH, 5);    
     
    if (isnan(h) || isnan(t)){
      Serial.println("Fallo en la lectura del sensor DHT!");
      return;
    }
    
    else{
      
      client.publish(topic3, bufferT);
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.println(F("%"));  
      Serial.print(F("Temperature: "));
      Serial.print(t);
      Serial.println(F("°C "));
      Serial.print(F("Heat index: "));
      Serial.print(hic);
      Serial.println(F("°C "));
      Serial.println("----------------------");
      }
      vTaskDelay(5000);
    
  }
   
}
*/

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if ((millis() - debounceWait2) > debounceTime2) {

        char bufferT[8];
        char bufferH[5];
        char bufferHic[5];
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        float hic = dht.computeHeatIndex(t, h, false);
        String strBufferT = String(t, 1);
        String strBufferH = String(h, 0);
        String strBufferHic = String(hic, 1);
        strBufferT.toCharArray(bufferT, 8);
        strBufferH.toCharArray(bufferH, 5);
        strBufferHic.toCharArray(bufferHic, 5);    
         
        if (isnan(h) || isnan(t)){
          Serial.println("Fallo en la lectura del sensor DHT!");
        
        }
        
        else{
          
          client.publish(topic3, bufferT);
          client.publish(topic4, bufferH);
          client.publish(topic5, bufferHic);
          Serial.print(F("Humidity: "));
          Serial.print(h);
          Serial.println(F("%"));  
          Serial.print(F("Temperature: "));
          Serial.print(t);
          Serial.println(F("°C "));
          Serial.print(F("Heat index: "));
          Serial.print(hic);
          Serial.println(F("°C "));
          Serial.println("----------------------");
          
        }
        debounceWait2 = millis();
  }
      
} 

 
