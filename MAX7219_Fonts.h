//http://xantorohara.github.io/led-matrix-editor/
//http://arduino.on.kg/matrix-font

/*
const uint8_t dig3x5[] PROGMEM = { 6,                  //
35,B00000111,B00000101,B00000101,B00000101,B00000111,  // 0
35,B00000010,B00000011,B00000010,B00000010,B00000111,  // 1
35,B00000111,B00000100,B00000111,B00000001,B00000111,  // 2
35,B00000111,B00000100,B00000111,B00000100,B00000111,  // 3
35,B00000101,B00000101,B00000111,B00000100,B00000100,  // 4
35,B00000111,B00000001,B00000111,B00000100,B00000111,  // 5
35,B00000111,B00000001,B00000111,B00000101,B00000111,  // 6
35,B00000111,B00000100,B00000100,B00000100,B00000100,  // 7
35,B00000111,B00000101,B00000111,B00000101,B00000111,  // 8
35,B00000111,B00000101,B00000111,B00000100,B00000111,  // 9
35,B00000000,B00000010,B00000000,B00000010,B00000000,  // :
35,B00000000,B00000000,B00000000,B00000000,B00000010,  // .
};
*/

const uint8_t dig3x6[] PROGMEM = { 7,                            //
36,B00000111,B00000101,B00000101,B00000101,B00000101,B00000111,  // 0
36,B00000010,B00000011,B00000010,B00000010,B00000010,B00000111,  // 1
36,B00000111,B00000100,B00000100,B00000010,B00000001,B00000111,  // 2
36,B00000111,B00000100,B00000110,B00000100,B00000100,B00000111,  // 3
36,B00000101,B00000101,B00000101,B00000111,B00000100,B00000100,  // 4
36,B00000111,B00000001,B00000111,B00000100,B00000100,B00000111,  // 5
36,B00000111,B00000001,B00000111,B00000101,B00000101,B00000111,  // 6
36,B00000111,B00000100,B00000100,B00000010,B00000010,B00000010,  // 7
36,B00000111,B00000101,B00000111,B00000101,B00000101,B00000111,  // 8
36,B00000111,B00000101,B00000101,B00000111,B00000100,B00000111,  // 9
36,B00000000,B00000000,B00000010,B00000000,B00000010,B00000000,  // :
36,B00000000,B00000010,B00000010,B00000001,B00000001,B00000000,  // слэш
36,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,  // пробел
};

const uint8_t dig3x7[] PROGMEM = { 8,                                       //
37,B00000010,B00000101,B00000101,B00000101,B00000101,B00000101,B00000010,  // 0
37,B00000010,B00000011,B00000010,B00000010,B00000010,B00000010,B00000111,  // 1
37,B00000010,B00000101,B00000100,B00000100,B00000010,B00000001,B00000111,  // 2
37,B00000011,B00000100,B00000100,B00000010,B00000100,B00000100,B00000011,  // 3
37,B00000001,B00000101,B00000101,B00000111,B00000100,B00000100,B00000100,  // 4
37,B00000111,B00000001,B00000011,B00000100,B00000100,B00000100,B00000011,  // 5
37,B00000010,B00000101,B00000001,B00000011,B00000101,B00000101,B00000010,  // 6
37,B00000111,B00000100,B00000100,B00000100,B00000010,B00000010,B00000010,  // 7
37,B00000010,B00000101,B00000101,B00000010,B00000101,B00000101,B00000010,  // 8
37,B00000010,B00000101,B00000101,B00000110,B00000100,B00000101,B00000010,  // 9
37,B00000000,B00000000,B00000010,B00000000,B00000010,B00000000,B00000000,  // :
37,B00000000,B00000000,B00000010,B00000010,B00000001,B00000001,B00000000,  // слэш
37,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010,  // .
37,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,  // пробел
};

/*
const uint8_t dig4x6[] PROGMEM = { 7,
46,B00000110,B00001001,B00001001,B00001001,B00001001,B00000110, //0
46,B00000010,B00000011,B00000010,B00000010,B00000010,B00000111, //1
46,B00000110,B00001001,B00001000,B00000110,B00000001,B00001111, //2
46,B00000110,B00001001,B00000100,B00001000,B00001001,B00000110, //3
46,B00001001,B00001001,B00001001,B00001111,B00001000,B00001000, //4
46,B00001111,B00000001,B00000111,B00001000,B00001001,B00000110, //5
46,B00000110,B00000001,B00000111,B00001001,B00001001,B00000110, //6
46,B00001111,B00001000,B00001000,B00000100,B00000010,B00000001, //7
46,B00000110,B00001001,B00000110,B00001001,B00001001,B00000110, //8
46,B00000110,B00001001,B00001001,B00001110,B00001000,B00000110, //9
36,B00000000,B00000010,B00000000,B00000000,B00000010,B00000000, //:
36,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010, //.
};
*/

const uint8_t dig4x8[] PROGMEM = { 9,
48,B00000110,B00001001,B00001001,B00001001,B00001001,B00001001,B00001001,B00000110, //0
48,B00000010,B00000011,B00000010,B00000010,B00000010,B00000010,B00000010,B00000111, //1
48,B00000110,B00001001,B00001001,B00001000,B00000100,B00000010,B00000001,B00001111, //2
48,B00000110,B00001001,B00001000,B00000100,B00001000,B00001000,B00001001,B00000110, //3
48,B00001001,B00001001,B00001001,B00001001,B00001110,B00001000,B00001000,B00001000, //4
48,B00001111,B00000001,B00000001,B00001111,B00001000,B00001000,B00001001,B00000110, //5
48,B00000110,B00000001,B00000001,B00000111,B00001001,B00001001,B00001001,B00000110, //6
48,B00001111,B00001000,B00001000,B00000100,B00000010,B00000010,B00000010,B00000010, //7
48,B00000110,B00001001,B00001001,B00000110,B00001001,B00001001,B00001001,B00000110, //8
48,B00000110,B00001001,B00001001,B00001001,B00001110,B00001000,B00001000,B00000110, //9
38,B00000000,B00000000,B00000010,B00000000,B00000000,B00000010,B00000000,B00000000, //:
38,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010, //.
38,B00000000,B00000010,B00000010,B00000010,B00000001,B00000001,B00000001,B00000000, // слэш
38,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000, // пробел
};

const uint8_t dig5x8[] PROGMEM = { 9,                                            //
58,B00001110,B00010001,B00010001,B00010001,B00010001,B00010001,B00010001,B00001110,  //0
58,B00000100,B00000110,B00000100,B00000100,B00000100,B00000100,B00000100,B00001110,  //1
58,B00001110,B00010001,B00010000,B00010000,B00001000,B00000100,B00000010,B00011111,  //2
58,B00001110,B00010001,B00010000,B00001100,B00010000,B00010000,B00010001,B00001110,  //3
58,B00010000,B00011000,B00010100,B00010010,B00010001,B00011111,B00010000,B00010000,  //4
58,B00011111,B00000001,B00000001,B00001111,B00010000,B00010000,B00010001,B00001110,  //5
58,B00001110,B00010001,B00000001,B00001111,B00010001,B00010001,B00010001,B00001110,  //6
58,B00011111,B00010000,B00010000,B00001000,B00000100,B00000010,B00000010,B00000010,  //7
58,B00001110,B00010001,B00010001,B00001110,B00010001,B00010001,B00010001,B00001110,  //8
58,B00001110,B00010001,B00010001,B00010001,B00011110,B00010000,B00010001,B00001110,  //9
38,B00000000,B00000000,B00000000,B00000010,B00000000,B00000010,B00000000,B00000000,  // :
38,B00000000,B00000000,B00000010,B00000000,B00000000,B00000000,B00000010,B00000000,  // :
58,B00000000,B00100000,B00010000,B00001000,B00000100,B00000010,B00000001,B00000000,  // слэш вправо
38,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,  // пробел
};  

/*
const uint8_t dig6x7[] PROGMEM = { 8,
67,B00011110,B00110011,B00111011,B00110111,B00110011,B00110011,B00011110, // 0
67,B00001100,B00001100,B00001110,B00001100,B00001100,B00001100,B00111111, // 1
67,B00011110,B00110011,B00110000,B00011000,B00000110,B00000011,B00111111, // 2
67,B00011110,B00110011,B00110000,B00011100,B00110000,B00110011,B00011110, // 3
67,B00011000,B00011100,B00011010,B00011001,B00111111,B00011000,B00011000, // 4
67,B00111111,B00000011,B00011111,B00110000,B00110000,B00110011,B00011110, // 5
67,B00011110,B00110011,B00000011,B00011111,B00110011,B00110011,B00011110, // 6
67,B00111111,B00110011,B00011000,B00011000,B00001100,B00001100,B00001100, // 7
67,B00011110,B00110011,B00110011,B00011110,B00110011,B00110011,B00011110, // 8
67,B00011110,B00110011,B00110011,B00111110,B00110000,B00110011,B00011110, // 9
47,B00000000,B00000110,B00000110,B00000000,B00000110,B00000110,B00000000, // :
47,B00000000,B00000010,B00000100,B00000000,B00000100,B00000010,B00000000, // : неполное двоеточие
47,B00000000,B00000100,B00000010,B00000000,B00000010,B00000100,B00000000, // : неполное двоеточие
67,B00000000,B00110000,B00011000,B00001100,B00000110,B00000011,B00000000, //  слэш вправо
};
*/

const uint8_t dig6x8[] PROGMEM = { 9,
68,B00011110,B00110011,B00110011,B00110011,B00110011,B00110011,B00110011,B00011110,  // 0
68,B00001100,B00001110,B00001110,B00001100,B00001100,B00001100,B00001100,B00111111,  // 1
68,B00011110,B00110011,B00110000,B00011000,B00001100,B00000110,B00110011,B00111111,  // 2
68,B00011110,B00110011,B00110000,B00011100,B00110000,B00110000,B00110011,B00011110,  // 3
68,B00111000,B00111100,B00110110,B00110011,B00110011,B00111111,B00110000,B00110000,  // 4
68,B00111111,B00000011,B00011111,B00110011,B00110000,B00110000,B00110011,B00011110,  // 5
68,B00011100,B00000110,B00000011,B00011111,B00110011,B00110011,B00110011,B00011110,  // 6
68,B00111111,B00110011,B00110000,B00110000,B00011000,B00001100,B00001100,B00001100,  // 7
68,B00011110,B00110011,B00110011,B00011110,B00110011,B00110011,B00110011,B00011110,  // 8
68,B00011110,B00110011,B00110011,B00110011,B00111110,B00110000,B00011000,B00001110,  // 9
48,B00000000,B00000110,B00000110,B00000000,B00000000,B00000110,B00000110,B00000000,  // :
48,B00000000,B00000010,B00000100,B00000000,B00000000,B00000100,B00000010,B00000000,  // : неполное двоеточие
48,B00000000,B00000100,B00000010,B00000000,B00000000,B00000010,B00000100,B00000000,  // : неполное двоеточие
48,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,  // пробел
}; 

const uint8_t daysOfTheWeek7x6[] PROGMEM = { 7,
76,B00100011,B01010101,B00010011,B00010101,B01010101,B00100011, //ВС
76,B01010111,B01010101,B01010101,B01110101,B01010101,B01010101, //ПН
76,B01110011,B00100101,B00100011,B00100101,B00100101,B00100011, //ВТ
76,B00110010,B01010101,B01010001,B00110001,B00010101,B00010010, //СР
76,B01110101,B00100101,B00100101,B00100110,B00100100,B00100100, //ЧТ
76,B01110111,B00100101,B00100101,B00100101,B00100101,B00100101, //ПТ
76,B01110010,B00010101,B00110001,B01010001,B01010101,B00110010, //СБ
};

const uint8_t month4x7[] PROGMEM = { 8,
47,B00001100,B00001010,B00001001,B00001001,B00001111,B00001001,B00001001, // А(0)
47,B00000111,B00001001,B00001001,B00000111,B00001001,B00001001,B00000111, // В(1)
47,B00001111,B00000001,B00000001,B00000001,B00000001,B00000001,B00000001, // Г(2)
57,B00001100,B00001010,B00001010,B00001010,B00001010,B00011111,B00010001, // Д(3)
47,B00001111,B00000001,B00000001,B00000111,B00000001,B00000001,B00001111, // Е(4)
47,B00001001,B00001001,B00001001,B00001101,B00001011,B00001001,B00001001, // И(5)
47,B00001001,B00001001,B00000101,B00000011,B00000101,B00001001,B00001001, // К(6)
47,B00001000,B00001100,B00001010,B00001001,B00001001,B00001001,B00001001, // Л(7)
57,B00010001,B00011011,B00010101,B00010001,B00010001,B00010001,B00010001, // М(8)
47,B00001001,B00001001,B00001001,B00001111,B00001001,B00001001,B00001001, // Н(9)
47,B00000110,B00001001,B00001001,B00001001,B00001001,B00001001,B00000110, // О(10)
47,B00001111,B00001001,B00001001,B00001001,B00001001,B00001001,B00001001, // П(11)
47,B00000111,B00001001,B00001001,B00000111,B00000001,B00000001,B00000001, // Р(12)
47,B00000110,B00001001,B00000001,B00000001,B00000001,B00001001,B00000110, // С(13)
57,B00011111,B00000100,B00000100,B00000100,B00000100,B00000100,B00000100, // Т(14)
57,B00001110,B00010101,B00010101,B00010101,B00001110,B00000100,B00000100, // Ф(15)
57,B00001001,B00010101,B00010101,B00010111,B00010101,B00010101,B00001001, // Ю(16)
47,B00001110,B00001001,B00001001,B00001110,B00001100,B00001010,B00001001, // Я(17)
};
