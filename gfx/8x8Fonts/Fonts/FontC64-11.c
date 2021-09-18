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
// Orignial name : "Kick20.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00,
0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,
0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00,
0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00,
0x00,0x66,0xAC,0xD8,0x36,0x6A,0xCC,0x00,
0x38,0x6C,0x68,0x76,0xDC,0xCE,0x7B,0x00,
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
0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x00,
0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00,
0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00,
0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x00,
0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00,
0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00,
0x7E,0x06,0x06,0x0C,0x18,0x18,0x18,0x00,
0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00,
0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00,
0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00,
0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30,
0x00,0x06,0x18,0x60,0x18,0x06,0x00,0x00,
0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,
0x00,0x60,0x18,0x06,0x18,0x60,0x00,0x00,
0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00,
0x7E,0x81,0x99,0x91,0x99,0x81,0x7E,0x00,
0x3C,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,
0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00,
0x1E,0x30,0x60,0x60,0x60,0x30,0x1E,0x00,
0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00,
0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00,
0x7E,0x60,0x60,0x78,0x60,0x60,0x60,0x00,
0x3C,0x66,0x60,0x6E,0x66,0x66,0x3E,0x00,
0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,
0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,
0x06,0x06,0x06,0x06,0x06,0x66,0x3C,0x00,
0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00,
0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00,
0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00,
0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00,
0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,
0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00,
0x78,0xCC,0xCC,0xCC,0xCC,0xDC,0x7E,0x00,
0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00,
0x3C,0x66,0x70,0x3C,0x0E,0x66,0x3C,0x00,
0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00,
0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,
0x66,0x66,0x66,0x66,0x3C,0x3C,0x18,0x00,
0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00,
0xC3,0x66,0x3C,0x18,0x3C,0x66,0xC3,0x00,
0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x00,
0xFE,0x0C,0x18,0x30,0x60,0xC0,0xFE,0x00,
0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,
0xC0,0x60,0x30,0x18,0x0C,0x06,0x03,0x00,
0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,
0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,
0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00,
0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00,
0x00,0x00,0x3C,0x60,0x60,0x60,0x3C,0x00,
0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00,
0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,
0x1C,0x30,0x7C,0x30,0x30,0x30,0x30,0x00,
0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x3C,
0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00,
0x18,0x00,0x18,0x18,0x18,0x18,0x0C,0x00,
0x0C,0x00,0x0C,0x0C,0x0C,0x0C,0x0C,0x78,
0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00,
0x18,0x18,0x18,0x18,0x18,0x18,0x0C,0x00,
0x00,0x00,0xEC,0xFE,0xD6,0xC6,0xC6,0x00,
0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00,
0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,
0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60,
0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06,
0x00,0x00,0x7C,0x66,0x60,0x60,0x60,0x00,
0x00,0x00,0x3C,0x60,0x3C,0x06,0x7C,0x00,
0x30,0x30,0x7C,0x30,0x30,0x30,0x1C,0x00,
0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00,
0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00,
0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00,
0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00,
0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x30,
0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00,
0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,
0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00,
0x72,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,
0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33,
};