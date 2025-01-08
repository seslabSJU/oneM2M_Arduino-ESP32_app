#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "pitches.h" // 음계 모음 헤더 파일 필요

#define PIR_PIN 15 //PIR 센서 GPIO핀 번호
#define LED_PIN 2  //LED GPIO핀 번호
#define BUZ_PIN 4  //수동형 피에조 부저 GPIO핀 번호 

#define ARDUINOJSON_SLOT_ID_SIZE 1
#define ARDUINOJSON_STRING_LENGTH_SIZE 1
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0

WiFiClient wifi; //WiFiclient 선언 

const char* ssid = "Your_SSID";         // WiFi SSID
const char* password = "your_PassWord"; // WiFi 비밀번호

const char* server = "Server Hosting PC(pr laptop) IPv4 Address"; // server가 돌아가고 있는 PC의 로컬 IP 주소
const int port = 3000;            // tinyIoT 서버 포트

bool pirState = false;  // PIR 센서 상태 (HIGH/LOW)
bool isToneOn = false;
unsigned long motionStartTime = 0;       // PIR이 처음 HIGH가 된 시간
unsigned long thresholdDuration = 5000;  // 5초 (필요시 변경)

int melody[] = {//esp32 전원 인가시 발생하는 초기화 음의 음계 
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {//esp32 전원 인가시 발생하는 초기화 음 duration
  4, 8, 8, 4, 4, 4, 4, 4
};

void printWifiStatus() { //wifi status 출력을 위한 함수 
  // SSID 출력
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // IP 주소 출력
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // RSSI 출력
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  // 현재 연결 상태 출력
  wl_status_t status = WiFi.status();
  Serial.print("WiFi Status: ");
  switch(status) {
    case WL_CONNECTED:
      Serial.println("Connected");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("No SSID Available");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("Connection Failed");
      break;
    case WL_DISCONNECTED:
      Serial.println("Disconnected");
      break;
    default:
      Serial.println("Unknown");
      break;
  }
}

void setup() {

  Serial.begin(9600);
  delay(1000); // 시리얼 초기화 대기
  Serial.println("Setup start");

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);

  for (int thisNote = 0; thisNote < 8; thisNote++) {//센서 초기화 알림음 
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZ_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZ_PIN);
  }

  for(int i = 0; i < 30; i++){//PIR 센서 초기화를 위해 30초 대기
    Serial.print(30-i);
    Serial.print("초 남았습니다. ");
    Serial.println("잠시 기다려주세요.");
    delay(1000);
  }

  digitalWrite(LED_PIN, LOW);//LED는 기본으로 LOW 처리 
  Serial.println("Sensors initialized.");

  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // WiFi 연결 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  printWifiStatus();

  setDevice();

  for (int thisNote = 0; thisNote < 8; thisNote++) {//기기 설정 완료 알림음 
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZ_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZ_PIN);
  }
  Serial.println("Device setting end.");

}

void loop() {
  int pirVal = digitalRead(PIR_PIN);

  if (pirVal == HIGH && pirState == false) {  // PIR 감지 시작
    pirState = true;
    motionStartTime = millis();  // HIGH 순간부터 시간 측정
    Serial.println(F("PIR 감지됨 (HIGH)"));
  }

  if (pirVal == LOW && pirState == true) {  // PIR 감지 종료
    pirState = false;
    isToneOn = false;
    Serial.println(F("PIR 해제됨 (LOW)"));
    post("/TinyIoT/Arduino/pir_sensor","CIN","","0");
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }

  if (pirState == true) {  // PIR 감지 상태 유지
    unsigned long currentTime = millis();
    // 특정 시간 이상 HIGH 상태 유지되었다면 LED, 부저 ON
    if (currentTime - motionStartTime >= thresholdDuration) {
      digitalWrite(LED_PIN, HIGH);
      if (!isToneOn) {
        post("/TinyIoT/Arduino/pir_sensor","CIN","","1");
        tone(BUZ_PIN, 262, 200);  // 부저 소리 발생
        post("/TinyIoT/Arduino/buzzer","CIN","","1");
        delay(100);
        post("/TinyIoT/Arduino/led","CIN","","1");
        isToneOn = true; 
      }
    }
  }

  // 주기적으로 WiFi 상태 출력
  printWifiStatus();
  delay(5000); // 5초 대기
}

void setDevice(){
  Serial.println(F("Setup Started!"));

  /* TinyIoT에서 Arduino AE가 존재하는지 확인, 없으면 생성 */
  int status = get("/TinyIoT/Arduino");
  delay(5000);
  if(status == 1) {
    Serial.println(F("Successfully retrieved!\n"));
  } else if(status == 0){
    Serial.println(F("Not Found... Creating AE\n"));

    int state = post("/TinyIoT","AE","Arduino","");
    delay(5000);
    if(state == 1){
      Serial.println(F("Successfully created AE\n"));
      delay(100);
      post("/TinyIoT/Arduino", "CNT", "pir_sensor", "") == 1 ? Serial.println(F("Successfully created pir_sensor CNT!\n")) : Serial.println(F("Failed to create pir_sensor CNT!\n"));
      delay(100);
      post("/TinyIoT/Arduino", "CNT", "led", "") == 1 ? Serial.println(F("Successfully created led CNT!\n")) : Serial.println(F("Failed to create led CNT!\n"));
      delay(100);
      post("/TinyIoT/Arduino", "CNT", "buzzer", "") == 1 ? Serial.println(F("Successfully created buzzer CNT!\n")) : Serial.println(F("Failed to create buzzer CNT!\n"));
    } else if(state == 0){
      Serial.println(F("Invalid Header or URI"));
    } else {
      Serial.println(F("Wifi not connected!"));
    }
  } else{
    Serial.println(F("Wifi not connected!"));
  }
  delay(30000);

  Serial.println(F("Setup completed!"));
}

/* Get Request(AE, CNT, CIN) */
int get(String path) {
  String request = "GET " + path + " HTTP/1.1\r\n";
  request += "Host: ";
  request += server;
  request += ":" + String(port) + "\r\n";
  request += "X-M2M-RI: retrieve\r\n";
  request += "X-M2M-Rvi: 2a\r\n";
  request += "X-M2M-Origin: CAdmin\r\n";
  request += "Accept: application/json\r\n";
  request += "Connection: close\r\n\r\n";

  Serial.println(F("---- HTTP REQUEST START ----"));
  Serial.println(request);
  Serial.println(F("---- HTTP REQUEST END ----"));

  if (wifi.connect(server, port)) {
    wifi.print(request);
    Serial.println(F("\nRequest sent"));

    int status = -1;
    bool headerParsed = false;
    bool isHeader = true;

    while (wifi.connected()) {
      while (wifi.available()) {
        String line = wifi.readStringUntil('\n');
        Serial.println(line); // 전체 응답 출력

        if (isHeader) {
          // HTTP 상태 코드 확인
          if (!headerParsed && line.startsWith("HTTP/1.1")) {
            status = line.substring(9, 12).toInt();
            Serial.print("Parsed Status Code: ");
            Serial.println(status);
            headerParsed = true;
          }

          // 헤더 끝 확인 (빈 줄)
          if (line == "\r") {
            isHeader = false; // 헤더 끝
          }
        }
      }
    }
    wifi.stop();

    Serial.print("Final Status Code: ");
    Serial.println(status);

    return status == 200 ? 1 : 0;
  } else {
    Serial.println(F("Connection to server failed"));
    return -1;
  }
}

/* Post Request Method(AE, CNT, CIN) */
int post(String path, String contentType, String name, String content){
  // HTTP Request Header
  String request = "POST " + path + " HTTP/1.1\r\n";
  request += "Host: ";
  request += server;
  request += ":";
  request += String(port);
  request += "\r\n";
  request += "X-M2M-RI: create\r\n";
  request += "X-M2M-Rvi: 2a\r\n";
  request += "X-M2M-Origin: SArduino\r\n"; //create를 할 때는 origin을 SArduino로 설정함, 원한다면 변경 가능 
  
  if(contentType.equals("AE")){
    request += "Content-Type: application/json;ty=2\r\n";
  } else if(contentType.equals("CNT")){
    request += "Content-Type: application/json;ty=3\r\n";
  } else if(contentType.equals("CIN")){
    request += "Content-Type: application/json;ty=4\r\n";
  } else{
    Serial.println(F("Unvalid Content Type!")); 
    return 0;
  }
  
  // JSON 본문 직렬화
  String body = serializeJsonBody(contentType, name, content);

  request += "Content-Length: " + String(body.length()) + "\r\n";
  request += "Accept: application/json\r\n";
  request += "Connection: close\r\n\r\n";

  // 요청 로그 출력
  Serial.println(F("---- HTTP REQUEST START ----"));
  Serial.println(request);
  Serial.println(body);
  Serial.println(F("---- HTTP REQUEST END ----"));

  // 헤더와 본문과 함께 요청 전송
  if (wifi.connect(server, port)) {
    wifi.print(request);
    wifi.print(body);
    Serial.println(F("\nRequest sent"));

    int status = -1;
    bool headerParsed = false;
    bool isHeader = true;

    // 응답 읽기
    while (wifi.connected()) {
      while (wifi.available()) {
        String line = wifi.readStringUntil('\n');
        Serial.println(line); // 응답 로그 출력

        if (isHeader) {
          if (!headerParsed && line.startsWith("HTTP/1.1")) {
            status = line.substring(9, 12).toInt();
            Serial.print("Parsed Status Code: ");
            Serial.println(status);
            headerParsed = true;
          }
          if (line == "\r") {
            isHeader = false;
          }
        }
      }
    }
    wifi.stop();

    Serial.print("Final Status Code: ");
    Serial.println(status);

    return status == 201 ? 1 : 0;
  } else {
    Serial.println(F("Connection to server failed"));
    return -1;
  }
}

String serializeJsonBody(String contentType, String name, String content){
  String body;
  StaticJsonDocument<256> doc;  // JSON 문서 크기 조정 (필요 시 증가)

  if(contentType.equals("AE")){
    JsonObject m2m_ae = doc.createNestedObject("m2m:ae");
    m2m_ae["rn"] = name;
    m2m_ae["api"] = "NArduino";
    JsonArray lbl = m2m_ae.createNestedArray("lbl");
    lbl.add("ae_" + name);
    m2m_ae.createNestedArray("srv").add("3");
    m2m_ae["rr"] = true;
  } else if(contentType.equals("CNT")){
    JsonObject m2m_cnt = doc.createNestedObject("m2m:cnt");
    m2m_cnt["rn"] = name;
    JsonArray lbl = m2m_cnt.createNestedArray("lbl");
    lbl.add("cnt_" + name);
    m2m_cnt["mbs"] = 16384;
  } else if(contentType.equals("CIN")){
    JsonObject m2m_cin = doc.createNestedObject("m2m:cin");
    m2m_cin["con"] = content;
  }
  
  serializeJson(doc, body);
  return body;
}

String unsignedToString(unsigned int value) {
    String result;
    do {
        result = char('0' + (value % 10)) + result;
        value /= 10;
    } while (value > 0);
    return result;
}


