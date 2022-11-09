// -------------------------  Вывод символа из MAX7219_Fonts.h
void showChar(char ch, int col, int row, const uint8_t *data) {

  int arraySize = pgm_read_byte(&data[0]);                    // Ширина массива символов
  int symbolSize = pgm_read_byte(&data[1 + ch * arraySize]);  // Размер символа
  int numCol = symbolSize / 10;                               // Размер символа - ширина
  int numRow = symbolSize % 10;                               // Размер символа - высота
  byte sign[numRow];                                          // Создаем массив символа
  for ( int i = 0; i < numRow; i++ ) {
    sign[i] = pgm_read_byte(&data[2 + ch * arraySize + i]);   // Заполняем массив символа из большого массива шрифтов
  }

  for (int y = 0; y < numRow; y++ ) {
    for (int x = 0; x < numCol; x++ ) {
      matrix.drawPixel(col + x, row + y, sign[y] & (1 << x)); // Рисуем символ
    }
  }
  matrix.write();
}

// -------------------------  ВЫВОД БОЛЬШИХ ЧАСОВ 6x8
void showClockBig() {
  showChar(h1, dx, dy, dig6x8);
  showChar(h0, dx + 7, dy, dig6x8);

  if (synchronizedTime) {                                               // если время синхронизировано
    if (secFr >= 0 && secFr <= 166)                                     // каждую секунду анимируем двоеточие (чтобы мигало)
      showChar(11, dx + 14, dy, dig6x8);
    if (secFr > 332 && secFr <= 498)
      showChar(12, dx + 14, dy, dig6x8);
    if (secFr > 166 && secFr <= 332 || secFr > 498 && secFr <= 1000)
      showChar(10, dx + 14, dy, dig6x8);
  } else {                                                              // если время не синхронизировано
    if ((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) // мигаем двоеточием в тревожном ритме
      showChar(10, dx + 14, dy, dig6x8);
    if ((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
      showChar(13, dx + 14, dy, dig6x8);
  }
  showChar(m1, dx + 19, dy, dig6x8);
  showChar(m0, dx + 26, dy, dig6x8);
}

// -------------------------  ВЫВОД СРЕДНИХ ЧАСОВ 5x8
void showClockMed() {
  showChar(h1, dx + 2, dy, dig5x8);
  showChar(h0, dx + 8, dy, dig5x8);

  if (synchronizedTime) {                                               // если время синхронизировано
    if (secFr >= 0 && secFr <= 332)                                     // каждую секунду анимируем двоеточие (чтобы мигало)
      showChar(10, dx + 14, dy, dig5x8);
    if (secFr > 332 && secFr <= 1000)
      showChar(11, dx + 14, dy, dig5x8);
  } else {                                                              // если время не синхронизировано
    if ((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) // мигаем двоеточием в тревожном ритме
      showChar(11, dx + 14, dy, dig5x8);
    if ((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
      showChar(13, dx + 14, dy, dig5x8);
  }
  showChar(m1, dx + 18, dy, dig5x8);
  showChar(m0, dx + 24, dy, dig5x8);
}

// -------------------------  ВЫВОД СРЕДНИХ ЧАСОВ 4x8 С СЕКУНДАМИ 3x7
void showClockMedSec() {
  showChar(h1, dx + 1, dy, dig4x8);
  showChar(h0, dx + 6, dy, dig4x8);

  if (synchronizedTime) {                                               // если время синхронизировано рисуем двоеточие
    showChar(10, dx + 10, dy, dig4x8);                                  // рисуем двоеточие
  } else {                                                              // если время не синхронизировано
    if ((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) // мигаем двоеточием в тревожном ритме
      showChar(10, dx + 10, dy, dig4x8);
    if ((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000))
      showChar(13, dx + 10, dy, dig4x8);
  }
  showChar(m1, dx + 13, dy, dig4x8);
  showChar(m0, dx + 18, dy, dig4x8);
  showChar(s1, dx + 24, dy + 1, dig3x7);
  showChar(s0, dx + 28, dy + 1, dig3x7);
}

// -------------------------  ВЫВОД ТОНКИХ ЧАСОВ С СЕКУНДАМИ 3x7
void showClockThin() {
  showChar(h1, dx + 2, dy+1, dig3x7);
  showChar(h0, dx + 6, dy+1, dig3x7);

  if (synchronizedTime) {                                                 // если время синхронизировано
    showChar(10, dx + 9, dy+1, dig3x7);                                     // рисуем двоеточие
    showChar(10, dx + 19, dy+1, dig3x7);
  } else {                                                                // если время не синхронизировано
    if ((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) { // мигаем двоеточием в тревожном ритме
      showChar(10, dx + 9, dy+1, dig3x7);
      showChar(10, dx + 19, dy+1, dig3x7);
    }
    if ((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000)) {
      showChar(13, dx + 9, dy+1, dig3x7);
      showChar(13, dx + 19, dy+1, dig3x7);
    }
  }
  showChar(m1, dx + 12, dy+1, dig3x7);
  showChar(m0, dx + 16, dy+1, dig3x7);
  showChar(s1, dx + 22, dy+1, dig3x7);
  showChar(s0, dx + 26, dy+1, dig3x7);
}

// -------------------------  ВЫВОД МЕЛКИХ ЧАСОВ 3x6
void showClockSmall() {
  showChar(h1, dx + 2, dy + 1, dig3x6);
  showChar(h0, dx + 6, dy + 1, dig3x6);

  if (synchronizedTime) {                                                 // если время синхронизировано
    showChar(10, dx + 9, dy + 1, dig3x6);                                 // рисуем двоеточие
    showChar(10, dx + 19, dy + 1, dig3x6);
  } else {                                                                // если время не синхронизировано
    if ((secFr >= 0 && secFr <= 100) || (secFr >= 200 && secFr <= 300)) { // мигаем двоеточием в тревожном ритме
      showChar(10, dx + 9, dy + 1, dig3x6);
      showChar(10, dx + 19, dy + 1, dig3x6);
    }
    if ((secFr > 100 && secFr <= 200) || (secFr > 300 && secFr <= 1000)) {
      showChar(12, dx + 9, dy + 1, dig3x6);
      showChar(12, dx + 19, dy + 1, dig3x6);
    }
  }
  showChar(m1, dx + 12, dy + 1, dig3x6);
  showChar(m0, dx + 16, dy + 1, dig3x6);
  showChar(s1, dx + 22, dy + 1, dig3x6);
  showChar(s0, dx + 26, dy + 1, dig3x6);
}

// -------------------------  ВЫВОД ДАТЫ 3x7 разделитель - слэш
void showDateSlash() {
  showChar(d1, dx + 1, dy+1, dig3x7);
  showChar(d0, dx + 5, dy+1, dig3x7);
  showChar(11, dx + 9, dy+1, dig3x7);
  showChar(mn1, dx + 12, dy+1, dig3x7);
  showChar(mn0, dx + 16, dy+1, dig3x7);
  showChar(11, dx + 20, dy+1, dig3x7);
  showChar(y1, dx + 23, dy+1, dig3x7);
  showChar(y0, dx + 27, dy+1, dig3x7);
}

// -------------------------  ВЫВОД ДАТЫ 3x7 разделитель - точка
void showDateDot() {
  showChar(d1, dx + 2, dy+1, dig3x7);
  showChar(d0, dx + 6, dy+1, dig3x7);
  showChar(12, dx + 9, dy+1, dig3x7);
  showChar(mn1, dx + 12, dy+1, dig3x7);
  showChar(mn0, dx + 16, dy+1, dig3x7);
  showChar(12, dx + 19, dy+1, dig3x7);
  showChar(y1, dx + 22, dy+1, dig3x7);
  showChar(y0, dx + 26, dy+1, dig3x7);
}

// -------------------------  ВЫВОД ДАТЫ 3x7 с месяцем и днем недели
void showDateFull() {
  showChar(dw, dx, dy + 2, daysOfTheWeek7x6); // Выводим день недели
  showChar(d1, dx + 8, dy+1, dig3x7);
  showChar(d0, dx + 12, dy+1, dig3x7);

  switch (mn1 * 10 + mn0) {                   // Выводим три буквы месяца
    case 1:                                   // ЯНВ
      showChar(17, dx + 17, dy+1, month4x7);
      showChar(9, dx + 22, dy+1, month4x7);
      showChar(1, dx + 27, dy+1, month4x7);
      break;
    case 2:                                   // ФЕВ
      showChar(15, dx + 17, dy+1, month4x7);
      showChar(4, dx + 23, dy+1, month4x7);
      showChar(1, dx + 28, dy+1, month4x7);
      break;
    case 3:                                   // МАР
      showChar(8, dx + 17, dy+1, month4x7);
      showChar(0, dx + 23, dy+1, month4x7);
      showChar(12, dx + 28, dy+1, month4x7);
      break;
    case 4:                                   // АПР
      showChar(0, dx + 17, dy+1, month4x7);
      showChar(11, dx + 22, dy+1, month4x7);
      showChar(12, dx + 27, dy+1, month4x7);
      break;
    case 5:                                   // МАИ
      showChar(8, dx + 17, dy+1, month4x7);
      showChar(0, dx + 23, dy+1, month4x7);
      showChar(5, dx + 28, dy+1, month4x7);
      break;
    case 6:                                   // ИЮН
      showChar(5, dx + 17, dy+1, month4x7);
      showChar(16, dx + 22, dy+1, month4x7);
      showChar(9, dx + 28, dy+1, month4x7);
      break;
    case 7:                                   // ИЮЛ
      showChar(5, dx + 17, dy+1, month4x7);
      showChar(16, dx + 22, dy+1, month4x7);
      showChar(7, dx + 28, dy+1, month4x7);
      break;
    case 8:                                   // АВГ
      showChar(0, dx + 17, dy+1, month4x7);
      showChar(1, dx + 22, dy+1, month4x7);
      showChar(2, dx + 27, dy+1, month4x7);
      break;
    case 9:                                   // СЕН
      showChar(13, dx + 17, dy+1, month4x7);
      showChar(4, dx + 22, dy+1, month4x7);
      showChar(9, dx + 27, dy+1, month4x7);
      break;
    case 10:                                  // ОКТ
      showChar(10, dx + 17, dy+1, month4x7);
      showChar(6, dx + 22, dy+1, month4x7);
      showChar(14, dx + 27, dy+1, month4x7);
      break;
    case 11:                                  // НОЯ
      showChar(9, dx + 17, dy+1, month4x7);
      showChar(10, dx + 22, dy+1, month4x7);
      showChar(17, dx + 27, dy+1, month4x7);
      break;
    case 12:                                  // ДЕК
      showChar(3, dx + 17, dy+1, month4x7);
      showChar(4, dx + 23, dy+1, month4x7);
      showChar(6, dx + 28, dy+1, month4x7);
      break;
  }
}
