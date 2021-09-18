// --------------------------------
// FONT PACKAGE (C format)
// consolecoder@hotmail.com 
//
// Here is a package of 44 nice 8x8 fonts, very usefull for GBA coding.
//
// Format of this font : 8x8 pixel, 1 color, 96 caracters.
// Each byte represent   8 pixels, each 8 bytes row represents 1 caracter.
//
// The caracter order is the same as ASCII table.
// The first caracter of this set correspond to ASCII caracter 32 (space).
//
// To find the index of caracter c, just do : (c-32)*8
// The fastest way to use this font "as it is", is to precalc a 256 entry table
// making the link between a byte and the 8 pixel issued from that byte.
//
// Fonts ripped from "POWER FONTS" (Amiga) by Kefrens (I think)
// Most of them are well known C64 fonts.
// Orignial name : "Cobra.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x30,0x30,0x30,0x30,0x20,0x00,0x20,0x00,
0xCC,0x44,0x88,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x30,0x10,0x20,0x00,0x00,0x00,0x00,0x00,
0x18,0x30,0x30,0x30,0x30,0x30,0x18,0x00,
0x30,0x18,0x18,0x18,0x18,0x18,0x30,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x10,0x20,
0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,
0x04,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,
0x58,0xCC,0xCC,0xCC,0xCC,0xCC,0x58,0x00,
0x70,0x30,0x30,0x30,0x30,0x30,0x78,0x00,
0x58,0xCC,0x0C,0x18,0x60,0x0C,0xFC,0x00,
0x68,0xCC,0x0C,0x28,0x0C,0xCC,0x68,0x00,
0x0C,0x2C,0x4C,0x8C,0xFE,0x0C,0x1E,0x00,
0xFC,0x30,0x80,0xD8,0x0C,0xCC,0xD8,0x00,
0x58,0xCC,0xC0,0xD8,0xCC,0xCC,0x58,0x00,
0xFC,0xC0,0x08,0x18,0x30,0x30,0x30,0x00,
0x58,0xCC,0xCC,0x58,0xCC,0xCC,0x58,0x00,
0x58,0xCC,0xCC,0x5C,0x0C,0xCC,0x58,0x00,
0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,
0x00,0x00,0x30,0x00,0x00,0x30,0x10,0x20,
0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00,
0x00,0x00,0xFC,0x00,0xFC,0x00,0x00,0x00,
0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00,
0x6C,0xC6,0x06,0x0C,0x18,0x00,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x0C,0x2C,0x26,0x5E,0x46,0xEE,0x00,
0xEC,0x66,0x66,0x6C,0x66,0x66,0xEC,0x00,
0x2E,0x62,0xE0,0xE0,0xE0,0x62,0x2E,0x00,
0xE8,0x6C,0x66,0x66,0x66,0x6C,0xE8,0x00,
0xEE,0x66,0x60,0x6C,0x60,0x66,0xEE,0x00,
0xEE,0x66,0x60,0x6C,0x60,0x60,0xF0,0x00,
0x2E,0x66,0xE0,0xEE,0xE6,0x66,0x2C,0x00,
0xF6,0x66,0x66,0x6E,0x66,0x66,0xF6,0x00,
0x78,0x30,0x30,0x30,0x30,0x30,0x78,0x00,
0x3C,0x18,0x18,0x18,0xD8,0xD8,0x70,0x00,
0xF6,0x64,0x68,0x6C,0x6E,0x66,0xF6,0x00,
0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,
0xC6,0xEE,0x7E,0xB6,0x86,0x86,0xCE,0x00,
0x66,0x72,0x3A,0x5E,0x4E,0x46,0xE6,0x00,
0x58,0xCC,0xCC,0xCC,0xCC,0xCC,0x58,0x00,
0xEC,0x66,0x66,0x6C,0x60,0x60,0xF0,0x00,
0x58,0xCC,0xCC,0xCC,0xCC,0x58,0x1C,0x00,
0xEC,0x66,0x66,0x6C,0x68,0x64,0xF6,0x00,
0x5C,0xCC,0xC0,0x78,0x0C,0xCC,0xD8,0x00,
0xFC,0xB4,0x30,0x30,0x30,0x30,0x30,0x00,
0xF6,0x62,0x62,0x62,0x62,0x62,0x2C,0x00,
0xF6,0x62,0x62,0x62,0x22,0x3C,0x18,0x00,
0xE6,0xC2,0xC2,0xD2,0xDA,0xEC,0xC6,0x00,
0xCE,0xCC,0x68,0x30,0x58,0x4C,0xCE,0x00,
0xE6,0x62,0x34,0x18,0x18,0x18,0x3C,0x00,
0xEC,0x8C,0x18,0x30,0x60,0xCC,0xDC,0x00,
0x38,0x30,0x30,0x30,0x30,0x30,0x38,0x00,
0x80,0xC0,0x60,0x30,0x18,0x0C,0x04,0x00,
0x38,0x18,0x18,0x18,0x18,0x18,0x38,0x00,
0x10,0x38,0x6C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0x30,0x20,0x10,0x00,0x00,0x00,0x00,0x00,
0x0C,0x0C,0x2C,0x26,0x5E,0x46,0xEE,0x00,
0xEC,0x66,0x66,0x6C,0x66,0x66,0xEC,0x00,
0x2E,0x62,0xE0,0xE0,0xE0,0x62,0x2E,0x00,
0xE8,0x6C,0x66,0x66,0x66,0x6C,0xE8,0x00,
0xEE,0x66,0x60,0x6C,0x60,0x66,0xEE,0x00,
0xEE,0x66,0x60,0x6C,0x60,0x60,0xF0,0x00,
0x2E,0x66,0xE0,0xEE,0xE6,0x66,0x2C,0x00,
0xF6,0x66,0x66,0x6E,0x66,0x66,0xF6,0x00,
0x78,0x30,0x30,0x30,0x30,0x30,0x78,0x00,
0x3C,0x18,0x18,0x18,0xD8,0xD8,0x70,0x00,
0xF6,0x64,0x68,0x6C,0x6E,0x66,0xF6,0x00,
0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,
0xC6,0xEE,0x7E,0xB6,0x86,0x86,0xCE,0x00,
0x66,0x72,0x3A,0x5E,0x4E,0x46,0xE6,0x00,
0x58,0xCC,0xCC,0xCC,0xCC,0xCC,0x58,0x00,
0xEC,0x66,0x66,0x6C,0x60,0x60,0xF0,0x00,
0x58,0xCC,0xCC,0xCC,0xCC,0x58,0x1C,0x00,
0xEC,0x66,0x66,0x6C,0x68,0x64,0xF6,0x00,
0x5C,0xCC,0xC0,0x78,0x0C,0xCC,0xD8,0x00,
0xFC,0xB4,0x30,0x30,0x30,0x30,0x30,0x00,
0xF6,0x62,0x62,0x62,0x62,0x62,0x2C,0x00,
0xF6,0x62,0x62,0x62,0x22,0x3C,0x18,0x00,
0xE6,0xC2,0xC2,0xD2,0xDA,0xEC,0xC6,0x00,
0xCE,0xCC,0x68,0x30,0x58,0x4C,0xCE,0x00,
0xE6,0x62,0x34,0x18,0x18,0x18,0x3C,0x00,
0xEC,0x8C,0x18,0x30,0x60,0xCC,0xDC,0x00,
0x18,0x30,0x30,0x60,0x30,0x30,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x30,0x18,0x18,0x0C,0x18,0x18,0x30,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
