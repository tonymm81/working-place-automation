/**
 * This is a simple library for SSH1106 controlled graphic LCD's. 
 * Written for a 1.3" OLED GLCD
 * See      http://www.banggood.com/1_3-Inch-7Pin-White-OLED-12864-SPI-Interface-LCD-Display-Module-For-Arduino-p-1067872.html
 *
 * Written by:  Erik van de Coevering
 * With thanks to Tim Barr from whom I've reused some code
 * Use this code in whatever way you like, as long as it stays free of charge!
 */

#ifndef SSH1106_H
#define SSH1106_H

#include "mbed.h"
#include "font_4x5.h"
#include "font_5x8.h"
#include "font_6x6.h"
#include "font_6x8.h"
#include "font_7x7.h"
#include "font_8x8.h"
#include "font_8x8_1.h"
#include "bold_font.h"
#include "font2d_hunter.h"
#include "font2d_formplex12.h"
#include "biohazard.h"
#include "highvoltage.h"
#include "einstein.h"
#include "test.h"
#include "copter.h"

#define LCDWIDTH 128
#define LCDHEIGHT 64
#define LCDPAGES 8

class SSH1106
{
public:

    // Constructor
    SSH1106(SPI &spi, DigitalOut &lcd_cs, DigitalOut &cd, DigitalOut &rst);

    // Initialize LCD
    void init(void);
    
    // Set contrast (0 - 63), initialized to 40
    void setContrast(char contrast);

    // Place cursor at position
    void setCursor(char column, char line);

    // Clear screen
    void clear(void);

    // Write text to LCD where font format is a 2-dimensional array (only 96x8 byte, 8x8 pixel fonts supported)
    void writeText2d(char column, char page, const char font_address[96][8], const char *text, int size);

    // Write text to LCD where font format is a 1-dimensional array. >8 pixel fonts are not working yet, will update soon
    void writeText(char column, char page, const char *font_address, const char *str, const uint8_t size);

    // Draw a 128x64 pixel bitmap
    void drawBitmap(const char *data);
    
    // Draw a horizontal line, start positions / height / width in pixels
    void drawLineHor(char posx, char posy, char height, char width);
    
    // Draw a vertical line, start positions / height / width in pixels
    void drawLineVert(char posx, char posy, char height, char width);
    
    //-----------------------------------------------------------------------------------------------------------------------------
    // Functions below are buffered versions; possible to write things on top of each other without clearing pixels (ORs all data).
    // Use update() to write buffer to LCD.
    // Clear buffer before writing 1st time (initialize all values)
    // writetext / drawbitmap will be added soon
    //-----------------------------------------------------------------------------------------------------------------------------
    
    // Clear buffer
    void clearBuffer(void);
    
    // Write buffer to LCD
    void update(void);
    
    // Draw a horizontal line, start positions / height / width in pixels. Buffered version; possible to write things on top of each other
    // without clearing pixels (ORs all data). Use update() to write buffer to LCD
    void drawbufferLineHor(char posx, char posy, char height, char width);
    
    // Draw a vertical line, start positions / height / width in pixels. Buffered version.
    void drawbufferLineVert(char posx, char posy, char height, char width);

private:

    SPI         *_lcd;
    DigitalOut  *_lcd_cs;
    DigitalOut  *_lcd_cd;
    DigitalOut   *_lcd_rst;
    uint8_t     _lcdbuffer[LCDWIDTH*LCDPAGES];
    char        buff[LCDWIDTH*LCDPAGES];

};

#endif
