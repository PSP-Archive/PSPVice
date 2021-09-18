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
// Orignial name : "Topaz.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00,
0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,
0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00,
0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00,
0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00,
0x38,0x6C,0x68,0x76,0xDC,0xCC,0x76,0x00,
0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00,
0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00,
0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,
0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,
0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30,
0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,
0x03,0x06,0x0C,0x18,0x30,0x60,0xC0,0x00,
0x3C,0x66,0x6E,0x7E,0x76,0x66,0x3C,0x00,
0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00,
0x3C,0x66,0x06,0x1C,0x30,0x66,0x7E,0x00,
0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00,
0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00,
0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00,
0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00,
0x7E,0x66,0x06,0x0C,0x18,0x18,0x18,0x00,
0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00,
0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00,
0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00,
0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30,
0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00,
0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,
0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00,
0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00,
0x7E,0xC3,0xDB,0xD3,0xDB,0xC3,0x7E,0x00,
0x18,0x3C,0x3C,0x66,0x7E,0xC3,0xC3,0x00,
0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00,
0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00,
0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00,
0xFE,0x66,0x60,0x78,0x60,0x66,0xFE,0x00,
0xFE,0x66,0x60,0x78,0x60,0x60,0xF0,0x00,
0x3C,0x66,0xC0,0xCE,0xC6,0x66,0x3E,0x00,
0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,
0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,
0x0E,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,
0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00,
0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,
0x82,0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0x00,
0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00,
0x38,0x6C,0xC6,0xC6,0xC6,0x6C,0x38,0x00,
0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00,
0x38,0x6C,0xC6,0xC6,0xC6,0x6C,0x3C,0x06,
0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE3,0x00,
0x3C,0x66,0x70,0x38,0x0E,0x66,0x3C,0x00,
0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00,
0x66,0x66,0x66,0x66,0x66,0x66,0x3E,0x00,
0xC3,0xC3,0x66,0x66,0x3C,0x3C,0x18,0x00,
0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00,
0xC3,0x66,0x3C,0x18,0x3C,0x66,0xC3,0x00,
0xC3,0xC3,0x66,0x3C,0x18,0x18,0x3C,0x00,
0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00,
0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,
0xC0,0x60,0x30,0x18,0x0C,0x06,0x03,0x00,
0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,
0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3C,0x06,0x1E,0x66,0x3B,0x00,
0xE0,0x60,0x6C,0x76,0x66,0x66,0x3C,0x00,
0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00,
0x0E,0x06,0x36,0x6E,0x66,0x66,0x3B,0x00,
0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,
0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00,
0x00,0x00,0x3B,0x66,0x66,0x3C,0xC6,0x7C,
0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00,
0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00,
0x06,0x00,0x06,0x06,0x06,0x06,0x66,0x3C,
0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00,
0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,
0x00,0x00,0x66,0x77,0x6B,0x63,0x63,0x00,
0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00,
0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,
0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0,
0x00,0x00,0x3D,0x66,0x66,0x3E,0x06,0x07,
0x00,0x00,0xEC,0x76,0x66,0x60,0xF0,0x00,
0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00,
0x08,0x18,0x3E,0x18,0x18,0x1A,0x0C,0x00,
0x00,0x00,0x66,0x66,0x66,0x66,0x3B,0x00,
0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00,
0x00,0x00,0x63,0x6B,0x6B,0x36,0x36,0x00,
0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00,
0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x70,
0x00,0x00,0x7E,0x4C,0x18,0x32,0x7E,0x00,
0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,
0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00,
0x72,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,
0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33,
};
