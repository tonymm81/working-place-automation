; // Compiler bug: remove if needed

/*  The array starts with a 8 byte header:
 *  1st Byte: 'F' first 2 bytes are always FV
 *  2nd Byte: 'V' for FONT VERTICAL
 *  3rd Byte: First code to define
 *  4th Byte: Last  code to define
 *  5th Byte: Width of character in dots
 *  6th Byte: Height of character in dots
 *  7th Byte: Height of character in bytes
 *  8th Byte: Bytes needed for each character (1..255)
 *            or 0 for big fonts calculate WidthInDots * HeightInBytes
 * After that font data will follow
 */

const char font_7x7[] = {
    70, 86, 32,127,  7,  7,  1,  7,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //  
    0x5e,0x00,0x00,0x00,0x00,0x00,0x00, // !
    0x0c,0x00,0x0c,0x00,0x00,0x00,0x00, // "
    0x14,0x3e,0x14,0x3e,0x14,0x00,0x00, // #
    0x2e,0x2a,0x7f,0x2a,0x3a,0x00,0x00, // $
    0x1c,0x14,0x1c,0x00,0x7c,0x00,0x70, // %
    0x78,0x4e,0x5a,0x2e,0x40,0x00,0x00, // &
    0x0c,0x00,0x00,0x00,0x00,0x00,0x00, // '
    0x7e,0x81,0x00,0x00,0x00,0x00,0x00, // (
    0x81,0x7e,0x00,0x00,0x00,0x00,0x00, // )
    0x04,0x1c,0x0e,0x1c,0x04,0x00,0x00, // *
    0x08,0x1c,0x08,0x00,0x00,0x00,0x00, // +
    0xc0,0x00,0x00,0x00,0x00,0x00,0x00, // ,
    0x08,0x08,0x08,0x00,0x00,0x00,0x00, // -
    0x40,0x00,0x00,0x00,0x00,0x00,0x00, // .
    0x70,0x0e,0x00,0x00,0x00,0x00,0x00, // /
    0x7c,0x44,0x7c,0x00,0x00,0x00,0x00, // 0
    0x08,0x7c,0x00,0x00,0x00,0x00,0x00, // 1
    0x64,0x54,0x4c,0x00,0x00,0x00,0x00, // 2
    0x44,0x54,0x7c,0x00,0x00,0x00,0x00, // 3
    0x1c,0x10,0x7c,0x00,0x00,0x00,0x00, // 4
    0x5c,0x54,0x34,0x00,0x00,0x00,0x00, // 5
    0x7c,0x54,0x74,0x00,0x00,0x00,0x00, // 6
    0x04,0x04,0x7c,0x00,0x00,0x00,0x00, // 7
    0x7c,0x54,0x7c,0x00,0x00,0x00,0x00, // 8
    0x1c,0x14,0x7c,0x00,0x00,0x00,0x00, // 9
    0x50,0x00,0x00,0x00,0x00,0x00,0x00, // :
    0xd0,0x00,0x00,0x00,0x00,0x00,0x00, // ;
    0x20,0x50,0x50,0x00,0x00,0x00,0x00, // <
    0x28,0x28,0x28,0x00,0x00,0x00,0x00, // =
    0x50,0x50,0x20,0x00,0x00,0x00,0x00, // >
    0x02,0x5a,0x0e,0x00,0x00,0x00,0x00, // ?
    0x7e,0x42,0x5a,0x5a,0x52,0x5e,0x00, // @
    0x7e,0x0a,0x0a,0x7e,0x00,0x00,0x00, // A
    0x7e,0x4a,0x4a,0x7c,0x00,0x00,0x00, // B
    0x7e,0x42,0x42,0x42,0x00,0x00,0x00, // C
    0x7e,0x42,0x42,0x7c,0x00,0x00,0x00, // D
    0x7e,0x4a,0x4a,0x42,0x00,0x00,0x00, // E
    0x7e,0x0a,0x0a,0x02,0x00,0x00,0x00, // F
    0x7e,0x42,0x4a,0x7a,0x00,0x00,0x00, // G
    0x7e,0x08,0x08,0x7e,0x00,0x00,0x00, // H
    0x42,0x42,0x7e,0x42,0x42,0x00,0x00, // I
    0x42,0x42,0x7e,0x02,0x00,0x00,0x00, // J
    0x7e,0x08,0x0e,0x78,0x00,0x00,0x00, // K
    0x7e,0x40,0x40,0x40,0x00,0x00,0x00, // L
    0x7e,0x02,0x06,0x02,0x7e,0x00,0x00, // M
    0x7e,0x04,0x08,0x7e,0x00,0x00,0x00, // N
    0x7e,0x42,0x42,0x7e,0x00,0x00,0x00, // O
    0x7e,0x0a,0x0a,0x0e,0x00,0x00,0x00, // P
    0x7e,0x42,0xc2,0x7e,0x00,0x00,0x00, // Q
    0x7e,0x0a,0x7a,0x0e,0x00,0x00,0x00, // R
    0x4e,0x4a,0x4a,0x7a,0x00,0x00,0x00, // S
    0x02,0x02,0x7e,0x02,0x02,0x00,0x00, // T
    0x7e,0x40,0x40,0x7e,0x00,0x00,0x00, // U
    0x1e,0x20,0x40,0x20,0x1e,0x00,0x00, // V
    0x7e,0x40,0x60,0x40,0x7e,0x00,0x00, // W
    0x42,0x24,0x18,0x24,0x42,0x00,0x00, // X
    0x0e,0x08,0x78,0x08,0x0e,0x00,0x00, // Y
    0x62,0x52,0x4a,0x46,0x00,0x00,0x00, // Z
    0xff,0x81,0x00,0x00,0x00,0x00,0x00, // [
    0x0e,0x70,0x00,0x00,0x00,0x00,0x00, // "\"
    0x81,0xff,0x00,0x00,0x00,0x00,0x00, // ]
    0x04,0x02,0x04,0x00,0x00,0x00,0x00, // ^
    0x40,0x40,0x40,0x40,0x40,0x00,0x00, // _
    0x02,0x04,0x00,0x00,0x00,0x00,0x00, // `
    0x7c,0x14,0x14,0x7c,0x00,0x00,0x00, // a
    0x7c,0x54,0x54,0x78,0x00,0x00,0x00, // b
    0x7c,0x44,0x44,0x44,0x00,0x00,0x00, // c
    0x7c,0x44,0x44,0x78,0x00,0x00,0x00, // d
    0x7c,0x54,0x54,0x44,0x00,0x00,0x00, // e
    0x7c,0x14,0x14,0x04,0x00,0x00,0x00, // f
    0x7c,0x44,0x54,0x74,0x00,0x00,0x00, // g
    0x7c,0x10,0x10,0x7c,0x00,0x00,0x00, // h
    0x44,0x44,0x7c,0x44,0x44,0x00,0x00, // i
    0x44,0x44,0x7c,0x04,0x00,0x00,0x00, // j
    0x7c,0x10,0x1c,0x70,0x00,0x00,0x00, // k
    0x7c,0x40,0x40,0x40,0x00,0x00,0x00, // l
    0x7c,0x04,0x0c,0x04,0x7c,0x00,0x00, // m
    0x7c,0x08,0x10,0x7c,0x00,0x00,0x00, // n
    0x7c,0x44,0x44,0x7c,0x00,0x00,0x00, // o
    0x7c,0x14,0x14,0x1c,0x00,0x00,0x00, // p
    0x7c,0x44,0xc4,0x7c,0x00,0x00,0x00, // q
    0x7c,0x14,0x74,0x1c,0x00,0x00,0x00, // r
    0x5c,0x54,0x54,0x74,0x00,0x00,0x00, // s
    0x04,0x04,0x7c,0x04,0x04,0x00,0x00, // t
    0x7c,0x40,0x40,0x7c,0x00,0x00,0x00, // u
    0x1c,0x20,0x40,0x20,0x1c,0x00,0x00, // v
    0x7c,0x40,0x60,0x40,0x7c,0x00,0x00, // w
    0x44,0x28,0x10,0x28,0x44,0x00,0x00, // x
    0x1c,0x10,0x70,0x10,0x1c,0x00,0x00, // y
    0x64,0x54,0x4c,0x44,0x00,0x00,0x00, // z
    0x08,0xf7,0x81,0x00,0x00,0x00,0x00, // 
    0xff,0x00,0x00,0x00,0x00,0x00,0x00, // |
    0x81,0xf7,0x08,0x00,0x00,0x00,0x00, // 
    0x18,0x08,0x10,0x18,0x00,0x00,0x00, // ~
    0x00,0x00,0x00,0x00,0x00,0x00,0x00
};