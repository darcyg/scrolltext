#include "HT1632.h"
#include "glcdfont.c"


#define swap(a, b) { uint16_t t = a; a = b; b = t; }

HT1632LEDMatrix::HT1632LEDMatrix(uint8_t data, uint8_t wr, uint8_t cs1) {
  matrices = (HT1632 *)malloc(sizeof(HT1632));

  matrices[0] = HT1632(data, wr, cs1);
  matrixNum  = 1;
  _width = 24 * matrixNum;
  _height = 16;
  setupBuffer();
}

HT1632LEDMatrix::HT1632LEDMatrix(uint8_t data, uint8_t wr, 
				 uint8_t cs1, uint8_t cs2) {
  matrices = (HT1632 *)malloc(2 * sizeof(HT1632));

  matrices[0] = HT1632(data, wr, cs1);
  matrices[1] = HT1632(data, wr, cs2);
  matrixNum  = 2;
  _width = 24 * matrixNum;
  _height = 16;
  setupBuffer();
}

HT1632LEDMatrix::HT1632LEDMatrix(uint8_t data, uint8_t wr, 
				 uint8_t cs1, uint8_t cs2, uint8_t cs3) {
  matrices = (HT1632 *)malloc(3 * sizeof(HT1632));

  matrices[0] = HT1632(data, wr, cs1);
  matrices[1] = HT1632(data, wr, cs2);
  matrices[2] = HT1632(data, wr, cs3);
  matrixNum  = 3;
  _width = 24 * matrixNum;
  _height = 16;
  setupBuffer();
}

HT1632LEDMatrix::HT1632LEDMatrix(uint8_t data, uint8_t wr, 
				 uint8_t cs1, uint8_t cs2, 
				 uint8_t cs3, uint8_t cs4) {
  matrices = (HT1632 *)malloc(4 * sizeof(HT1632));

  matrices[0] = HT1632(data, wr, cs1);
  matrices[1] = HT1632(data, wr, cs2);
  matrices[2] = HT1632(data, wr, cs3);
  matrices[3] = HT1632(data, wr, cs4);
  matrixNum  = 4;
  _width = 24 * matrixNum;
  _height = 16;
  setupBuffer();
}

void HT1632LEDMatrix::setupBuffer() {
  _rightBufferWidth = 16;
  for (uint8_t i=0; i<(_rightBufferWidth*16/8); i++) {
    _rightBuffer[i] = 0;
  }
}


void HT1632LEDMatrix::setPixel(uint8_t x, uint8_t y) {
  drawPixel(x, y, 1);
}
void HT1632LEDMatrix::clrPixel(uint8_t x, uint8_t y) {
  drawPixel(x, y, 0);
}

uint16_t pixelInMatrix(uint8_t x, uint8_t y) {
  uint16_t i;

  if (x < 8) {
    i = 7;
  } else if (x < 16) {
    i = 128 + 7;
  } else {
    i = 256 + 7;
  }
  i -= (x % 8);

  if (y < 8) {
    y *= 2;
  } else {
    y = (y-8) * 2 + 1;
  } 

  i += y * 8;
  return i;
}

void HT1632LEDMatrix::drawPixel(uint8_t x, uint8_t y, uint8_t color) {
  if (y >= _height) return;
  if (x >= _width + _rightBufferWidth) return;
  //Serial.println("Hello");
  // if it's in the buffer area, just update the buffer.
  // don't draw anything.
  if (x >= _width && x < _width + _rightBufferWidth) {
    //Serial.println("Drawing to buffer");
    x %= 24;
    uint16_t i = pixelInMatrix(x, y);
    if (color) {
      setPixelInBuffer(i);
    }
    else {
      clrPixelInBuffer(i);
    }
    return;
  }
  //Serial.println("Not drawing to buffer");
  uint8_t m;
  // figure out which matrix controller it is
  m = x / 24;
  x %= 24;
	
  uint16_t i = pixelInMatrix(x, y);

  if (color) 
    matrices[m].setPixel(i);
  else
    matrices[m].clrPixel(i);
}

uint8_t HT1632LEDMatrix::getPixel(uint8_t x, uint8_t y) {
  uint8_t m;
  // figure out which matrix controller it is
  m = x / 24;
  x %= 24;

  uint16_t i = pixelInMatrix(x, y);
  return matrices[m].getPixel(i);
}

uint8_t HT1632LEDMatrix::getPixelInBuffer(uint8_t x, uint8_t y) {
  uint16_t i = pixelInMatrix(x, y);
  return getPixelInBuffer(i);
}

void HT1632LEDMatrix::shiftLeft() {
  // for each matrix, shift the entire panel left by one pixel
  // and then set the rightmost column equal to the leftmost
  // column of the next panel. Set the last column of the last
  // panel to the first column of the right-side buffer.
  // Then shift the right buffer left by one pixel
	
  for (int m = 0; m < matrixNum; m++) {
    matrices[m].shiftLeft();
    for (uint8_t y = 0; y < _height; y++) {
      int colorToDraw;
      if (m < matrixNum-1) {
	colorToDraw = getPixel((m+1)*24, y);
      }
      else {
	colorToDraw = getPixelInBuffer(0, y);
      }
      drawPixel((m+1)*24 - 1, y, colorToDraw);
    }
  }

  shiftBufferLeft();
}

uint8_t HT1632LEDMatrix::width() {
  return _width;
}

uint8_t HT1632LEDMatrix::height() {
  return _height;
}

void HT1632LEDMatrix::begin(uint8_t type) {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].begin(type);
  }
}

void HT1632LEDMatrix::clearScreen() {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].clearScreen();
  }
}

void HT1632LEDMatrix::fillScreen() {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].fillScreen();
  }
}

void HT1632LEDMatrix::setBrightness(uint8_t b) {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].setBrightness(b);
  }
}

void HT1632LEDMatrix::blink(boolean b) {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].blink(b);
  }
}

void HT1632LEDMatrix::writeScreen() {
  for (uint8_t i=0; i<matrixNum; i++) {
    matrices[i].writeScreen();
  }
}

// bresenham's algorithm - thx wikpedia
void HT1632LEDMatrix::drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, 
		      uint8_t color) {
  uint16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// draw a rectangle
void HT1632LEDMatrix::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  drawLine(x, y, x+w-1, y, color);
  drawLine(x, y+h-1, x+w-1, y+h-1, color);

  drawLine(x, y, x, y+h-1, color);
  drawLine(x+w-1, y, x+w-1, y+h-1, color);
}

// fill a rectangle
void HT1632LEDMatrix::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      drawPixel(i, j, color);
    }
  }
}



// draw a circle outline
void HT1632LEDMatrix::drawCircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    
  }
}


// fill a circle
void HT1632LEDMatrix::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawLine(x0, y0-r, x0, y0+r+1, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawLine(x0+x, y0-y, x0+x, y0+y+1, color);
    drawLine(x0-x, y0-y, x0-x, y0+y+1, color);
    drawLine(x0+y, y0-x, x0+y, y0+x+1, color);
    drawLine(x0-y, y0-x, x0-y, y0+x+1, color);
  }
}

void HT1632LEDMatrix::setCursor(uint8_t x, uint8_t y) {
  cursor_x = x; 
  cursor_y = y;
}

void HT1632LEDMatrix::setTextSize(uint8_t s) {
  textsize = s;
}

void HT1632LEDMatrix::setTextColor(uint8_t c) {
  textcolor = c;
}

#if ARDUINO >= 100
size_t HT1632LEDMatrix::write(uint8_t c) {
#else
void HT1632LEDMatrix::write(uint8_t c) {
#endif
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textsize);
    cursor_x += textsize*6;
  }
#if ARDUINO >= 100
  return 1;
#endif
}


// draw a character
void HT1632LEDMatrix::drawChar(uint8_t x, uint8_t y, char c, 
			      uint16_t color, uint8_t size) {
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  drawPixel(x+i, y+j, color);
	else {  // big size
	  fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}


void HT1632LEDMatrix::drawBitmap(uint8_t x, uint8_t y, 
			const uint8_t *bitmap, uint8_t w, uint8_t h,
			uint8_t color) {
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
	drawPixel(x+i, y+j, color);
      }
    }
  }
}

uint8_t HT1632LEDMatrix::getPixelInBuffer(uint16_t i) {
  return _rightBuffer[i/8] & _BV(i%8);
}

void HT1632LEDMatrix::setPixelInBuffer(uint16_t i) {
  _rightBuffer[i/8] |= _BV(i%8); 
}

void HT1632LEDMatrix::clrPixelInBuffer(uint16_t i) {
  _rightBuffer[i/8] &= ~_BV(i%8); 
}

void HT1632LEDMatrix::shiftBufferLeft() {
  // buffer is made up of 32 bytes representing the pixels
  for (uint16_t byte = 0; byte < 32; byte++) {
    // shift each byte left by one bit
    _rightBuffer[byte] = _rightBuffer[byte] << 1;
    if (byte <= 15) {
      // bytes numbered <= 15 in the buffer are in the left column
      // nextByte is the byte to the right of the current byte
      uint16_t nextByte = _rightBuffer[byte + 16];
      // grab the most significant bit (i.e. leftmost bit of that byte) and
      // copy it into the least significant bit (rightmost bit) of the current byte
      bool msb = nextByte & (1 << 7);
      if (msb) {
	_rightBuffer[byte] |= (1 << 0);
      }
      else {
	_rightBuffer[byte] &= ~(1 << 0);
      }
    }
  }
}


//////////////////////////////////////////////////////////////////////////


HT1632::HT1632(int8_t data, int8_t wr, int8_t cs, int8_t rd) {
  _data = data;
  _wr = wr;
  _cs = cs;
  _rd = rd;

  for (uint8_t i=0; i<48; i++) {
    ledmatrix[i] = 0;
  }
}

void HT1632::begin(uint8_t type) {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH);
  pinMode(_data, OUTPUT);
  
  if (_rd >= 0) {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
  }

  sendcommand(HT1632_SYS_EN);
  sendcommand(HT1632_LED_ON);
  sendcommand(HT1632_BLINK_OFF);
  sendcommand(HT1632_MASTER_MODE);
  sendcommand(HT1632_INT_RC);
  sendcommand(type);
  sendcommand(HT1632_PWM_CONTROL | 0xF);
  
  WIDTH = 24;
  HEIGHT = 16;
}

void HT1632::setBrightness(uint8_t pwm) {
  if (pwm > 15) pwm = 15;
  sendcommand(HT1632_PWM_CONTROL | pwm);
}

void HT1632::blink(boolean blinky) {
  if (blinky) 
    sendcommand(HT1632_BLINK_ON);
  else
    sendcommand(HT1632_BLINK_OFF);
}

void HT1632::setPixel(uint16_t i) {
  ledmatrix[i/8] |= _BV(i%8); 
}

void HT1632::clrPixel(uint16_t i) {
  ledmatrix[i/8] &= ~_BV(i%8); 
}

uint8_t HT1632::getPixel(uint16_t i) {
  return ledmatrix[i/8] & _BV(i%8);
}

void HT1632::shiftLeft() {
  // note that each panel is made up of 48 bytes representing the pixels
  // 8 bits (pixels) to a byte equals 48 * 8 = 384 pixels per panel
  for (uint16_t byte = 0; byte < 48; byte++) {
    // shift each byte left by one bit
    ledmatrix[byte] = ledmatrix[byte] << 1;
    if (byte <= 31) {
      // bytes numbered <= 31 in the panel are in the middle and left columns
      // nextByte is the byte to the right of the current byte (in the same panel)
      uint16_t nextByte = ledmatrix[byte + 16];
      // grab the most significant bit (i.e. leftmost bit of that byte) and
      // copy it into the least significant bit (rightmost bit) of the current byte
      bool msb = nextByte & (1 << 7);
      if (msb) {
	ledmatrix[byte] |= (1 << 0);
      }
      else {
	ledmatrix[byte] &= ~(1 << 0);
      }
    }
    // note that the rightmost pixel of the panel is not set here because we
    // don't know about the state of the neighboring panels. That pixel will
    // be set in HT1632LEDMatrix::shiftLeft()
  }
}

void HT1632::dumpScreen() {
  Serial.println("---------------------------------------");

  for (uint16_t i=0; i<(WIDTH*HEIGHT/8); i++) {
    Serial.print("0x");
    Serial.print(ledmatrix[i], HEX);
    Serial.print(" ");
    if (i % 3 == 2) Serial.println();
  }

  Serial.println("\n---------------------------------------");
}

void HT1632::writeScreen() {

  digitalWrite(_cs, LOW);

  writedata(HT1632_WRITE, 3);
  // send with address 0
  writedata(0, 7);

  for (uint16_t i=0; i<(WIDTH*HEIGHT/8); i+=2) {
    uint16_t d = ledmatrix[i];
    d <<= 8;
    d |= ledmatrix[i+1];

    writedata(d, 16);
  }
  digitalWrite(_cs, HIGH);
}


void HT1632::clearScreen() {
  for (uint8_t i=0; i<(WIDTH*HEIGHT/8); i++) {
    ledmatrix[i] = 0;
  }
  writeScreen();
}


void HT1632::writedata(uint16_t d, uint8_t bits) {
  pinMode(_data, OUTPUT);
  for (uint8_t i=bits; i > 0; i--) {
    digitalWrite(_wr, LOW);
   if (d & _BV(i-1)) {
     digitalWrite(_data, HIGH);
   } else {
     digitalWrite(_data, LOW);
   }
  digitalWrite(_wr, HIGH);
  }
  pinMode(_data, INPUT);
}




void HT1632::writeRAM(uint8_t addr, uint8_t data) {
  //Serial.print("Writing 0x"); Serial.print(data&0xF, HEX);
  //Serial.print(" to 0x"); Serial.println(addr & 0x7F, HEX);

  uint16_t d = HT1632_WRITE;
  d <<= 7;
  d |= addr & 0x7F;
  d <<= 4;
  d |= data & 0xF;
 
  digitalWrite(_cs, LOW);
  writedata(d, 14);
  digitalWrite(_cs, HIGH);
}


void HT1632::sendcommand(uint8_t cmd) {
  uint16_t data = 0;
  data = HT1632_COMMAND;
  data <<= 8;
  data |= cmd;
  data <<= 1;
  
  digitalWrite(_cs, LOW);
  writedata(data, 12);
  digitalWrite(_cs, HIGH);  
}


void HT1632::fillScreen() {
  for (uint8_t i=0; i<(WIDTH*HEIGHT/8); i++) {
    ledmatrix[i] = 0xFF;
  }
  writeScreen();
}
