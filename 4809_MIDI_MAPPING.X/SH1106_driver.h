/* 
 * File:   I2C_Master.h
 * Author: austin
 *
 * Created on June 13, 2019, 12:33 PM
 * 
 * @VERSION: 2.1
 * 
 * @NOTE: use i2c Master 2.1(oled) as speed has been optimized to 400khz
 */

#ifndef SH1106_DRIVER_H
#define	SH1106_DRIVER_H

#include <avr/io.h>
#include <string.h>
#include "I2C_Master.h"
#include <avr/pgmspace.h>

/* Parameter used in the SH1106_init function for 'if' statements */
uint8_t _vccstate;

/* SH1106 driver slave address */
#define SLAVEADDR 0x78>>1

/* width and Height of the OLED display in pixels */
#define SH1106_OLED_WIDTH    128
#define SH1106_OLED_HEIGHT    64

/* Allow the user to write a color into the parmeters of the corresponding functions */
#define WHITE 0x00
#define BLACK 0x01 
#define INVERSE 0x02

/* Hex values coresponding to the various commands one can send to the driver */
#define SH1106_SETCONTRAST 0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON 0xA5
#define SH1106_NORMALDISPLAY 0xA6
#define SH1106_INVERTDISPLAY 0xA7
#define SH1106_DISPLAYOFF 0xAE
#define SH1106_DISPLAYON 0xAF

#define SH1106_SETDISPLAYOFFSET 0xD3
#define SH1106_SETCOMPINS 0xDA

#define SH1106_SETVCOMDETECT 0xDB

#define SH1106_SETDISPLAYCLOCKDIV 0xD5
#define SH1106_SETPRECHARGE 0xD9

#define SH1106_SETMULTIPLEX 0xA8

#define SH1106_SETLOWCOLUMN 0x02 //Previously 0x00
#define SH1106_SETHIGHCOLUMN 0x10

#define SH1106_SETSTARTLINE 0x40

#define SH1106_MEMORYMODE 0x20
#define SH1106_COLUMNADDR 0x21
#define SH1106_PAGEADDR   0x22

#define SH1106_COMSCANINC 0xC0
#define SH1106_COMSCANDEC 0xC8

#define SH1106_SEGREMAP 0xA0

#define SH1106_CHARGEPUMP 0x8D

#define SH1106_EXTERNALVCC 0x1
#define SH1106_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SH1106_ACTIVATE_SCROLL 0x2F
#define SH1106_DEACTIVATE_SCROLL 0x2E
#define SH1106_SET_VERTICAL_SCROLL_AREA 0xA3
#define SH1106_RIGHT_HORIZONTAL_SCROLL 0x26
#define SH1106_LEFT_HORIZONTAL_SCROLL 0x27
#define SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

/// Font data stored PER GLYPH
typedef struct {
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
        uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	int8_t   xOffset;          ///< X dist from cursor pos to UL corner
        int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct { 
	uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;       ///< Glyph array
	uint8_t   first;       ///< ASCII extents (first char)
        uint8_t   last;        ///< ASCII extents (last char)
	uint8_t   yAdvance;    ///< Newline distance (y axis)
} GFXfont;


void SH1106_init(uint8_t);
void SH1106_sendCommand(uint8_t);
void SH1106_drawPixel(uint8_t,  uint8_t, uint8_t);
void SH1106_clearDisplay(void);
void SH1106_display(void);
void SH1106_drawLine(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void SH1106_drawVLine(uint8_t, uint8_t, uint8_t, uint8_t);
void SH1106_drawHLine(uint8_t, uint8_t,  uint8_t, uint8_t);
void SH1106_drawRect(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void SH1106_drawRectFill(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void SH1106_drawCircle(uint8_t, uint8_t, uint8_t, uint8_t);
void SH1106_drawCircleFill(uint8_t, uint8_t, uint8_t,uint8_t);
void SH1106_drawTriangle(int8_t, int8_t, int8_t, int8_t, int8_t, int8_t, uint8_t);
void SH1106_drawTriangleFill(int8_t, int8_t, int8_t, int8_t, int8_t, int8_t, uint8_t);
void SH1106_setFont(const GFXfont *f);
void SH1106_setTextSize(uint8_t);
void SH1106_setCursor(uint8_t, uint8_t);
uint8_t SH1106_getCursorX();
uint8_t SH1106_getCursorY();
void SH1106_writeChar(uint8_t);
void SH1106_writeString(char *str);
#endif	/* SH1106_DRIVER_H */
