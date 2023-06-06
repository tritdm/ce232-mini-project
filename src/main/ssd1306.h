#ifndef MAIN_SSD1366_H_
#define MAIN_SSD1366_H_

// Following definitions are bollowed from 
// http://robotcantalk.blogspot.com/2015/03/interfacing-arduino-with-ssd1306-driven.html

// SLA (0x3C) + WRITE_MODE (0x00) =  0x78 (0b01111000)
#define OLED_I2C_ADDRESS   0x3C

// Control byte
#define OLED_CONTROL_BYTE_CMD_SINGLE    0x80    /* 0b10000000 */ 
#define OLED_CONTROL_BYTE_CMD_STREAM    0x00    /* 0b00000000 */ 
#define OLED_CONTROL_BYTE_DATA_STREAM   0x40    /* 0b01000000 */ 

// Fundamental commands (pg.28)
#define OLED_CMD_SET_CONTRAST           0x81    /* 0b10000001 follow with 0x7F */ 
#define OLED_CMD_DISPLAY_RAM            0xA4    /* 0b10100100 */ 
#define OLED_CMD_DISPLAY_ALLON          0xA5    /* 0b10100101 */ 
#define OLED_CMD_DISPLAY_NORMAL         0xA6    /* 0b10100110 */ 
#define OLED_CMD_DISPLAY_INVERTED       0xA7    /* 0b10100111 */ 
#define OLED_CMD_DISPLAY_OFF            0xAE    /* 0b10101110 */ 
#define OLED_CMD_DISPLAY_ON             0xAF    /* 0b10101111 */ 

// Addressing Command Table (pg.30)
#define OLED_CMD_SET_MEMORY_ADDR_MODE   0x20    /* 0b00100000 follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD */ 
#define OLED_CMD_SET_COLUMN_RANGE       0x21    /* 0b00100001 can be used only in HORZ/VERT mode - follow with 0x00 and 0x7F = COL127 */ 
#define OLED_CMD_SET_PAGE_RANGE         0x22    /* 0b00100010 can be used only in HORZ/VERT mode - follow with 0x00 and 0x07 = PAGE7 */ 

// Hardware Config (pg.31)
#define OLED_CMD_SET_DISPLAY_START_LINE 0x40    /* 0b01000000 */ 
#define OLED_CMD_SET_SEGMENT_REMAP      0xA1    /* 0b10100001 */
#define OLED_CMD_SET_MUX_RATIO          0xA8    /* 0b10101011 follow with 0x3F = 64 MUX */ 
#define OLED_CMD_SET_COM_SCAN_MODE      0xC8    /* 0b11001011 */
#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3    /* 0b11010011 follow with 0x00 */ 
#define OLED_CMD_SET_COM_PIN_MAP        0xDA    /* 0b11011010 follow with 0x12 */ 
#define OLED_CMD_NOP                    0xE3    /* 0b11100011 NOP */ 

// Timing and Driving Scheme (pg.32)
#define OLED_CMD_SET_DISPLAY_CLK_DIV    0xD5    /* 0b11010101 follow with 0x80 */ 
#define OLED_CMD_SET_PRECHARGE          0xD9    /* 0b11011001 follow with 0xF1 */ 
#define OLED_CMD_SET_VCOMH_DESELCT      0xDB    /* 0b11011011 follow with 0x30 */ 

// Charge Pump (pg.62)
#define OLED_CMD_SET_CHARGE_PUMP        0x8D    /* 0b10001101 follow with 0x14 */ 

#endif /* MAIN_SSD1366_H_ */