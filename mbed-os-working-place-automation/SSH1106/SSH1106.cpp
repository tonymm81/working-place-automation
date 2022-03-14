/**
 * This is a simple library for SSH1106 controlled graphic LCD's.
 * Written for a cheap 1.3" OLED GLCD
 * See      http://www.dx.com/p/open-smart-1-8-128-64-lcd-display-breakout-module-w-blue-backlit-444694
 *
 * Written by:  Erik van de Coevering
 * With thanks to Tim Barr from whom I've reused some code
 * Use this code in whatever way you like, as long as it stays free of charge!
 */

#include "SSH1106.h"

SSH1106::SSH1106(SPI &lcd, DigitalOut &lcd_cs, DigitalOut &lcd_cd, DigitalOut &lcd_rst)
{
    _lcd = &lcd;
    _lcd_cs = &lcd_cs;
    _lcd_cd = &lcd_cd;
    _lcd_rst = &lcd_rst;
}

void SSH1106::init()
{
    _lcd_cs->write(1);
    _lcd->frequency(8000000);      // Abusing SPI to save some time.. Try a lower freq if this doesn't work // old 24000000 // old 12000000
    _lcd->format(8,3);
    _lcd_cs->write(0);              // enable SPI
    _lcd_cd->write(0);              // COMMAND mode

    _lcd_rst->write(0);
    ThisThread::sleep_for(100);
    _lcd_rst->write(1);

    ThisThread::sleep_for(50);

    _lcd->write(0xAE);              // Display off
    _lcd->write(0x02);              // Set lower column address
    _lcd->write(0x10);              // Set higher column address
    _lcd->write(0x40);              // Set display start line
    _lcd->write(0xB0);              // Set page address
    _lcd->write(0x81);              // Set contrast to
    _lcd->write(0x80);              // 128
    _lcd->write(0xA1);              // Segment remap
    _lcd->write(0xA6);              // Inverse: normal (A7 = inverse) // old value A6
    _lcd->write(0xA8);              // Multiplex ratio
    _lcd->write(0x3F);              // Duty = 1/32
    _lcd->write(0xAD);              // Charge pump enable
    _lcd->write(0x8B);              // External VCC
    _lcd->write(0x33);              // VPP: 9v
    _lcd->write(0xC8);              // Com scan direction
    _lcd->write(0xD3);              // Display offset
    _lcd->write(0x00);              // 0x20
    _lcd->write(0xD5);              // Osc division
    _lcd->write(0x80);
    _lcd->write(0xD9);              // Pre-charge period
    _lcd->write(0x1F);              // 0x22
    _lcd->write(0xDA);              // Set com pins
    _lcd->write(0x12);
    _lcd->write(0xDB);              // Set vcomh
    _lcd->write(0x40);
    _lcd->write(0xAF);              // Display ON
    _lcd_cs->write(1);
    _lcd_cd->write(1);
}

void SSH1106::setContrast(char contrast)
{
    _lcd_cs->write(0);              // enable SPI
    _lcd_cd->write(0);              // command mode
    _lcd->write(0x81);              // command to set contrast
    _lcd->write(contrast);          // set contrast
    _lcd_cs->write(1);
    _lcd_cd->write(1);
}

void SSH1106::setCursor(char column, char line)
{
    int i, j;
    column = column+2; // column+4

    i=(column&0xF0)>>4;
    j=column&0x0F;
    _lcd_cd->write(0);
    _lcd->write(0xb0+line);
    _lcd->write(0x10+i);
    _lcd->write(j);
    _lcd_cd->write(1);
}

void SSH1106::clear()
{
    _lcd_cs->write(0);
    _lcd_cd->write(1);

    for(unsigned short j = 0; j < LCDPAGES; j++) {
        SSH1106::setCursor(0, j);
        for(unsigned short i = 0; i < LCDWIDTH ; i++) {
            _lcd->write(0x00);
        }
    }

    SSH1106::setCursor(0, 0);

    _lcd_cs->write(1);
}

void SSH1106::writeText2d(char column, char page, const char font_address[96][8], const char *text, int size)
{
    _lcd_cs->write(0);
    SSH1106::setCursor(column, page);
    for(int i=0; i<size; i++) {
        for(int a=0; a<8; a++) {
            _lcd->write((font_address[(text[i]-32)][a]));
        }
    }
    _lcd_cs->write(1);
}

void SSH1106::writeText(char column, char page, const char *font_address, const char *text, const uint8_t size)
{
    // Position of character data in memory array
    uint16_t pos_array;
    // temporary column, page address, and column_cnt are used
    // to stay inside display area
    uint8_t i,y, column_cnt = 0;
    uint8_t count = 0;

    // font information, needed for calculation
    uint8_t start_code, last_code, width, page_height, bytes_p_char;

    uint8_t *txtbuffer;

    start_code   = font_address[2];  // get first defined character
    last_code    = font_address[3];  // get last defined character
    width        = font_address[4];  // width in pixel of one char
    page_height  = font_address[6];  // page count per char
    bytes_p_char = font_address[7];  // bytes per char

    _lcd_cs->write(0);  // Enable SPI
    _lcd_cd->write(1);  // Data mode

    if(page_height + page > LCDPAGES) //stay inside display area
        page_height = LCDPAGES - page;

    // The string is displayed character after character. If the font has more then one page,
    // the top page is printed first, then the next page and so on
    for(y = 0; y < page_height; y++) {
        txtbuffer = &_lcdbuffer[page*LCDWIDTH + column];
        column_cnt = 0;                 // clear column_cnt start point
        i = 0;
        while(( i < size) && ((column_cnt + column) < LCDWIDTH)) {
            if(text[i] < start_code || (uint8_t)text[i] > last_code) //make sure data is valid
                i++;
            else {
                // calculate position of ASCII character in font array
                // bytes for header + (ASCII - startcode) * bytes per char)
                pos_array = 8 + (uint8_t)(text[i++] - start_code) * bytes_p_char;

                // get the dot pattern for the part of the char to print
                pos_array += y*width;

                // stay inside display area
                if((column_cnt + width + column) > LCDWIDTH)
                    column_cnt = LCDWIDTH-width;

                // copy character data to buffer
                memcpy (txtbuffer+column_cnt,font_address+pos_array,width);
            }

            column_cnt += width;
        }
        SSH1106::setCursor(column,page);  // set start position x and y

        do {
            _lcd->write(txtbuffer[count]);
            count++;
        } while ((count <= column_cnt));
    }

    _lcd_cs->write(1);  // Disable SPI

}

void SSH1106::drawBitmap(const char *data)
{
    int cnt = 0;
    _lcd_cs->write(0);
    _lcd_cd->write(1);
    SSH1106::setCursor(0,0);
    for(int row=0; row<LCDPAGES; row++) {
        SSH1106::setCursor(0, row);
        for(int column=0; column<LCDWIDTH; column++) {
            _lcd->write(data[cnt]);
            cnt++;
        }
    }
    _lcd_cs->write(1);
}

void SSH1106::drawLineHor(char posx, char posy, char height, char width)
{
    char page, offset, offset2;
    char buffer[2] = {0xFF, 0xFF};

    _lcd_cs->write(0);
    _lcd_cd->write(1);

    if(width+posx > LCDWIDTH) width = (LCDWIDTH-posx); // keep inside display area

    page = posy/8;
    offset = posy - (page*8);
    buffer[0] = buffer[0] >> (8-height);
    buffer[0] = buffer[0] << offset;

    if((offset + height) > 8) {
        offset2 = ((offset+height)-8);
        buffer[1] = buffer[1] - (0xFF << (offset2));
    }

    SSH1106::setCursor(posx, page);

    for(int i=0; i<width; i++) _lcd->write(buffer[0]);

    if(buffer[1] != 0xFF && (page+1) < 8) {         // only write if line takes up > 1 page & keep inside display area
        SSH1106::setCursor(posx, (page+1));
        for(int i=0; i<width; i++) _lcd->write(buffer[1]);
    }
    _lcd_cs->write(1);
}

void SSH1106::drawLineVert(char posx, char posy, char height, char width)
{
    char page, pagecount, offset, offset2;
    
    _lcd_cs->write(0);
    _lcd_cd->write(1);

    page = posy/8;
    pagecount = height/8;
    offset2 = height - (pagecount*8);

    SSH1106::setCursor(posx, page);
    for(int i=0; i<width; i++) _lcd->write((0xFF>>offset));

    for(; pagecount > 1; pagecount--) {
        page++;
        SSH1106::setCursor(posx, page);
        for(int i=0; i<width; i++) _lcd->write(0xFF);
    }

    SSH1106::setCursor(posx, (page+1));
    for(int i=0; i<width; i++) _lcd->write((0xFF<<offset2));

    _lcd_cs->write(1);
}

void SSH1106::clearBuffer(void)
{
    for(int i=0; i<(LCDWIDTH*LCDPAGES); i++) buff[i] = 0;
}

void SSH1106::update(void)
{
    int cnt = 0;
    _lcd_cs->write(0);
    _lcd_cd->write(1);
    SSH1106::setCursor(0,0);
    for(int row=0; row<LCDPAGES; row++) {
        SSH1106::setCursor(0, row);
        for(int column=0; column<LCDWIDTH; column++) {
            _lcd->write(buff[cnt]);
            cnt++;
        }
    }
    _lcd_cs->write(1);
}

void SSH1106::drawbufferLineHor(char posx, char posy, char height, char width)
{
    char page, offset, offset2;
    int cursor;
    char buffer[2] = {0xFF, 0xFF};

    if(width+posx > LCDWIDTH) width = (LCDWIDTH-posx); // keep inside display area

    page = posy/LCDPAGES;
    offset = posy - (page*LCDPAGES);
    buffer[0] = buffer[0] >> (8-height);
    buffer[0] = buffer[0] << offset;

    if((offset + height) > 8) {
        offset2 = ((offset+height)-8);
        buffer[1] = buffer[1] - (0xFF << (offset2));
    }

    cursor = posx + (page*LCDWIDTH);

    for(int i=0; i<width; i++) SSH1106::buff[cursor+i] |= buffer[0];

    if(buffer[1] != 0xFF && (page+1) < LCDPAGES) {         // only write if line takes up > 1 page & keep inside display area
        for(int i=0; i<width; i++) SSH1106::buff[cursor+i+LCDWIDTH] |= buffer[1];
    }
}

void SSH1106::drawbufferLineVert(char posx, char posy, char height, char width)
{
    char page, pagecount, offset, offset2;
    int cursor;

    page = posy/LCDPAGES;
    pagecount = height/LCDPAGES;
    offset2 = height - (pagecount*LCDPAGES);
    cursor = posx + (page*LCDWIDTH); // LCDWIDTH

    for(int i=0; i<width; i++) SSH1106::buff[cursor+i] |= (0xFF>>offset);

    for(; pagecount > 1; pagecount--) {
        page++;
        cursor += LCDWIDTH;
        for(int i=0; i<width; i++) SSH1106::buff[cursor+i] |= 0xFF;
    }

    cursor += LCDWIDTH;
    for(int i=0; i<width; i++) SSH1106::buff[cursor+i] |= (0xFF >> offset2);
}