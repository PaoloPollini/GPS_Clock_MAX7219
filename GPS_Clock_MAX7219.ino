/*
GPS Clock v1.0
(c)saigon 2020  
Written: Sep 26 2020.
Last Updated: Feb 07 2022

Подключение матрицы
MAX7219   ->   Arduino Nano
 Vcc o----------- +5v
 DIN o----------- D11
  CS o----------- D8
 CLK o----------- D13
 Gnd o----------- Gnd

Подключение часов реального времени
DS3231     ->    Arduino Nano
   Vcc o----------- +5v
D(SDA) o----------- A4
C(SCL) o----------- A5
   Gnd o----------- Gnd

Подключение GPS модуля
NEO-6M-0-001  ->  Arduino Nano
  Vcc o----------- +5v
   Rx o----------- D7
   Tx o----------- D9
  Gnd o----------- Gnd

Подключение сенсорной кнопки
ttp223    ->    Arduino Nano
  Vcc o----------- +3.3v
  I/O o----------- D2
  Gnd o----------- Gnd

Подключение фоторезистора
Фоторезистор    ->    Arduino Nano
   +------------------ +5v
   |
  | |
  |R| Фоторезистор
  | |
   |
   +------------------ A3
   |
  | |
  |R| 10 kOm
  | |
   |
   +------------------ Gnd
  
*/


#include <Wire.h>
#include <RTClib.h>
#include <TinyGPS++.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <SoftwareSerial.h>
#include "MAX7219_Fonts.h"                        // Подключаем нашу библиотеку шрифтов

// Пины
#define pinCS           8                         // Подключение пина CS матрицы
#define BUTTON_PIN      2                         // Пин сенсорной кнопки
#define BRIGHT_PIN      A3                        // Пин фоторезистора
#define RXPIN           7                         // RX пин GPS модуля
#define TXPIN           9                         // TX пин GPS модуля


// управление яркостью
#define BRIGHT_CONST 1                            // яркость матрицы при отключенном управлении яркостью
#define BRIGHT_CONTROL 0                          // 0- запретить/ 1- разрешить управление яркостью (при отключении яркость всегда будет равна BRIGHT_CONST)
#define BRIGHT_THRESHOLD 150                      // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define MATRIX_BRIGHT_MAX 9                       // макс яркость матрицы (1 - 15)
#define MATRIX_BRIGHT_MIN 1                       // мин яркость матрицы (1 - 15)



int TIMEZONE = 3;                                 // Часовая зона
static const uint32_t GPSBaud = 9600;             // скорость порта GPS модуля
int GPSyear;                                      // Год с GPS
char GPSday, GPSmonth, GPShour, GPSmin, GPSsec;   // Дата и время с GPS
unsigned long GPSage;

int screen;                                       // Набор символов, отображаемый в текущий момент времени на экране
int timeScreen=3;                                 // Вид часов на экране. Варианты: 1- Большие часы 6х8, 2- Средние часы 5х8, 3- Средние часы 4x8 с секундами 3x7, 4- Тонкие часы с секундами 3x7, 5- Мелкие часы 3x6
int dateScreen=3;                                 // Вид даты на экране. Варианты: 1- 3x7 разделитель слэш, 2- 3x7 разделитель точка, 3- 3x6 с месяцем и днем недели
int lastScreen=1;                                 // Запоминаем последний отображаемый экран
unsigned long showDateTimer = 0;                  // Таймер показа даты



int buttonKey = 0;                                // переменная для хранения количества нажатий
boolean buttonState = false;                      // кнопка нажата = true
boolean buttonFlag = false;                       // флаг состояния кнопки
unsigned long buttonTimer = 0;                    // время нажатия кнопки
unsigned long buttonLastClick = 0;                // время нажатия кнопки для подсчета кликов
int holdTime = 1000;                              // время, в течение которого нажатие можно считать удержанием кнопки
int doubleTime = 500;                             // время, в течение которого нажатия можно считать двойным

boolean buttonSingle = false;                     // флаг состояния "краткое нажатие"
boolean buttonDouble = false;                     // флаг состояния "двойное нажатие"
boolean buttonHold = false;                       // флаг состояния "долгое нажатие"



int dx=0,dy=0;                                    // начальные координаты на светодиодной матрице
int h1,h0,m1,m0,s1,s0,secFr,lastSec=1,lastHour=0; // h1 - десятки часов, h0 - еденицы часов и так далее, secFr- секундный цикл,
int d1,d0,mn1,mn0,y1,y0,dw,lastDay=-1;            // d1 - десятки дней, d0 - еденицы дней и так далее...
int key = 3;                                      // флаг кнопки
int matrixBrightness = 1;                         // переменная для хранения текущей яркости матрицы


boolean showDate = true;                          // флаг включения показа даты в конце каждой минуты
int showDateInterval = 5;                         // время отображения даты на экране в конце каждой минуты в секундах
boolean synchronizedTime = false;                 // если более одного часа не прошла синхронизация времени с GPS, двоеточие между разрядами часов и минут будет мигать в тревожном ритме,
                                                  // а попытки синхронизации времени будут выполняться каждую секунду

unsigned long dotsTimer;                          // таймер для отсчета дробных долей секунды


Max72xxPanel matrix = Max72xxPanel(pinCS, 4, 1);  // Инициализируем матрицу
RTC_DS3231 rtc;                                   // Часы реального времени
TinyGPSPlus gps;                                  // GPS модуль
SoftwareSerial ss(TXPIN, RXPIN);                  // Последовательный интерфейс

String hour;              // часы
String minute;            // минуты
int spacer = 1;           // расстояние между символами
int width = 5 + spacer;
boolean debug = true;     // отладка
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// --------------------------------------------------------------------- SETUP 
void setup(){
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Initialize DS3231
  if (! rtc.begin()) Serial.println("Couldn't find RTC");
  else  Serial.println("RTC OK");

  if (!rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  matrix.setIntensity(matrixBrightness);  // устанавливаем яркость матрицы

// начальные координаты матриц 8*8
  matrix.setRotation(0, 1);        // 1 матрица
  matrix.setRotation(1, 1);        // 2 матрица
  matrix.setRotation(2, 1);        // 3 матрица
  matrix.setRotation(3, 1);        // 4 матрица
  
  matrix.fillScreen(LOW);

}
// --------------------------------------------------------------------- LOOP 
void loop(){

  DateTime now = rtc.now();                                                   // получаем значение даты и времени
  if(now.second()!=lastSec){                                                  // проверка начала новой секунды
    lastSec=now.second();
    dotsTimer = millis();                                                     // в начале новой секунды запускаем таймер 

  h1=now.hour()/10;                                                           // записываем десятки часов
  h0=now.hour()%10;                                                           // единицы часов
  m1=now.minute()/10;                                                         // десятки минут
  m0=now.minute()%10;                                                         // единицы минут
  s1=now.second()/10;                                                         // десятки секунд
  s0=now.second()%10;                                                         // единицы секунд
  d1=now.day()/10;                                                            // десятки дней
  d0=now.day()%10;                                                            // единицы дней
  dw=now.dayOfTheWeek();                                                      // день недели
  mn1=now.month()/10;                                                         // десятки месяца
  mn0=now.month()%10;                                                         // единицы месяца
  y1=(now.year()-2000)/10;                                                    // десятки года
  y0=(now.year()-2000)%10;                                                    // единицы года

  checkBrightness();                                                          // установка яркости матрицы 1 раз в секунду
  }

  secFr=(millis() - dotsTimer);                                               // dots - меняет значение от 0 до 1000 в течении каждой секунды
  sensButton();                                                               // Вызов функции обработки нажатия кнопки
 
  if ((now.hour()==0) && (now.minute()==0)) {                                 // Синхронизация времени по GPS в начале каждого часа
      synchronizedTime = false;
      syncTime();
     }


  if (buttonSingle) Serial.println("Одиночное нажатие");
  if (buttonDouble) Serial.println("Двойное нажатие");
  if (buttonHold) Serial.println("Удержание кнопки");


  if (buttonSingle) {                                                         // Обработка одиночного нажатия кнопки в основном режиме показа времени и даты
    if (screen < 6) {                                                         // Одиночный клик включает показ даты на время заданное в showDateInterval
      showDateTimer = millis();
      screen = dateScreen + 5;
    } else {
      screen = timeScreen;
      showDateTimer = 0;
    }
  }

  if (screen > 6 && millis() - showDateTimer > showDateInterval * 1000) {     // Возвращаем показ времени 
    screen = timeScreen;
  }

  if (showDate && now.second() >= 30 && now.second() < 30 + showDateInterval) {    // Показываем дату в середине каждой минуты
    screen = dateScreen + 5;
  }








    if (gps.encode(ss.read()))
      displayInfo();
      
 if (now.second() >= 45 && now.second() < 47 && !synchronizedTime) 
  syncTime();

if (now.second() >= 30 && now.second() < 45) 
  synchronizedTime = false;
  


  if (lastScreen != screen){                                                        // Очищаем экран если изменился
      matrix.fillScreen(LOW);
      lastScreen = screen;
  }

   switch (screen) {
    case 1:
    showClockBig();     // Большие часы 6х8
      break;
    case 2:
    showClockMed();     // Средние часы 5х8
      break;
    case 3:
    showClockMedSec();  // Средние часы 4x8 с секундами 3x7
      break;
    case 4:
    showClockThin();    // Тонкие часы с секундами 3x7
      break;
    case 5:
    showClockSmall();   // Мелкие часы 3x6
      break;
    case 6:
    showDateSlash();    // Дата 3x7 разделитель слэш
      break; 
    case 7:
    showDateDot();      // Дата 3x7 разделитель точка
      break;
    case 8:
    showDateFull();     // Дата 3x6 с месяцем и днем недели
      break;
   }
      

}
// --------------------------------------------------------------------- END LOOP 


// -------------------------------------------------------------- Вывод текста
void text(String tape) {

int x = 8;                //отступ слева, координата X начала текста
int y = 0;                //отступ сверху, координата Y начала текста

  //matrix.fillScreen(LOW);

     
    for ( int i = 0 ; i < tape.length(); i++ ) {
      int letter = i;
      matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
     if (i == 2) x += width - 1;    // уменьшаем отступ между двоеточием и первой десяткой минут (так симметричнее)
       else x += width;
    }
    matrix.write(); // отправка данных на дисплей
}

// -----------------------------------------------------------  Вывод символа из MAX7219_Fonts.h
void showChar(char ch, int col, int row, const uint8_t *data){
  
  int arraySize = pgm_read_byte(&data[0]);                    // Ширина массива символов
  int symbolSize = pgm_read_byte(&data[1+ch*arraySize]);      // Размер символа
  int numCol = symbolSize/10;                                 // Размер символа - ширина
  int numRow = symbolSize%10;                                 // Размер символа - высота
  byte w[numRow];                                             // Создаем массив символа
    for ( int i = 0; i < numRow; i++ ) { 
    w[i]=pgm_read_byte(&data[2+ch*arraySize+i]);              // Заполняем массив символа из большого массива шрифтов 
}
    //Serial.println(numCol);
   // Serial.println(numRow);
  
  for (int y = 0; y < numRow; y++ ) {
    for (int x = 0; x < numCol; x++ ) {
      matrix.drawPixel(col + x, row + y, w[y] & (1 << x));     // Рисуем символ
    }
  }
  matrix.write();
}




// ------------------------------------------------------- Функция обработки нажатия сенсорной кнопки
void sensButton() {
  
 buttonSingle = false;
 buttonDouble = false;
 buttonHold = false;    
 
 buttonState=digitalRead(BUTTON_PIN);                                         // считываем состояние сенсорной кнопки


  if (buttonState && !buttonFlag) {                                           // если кнопка нажата
    buttonFlag = true;
    buttonTimer = millis();
      if (millis() - buttonLastClick > doubleTime) {
        buttonLastClick = millis();
        buttonKey = 0;
      }
  }

    if (buttonState && buttonFlag && millis() - buttonTimer >= holdTime) {    // Долгое нажатие
      buttonHold = true;
      buttonTimer = millis();
      //Serial.println("Долгое нажатие");
    }

  if (!buttonState && buttonFlag) {                                           //если кнопка отпущена, выйти из цикла
    buttonFlag = false;
    buttonKey++;

    if (!buttonHold && buttonKey < 2 && millis() - buttonTimer < holdTime) {  // Одиночный клик
      buttonSingle = true;
     // Serial.println("Одиночное нажатие");
    }

    if (buttonKey > 1 && millis() - buttonLastClick <= doubleTime) {          // Двойной клик
      buttonDouble = true;
      //Serial.println("Двойное нажатие");      
    }

   // buttonHold = false;
  }

}

/*
// ------------------------------------------------------- Функция обработки нажатия сенсорной кнопки
void sensKey() {

    if (digitalRead(BUTTON_PIN) && button_state==0) {        //выбор режимов кнопкой. Если кнопка нажата
    button_state=1;
     button_press=millis();      //запомнить время нажатия
    matrix.fillScreen(LOW);
if (screen>5){
   dateScreen++;
  if (dateScreen > 3) dateScreen = 1; 
} else {
  timeScreen++;
  if (timeScreen > 5) timeScreen = 1;
}
  }
  if (digitalRead(BUTTON_PIN)==0 && button_state==1) {  //если кнопка отпущена, выйти из цикла
    button_state=0;

  }
 }
*/


// ------------------------------------------------------- Функция изменения яркости матрицы
void checkBrightness() {
  if (BRIGHT_CONTROL){                                                                      // если разрешена автояркость
  int val = analogRead(BRIGHT_PIN);                                                         // узнаем уровень освещенности
  int matrixBrightness = map(val, 0, 1023, MATRIX_BRIGHT_MIN, MATRIX_BRIGHT_MAX);           // преобразуем полученное значение в уровень яркости матрицы
  if (val <= BRIGHT_THRESHOLD) matrixBrightness = MATRIX_BRIGHT_MIN;                        // если уровень освещенности ниже минимального порога, яркость устанавливаем на минимальную
  matrix.setIntensity(matrixBrightness);                                                    // устанавливаем яркость матрицы
   Serial.println(val);
   Serial.println(matrixBrightness);
  } else {
   matrix.setIntensity(BRIGHT_CONST);                                                       // если запрещена автояркость, устанавливаем постоянную яркость
  }
}

// ------------------------------------------------------- Функция синхронизации времени 
void syncTime()
{
byte daysinamonth [13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};      // Количество дней в месяцах в невисокосном году

 // Синхронизируем время если получена корректная информация с GPS
 while (ss.available() > 0)
   if (gps.encode(ss.read()))
    {

 if (gps.date.isValid())
 {
  GPSday = gps.date.day();
  GPSmonth = gps.date.month();
  GPSyear = gps.date.year();
 }
 if (gps.time.isValid())
 {  
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
    if (GPSday > daysinamonth[GPSmonth])
      {
       GPSday = 1;
       GPSmonth++;
       if (GPSmonth > 12) {GPSmonth = 1; GPSyear++;}
      }
    }
  if (GPShour < 0) {                            // Корректируем время с GPS под часовые зоны западной долготы
    GPShour = GPShour + 24;
    GPSday--;
    if (GPSday < 1)
      {      
       GPSmonth--;
       if (GPSmonth < 1) {GPSmonth = 12; GPSyear--;}
       GPSday = daysinamonth[GPSmonth];
      }
    }

  if ((GPSage < 1500) && (gps.date.month() != 0)) {
    rtc.adjust(DateTime(GPSyear, GPSmonth, GPSday, GPShour, GPSmin, GPSsec));   // Пишем время с GPS в часы реального времени
    synchronizedTime = true;
    } 
  }
}

// -------------------------------------------------------------- ВЫВОД БОЛЬШИХ ЧАСОВ 6x8
void showClockBig(){
   showChar(h1, dx, dy, dig6x8);
   showChar(h0, dx+7, dy, dig6x8);

 if (synchronizedTime){                                               // если время синхронизировано
  if(secFr >= 0 && secFr <= 166)                                      // каждую секунду анимируем двоеточие (чтобы мигало)
   showChar(11, dx+14, dy, dig6x8);
  if(secFr > 332 && secFr <= 498)
   showChar(12, dx+14, dy, dig6x8);
  if(secFr > 166 && secFr <= 332 || secFr > 498 && secFr <= 1000)
   showChar(10, dx+14, dy, dig6x8);
 } else {                                                             // если время не синхронизировано
  if((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300))  // мигаем двоеточием в тревожном ритме
   showChar(10, dx+14, dy, dig6x8);
  if((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
   showChar(13, dx+14, dy, dig6x8);
  }
   showChar(m1, dx+19, dy, dig6x8);
   showChar(m0, dx+26, dy, dig6x8);
}

// -------------------------------------------------------------- ВЫВОД СРЕДНИХ ЧАСОВ 5x8
void showClockMed(){
   showChar(h1, dx+2, dy, dig5x8);
   showChar(h0, dx+8, dy, dig5x8);

 if (synchronizedTime){                                               // если время синхронизировано
  if(secFr >= 0 && secFr <= 332)                                      // каждую секунду анимируем двоеточие (чтобы мигало)
   showChar(10, dx+14, dy, dig5x8);
  if(secFr > 332 && secFr <= 1000)
   showChar(11, dx+14, dy, dig5x8);
 } else {                                                             // если время не синхронизировано
  if((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300))  // мигаем двоеточием в тревожном ритме
   showChar(11, dx+14, dy, dig5x8);
  if((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
   showChar(13, dx+14, dy, dig5x8);
  }
   showChar(m1, dx+18, dy, dig5x8);
   showChar(m0, dx+24, dy, dig5x8);
}

// -------------------------------------------------------------- ВЫВОД СРЕДНИХ ЧАСОВ 4x8 С СЕКУНДАМИ 3x7
void showClockMedSec(){
   showChar(h1, dx+1, dy, dig4x8);
   showChar(h0, dx+6, dy, dig4x8);

 if (synchronizedTime){                                               // если время синхронизировано рисуем двоеточие
   showChar(10, dx+10, dy, dig4x8);                                   // рисуем двоеточие
 } else {                                                             // если время не синхронизировано
  if((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300))  // мигаем двоеточием в тревожном ритме
   showChar(10, dx+10, dy, dig4x8);
  if((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
   showChar(13, dx+10, dy, dig4x8);
  }
   showChar(m1, dx+13, dy, dig4x8);
   showChar(m0, dx+18, dy, dig4x8);
   showChar(s1, dx+24, dy+1, dig3x7);
   showChar(s0, dx+28, dy+1, dig3x7);
}

// -------------------------------------------------------------- ВЫВОД ТОНКИХ ЧАСОВ С СЕКУНДАМИ 3x7
void showClockThin(){
   showChar(h1, dx+2, dy, dig3x7);
   showChar(h0, dx+6, dy, dig3x7);

 if (synchronizedTime){                                                // если время синхронизировано
   showChar(10, dx+9, dy, dig3x7);                                     // рисуем двоеточие
   showChar(10, dx+19, dy, dig3x7);    
 } else {                                                              // если время не синхронизировано
  if((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) { // мигаем двоеточием в тревожном ритме
   showChar(10, dx+9, dy, dig3x7);
   showChar(10, dx+19, dy, dig3x7);}
  if((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000)) {
   showChar(13, dx+9, dy, dig3x7);
   showChar(13, dx+19, dy, dig3x7);}
  }
   showChar(m1, dx+12, dy, dig3x7);
   showChar(m0, dx+16, dy, dig3x7);
   showChar(s1, dx+22, dy, dig3x7);
   showChar(s0, dx+26, dy, dig3x7);
}

// -------------------------------------------------------------- ВЫВОД МЕЛКИХ ЧАСОВ 3x6
void showClockSmall(){
   showChar(h1, dx+2, dy+1, dig3x6);
   showChar(h0, dx+6, dy+1, dig3x6);

 if (synchronizedTime){                                                 // если время синхронизировано
   showChar(10, dx+9, dy+1, dig3x6);                                    // рисуем двоеточие
   showChar(10, dx+19, dy+1, dig3x6);    
 } else {                                                               // если время не синхронизировано
  if((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) {  // мигаем двоеточием в тревожном ритме
   showChar(10, dx+9, dy+1, dig3x6);
   showChar(10, dx+19, dy+1, dig3x6);}
  if((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000)) {
   showChar(12, dx+9, dy+1, dig3x6);
   showChar(12, dx+19, dy+1, dig3x6);}
  }
   showChar(m1, dx+12, dy+1, dig3x6);
   showChar(m0, dx+16, dy+1, dig3x6);
   showChar(s1, dx+22, dy+1, dig3x6);
   showChar(s0, dx+26, dy+1, dig3x6);
}

// -------------------------------------------------------------- ВЫВОД ДАТЫ 3x7 разделитель - слэш
void showDateSlash(){
   showChar(d1, dx+1, dy, dig3x7);
   showChar(d0, dx+5, dy, dig3x7); 
   showChar(11, dx+9, dy, dig3x7); 
   showChar(mn1, dx+12, dy, dig3x7);
   showChar(mn0, dx+16, dy, dig3x7);
   showChar(11, dx+20, dy, dig3x7); 
   showChar(y1, dx+23, dy, dig3x7);
   showChar(y0, dx+27, dy, dig3x7);
}

// -------------------------------------------------------------- ВЫВОД ДАТЫ 3x7 разделитель - точка
void showDateDot(){
   showChar(d1, dx+2, dy, dig3x7);
   showChar(d0, dx+6, dy, dig3x7); 
   showChar(12, dx+9, dy, dig3x7); 
   showChar(mn1, dx+12, dy, dig3x7);
   showChar(mn0, dx+16, dy, dig3x7);
   showChar(12, dx+19, dy, dig3x7); 
   showChar(y1, dx+22, dy, dig3x7);
   showChar(y0, dx+26, dy, dig3x7);
}

// -------------------------------------------------------------- ВЫВОД ДАТЫ 3x6 с месяцем и днем недели
void showDateFull(){
   showChar(dw, dx, dy+1, daysOfTheWeek7x6);  // Выводим день недели
   showChar(d1, dx+8, dy, dig3x7);
   showChar(d0, dx+12, dy, dig3x7);
  
   switch (mn1*10+mn0) {                      // Выводим три буквы месяца 
    case 1:                                   // ЯНВ
      showChar(17, dx+17, dy, month4x7);
      showChar(9, dx+22, dy, month4x7);
      showChar(1, dx+27, dy, month4x7);
      break;
    case 2:                                   // ФЕВ
      showChar(15, dx+17, dy, month4x7);
      showChar(4, dx+23, dy, month4x7); 
      showChar(1, dx+28, dy, month4x7);
      break;
    case 3:                                   // МАР
      showChar(8, dx+17, dy, month4x7);
      showChar(0, dx+23, dy, month4x7);
      showChar(12, dx+28, dy, month4x7);
      break;
    case 4:                                   // АПР
      showChar(0, dx+17, dy, month4x7);
      showChar(11, dx+22, dy, month4x7);
      showChar(12, dx+27, dy, month4x7);
      break;
    case 5:                                   // МАИ
      showChar(8, dx+17, dy, month4x7);
      showChar(0, dx+23, dy, month4x7);
      showChar(5, dx+28, dy, month4x7);
      break;
    case 6:                                   // ИЮН
      showChar(5, dx+17, dy, month4x7);
      showChar(16, dx+22, dy, month4x7);
      showChar(9, dx+28, dy, month4x7);
      break;
    case 7:                                   // ИЮЛ
      showChar(5, dx+17, dy, month4x7);
      showChar(16, dx+22, dy, month4x7);
      showChar(7, dx+28, dy, month4x7);
      break;
    case 8:                                   // АВГ
      showChar(0, dx+17, dy, month4x7);
      showChar(1, dx+22, dy, month4x7);
      showChar(2, dx+27, dy, month4x7);
      break;
    case 9:                                   // СЕН
      showChar(13, dx+17, dy, month4x7);
      showChar(4, dx+22, dy, month4x7);
      showChar(9, dx+27, dy, month4x7);
      break;
    case 10:                                  // ОКТ
      showChar(10, dx+17, dy, month4x7);
      showChar(6, dx+22, dy, month4x7);
      showChar(14, dx+27, dy, month4x7);
      break;
    case 11:                                  // НОЯ
      showChar(9, dx+17, dy, month4x7);
      showChar(10, dx+22, dy, month4x7);
      showChar(17, dx+27, dy, month4x7);
      break;
    case 12:                                  // ДЕК
      showChar(3, dx+17, dy, month4x7);
      showChar(4, dx+23, dy, month4x7);
      showChar(6, dx+28, dy, month4x7);
      break;
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



/*      
  if (screen == 1)    showClockBig();    // Большие часы 6х8
  if (screen == 2)    showClockMed();    // Средние часы 5х8
  if (screen == 3)    showClockMedSec();  // Средние часы 4x8 с секундами 3x7
  if (screen == 4)    showClockThin();   // Тонкие часы с секундами 3x7
  if (screen == 5)    showClockSmall();  // Мелкие часы 3x6
  if (screen == 6)    showDateSlash();   // Дата 3x7 разделитель слэш
  if (screen == 7)    showDateDot();     // Дата 3x7 разделитель точка
  if (screen == 8)    showDateFull();    // Дата 3x6 с месяцем и днем недели


      
      Serial.print(dotsTimer);
      Serial.print(" ");
      Serial.print(now.second());
      Serial.print(" ");      
      Serial.print(secFr); 
      Serial.println();  

 
  while (ss.available() > 0)
   if (gps.encode(ss.read()))
    {
  GPSday = gps.date.day();
  GPSmonth = gps.date.month();
  GPSyear = gps.date.year();
  GPShour = gps.time.hour();
  GPSmin = gps.time.minute();
  GPSsec = gps.time.second();
  GPSage = gps.time.age();

Serial.print(GPSyear);
Serial.print(F("/"));
Serial.print(GPSmonth);
Serial.print(F("/"));
Serial.print(GPSday);
Serial.print(F("  "));
Serial.print(GPShour);
Serial.print(F(":"));
Serial.print(GPSmin);
Serial.print(F(":"));
Serial.print(GPSsec);
Serial.println();

Serial.print(now.year());
Serial.print(F("/"));
Serial.print(now.month());
Serial.print(F("/"));
Serial.print(now.day());
Serial.print(F("  "));
Serial.print(now.hour());
Serial.print(F(":"));
Serial.print(now.minute());
Serial.print(F(":"));
Serial.print(now.second());
Serial.println();
 Serial.println(ss.available(), DEC);
    } 


 if (debug) {                       // если разрешен дебаг, выводим в порт
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);

    Serial.print(" Temperature: ");
    Serial.print(rtc.getTemperature() - 2.25);
    Serial.println(" C");
 }
*/
