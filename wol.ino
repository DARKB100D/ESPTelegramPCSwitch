#include "config.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;
const char BotToken[] = SECRET_BOTTOKEN;

WiFiClientSecure net_ssl;
UniversalTelegramBot *bot;

int Bot_mtbs = 1500;
long Bot_lasttime = 0;

#define whiteList_length 1
String whiteList[whiteList_length] = {
  "350367633"
};

const int power_sw_pin = 12;
const int power_led_in_pin = 13;

//  4 SDA (I²C)
//  5 SCL (I²C)
// 12 MISO (SPI)
// 13 MOSI (SPI)
// 14 SCK (SPI)

const String keyboardJson_work =
  "[[\"\xF0\x9F\x94\xB4 Выключить\", \"\xF0\x9F\x94\x84 Перезагрузить\"],[\"\xE2\x99\xBB Обновить\"],[\"\xE2\x9C\xA8 Ещё\"]]";
const String keyboardJson_off =
  "[[\"\xF0\x9F\x86\x99 Включить\"],[\"\xE2\x99\xBB Обновить\"],[\"\xE2\x9C\xA8 Ещё\"]]";
const String keyboardJson_menu =
  "[[\"\xE2\x9A\xAB ESP\"],[\"\xF0\x9F\x94\x99 Назад\"]]";

const String state_work = "\xE2\x9C\x85 (работает)";
const String state_off = "\xE2\x9D\x8C (выключен)";

void setup() {
  Serial.begin(9600);
  
  WiFi.disconnect(); // По правильному - отключимся от предыдущих соединений.
  WiFi.mode(WIFI_STA);
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

  net_ssl.setInsecure();
  bot = new UniversalTelegramBot(BotToken, net_ssl);

  sendESPStatus(whiteList[0]);
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

int getState() {
  if (digitalRead(power_led_in_pin)) return 3; // working
  return 0; // soft-off
}

void sendStatus(String &chat_id) {
  String state_msg = "Состояние ПК: ";
  String msg = "\nДоступны действия:";
  auto state = getState();
  switch (state) {
    case 0:
      bot->sendMessageWithReplyKeyboard(chat_id, state_msg + state_work + msg, "", keyboardJson_work, true);
      break;
    default:
      bot->sendMessageWithReplyKeyboard(chat_id, state_msg + state_off + msg, "", keyboardJson_off, true);
      break;
  }
}

bool validateChat(String &chat_id) {
  if (chat_id == "") return false;
  for (int i = 0; i < whiteList_length; i++) if (chat_id.equals(whiteList[i])) return true;
  bot->sendMessage(chat_id, "401 Unauthorized.");
  return false;
}

void power_sw(int duration) {
  digitalWrite(power_sw_pin, HIGH);
  delay(duration);
  digitalWrite(power_sw_pin, LOW);
}

void reset_sw() {
  power_sw(8000);
  delay(500);
  power_sw(200);
}

String format2digit(int value) {
  String result;
  if (value < 10) result += "0";
  result += String(value);
  return result;
}

String getUptime() {
  long secs = millis() / 1000; //convect milliseconds to seconds
  long mins = secs / 60; //convert seconds to minutes
  long hours = mins / 60; //convert minutes to hours
  long days = hours / 24; //convert hours to days
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max
  String uptime = "";
  if (days > 0) uptime += String(days) + "d ";
  uptime += format2digit(hours) + ":";
  uptime += format2digit(mins) + ":";
  uptime += format2digit(secs);
  return uptime;
}

void sendESPStatus(String &chat_id) {
    String values = "SSID: " + WiFi.SSID() + "\n";
    values += "IP: " + WiFi.localIP().toString() + "\n";
    values += "Uptime: " + getUptime();
    bot->sendMessage(chat_id, values); 
} 

void executeCommand(String &text, String &chat_id) {
  Serial.println(text);
  if (text.equals("/status") || text.equals("/start") 
  || text.equals("u267b u041eu0431u043du043eu0432u0438u0442u044c") // обновить
  || text.equals("ud83dudd19 u041du0430u0437u0430u0434")) { // назад
    sendStatus(chat_id);
    return;
  }
  if (text.equals("/power_on") 
  || text.equals("ud83cudd99 u0412u043au043bu044eu0447u0438u0442u044c")) { // включить
    power_sw(200);
    sendStatus(chat_id);
    return;
  }
  if (text.equals("/power_off") || text.equals("ud83dudd34 u0412u044bu043au043bu044eu0447u0438u0442u044c")) { // выключить
    power_sw(8000);
    sendStatus(chat_id);
    return;
  }
  if (text.equals("/hard_reset") || text.equals("ud83dudd04 u041fu0435u0440u0435u0437u0430u0433u0440u0443u0437u0438u0442u044c")) { // перезагрузить
    reset_sw();
    sendStatus(chat_id);
    return;
  }
  if (text.equals("/status_esp") || text.equals("u26ab ESP")) { // ESP
    sendESPStatus(chat_id);
    return;
  }
  if (text.equals("u2728 u0415u0449u0451")) { // Ещё
    bot->sendMessageWithReplyKeyboard(chat_id, "Дополнительно:", "", keyboardJson_menu, true);
    return;
  }
  bot->sendMessage(chat_id, "404 Not Found.");
}

void handleNewMessages(int &numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot->messages[i].chat_id);
    if (!validateChat(chat_id)) continue;
    executeCommand(bot->messages[i].text, chat_id);
  }
}
