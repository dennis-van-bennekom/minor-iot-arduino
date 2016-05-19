#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

int soundValue = 0;

const char* ssid = "jeinternet";
const char* pass = "supergeheim";

const char* hostPost = "http://iot.dennisvanbennekom.com/data.php";
const char* hostGet = "http://iot.dennisvanbennekom.com/settings.json";

HTTPClient http;

void post(int value) {
  http.begin(hostPost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String payload = "alarm=" + String(value);

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.println("POST Success");
    String response = http.getString();
  } else {
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}

String get() {
  http.begin(hostGet);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(response);
    String alarm = root["alarm"];

    http.end();

    return alarm;
  } else {
    Serial.println(http.errorToString(httpCode).c_str());

    http.end();
    
    return String();
  }
}

void setup() {
  pinMode(A0, INPUT);
  pinMode(D8, OUTPUT);
  
  Serial.begin(9600);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting.");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("Connected!");
}

void loop() {
  soundValue = analogRead(A0);

  // Hier map ik de analoge waarde naar een 0 tot 10 getal 
  // zodat we die verder makkelijk kunnen gebruiken
  int sound = map(soundValue, 0, 512, 0, 10);
  Serial.println("Sound Level: " + String(sound));
  
  post(sound);

  int alarm = get().toInt();

  if (sound >= alarm) {
    digitalWrite(D8, HIGH);
  } else {
    digitalWrite(D8, LOW);
  }
  
  delay(5000);
}

