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

const int BlueLed = 2; // LED_BUILTIN

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

  pinMode(LED_BUILTIN, OUTPUT);

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

void viewKeyBoard(String &chat_id) {
  String keyboardJson = "[[ \"/on\", \"/off\"]]";
  bot->sendMessageWithReplyKeyboard(chat_id, "Выберите действие", "", keyboardJson, true);
}

bool validateChat(String &chat_id) {
  if (chat_id == "") return false;
  if (!chat_id.equals("350367633")) {
    bot->sendMessage(chat_id, "401 Unauthorized.");
    return false;
  }
  return true;
}

void executeCommand(String &text, String &chat_id) {
  if (text.equals("/on")) {
    digitalWrite(LED_BUILTIN, LOW);
    bot->sendMessage(chat_id, "The Led 1 is now ON");
    return;
  }
  if (text.equals("/off")) {
    digitalWrite(LED_BUILTIN, HIGH);
    bot->sendMessage(chat_id, "The Led 1 is now OFF");
    return;
  }
  bot->sendMessage(chat_id, "404 Not Found.");
}

void handleNewMessages(int &numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot->messages[i].chat_id);
    if (!validateChat(chat_id)) continue;
    executeCommand(bot->messages[i].text, chat_id);
    viewKeyBoard(chat_id);
  }
}
