/*
 * File:   SH1106_driver.c
 * Author: Pete
 *
 * Created on June 25, 2019, 3:06 PM
 * 
 * @VERSION 2.1
 */

#include "SH1106_driver.h"
#include <stdlib.h>

/* The X and Y positions for the cursor position*/
uint8_t xCursor, yCursor = 0;

/* Default scalings for the font size */
uint8_t xTextSize = 1, yTextSize = 1;

/* Default font color */
uint8_t textcolor = WHITE;

/* Default Font */
GFXfont *gfxFont;

/* Macro to swap two 8 bit integers */
#define _swap_int8_t(a, b){ int16_t t = a; a = b; b = t; }

/* Macros to read stuff */
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

inline GFXglyph * pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c){
    return &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
}

inline uint8_t * pgm_read_bitmap_ptr(const GFXfont *gfxFont){
    return (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
}

static void SH1106_drawCircleFillHelper(uint8_t, uint8_t, uint8_t, uint8_t, int8_t, uint8_t);
static void SH1106_drawChar(int8_t, int8_t, unsigned char, uint8_t, uint8_t, uint8_t);

/* Buffer loaded with the Mike Borish TM startup image */
static uint8_t buffer[SH1106_OLED_HEIGHT * SH1106_OLED_WIDTH / 8] = { 
0x84, 0x00, 0x04, 0x00, 0x00, 0x80, 0x00, 0x0C, 0x00, 0x00, 
0x80, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
0x03, 0xFF, 0x00, 0x00, 0x08, 0x00, 0x00, 0xFF, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 
0x1F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 
0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x08, 
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x01, 0x10, 0x00, 
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x3F, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xE0, 0xFC, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 
0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 
0x00, 0x30, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x20, 
0x30, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0xC0, 
0x84, 0x07, 0x07, 0x0F, 0x0F, 0x8F, 0x9F, 0x1F, 0xFF, 0xFF, 
0xFF, 0xFF, 0x9F, 0x0F, 0x0F, 0x2F, 0x2F, 0x3F, 0x3F, 0x3F, 
0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xE6, 0x06, 0x00, 0x00, 
0x00, 0x00, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0x00, 0x00, 0x30, 0x20, 0x00, 0x00, 0x20, 0x20, 
0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x20, 0x20, 0x00, 
0x00, 0x02, 0x02, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 
0xF3, 0x00, 0x00, 0x00, 0x08, 0x0C, 0xFF, 0x00, 0x00, 0x00, 
0x00, 0x08, 0xFF, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xE6, 0xFF, 0xFD, 
0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x20, 0x30, 0x00, 0x00, 
0x00, 0x30, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x02, 0x86, 0x59, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x3F, 0xFF, 0xFF, 0xEF, 
0xD7, 0x93, 0x98, 0x98, 0x9C, 0x94, 0x9E, 0x9E, 0x97, 0xBF, 
0xB7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 
0x7F, 0x1F, 0xF8, 0x3F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x0F, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x3F, 
0x04, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x7F, 0xFF, 0xFF, 
0xFF, 0xF3, 0xF3, 0xF3, 0xF3, 0xFB, 0xFF, 0xFF, 0xFF, 0x7F, 
0x7F, 0x1F, 0x83, 0x60, 0x01, 0x80, 0xC0, 0xE0, 0xF8, 0xF8, 
0xFB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x20, 0xE0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x20, 
0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x20, 0x00, 0x00, 0xC0, 0xE0, 0xC0, 0x81, 0x81, 
0x81, 0x81, 0xC1, 0xC1, 0xC1, 0xE1, 0xE4, 0xF2, 0xF3, 0xF9, 
0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFC, 0xF8, 0xF0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF
};

/* @NAME: SH1106_init
 *
 * @DESCRIPTION: Initializes the SH1106 driver by allowing the user to pass in
 *               parameters of the VCC state (internal or external) and the slave 
 *               device's address. It then runs through the necessary initializing 
 *               sequence for the driver.
 * 
 * @PARAM:
 *          vccstate: the user can choose between the defines: SH1106_EXTERNALVCC
 *                    or SH1106_SWITCHCAPVCC. The first, SH1106_EXTERNALVCC, designates 
 *                    an external power source to the driver, such as from the ATMega4809's
 *                    VBUS pin. The second, SH1106_SWITCHCAPVCC, allows the user to use an
 *                    internal voltage generator.
 * 
 */
void SH1106_init(uint8_t vccstate) {
  _vccstate = vccstate;

    // Init sequence for 128x64 OLED module
    SH1106_sendCommand(SH1106_DISPLAYOFF);                    // 0xAE
    SH1106_sendCommand(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
    SH1106_sendCommand(0x80);                                  // the suggested ratio 0x80
    SH1106_sendCommand(SH1106_SETMULTIPLEX);                  // 0xA8
    SH1106_sendCommand(0x3F);
    SH1106_sendCommand(SH1106_SETDISPLAYOFFSET);              // 0xD3
    SH1106_sendCommand(0x00);                                   // no offset
	
    SH1106_sendCommand(SH1106_SETSTARTLINE | 0x0);            // line #0 0x40
    SH1106_sendCommand(SH1106_CHARGEPUMP);                    // 0x8D
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_sendCommand(0x10); }
    else 
      { SH1106_sendCommand(0x14); }
    SH1106_sendCommand(SH1106_MEMORYMODE);                    // 0x20
    SH1106_sendCommand(0x00);                                  // 0x0 act like ks0108
    SH1106_sendCommand(SH1106_SEGREMAP | 0x1);
    SH1106_sendCommand(SH1106_COMSCANDEC);
    SH1106_sendCommand(SH1106_SETCOMPINS);                    // 0xDA
    SH1106_sendCommand(0x12);
    SH1106_sendCommand(SH1106_SETCONTRAST);                   // 0x81
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_sendCommand(0x9F); }
    else 
      { SH1106_sendCommand(0xCF); }
    SH1106_sendCommand(SH1106_SETPRECHARGE);                  // 0xd9
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_sendCommand(0x22); }
    else 
      { SH1106_sendCommand(0xF1); }
    SH1106_sendCommand(SH1106_SETVCOMDETECT);                 // 0xDB
    SH1106_sendCommand(0x40);
    SH1106_sendCommand(SH1106_DISPLAYALLON_RESUME);           // 0xA4
    SH1106_sendCommand(SH1106_NORMALDISPLAY);                 // 0xA6

  SH1106_sendCommand(SH1106_DISPLAYON);//--turn on oled panel
}

/* @NAME: SH1106_sendCommand
 *
 * @DESCRIPTION: Allows the user to send a command to the SH1106 driver by passing in the
 *               command to be sent
 * 
 * @PARAM:
 *          command: the command to be sent to the SH1106 driver
 */
void SH1106_sendCommand(uint8_t command){
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    uint8_t writeBuffer[2];
    writeBuffer[0] = control;
    writeBuffer[1] = command;
    TWI0_start(SLAVEADDR, 0, writeBuffer, 2);
    while(TWI0_getResult() != TWI_RESULT_OK){;}
}

/* @NAME: SH1106_drawPixel
 *
 * @DESCRIPTION: Allows the User to draw a pixel to the display by specifying an X 
 *               coordinate, a Y coordinate, and a color (Black, White, or Inverse 
 *               which inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x: the x coordinate to draw the pixel to
 * 
 *          y: the y coordinate to draw the pixel to
 * 
 *          color: the color of the pixel. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawPixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x < 0) || (x >= SH1106_OLED_WIDTH) || (y < 0) || (y >= SH1106_OLED_HEIGHT))
    return;

  // x is which column
    switch (color) 
    {
      case WHITE:   buffer[x+ (y/8)*SH1106_OLED_WIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*SH1106_OLED_WIDTH] &= ~(1 << (y&7)); break; 
      case INVERSE: buffer[x+ (y/8)*SH1106_OLED_WIDTH] ^=  (1 << (y&7)); break; 
    } 
}

/* @NAME: SH1106_clearDisplay
 *
 * @DESCRIPTION: Allows the user to clear the display by drawing all of the pixels black. The 
 *               SH1106_display function must be called after this to display the cleared screen.
 */
void SH1106_clearDisplay(void){ 
    uint8_t height = 64;
	uint8_t width = 132; 
	uint8_t i, j = 0;
    
    for ( i = 0; i < height; i++) {
		for(j = 0; j < width; j++){
            SH1106_drawPixel(j, i, BLACK);
        }
	}
}

/* @NAME: SH1106_display
 *
 * @DESCRIPTION: Displays the screen of pixels by cycling through all of the steps in the buffer
 */
void SH1106_display(void){
    SH1106_sendCommand(SH1106_SETLOWCOLUMN | 0x0);  // low col = 0
    SH1106_sendCommand(SH1106_SETHIGHCOLUMN | 0x0);  // hi col = 0
    SH1106_sendCommand(SH1106_SETSTARTLINE | 0x0); // line #0
    
	uint8_t width=(SH1106_OLED_WIDTH + 4) >> 3;
    
    
    for (int8_t i = (SH1106_OLED_HEIGHT/8)-1; i >= 0; i--) {
		
		// send a bunch of data in one xmission
        SH1106_sendCommand(0xB0 + i);//set page address
        SH1106_sendCommand(SH1106_SETLOWCOLUMN);//set lower column address
        SH1106_sendCommand(SH1106_SETHIGHCOLUMN);//set higher column address
		
        for(uint16_t j = SH1106_OLED_WIDTH; j > 0; j--){
            uint8_t writeBuffer[width + 1];
			writeBuffer[0] = 0x40;
            for (uint8_t x=0; x<16; x++) {
                writeBuffer[x+1] = buffer[i*SH1106_OLED_WIDTH+SH1106_OLED_WIDTH-j];
                j--;
            }
            j++;
            TWI0_start(SLAVEADDR, 0, writeBuffer, width + 1);
            while(TWI0_getResult() != TWI_RESULT_OK){;}
        }
	}
}

/* @NAME: SH1106_drawLine
 *
 * @DESCRIPTION: Allows the User to draw a line to the display by specifying starting
 *               X and Y points, ending X and Y points, and a color (Black, White, or 
 *               Inverse which inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the line from
 * 
 *          y0: the starting y coordinate to draw the line from
 * 
 *          x1: the ending x coordinate to draw the line to
 * 
 *          y1: the ending y coordinate to draw the line to
 * 
 *          color: the color of the line. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color){
    /* steep: True if the slope of the line is greater than 1 */
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int8_t(x0, y0);
        _swap_int8_t(x1, y1);
    }
    /* Draws from left to right, so if the user specifies the starting x location 
     * to be to the right of the ending x location, it swaps them */
    if (x0 > x1) {
        _swap_int8_t(x0, x1);
        _swap_int8_t(y0, y1);
    }
    
    /* dx: change in x location
     * dy: change in y location*/
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;
    /* If the y starting point is less than the y end point, the step is positive
     * Otherwise it is negative. */
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            SH1106_drawPixel(y0, x0, color);
        } else {
            SH1106_drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/*@NAME: SH1106_drawVLine
 * 
 * @DESCRIPTION: draws a vertical line
 * 
 * @PARAM:
 *          x: the x-position of the line
 * 
 *          y0: the starting y position of the line
 * 
 *          y1: the ending y position of the line
 * 
 *          color: the color of the line. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawVLine(uint8_t x, uint8_t y0, uint8_t y1, uint8_t color){
    SH1106_drawLine(x, y0, x, y1, color);
}


/* @NAME: SH1106_drawHLine
 * 
 * @DESCRIPTION: draws a horizontal line
 * 
 * @PARAM:
 *          x0: starting x position
 * 
 *          x1: ending x position
 * 
 *          y: y position of the line
 * 
 *          color: the color of the line. Use either of the BLACK, WHITE, or INVERSE defines here.
 * 
 */
void SH1106_drawHLine(uint8_t x0, uint8_t y,  uint8_t x1, uint8_t color){
    SH1106_drawLine(x0, y, x1, y, color);
}

/* @NAME: SH1106_drawRect
 * 
 * @DESCRIPTION: draws the outline of a rectangle
 * 
 *  @PARAM:
 *          x0: starting x position
 * 
 *          y0: starting y position
 * 
 *          x1: ending x position
 * 
 *          y1: ending y position
 * 
 *          color: the color of the line. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color){
    SH1106_drawVLine(x0, y0, y1, color);
    SH1106_drawVLine(x1, y0, y1, color);
    SH1106_drawHLine(x0, y0, x1, color);
    SH1106_drawHLine(x0, y1, x1, color);
}

/* @NAME: SH1106_drawRectFill
 * 
 * @DESCRIPTION: draws a completely filled in rectangle
 * 
 * @PARAM:
 *          x0: starting x position
 * 
 *          y0: starting y position
 * 
 *          x1: ending x position
 * 
 *          y1: ending y position
 * 
 *          color: the color of the rectangle. Use either of the BLACK, WHITE, or INVERSE defines here.
 * 
 */
void SH1106_drawRectFill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color){
    
    //finds which of the x's are higher
    uint8_t xHigh, xLow;
    
    if(x0 > x1){
        xHigh = x0;
        xLow = x1;
    }
    else{
        xHigh = x1;
        xLow = x0;
    }
    
    for(uint8_t xPos = xLow; xPos <= xHigh; xPos++){
        SH1106_drawVLine(xPos, y0, y1, color);
    }
}

/* @NAME: SH1106_drawCircle
 *
 * @DESCRIPTION: Allows the User to draw a Circle outline to the display by specifying starting
 *               X and Y points, a radius, and a color (Black, White, or Inverse which
 *               inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the circle at
 * 
 *          y0: the starting y coordinate to draw the circle at
 * 
 *          r: the radius of the circle
 * 
 *          color: the color of the circle. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {

    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;


    SH1106_drawPixel(x0  , y0+r, color);
    SH1106_drawPixel(x0  , y0-r, color);
    SH1106_drawPixel(x0+r, y0  , color);
    SH1106_drawPixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SH1106_drawPixel(x0 + x, y0 + y, color);
        SH1106_drawPixel(x0 - x, y0 + y, color);
        SH1106_drawPixel(x0 + x, y0 - y, color);
        SH1106_drawPixel(x0 - x, y0 - y, color);
        SH1106_drawPixel(x0 + y, y0 + x, color);
        SH1106_drawPixel(x0 - y, y0 + x, color);
        SH1106_drawPixel(x0 + y, y0 - x, color);
        SH1106_drawPixel(x0 - y, y0 - x, color);
    }
}
/* @NAME: SH1106_drawCircleFill
 *
 * @DESCRIPTION: Allows the User to draw a Circle Fill to the display by specifying starting
 *               X and Y points, a radius, and a color (Black, White, or Inverse which
 *               inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the circle at
 * 
 *          y0: the starting y coordinate to draw the circle at
 * 
 *          r: the radius of the circle
 * 
 *          color: the color of the circle. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawCircleFill(uint8_t x0, uint8_t y0, uint8_t r,uint8_t color) {
    SH1106_drawVLine(x0, y0-r, y0-r+2*r+1, color);
    SH1106_drawCircleFillHelper(x0, y0, r, 3, 0, color);
}

/* @NAME: SH1106_drawCircleFillHelper
 *
 * @DESCRIPTION: Helper function for drawCircleFill
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the circle at
 * 
 *          y0: the starting y coordinate to draw the circle at
 * 
 *          r: the radius of the circle
 * 
 *          corners: Mask bits indicating which quarters we're doing
 *          
 *          delta: Offset from center-point
 * 
 *          color: the color of the circle. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
static void SH1106_drawCircleFillHelper(uint8_t x0, uint8_t y0, uint8_t r, uint8_t corners, int8_t delta, uint8_t color) {
    int8_t f     = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x     = 0;
    int8_t y     = r;
    int8_t px    = x;
    int8_t py    = y;
    delta++; // Avoid some +1's in the loop
    while(x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if(x < (y + 1)) {
            if(corners & 1) SH1106_drawVLine(x0+x, y0-y, y0-y+2*y+delta, color);
            if(corners & 2) SH1106_drawVLine(x0-x, y0-y, y0-y+2*y+delta, color);
        }
        if(y != py) {
            if(corners & 1) SH1106_drawVLine(x0+py, y0-px, y0-px+2*px+delta, color);
            if(corners & 2) SH1106_drawVLine(x0-py, y0-px, y0-px+2*px+delta, color);
            py = y;
        }
        px = x;
    }
}

/* @NAME: SH1106_drawTriangle
 *
 * @DESCRIPTION: Allows the User to draw a Triangle outline to the display by specifying the
 *               X and Y points for the vertices, and a color (Black, White, or Inverse which
 *               inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the Triangle at
 * 
 *          y0: the starting y coordinate to draw the Triangle at
 * 
 *          x1: the x coordinate of the second vertex to draw the Triangle at
 * 
 *          y1: the y coordinate of the second vertex to draw the Triangle at
 * 
 *          x2: the x coordinate of the third vertex to draw the Triangle at
 * 
 *          y2: the y coordinate of the third vertex to draw the Triangle at
 * 
 *          color: the color of the circle. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawTriangle(int8_t x0, int8_t y0,
        int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color) {
    SH1106_drawLine(x0, y0, x1, y1, color);
    SH1106_drawLine(x1, y1, x2, y2, color);
    SH1106_drawLine(x2, y2, x0, y0, color);
}

/* @NAME: SH1106_drawTriangleFill
 *
 * @DESCRIPTION: Allows the User to draw a Filled Triangle to the display by specifying the
 *               X and Y points for the vertices, and a color (Black, White, or Inverse which
 *               inverts the current color of the pixel). The SH1106_display
 *               function must be called after the pixels are drawn to display them.
 * 
 * @PARAM:
 *          x0: the starting x coordinate to draw the Triangle at
 * 
 *          y0: the starting y coordinate to draw the Triangle at
 * 
 *          x1: the x coordinate of the second vertice to draw the Triangle at
 * 
 *          y1: the y coordinate of the second vertice to draw the Triangle at
 * 
 *          x2: the x coordinate of the third vertice to draw the Triangle at
 * 
 *          y2: the y coordinate of the third vertice to draw the Triangle at
 * 
 *          color: the color of the circle. Use either of the BLACK, WHITE, or INVERSE defines here.
 */
void SH1106_drawTriangleFill(int8_t x0, int8_t y0,
        int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int8_t(y0, y1); _swap_int8_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int8_t(y2, y1); _swap_int8_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int8_t(y0, y1); _swap_int8_t(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        SH1106_drawHLine(a, y0, b, color); /* changed here */
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int8_t(a,b);
        SH1106_drawHLine(a, y, b, color); /* changed here */
    }


    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int8_t(a,b);
        SH1106_drawHLine(a, y, b, color);
    }
}

/* @NAME: SH1106_setFont
 * 
 * @DESCRIPTION: Sets the font style. Must be set in order to write chars.
 * 
 * @PARAM:
 *          *font = pointer to where the font is stored in program memory
 */
void SH1106_setFont(const GFXfont *font) {
    gfxFont = (GFXfont *)font;
}

/* @NAME: SH1106_setTextSize
 * 
 * @DESCRIPTION: sets the text size scale. Default is 1. Must be integer value
 * 
 * @PARAM:
 *          scale: the scale to set the font
 */
void SH1106_setTextSize(uint8_t scale){
    xTextSize = scale;
    yTextSize = scale;
}

/* @NAME: SH1106_setCursor
 * 
 * @DESCRIPTION: Sets the cursor position. Should be the bottom left of the character.
 * 
 * @PARAM:
 *          x: x position
 *          y: y position
 */
void SH1106_setCursor(uint8_t x, uint8_t y){
    xCursor = x;
    yCursor = y;
}

/* @NAME: SH1106_getCursorX
 * 
 * @DESCRIPTION: Returns the x position of the cursor
 */
uint8_t SH1106_getCursorX(){
    return xCursor;
}

/* @NAME: SH1106_getCursorY
 * 
 * @DESCRIPTION: Returns the y position of the cursor
 */
uint8_t SH1106_getCursorY(){
    return yCursor;
}

/* @NAME: SH1106_drawChar
 * 
 * @DESCRIPTION: Draws the character to the screen. Should only be called by writeChar.
 * 
 * @PARAM:
 *          x: x position of the character
 *          y: y position of the character
 *          c: character to be written
 *          color: color to be used (BLACK or WHITE)
 *          size_x: Size of character in x direction
 *          size_y: Size of character in y direction
 */
static void SH1106_drawChar(int8_t x, int8_t y, unsigned char c,
  uint8_t color, uint8_t size_x, uint8_t size_y) {
    
    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph  = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t  *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t  w  = pgm_read_byte(&glyph->width),
             h  = pgm_read_byte(&glyph->height);
    int8_t   xo = pgm_read_byte(&glyph->xOffset),
             yo = pgm_read_byte(&glyph->yOffset);
    uint8_t  xx, yy, bits = 0, bit = 0;
    int8_t  xo16 = 0, yo16 = 0;

    if(size_x > 1 || size_y > 1) {
        xo16 = xo;
        yo16 = yo;
    }
    for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
            if(!(bit++ & 7)) {
                bits = pgm_read_byte(&bitmap[bo++]);
            }
            if(bits & 0x80) {
                if(size_x == 1 && size_y == 1) {
                    SH1106_drawPixel(x+xo+xx, y+yo+yy, color);
                } else {
                    SH1106_drawRectFill(x+(xo16+xx)*size_x, y+(yo16+yy)*size_y,
                      size_x + (x +(xo16+xx)*size_x), size_y + (y+(yo16+yy)*size_y), color);
                }
            }
            bits <<= 1;
        }
    }
}

/* @NAME: SH1106_writeChar
 * 
 * @DESCRIPTION: Handles the filtering and cursor position when writing a char. Calls SH1106_drawChar()
 * 
 * @PARAM:
 *          c: the char to be drawn
 */
void SH1106_writeChar(uint8_t c) {
    if(c == '\n') {
        SH1106_setCursor(0, yCursor + (int8_t)yTextSize *(uint8_t)pgm_read_byte(&gfxFont->yAdvance));
    } else if(c != '\r') {
        uint8_t first = pgm_read_byte(&gfxFont->first);
        if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
            GFXglyph *glyph  = pgm_read_glyph_ptr(gfxFont, c - first);
            uint8_t   w     = pgm_read_byte(&glyph->width),
                      h     = pgm_read_byte(&glyph->height);
            if((w > 0) && (h > 0)) { // Is there an associated bitmap?
                int8_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
                //allows the text to wrap (NOTE: not intelligent)
                if(((xCursor + xTextSize * (xo + w)) > SH1106_OLED_WIDTH)) {
                        xCursor  = 0;
                        yCursor += (int8_t)yTextSize *
                          (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                    }

                SH1106_drawChar(xCursor, yCursor, c, textcolor, xTextSize, yTextSize);
            }
            xCursor += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int8_t)xTextSize;
        }
    }
}

/* @NAME: SH1106_writeString
 * 
 * @DESCRIPTION: Allows the user to write a string to the OLED. Calls SH1106_writeChar();
 * 
 * @PARAM:
 *          *str: string to be written out
 */
void SH1106_writeString(char *str){
    for(size_t i = 0; i < strlen(str); i++){
        SH1106_writeChar(str[i]);
    }
}
