/*******************************************************************

  Air Raid Alert ESP8266
  https://github.com/pavel-fomychov/air-raid-alert-esp8266

  Autor: Pavel Fomychov
  YouTube: https://www.youtube.com/c/PavelFomychov
  Instagram: https://www.instagram.com/pavel.fomychov/

  Board: ESP8266
  Upload Speed: 115200

*******************************************************************/

#define WIFI_SSID "TP-LINK"   // Имя точки доступа WiFi
#define WIFI_PASS "12345678"  // Пароль точки доступа WiFi

#define pinBuzzer    4  // Вывод бузера подключить к D2(GPIO4) платы ESP8266
#define pinRedLED   13  // Вывод красного светодиода подключить к D7(GPIO13) платы ESP8266
#define pinGreenLED 14  // Вывод зеленого светодиода подключить к D5(GPIO14) платы ESP8266

#include <ESP8266WiFi.h>      // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <WiFiClient.h>
WiFiClient client;

#include <ArduinoJson.h>      // https://arduinojson.org/v6/doc/installation/

// Необходимо раскомментировать свой регион!
//String city = "Cherkasy";
//String city = "Chernihiv";
//String city = "Chernivtsi";
//String city = "Crimea";
//String city = "Dnipropetrovs'k";
//String city = "Donets'k";
//String city = "Ivano-Frankivs'k";
//String city = "Kharkiv";
//String city = "Kherson";
//String city = "Khmel'nyts'kyy";
//String city = "Kirovohrad";
//String city = "Kyiv City";
//String city = "Kyiv";
//String city = "Luhans'k";
//String city = "L'viv";
//String city = "Mykolayiv";
//String city = "Odessa";
//String city = "Poltava";
//String city = "Rivne";
//String city = "Sevastopol";
//String city = "Sumy";
//String city = "Ternopil'";
//String city = "Transcarpathia";
//String city = "Vinnytsya";
//String city = "Volyn";
//String city = "Zaporizhzhya";
//String city = "Zhytomyr";

boolean alarmFlag = false;
boolean startAlarm = false;
boolean stopAlarm = false;
boolean greenFlag = false;
boolean redFlag = false;
uint32_t timeGreenLED = 0;
uint32_t timeRedLED = 0;

void setup() {
  // Инициализация монитора порта
  Serial.begin(9600);

  // Конфигурация выводов
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);

  // Настройка выводов
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
  digitalWrite(16, LOW);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    ConnectAPI();
    delay(1000);
  } else {
    ConnectWiFi();
  }
}

// Подключение к WiFi
void ConnectWiFi() {
  Serial.println(F("Соединение с сетью WiFi..."));
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(180);
    greenLED();
    delay(180);
    LEDoff();
  }
  Serial.println(F("Соединение успешно."));
}

// Выполнение запроса к API и обработка данных
boolean ConnectAPI() {
  client.setTimeout(3000);
  if (!client.connect("sirens.in.ua", 80)) {
    Serial.println(F("Ошибка подключения к sirens.in.ua"));
    return false;
  }

  // Отправка HTTP запроса
  client.println(F("GET /api/v1/ HTTP/1.0"));
  client.println(F("Host: sirens.in.ua"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Ошибка при отправке запроса"));
    client.stop();
    return false;
  }

  // Проверка кода ответа
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // Ответ должен быть: "HTTP/1.0 200 OK" или "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.println(status);
    client.stop();
    return false;
  }

  // Отбрасывание HTTP заголовков
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Некорректный ответ"));
    client.stop();
    return false;
  }

  // Обработка объекта JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("Ошибка обработки JSON: "));
    Serial.println(error.f_str());
    client.stop();
    return false;
  }

  // Завершение соединения
  client.stop();

  // Получение статуса по указанной области
  String result = doc[city].as<String>();
  Serial.print(F("Response: "));
  Serial.println(result);

  // Обработка статуса
  if (result == "full") {
    redLED();
    startAlarm = true;
    stopAlarm = false;
  } else if (result == "partial" || result == "no_data") {
    redBlink();
  } else {
    greenLED();
    startAlarm = false;
    stopAlarm = true;
  }
  buzzer();

  return true;
}

// Активация буззера
void buzzer() {
  if (startAlarm && !alarmFlag) {
    for (uint32_t i = 0; i < 3; i++) {
      siren();
    }
    alarmFlag = true;
  }
  if (stopAlarm && alarmFlag) {
    for (uint32_t i = 0; i < 1; i++) {
      anthem();
    }
    alarmFlag = false;
  }
}

// Сигнал сирены
void siren() {
  for (uint16_t up = 200; up < 800; up++) {
    tone(pinBuzzer, up);
    delay(10);
  }
  for (uint16_t down = 800; down > 200; down--) {
    tone(pinBuzzer, down);
    delay(10);
  }
  noTone(pinBuzzer);
}

// Гимн Украины
void anthem() {
  tone(pinBuzzer, 659.26, 750);
  delay(1 + 750);
  tone(pinBuzzer, 659.26, 250);
  delay(1 + 250);
  tone(pinBuzzer, 659.26, 250);
  delay(1 + 250);
  tone(pinBuzzer, 587.33, 250);
  delay(1 + 250);
  tone(pinBuzzer, 698.46, 250);
  delay(1 + 250);
  tone(pinBuzzer, 783.99, 750);
  delay(1 + 750);
  tone(pinBuzzer, 698.46, 250);
  delay(1 + 250);
  tone(pinBuzzer, 659.26, 500);
  delay(1 + 500);
  tone(pinBuzzer, 587.33, 500);
  delay(1 + 500);
  tone(pinBuzzer, 523.25, 500);
  delay(1 + 500);
  tone(pinBuzzer, 659.26, 500);
  delay(1 + 500);
  tone(pinBuzzer, 493.88, 500);
  delay(1 + 500);
  tone(pinBuzzer, 659.26, 500);
  delay(1 + 500);
  tone(pinBuzzer, 440.10, 250);
  delay(1 + 250);
  tone(pinBuzzer, 415.30, 250);
  delay(1 + 250);
  tone(pinBuzzer, 440.10, 250);
  delay(1 + 250);
  tone(pinBuzzer, 493.88, 250);
  delay(1 + 250);
  tone(pinBuzzer, 523.25, 500);
  delay(1 + 500);
  tone(pinBuzzer, 587.33, 500);
  delay(1 + 500);
  tone(pinBuzzer, 659.26, 750);
  delay(1 + 750);
  tone(pinBuzzer, 659.26, 250);
  delay(1 + 250);
  tone(pinBuzzer, 659.26, 250);
  delay(1 + 250);
  tone(pinBuzzer, 587.33, 250);
  delay(1 + 250);
  tone(pinBuzzer, 659.26, 250);
  delay(1 + 250);
  tone(pinBuzzer, 698.46, 250);
  delay(1 + 250);
  tone(pinBuzzer, 783.99, 750);
  delay(1 + 750);
  tone(pinBuzzer, 698.46, 250);
  delay(1 + 250);
  tone(pinBuzzer, 659.26, 500);
  delay(1 + 500);
  tone(pinBuzzer, 587.33, 500);
  delay(1 + 500);
  tone(pinBuzzer, 523.25, 500);
  delay(1 + 500);
  tone(pinBuzzer, 440.10, 500);
  delay(1 + 500);
  tone(pinBuzzer, 659.26, 750);
  delay(1 + 750);
  tone(pinBuzzer, 415.30, 250);
  delay(1 + 250);
  tone(pinBuzzer, 440.10, 1000);
  delay(1 + 1000);
  tone(pinBuzzer, 440.10, 500);
  delay(1 + 500);
}

// Моргание красным светодиодом
void redBlink() {
  if (timeRedLED < millis()) {
    if (redFlag) {
      redLED();
    } else {
      LEDoff();
    }
    timeRedLED = millis() + 180;
    redFlag = !redFlag;
  }
}

// Моргание зеленым светодиодом
void greenBlink() {
  if (timeGreenLED < millis()) {
    if (greenFlag) {
      greenLED();
    } else {
      LEDoff();
    }
    timeGreenLED = millis() + 180;
    greenFlag = !greenFlag;
  }
}

// Свечение красным светодиодом
void redLED() {
  digitalWrite(pinRedLED, HIGH);
  digitalWrite(pinGreenLED, LOW);
}

// Свечение зеленым светодиодом
void greenLED() {
  digitalWrite(pinRedLED, LOW);
  digitalWrite(pinGreenLED, HIGH);
}

// Отключение светодиода
void LEDoff() {
  digitalWrite(pinRedLED, LOW);
  digitalWrite(pinGreenLED, LOW);
}
