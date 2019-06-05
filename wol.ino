#include "config.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;
const char BotToken[] = SECRET_BOTTOKEN;

WiFiClientSecure net_ssl;
TelegramBot bot (BotToken, net_ssl);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  //WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  net_ssl.setInsecure();
  bot.begin();

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) return;
  message m = bot.getUpdates();
  Serial.println(m.chat_id);
  
  if (!validateMessage(m))return;
  executeCommand(m.text, m.chat_id);
  delay(1500);
}

bool validateMessage(message m) {
  if (m.chat_id == "") return false;
  if (!m.chat_id.equals("350367633")) {
    bot.sendMessage(m.chat_id, "401 Unauthorized.");
    return false;
  }
  return true;
}
  
void executeCommand(String &text, String &chat_id) {
  if (text.equals("/on")) {
    digitalWrite(LED_BUILTIN, LOW);
    bot.sendMessage(chat_id, "The Led 1 is now ON");
    return;
  }
  if (text.equals("/off")) {
    digitalWrite(LED_BUILTIN, HIGH);
    bot.sendMessage(chat_id, "The Led 1 is now OFF");
    return;
  } 
  bot.sendMessage(chat_id, "404 Not Found.");
 }
