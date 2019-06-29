# ESP Telegram PC Switch
Устройство на базе микроконтроллера ESP 8266 позволяет через Telegram бота осуществлять контроль состояния компьютера, включать, выключать или перезагружать. Подключается напрямую к материнской плате (Front Panel) паралельно с передней панелью, имитирует нажатие кнопки Power SW, питается от USB порта.

![alt text](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/photos/Telegram_bot_0.png "Бот")
![alt text](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/photos/Telegram_bot_1.png "Бот")
![alt text](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/photos/final.JPG "Финал")

### Компоненты
[Схема подключения](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/schemes/Schematic_ESP-WoL.pdf)

Для устройства:

| Компонент | Количество |
| --- | :---: |
| ESP8266-12E | 1 |
| Плата-адаптер для ESP8266 | 1 |
| ASM1117-3.3 | 1 |
| Конденсатор 330uF | 1 |
| Транзистор 2N2222 | 2 |
| Резистор 470 Ом | 2 |
| Резистор 470 Ом | 2 |
| Резистор 10 КОм | 1 |

Для подключения к материнской плате:

| Компонент | Количество |
| --- | :---: |
| Провод двухжильный | несколько метров |
| 0.100" (2.54 mm) Female Header: 1x2-Pin | 7  |
| 0.100" (2.54mm) Pitch Connector Headers | 5  |
| PCB макетная плата 3х7 | 1 |
| Корпус | 1 |

![alt text](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/photos/front.JPG "front")
![alt text](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/photos/side.JPG "side")

### Получение токена
Необходимо зарегистрировать бота и получить уникальный id, являющийся одновременно и токеном. Для этого в Telegram существует специальный бот — [@BotFather](tg://resolve?domain=BotFather). 
Пишем ему /start и получаем список всех его команд. 
Первая и главная — /newbot — отправляем ему и бот просит придумать имя новому боту. Следующим шагом бот запросит username, в конце оно должно оканчиваться на «bot». В случае успеха BotFather возвращает токен бота и ссылку для быстрого добавления бота в контакты.

### Настройка конфигурацонного файла config.h
Необходимо создать файл config.h в директорию рядом со скетчем wol.ino. В файле необходимо указать:
 
  - Имя сети  Wi-Fi, к которой подкючается устройство;
  - Пароль от Wi-Fi сети;
  - Токен telegram бота из предыдущего пункта.

Пример:

    #define SECRET_SSID "DIR-300"
    #define SECRET_PASS "1234567890"
    #define SECRET_BOTTOKEN "123456789:AAEIh14fcBOJdsq_-6b1NEUIDk_LdPMsYbM"

### Авторизация пользователей
Бот будет выполнять действия только тех пользователей, чьи Telegram ID входят в белый список. Получить Telegram ID можно при помощи [бота @getmyid_bot](tg://resolve?domain=getmyid_bot).
Белый список находится в коде wol.ino, переменная whiteList_length это количество пользователей, а переменная whiteList это массив идентификаторов пользователей.

    #define whiteList_length 1
    String whiteList[whiteList_length] = {
      "350367633"
    };

### Статический IP
Для стабильной работы бота необходимо назначить ему IP-адрес в функции setup:

    WiFi.config(IPAddress(192, 168, 1, 40),
              IPAddress(192, 168, 1, 1),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));

### Сторонние библиотеки
Для сборки проекта понадобятся стороннние библиотеки, которые находятся в папке [libs](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/libs/). Добавление библиотек в Arduino IDE описано в следующем пункте.
Ссылки на git репозитории:
- [witnessmenow/Universal-Arduino-Telegram-Bot V1.1.0](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/V1.1.0)
- [bblanchon/ArduinoJson V5.x](https://github.com/bblanchon/ArduinoJson/tree/5.x)

### Прошивка ESP 8266
1. Скачиваем и устанавливаем [Arduino IDE](https://www.arduino.cc/en/Main/Software);
2. Настраиваем Arduino IDE согласно [статье](http://geekmatic.in.ua/arduino_ide_with_wifi_esp8266);
3. Добавляем библиотеки из папки [libs](https://github.com/DARKB100D/ESPTelegramPCSwitch/blob/master/libs/):
Войдите в меню: Скетч > Подключить библиотеку > Добавить .ZIP библиотеку...
В появившемся окне выберите ZIP файл библиотеки, который Вы скачали. После чего нажмите на кнопку «Открыть»;
4. Различных плат огромное количество, поэтому подготовку платы к перепрошивки описывать не стану, в интернете информации достаточно;
5. Прошиваем микроконтроллер. 

### Лицензия
- **[GPL-3.0 license](https://opensource.org/licenses/GPL-3.0)**
- Copyright 2019 © [Илья Ковалёв](https://github.com/DARKB100D).
