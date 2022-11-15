/*
GPS Clock v1.1
(c)saigon 2020-2022  
Written: Sep 26 2020.
Last Updated: Nov 15 2022
*/

#include <Wire.h>
#include <RTClib.h>
#include <TinyGPS++.h>
#include <EncButton2.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <SoftwareSerial.h>
#include "MAX7219_Fonts.h"                        // Подключаем нашу библиотеку шрифтов

// Пины
#define CS_PIN       8                            // Подключение пина CS матрицы
#define BUTTON_PIN   2                            // Пин сенсорной кнопки
#define BRIGHT_PIN   A3                           // Пин фоторезистора
#define RXPIN        9                            // RX пин GPS модуля
#define TXPIN        7                            // TX пин GPS модуля

// управление яркостью
#define BRIGHT_CONST        1                     // яркость матрицы при отключенном управлении яркостью
#define BRIGHT_CONTROL      0                     // 0- запретить/ 1- разрешить управление яркостью (при отключении яркость всегда будет равна BRIGHT_CONST)
#define BRIGHT_THRESHOLD    150                   // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define MATRIX_BRIGHT_MAX   9                     // макс яркость матрицы (1 - 15)
#define MATRIX_BRIGHT_MIN   1                     // мин яркость матрицы (1 - 15)

#define TIMEZONE           3                      // часовой пояс
#define SHOW_DATE_TIME     5                      // время отображения текущей даты по нажатию кнопки
#define SHOW_MENU_TIME     10                     // время автовыхода из меню настроек

byte screen;                                      // Набор символов, отображаемый в текущий момент времени на экране
byte timeScreen = 3;                              // Вид часов на экране. Варианты: 1-Большие часы 6х8, 2-Средние часы 5х8, 3-Средние часы 4x8 с секундами 3x7, 4-Тонкие часы с секундами 3x7, 5-Мелкие часы 3x6
byte dateScreen = 3;                              // Вид даты на экране. Варианты: 1-3x7 разделитель слэш, 2-3x7 разделитель точка, 3-3x6 с месяцем и днем недели
byte lastScreen;                                  // Запоминаем последний отображаемый экран
byte tempScreen;                                  // Запоминаем последний отображаемый экран для режима мигания 


unsigned long dotsTimer;                          // таймер для отсчета дробных долей секунды
unsigned long dataTimer;                          // таймер показа даты по нажатию кнопки
unsigned long menuTimer;                          // таймер показа меню
byte menuStatus = 0;                              // флаг состояния меню: 0-выкл, 1-выбор вида часов, 2-выбор вида даты, 3-часовая зона, 
                                                  //                      4-автояркость вкл/выкл, 5-показ даты в середине минуты вкл/выкл, 6-время отображения даты в середине минуты 1-30сек.

int secFr;                                        // секундный цикл
byte dx=0,dy=0;                                   // начальные координаты на светодиодной матрице
byte h1,h0,m1,m0,s1,s0,lastSec=1,lastHour=0;      // h1 - десятки часов, h0 - единицы часов и так далее
byte d1,d0,mn1,mn0,y1,y0,dw,lastDay=-1;           // d1 - десятки дней, d0 - единицы дней и так далее...


boolean synchronizedTime = false;                 // если более одного часа не прошла синхронизация времени с GPS, двоеточие между разрядами часов и минут будет мигать в тревожном ритме,
                                                  // а попытки синхронизации времени будут выполняться каждую секунду

static const uint32_t GPSBaud = 9600;             // скорость порта GPS модуля


Max72xxPanel matrix = Max72xxPanel(CS_PIN, 4, 1); // Инициализируем матрицу
RTC_DS3231 rtc;                                   // Часы реального времени
TinyGPSPlus gps;                                  // GPS модуль
SoftwareSerial ss(RXPIN, TXPIN);                  // Последовательный интерфейс
EncButton2<EB_BTN> btn(INPUT, BUTTON_PIN);        // Cенсорная кнопка


// ------------------------------------------------ SETUP 
void setup(){
  Serial.begin(9600);
  ss.begin(GPSBaud);

  btn.setButtonLevel(HIGH);        // уровень кнопки: LOW - кнопка подключает GND (по умолч.), HIGH - кнопка подключает VCC
  
  // Initialize DS3231
  if (!rtc.begin()) Serial.println("Couldn't find RTC");
  else  Serial.println("RTC OK");

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  

// начальные координаты матриц 8*8
  matrix.setRotation(0, 1);        // 1 матрица
  matrix.setRotation(1, 1);        // 2 матрица
  matrix.setRotation(2, 1);        // 3 матрица
  matrix.setRotation(3, 1);        // 4 матрица
  
  matrix.setIntensity(MATRIX_BRIGHT_MIN);  // устанавливаем яркость матрицы
  matrix.fillScreen(LOW);                  // очищаем матрицу
  matrix.write();
  
  screen = timeScreen;
}
// --------------------------------------------------------------------- LOOP 
void loop(){

  btn.tick();                             // опрос кнопки
  DateTime now = rtc.now();               // получаем значение даты и времени
  if(now.second()!=lastSec){              // проверка начала новой секунды
    lastSec=now.second();
    dotsTimer = millis();                 // в начале новой секунды запускаем таймер 

  h1=now.hour()/10;                       // записываем десятки часов
  h0=now.hour()%10;                       // единицы часов
  m1=now.minute()/10;                     // десятки минут
  m0=now.minute()%10;                     // единицы минут
  s1=now.second()/10;                     // десятки секунд
  s0=now.second()%10;                     // единицы секунд
  d1=now.day()/10;                        // десятки дней
  d0=now.day()%10;                        // единицы дней
  dw=now.dayOfTheWeek();                  // день недели
  mn1=now.month()/10;                     // десятки месяца
  mn0=now.month()%10;                     // единицы месяца
  y1=(now.year()-2000)/10;                // десятки года
  y0=(now.year()-2000)%10;                // единицы года

  checkBrightness();                      // установка яркости матрицы 1 раз в секунду
  }

  secFr=(millis() - dotsTimer);                                               // secFr - меняет значение от 0 до 1000 в течении каждой секунды
                                                        
 
  if ((now.minute()==0) && (now.second()==0)) {                                 // Синхронизация времени по GPS в начале каждого часа
      synchronizedTime = false;
      syncTime();
  }



 if (gps.encode(ss.read()))
  displayInfo();
      
 if (now.second() >= 45 && now.second() < 47 && !synchronizedTime) 
  syncTime();

 if (now.second() >= 30 && now.second() < 45) 
  synchronizedTime = false;
  


// --------- Обработка нажатия кнопки
  if (btn.click()) {                     // одиночное нажатие
    menuTimer = millis();

   switch (menuStatus){
   case 0:    // в обычном режиме
   if (screen < 6){    // если нажата кнопка в режиме показа времени, включаем показ даты на SHOW_DATE_TIME секунд
      screen = dateScreen + 5;
      dataTimer = millis();
    } else {
    if (screen > 5){   // выход из режима показа даты по нажатию
      screen = timeScreen;
        }
      }    
      break;
   case 1:    // режим выбора вида отображения времени
        if (++timeScreen > 5) timeScreen = 1;    
      break;
   case 2:    // режим выбора вида отображения даты
        if (++dateScreen > 3) dateScreen = 1; 
      break;  
   }   
  } // END одиночное нажатие
  
  if (screen > 5 && millis() - dataTimer > SHOW_DATE_TIME * 1000 && menuStatus == 0){   // возврат к показу текущего времени
    screen = timeScreen;
  }  

  if (millis() - menuTimer > SHOW_MENU_TIME * 1000 && menuStatus > 0){   // выход из меню
    menuStatus = 0;
    screen = timeScreen;
  } 


  if (btn.held()){          // удержание кнопки
    menuTimer = millis();
    if (++menuStatus > 2) {
      menuStatus = 0;
      screen = timeScreen;
    }
  }

  if (menuStatus == 1) tempScreen = timeScreen;
  if (menuStatus == 2) tempScreen = dateScreen+5;



// -------------------------  РЕАЛИЗАЦИЯ РЕЖИМА МИГАНИЯ
  if (menuStatus > 0) {
    if ((secFr >= 0 && secFr <= 500) || (secFr >= 0 && secFr <= 500)){
      screen = 0;
  }
    if ((secFr > 500 && secFr <= 1000) || (secFr > 500 && secFr <= 1000))
      screen = tempScreen;
  }
// -------------------------




  if (lastScreen != screen){              // Очищаем экран если изменился
      matrix.fillScreen(LOW);
      matrix.write();
      lastScreen = screen;
  }

  switch (screen){
   case 1: showClockBig();     break;  // Большие часы 6х8
   case 2: showClockMed();     break;  // Средние часы 5х8
   case 3: showClockMedSec();  break;  // Средние часы 4x8 с секундами 3x7
   case 4: showClockThin();    break;  // Тонкие часы с секундами 3x7
   case 5: showClockSmall();   break;  // Мелкие часы 3x6
   case 6: showDateSlash();    break;  // Дата 3x7 разделитель слэш
   case 7: showDateDot();      break;  // Дата 3x7 разделитель точка
   case 8: showDateFull();     break;  // Дата 3x6 с месяцем и днем недели
  }
}
// --------------------------------------------------------------------- END LOOP 




// ------------------------------------------------------- Функция изменения яркости матрицы
void checkBrightness() {
int matrixBrightness;                                                             // переменная для хранения текущей яркости матрицы
  if (BRIGHT_CONTROL){                                                            // если разрешена автояркость
  int val = analogRead(BRIGHT_PIN);                                               // узнаем уровень освещенности
  int matrixBrightness = map(val, 0, 1023, MATRIX_BRIGHT_MIN, MATRIX_BRIGHT_MAX); // преобразуем полученное значение в уровень яркости матрицы
  if (val <= BRIGHT_THRESHOLD) matrixBrightness = MATRIX_BRIGHT_MIN;              // если уровень освещенности ниже минимального порога, яркость устанавливаем на минимальную
  matrix.setIntensity(matrixBrightness);                                          // устанавливаем яркость матрицы
   Serial.println(val);
   Serial.println(matrixBrightness);
  } else {
   matrix.setIntensity(BRIGHT_CONST);                                             // если запрещена автояркость, устанавливаем постоянную яркость
  }
}

// ------------------------------------------------------- Функция синхронизации времени 
void syncTime() {
  
int GPSyear;                                                        // Год с GPS
byte GPSday, GPSmonth, GPShour, GPSmin, GPSsec;                     // Дата и время с GPS
unsigned long GPSage;                                               // Время с последнего получения данных от GPS
byte daysinamonth [13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};   // Количество дней в месяцах в невисокосном году

 // Синхронизируем время если получена корректная информация с GPS
 while (ss.available() > 0)
   if (gps.encode(ss.read())){

 if (gps.date.isValid()) {
  GPSday = gps.date.day();
  GPSmonth = gps.date.month();
  GPSyear = gps.date.year();
 }
 if (gps.time.isValid()) {  
  GPShour = gps.time.hour();
  GPSmin = gps.time.minute();
  GPSsec = gps.time.second();
  GPSage = gps.time.age();
 }
 GPShour=GPShour+TIMEZONE;                     // Корректируем время с GPS под указанную часовую зону

  if (((GPSyear % 4 == 0) && (GPSyear % 100 != 0)) || (GPSyear % 400 == 0)) {daysinamonth[2] = 29;} // Корректируем февраль, если год високосный
  
  if (GPShour > 23) {                           // Корректируем время с GPS под часовые зоны восточной долготы
    GPShour = GPShour - 24;
    GPSday++;
    if (GPSday > daysinamonth[GPSmonth]) {
       GPSday = 1;
       GPSmonth++;
       if (GPSmonth > 12) {GPSmonth = 1; GPSyear++;}
      }
    }
  if (GPShour < 0) {                            // Корректируем время с GPS под часовые зоны западной долготы
    GPShour = GPShour + 24;
    GPSday--;
    if (GPSday < 1) {      
       GPSmonth--;
       if (GPSmonth < 1) {GPSmonth = 12; GPSyear--;}
       GPSday = daysinamonth[GPSmonth];
      }
    }

  if ((GPSage < 500) && (gps.date.month() != 0)) {
    rtc.adjust(DateTime(GPSyear, GPSmonth, GPSday, GPShour, GPSmin, GPSsec));   // Пишем время с GPS в часы реального времени
    synchronizedTime = true;
    } 
  }
}






void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    if (gps.date.day() < 10) Serial.print(F("0"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    if (gps.date.month() < 10) Serial.print(F("0"));
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
    Serial.print(F(" Age: "));
    Serial.print(gps.time.age());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
