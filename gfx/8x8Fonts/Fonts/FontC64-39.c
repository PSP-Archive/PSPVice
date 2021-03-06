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
// Orignial name : "TempFont.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x00,
0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,
0x6C,0xFE,0x6C,0x6C,0x6C,0xFE,0x6C,0x00,
0x10,0x7E,0xD0,0x7C,0x16,0xFC,0x10,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,
0x18,0x30,0x60,0x60,0x60,0x30,0x18,0x00,
0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,
0x00,0x54,0x38,0x7C,0x38,0x54,0x00,0x00,
0x00,0x18,0x18,0x7E,0x7E,0x18,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30,
0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,
0x00,0x03,0x06,0x0C,0x18,0x30,0x60,0x00,
0x7C,0xFE,0xCE,0xD6,0xE6,0xFE,0x7C,0x00,
0x30,0x70,0x30,0x30,0x30,0xFC,0xFC,0x00,
0xFC,0xFE,0x0E,0x3C,0xF0,0xFE,0xFE,0x00,
0xFC,0xFE,0x06,0x7C,0x06,0xFE,0xFC,0x00,
0xC0,0xC0,0xCC,0xCC,0xFE,0xFE,0x0C,0x00,
0xFE,0xFE,0xC0,0xFC,0x0E,0xFE,0xFC,0x00,
0x7E,0xFE,0xC0,0xFC,0xC6,0xFE,0x7C,0x00,
0xFE,0xFE,0x0E,0x1C,0x38,0x38,0x38,0x00,
0x7C,0xFE,0xC6,0x7C,0xC6,0xFE,0x7C,0x00,
0x7C,0xFE,0xC6,0xFE,0x06,0xFE,0x7C,0x00,
0x00,0x30,0x30,0x00,0x30,0x30,0x00,0x00,
0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00,
0x0E,0x18,0x30,0x60,0x30,0x18,0x0E,0x00,
0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,
0x70,0x18,0x0C,0x06,0x0C,0x18,0x70,0x00,
0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x3E,0x36,0x66,0x7E,0xC6,0xC6,0x00,
0xFC,0xFE,0x06,0xFC,0xC6,0xFE,0xFC,0x00,
0x7C,0xFE,0xC6,0xC0,0xC6,0xFE,0x7C,0x00,
0xFC,0xFE,0x06,0xC6,0xC6,0xFE,0xFC,0x00,
0x7E,0xFE,0xC0,0xFE,0xC0,0xFE,0x7E,0x00,
0xFE,0xFE,0x00,0xFC,0xC0,0xC0,0xC0,0x00,
0x7C,0xFE,0xC0,0xCE,0xC6,0xFE,0x7C,0x00,
0xC6,0xC6,0xC6,0xF6,0xC6,0xC6,0xC6,0x00,
0x7E,0x7E,0x18,0x18,0x18,0x7E,0x7E,0x00,
0x7E,0x7E,0x0C,0xCC,0xCC,0xFC,0x78,0x00,
0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00,
0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x7E,0x00,
0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00,
0xCC,0xEC,0xFC,0xFC,0xDC,0xCC,0xCC,0x00,
0x7C,0xFE,0xC6,0xC6,0xC6,0xFE,0x7C,0x00,
0xFC,0xFE,0x06,0xFC,0xC0,0xC0,0xC0,0x00,
0x7C,0xFE,0xC6,0xC6,0xC6,0xFE,0x7B,0x00,
0xF8,0xFE,0x06,0xFC,0xC6,0xC6,0xC6,0x00,
0x7E,0xFE,0xC0,0x7C,0x06,0xFE,0xFC,0x00,
0xF8,0xFC,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,
0xC6,0xC6,0xC6,0xC6,0xC6,0xFE,0x7C,0x00,
0xC6,0xC6,0xC6,0xC6,0xEE,0x7C,0x38,0x00,
0xC6,0xC6,0xD6,0xFE,0xFE,0xEE,0xC6,0x00,
0xC6,0xEE,0x7C,0x38,0x7C,0xEE,0xC6,0x00,
0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00,
0xFE,0xFE,0x1C,0x38,0x70,0xFE,0xFE,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x3E,0x36,0x66,0x7E,0xC6,0xC6,0x00,
0xFC,0xFE,0x06,0xFC,0xC6,0xFE,0xFC,0x00,
0x7C,0xFE,0xC6,0xC0,0xC6,0xFE,0x7C,0x00,
0xFC,0xFE,0x06,0xC6,0xC6,0xFE,0xFC,0x00,
0x7E,0xFE,0xC0,0xFE,0xC0,0xFE,0x7E,0x00,
0xFE,0xFE,0x00,0xFC,0xC0,0xC0,0xC0,0x00,
0x7C,0xFE,0xC0,0xCE,0xC6,0xFE,0x7C,0x00,
0xC6,0xC6,0xC6,0xF6,0xC6,0xC6,0xC6,0x00,
0x7E,0x7E,0x18,0x18,0x18,0x7E,0x7E,0x00,
0x7E,0x7E,0x0C,0xCC,0xCC,0xFC,0x78,0x00,
0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00,
0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x7E,0x00,
0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00,
0xCC,0xEC,0xFC,0xFC,0xDC,0xCC,0xCC,0x00,
0x7C,0xFE,0xC6,0xC6,0xC6,0xFE,0x7C,0x00,
0xFC,0xFE,0x06,0xFC,0xC0,0xC0,0xC0,0x00,
0x7C,0xFE,0xC6,0xC6,0xC6,0xFE,0x7B,0x00,
0xF8,0xFE,0x06,0xFC,0xC6,0xC6,0xC6,0x00,
0x7E,0xFE,0xC0,0x7C,0x06,0xFE,0xFC,0x00,
0xF8,0xFC,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,
0xC6,0xC6,0xC6,0xC6,0xC6,0xFE,0x7C,0x00,
0xC6,0xC6,0xC6,0xC6,0xEE,0x7C,0x38,0x00,
0xC6,0xC6,0xD6,0xFE,0xFE,0xEE,0xC6,0x00,
0xC6,0xEE,0x7C,0x38,0x7C,0xEE,0xC6,0x00,
0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00,
0xFE,0xFE,0x1C,0x38,0x70,0xFE,0xFE,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
