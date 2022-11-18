// --------- Обработка нажатия кнопки
void checkButton() {
  if (btn.click()) {                     // одиночное нажатие
    menuTimer = millis();
    matrix.fillScreen(LOW);
    matrix.write();

    switch (menuStatus) {
      case 0:    // в обычном режиме
        if (screen < 6) {   // если нажата кнопка в режиме показа времени, включаем показ даты на SHOW_DATE_TIME секунд
          screen = dateScreen + 5;
          dataTimer = millis();
        } else {
          if (screen > 5) {  // выход из режима показа даты по нажатию
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
      case 3:    // режим включения автоматического изменения яркости матрицы
        BRIGHT_CONTROL = !BRIGHT_CONTROL;
        break;
      case 4:    // режим настройки минимальной яркости матрицы
        if (++MATRIX_BRIGHT_MIN > 15) MATRIX_BRIGHT_MIN = 1;
        checkBrightness();
        break;
      case 5:    // режим настройки максимальной яркости матрицы
        if (++MATRIX_BRIGHT_MAX > 15) MATRIX_BRIGHT_MAX = 1;
        if (MATRIX_BRIGHT_MAX < MATRIX_BRIGHT_MIN) MATRIX_BRIGHT_MAX = MATRIX_BRIGHT_MIN;
        checkBrightness();
        break;
    }
  }

  if (btn.held()) {         // удержание кнопки
    menuTimer = millis();
    if (++menuStatus > maxMenuItem) { // шагаем по меню
      menuStatus = 0;
      screen = timeScreen;
    }
  }
}
// --------- END Обработка нажатия кнопки
