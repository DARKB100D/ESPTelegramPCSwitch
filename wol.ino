#include "config.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;
const char BotToken[] = SECRET_BOTTOKEN;

WiFiClientSecure net_ssl;
UniversalTelegramBot *bot;

int Bot_mtbs = 1000;
long Bot_lasttime = 0;

#define whiteList_length 1
String whiteList[whiteList_length] = {
  "350367633"
};

const int power_sw_pin = 12;
const int power_led_in_pin = 13;
const int good_night_control_pin = 14;

//  4 SDA (I²C)
//  5 SCL (I²C)
// 12 MISO (SPI)
// 13 MOSI (SPI)
// 14 SCK (SPI)

bool is_sleep = false;

const String keyboardJson_work =
  "[[\"\xF0\x9F\x94\xB4 Выключить\", \"\xF0\x9F\x94\x84 Перезагрузить\"],[\"\xE2\x99\xBB Обновить\"],[\"\xE2\x9C\xA8 Ещё\"]]";
const String keyboardJson_sleep =
  "[[\"\xF0\x9F\x86\x99 Разбудить\"],[\"\xE2\x99\xBB Обновить\"],[\"\xE2\x9C\xA8 Ещё\"]]";
const String keyboardJson_off =
  "[[\"\xF0\x9F\x86\x99 Включить\"],[\"\xE2\x99\xBB Обновить\"],[\"\xE2\x9C\xA8 Ещё\"]]";
const String keyboardJson_menu =
  "[[\"\xF0\x9F\x94\x85 Индикаторы\"],[\"\xE2\x9A\xAB ESP\"],[\"\xF0\x9F\x94\x99 Назад\"]]";

const String state_sleep = "\xF0\x9F\x92\xA4 (сон)";
const String state_work = "\xE2\x9C\x85 (работает)";
const String state_off = "\xE2\x9D\x8C (выключен)";

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA); // Начинаем подключаться
  WiFi.disconnect(); // По правильному - отключимся от предыдущих соединений.
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  pinMode(power_sw_pin, OUTPUT);
  digitalWrite(power_sw_pin, 0);

  pinMode(power_led_in_pin, INPUT);
  pinMode(good_night_control_pin, OUTPUT);

  net_ssl.setInsecure();
  bot = new UniversalTelegramBot(BotToken, net_ssl);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() > Bot_lasttime + Bot_mtbs) {
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}

void sendStatus(String &chat_id) {
  String keyboardJson = "[[ \"/on\", \"/off\"]]";
  String state = "Сон";
  if (!is_sleep) {
    if (digitalRead(power_led_pin)) state = "Работает";
    else state = "Выключен";
  }
  bot->sendMessageWithReplyKeyboard(chat_id, "Состояние: " + state, "", keyboardJson, true);
}

bool validateChat(String &chat_id) {
  if (chat_id == "") return false;
  if (!chat_id.equals("350367633")) {
    bot->sendMessage(chat_id, "401 Unauthorized.");
    return false;
  }
  return true;
}

void power_sw(int duration) {
    digitalWrite(power_sw_pin, LOW);
    delay(duration);
    digitalWrite(power_sw_pin, HIGH);
}

void reset_sw() {
    power_sw(8000);
    delay(500);
    power_sw(200);
}

void executeCommand(String &text, String &chat_id) {
  if (text.equals("/power_on")) {
    power_sw(200);
    return;
  }
  if (text.equals("/power_off")) {
    power_sw(8000);
    return;
  }
  if (text.equals("/hard_reset")) {
    reset_sw();
    return;
  }
  if (text.equals("/good_night_led_on")) {
    return;
  }
  if (text.equals("/good_night_led_off")) {
    return;
  }
  if (text.equals("/status")) {
    return;
  }
  if (text.equals("/start")) {
    String start_msg = "/power_on - Включить (разбудить)\n"
    "/power_off - Жесткое выключение\n"
    "/hard_reset - Жесткая перезагрузка\n"
    "/status - Теущее состояние ПК\n";
    bot->sendMessage(chat_id, start_msg);
    return;
  }
  bot->sendMessage(chat_id, "404 Not Found.");
}

void handleNewMessages(int &numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot->messages[i].chat_id);
    if (!validateChat(chat_id)) continue;
    executeCommand(bot->messages[i].text, chat_id);
    sendStatus(chat_id);
  }
}
