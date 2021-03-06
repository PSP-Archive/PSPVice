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
// Orignial name : "Parallax.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1C,0x1C,0x18,0x10,0x10,0x40,0x60,0x00,
0xCC,0xCC,0x44,0x00,0x00,0x00,0x00,0x00,
0xD8,0xD8,0xFE,0x6C,0xFE,0x36,0x36,0x00,
0x30,0x7C,0xC0,0x78,0x0C,0xF8,0x30,0x00,
0xCC,0xDC,0x18,0x30,0x60,0xE6,0xC6,0x00,
0x78,0xCC,0x78,0x70,0xCE,0xCC,0x7E,0x00,
0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00,
0x18,0x30,0x60,0x60,0x30,0x18,0x0C,0x00,
0x60,0x30,0x18,0x0C,0x0C,0x18,0x30,0x00,
0x00,0xCC,0x78,0x18,0x3C,0x66,0x00,0x00,
0x00,0x30,0x30,0xFE,0x18,0x18,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x10,
0x00,0x00,0x00,0xFE,0xE0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,
0x00,0x0C,0x18,0x30,0x30,0x60,0xC0,0x00,
0xFE,0xC6,0xC6,0xC6,0xE6,0xE6,0xFE,0x00,
0x30,0x30,0x30,0x38,0x38,0x38,0x38,0x00,
0xFE,0xE6,0x06,0xFE,0xC0,0xE0,0xFE,0x00,
0xFE,0xE6,0x06,0x3C,0x06,0xEE,0xFE,0x00,
0xC0,0xC0,0xE0,0xEC,0xFE,0x1C,0x1C,0x00,
0xFE,0xC0,0xE0,0xFE,0x06,0xE6,0xFE,0x00,
0xFE,0xCE,0xE0,0xFE,0xC6,0xE6,0xFE,0x00,
0xFE,0xE6,0x06,0x06,0x0E,0x0E,0x0E,0x00,
0xFE,0xC6,0xE6,0x7C,0xC6,0xE6,0xFE,0x00,
0xFE,0xC6,0xE6,0xFE,0x06,0xE6,0xFE,0x00,
0x00,0x00,0x30,0x30,0x00,0x18,0x18,0x00,
0x00,0x00,0x30,0x30,0x00,0x18,0x18,0x08,
0x38,0x60,0xC0,0xC0,0x60,0x30,0x1C,0x00,
0x00,0xFE,0xE0,0x00,0xFE,0xE0,0x00,0x00,
0xE0,0x30,0x1C,0x0E,0x0C,0x18,0x70,0x00,
0x78,0x9C,0x0C,0x38,0x60,0x80,0xC0,0x60,
0x38,0x44,0x9A,0xA2,0x9A,0x44,0x38,0x00,
0xC0,0xE0,0xF0,0xD8,0xFC,0xC6,0xC3,0x00,
0xE0,0xD8,0xC8,0xF8,0xCC,0xC6,0xFC,0x00,
0x60,0xD0,0xC0,0xC0,0xC0,0xE2,0x3C,0x00,
0xE0,0xF0,0xD8,0xCC,0xC6,0xC6,0xFC,0x00,
0x70,0xC0,0xC0,0xF8,0xC0,0xE0,0x3E,0x00,
0x38,0x60,0x60,0x7C,0x60,0x60,0x60,0x00,
0x30,0x68,0xC0,0xC0,0xCC,0xE6,0x3C,0x00,
0xD8,0xD8,0xCC,0xFC,0xC6,0xC6,0xC6,0x00,
0x20,0x20,0x30,0x30,0x38,0x38,0x38,0x00,
0xFC,0x30,0x18,0x0C,0xC6,0xC6,0x7C,0x00,
0xC8,0xD8,0xF0,0xE0,0xF0,0xDC,0xC6,0x00,
0x80,0x80,0xC0,0xC0,0xE0,0xE0,0xFE,0x00,
0xC8,0xD8,0xFC,0xF4,0xD6,0xC6,0xC6,0x00,
0xD8,0xD8,0xEC,0xEC,0xD6,0xCE,0xC6,0x00,
0x38,0x5C,0xCC,0xC6,0xC6,0xE6,0x3C,0x00,
0xE0,0xD8,0xCC,0xCC,0xF8,0xC0,0xC0,0x00,
0x38,0x5C,0xCC,0xC6,0xCA,0xE4,0x3A,0x00,
0xE0,0xD8,0xCC,0xF8,0xF0,0xD8,0xC6,0x00,
0x60,0xD0,0xC0,0x78,0x06,0xE6,0x7C,0x00,
0xFC,0xD8,0x18,0x0C,0x0C,0x0E,0x0E,0x00,
0xD8,0xD8,0xCC,0xCC,0xC6,0xE6,0x7C,0x00,
0xCC,0xCC,0xCC,0x6C,0x6C,0x3C,0x18,0x00,
0xD8,0xCC,0xCC,0xD6,0xD6,0xFE,0x64,0x00,
0xD8,0xD8,0x70,0x78,0xCC,0xC6,0xC6,0x00,
0xCC,0xCC,0x6C,0x78,0x38,0x18,0x18,0x00,
0xF8,0xD8,0x30,0x60,0x60,0xC0,0xFE,0x00,
0x78,0x60,0x60,0x30,0x30,0x30,0x3C,0x00,
0xC0,0x60,0x30,0x18,0x0C,0x06,0x03,0x00,
0x78,0x18,0x18,0x0C,0x0C,0x0C,0x3C,0x00,
0x10,0x38,0x6C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,
0xC0,0xE0,0xF0,0xD8,0xFC,0xC6,0xC3,0x00,
0xE0,0xD8,0xC8,0xF8,0xCC,0xC6,0xFC,0x00,
0x60,0xD0,0xC0,0xC0,0xC0,0xE2,0x3C,0x00,
0xE0,0xF0,0xD8,0xCC,0xC6,0xC6,0xFC,0x00,
0x70,0xC0,0xC0,0xF8,0xC0,0xE0,0x3E,0x00,
0x38,0x60,0x60,0x7C,0x60,0x60,0x60,0x00,
0x30,0x68,0xC0,0xC0,0xCC,0xE6,0x3C,0x00,
0xD8,0xD8,0xCC,0xFC,0xC6,0xC6,0xC6,0x00,
0x20,0x20,0x30,0x30,0x38,0x38,0x38,0x00,
0xFC,0x30,0x18,0x0C,0xC6,0xC6,0x7C,0x00,
0xC8,0xD8,0xF0,0xE0,0xF0,0xDC,0xC6,0x00,
0x80,0x80,0xC0,0xC0,0xE0,0xE0,0xFE,0x00,
0xC8,0xD8,0xFC,0xF4,0xD6,0xC6,0xC6,0x00,
0xD8,0xD8,0xEC,0xEC,0xD6,0xCE,0xC6,0x00,
0x38,0x5C,0xCC,0xC6,0xC6,0xE6,0x3C,0x00,
0xE0,0xD8,0xCC,0xCC,0xF8,0xC0,0xC0,0x00,
0x38,0x5C,0xCC,0xC6,0xCA,0xE4,0x3A,0x00,
0xE0,0xD8,0xCC,0xF8,0xF0,0xD8,0xC6,0x00,
0x60,0xD0,0xC0,0x78,0x06,0xE6,0x7C,0x00,
0xFC,0xD8,0x18,0x0C,0x0C,0x0E,0x0E,0x00,
0xD8,0xD8,0xCC,0xCC,0xC6,0xE6,0x7C,0x00,
0xCC,0xCC,0xCC,0x6C,0x6C,0x3C,0x18,0x00,
0xD8,0xCC,0xCC,0xD6,0xD6,0xFE,0x64,0x00,
0xD8,0xD8,0x70,0x78,0xCC,0xC6,0xC6,0x00,
0xCC,0xCC,0x6C,0x78,0x38,0x18,0x18,0x00,
0xF8,0xD8,0x30,0x60,0x60,0xC0,0xFE,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
