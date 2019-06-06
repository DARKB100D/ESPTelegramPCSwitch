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
String whiteList[1] = {"350367633"};

const int power_sw_pin = 2;
const int power_led_pin = 2;
const int hdd_led_pin = 2;

bool is_sleep = false;

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
  digitalWrite(power_sw_pin, 1);

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
