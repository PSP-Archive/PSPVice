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
// Orignial name : "Count.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x30,0x30,0x30,0x30,0x00,0x30,0x30,0x00,
0x6C,0x24,0x48,0x00,0x00,0x00,0x00,0x00,
0x68,0xFC,0xFC,0x68,0x68,0xFC,0x68,0x00,
0x30,0x78,0xE4,0x30,0x9C,0x78,0x30,0x00,
0x00,0xC4,0xC8,0x10,0x20,0x4C,0x8C,0x00,
0x78,0xE4,0x70,0xE0,0xEC,0xE4,0x78,0x00,
0x30,0x10,0x20,0x00,0x00,0x00,0x00,0x00,
0x18,0x38,0x70,0x60,0x70,0x38,0x18,0x00,
0x60,0x70,0x38,0x18,0x38,0x70,0x60,0x00,
0x30,0xB4,0x78,0xFC,0x78,0xB4,0x30,0x00,
0x00,0x00,0x30,0x78,0x78,0x30,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x10,0x20,
0x00,0x00,0x00,0x7C,0x7C,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,
0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,
0x7C,0xEC,0xF4,0xE4,0xE4,0xE4,0xF8,0x00,
0x38,0x78,0x38,0x38,0x38,0x38,0x38,0x00,
0x70,0xB8,0x38,0x30,0x70,0xE0,0xFC,0x00,
0x78,0x9C,0x1C,0x38,0x1C,0x9C,0x78,0x00,
0x38,0x78,0xB8,0xB8,0xFC,0x38,0x38,0x00,
0xFC,0xFC,0x80,0xF8,0x3C,0xBC,0x78,0x00,
0x78,0xE4,0xE0,0xF8,0xE4,0xE4,0x78,0x00,
0xFC,0x38,0x38,0x70,0x70,0xE0,0xE0,0x00,
0x78,0xE4,0xE4,0x78,0xE4,0xE4,0x78,0x00,
0x78,0x9C,0x9C,0x7C,0x1C,0x9C,0x78,0x00,
0x00,0x00,0x60,0x60,0x00,0x60,0x60,0x00,
0x00,0x00,0x60,0x60,0x00,0x60,0x20,0x40,
0x0C,0x1C,0x38,0x70,0x38,0x1C,0x0C,0x00,
0x00,0x00,0x7C,0x7C,0x00,0x7C,0x7C,0x00,
0xC0,0xE0,0x70,0x38,0x30,0x70,0xE0,0xC0,
0x78,0x9C,0x1C,0x1C,0x38,0x00,0x38,0x00,
0x78,0xC4,0xD4,0xEC,0xD8,0xC0,0x7C,0x00,
0x78,0xFC,0x9C,0x9C,0xFC,0x9C,0x9C,0x00,
0xF8,0xE4,0xE4,0xF8,0xE4,0xE4,0xF8,0x00,
0x78,0xE4,0xE0,0xE0,0xE0,0xE4,0x78,0x00,
0xF8,0xE4,0xE4,0xE4,0xE4,0xE4,0xF8,0x00,
0x7C,0xE0,0xE0,0xF8,0xE0,0xE0,0x7C,0x00,
0x7C,0xE0,0xE0,0xF8,0xE0,0xE0,0xE0,0x00,
0x78,0xE4,0xE0,0xE0,0xEC,0xE4,0x78,0x00,
0xE4,0xE4,0xE4,0xFC,0xE4,0xE4,0xE4,0x00,
0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x00,
0x1C,0x1C,0x1C,0x1C,0x1C,0x5C,0x38,0x00,
0xE4,0xE4,0xE4,0xF8,0xE4,0xE4,0xE4,0x00,
0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFC,0x00,
0x68,0xFC,0xD4,0xD4,0xD4,0xD4,0xD4,0x00,
0x78,0xE4,0xE4,0xE4,0xE4,0xE4,0xE4,0x00,
0x78,0xE4,0xE4,0xE4,0xE4,0xE4,0x78,0x00,
0xF8,0xE4,0xE4,0xF8,0xE0,0xE0,0xE0,0x00,
0x78,0xE4,0xE4,0xE4,0xE4,0xE4,0x7C,0x06,
0xF8,0xE4,0xE4,0xF8,0xE4,0xE4,0xE4,0x00,
0x78,0xC4,0xF0,0x78,0x3C,0x8C,0x78,0x00,
0x7C,0x38,0x38,0x38,0x38,0x38,0x38,0x00,
0xE4,0xE4,0xE4,0xE4,0xE4,0xE4,0x78,0x00,
0xE4,0xE4,0xE4,0x78,0x78,0x78,0x30,0x00,
0xAC,0xAC,0xAC,0xAC,0xAC,0xFC,0x58,0x00,
0xE4,0xE4,0xE4,0x78,0xE4,0xE4,0xE4,0x00,
0xE4,0xE4,0xE4,0x7C,0x38,0x38,0x38,0x00,
0xFC,0x1C,0x18,0x30,0x60,0xE0,0xFC,0x00,
0x3C,0x70,0x70,0x70,0x70,0x70,0x70,0x3C,
0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x00,
0x78,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x78,
0x00,0x00,0x20,0x70,0xC8,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0x30,0x30,0x20,0x10,0x00,0x00,0x00,0x00,
0x00,0x00,0x78,0x9C,0x9C,0x9C,0x6C,0x00,
0xE0,0xE0,0xF8,0xE4,0xE4,0xE4,0x78,0x00,
0x00,0x00,0x78,0xE4,0xE0,0xE4,0x78,0x00,
0x1C,0x1C,0x7C,0x9C,0x9C,0x9C,0x78,0x00,
0x00,0x00,0x78,0xE4,0xF8,0xE0,0x7C,0x00,
0x38,0x74,0x70,0x7C,0x70,0x70,0x70,0x00,
0x00,0x00,0x78,0x9C,0x9C,0x7C,0x1C,0xF8,
0xE0,0xE0,0xE0,0xF8,0xE4,0xE4,0xE4,0x00,
0x38,0x00,0x38,0x38,0x38,0x38,0x38,0x00,
0x1C,0x00,0x1C,0x1C,0x1C,0x1C,0x5C,0x38,
0xE0,0xE4,0xE4,0xF8,0xE4,0xE4,0xE4,0x00,
0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x00,
0x00,0x00,0x68,0xFC,0xD4,0xD4,0xD4,0x00,
0x00,0x00,0x78,0xE4,0xE4,0xE4,0xE4,0x00,
0x00,0x00,0x78,0xE4,0xE4,0xE4,0x78,0x00,
0x00,0x00,0xF8,0xE4,0xE4,0xE4,0xF8,0xE0,
0x00,0x00,0x7C,0x9C,0x9C,0x9C,0x7C,0x1C,
0x00,0x00,0x38,0x74,0x70,0x70,0x70,0x00,
0x00,0x00,0x78,0xC4,0x30,0x8C,0x78,0x00,
0x70,0x70,0x7C,0x70,0x70,0x74,0x38,0x00,
0x00,0x00,0xE4,0xE4,0xE4,0xE4,0x78,0x00,
0x00,0x00,0xCA,0xCA,0xD2,0xD2,0x60,0x00,
0x00,0x00,0xAC,0xAC,0xAC,0xFC,0x58,0x00,
0x00,0x00,0xE4,0xE4,0x78,0xE4,0xE4,0x00,
0x00,0x00,0x9C,0x9C,0x9C,0x7C,0x1C,0xF8,
0x00,0x00,0xFC,0x38,0x70,0xE0,0xFC,0x00,
0x1C,0x3C,0x30,0x70,0x30,0x30,0x3C,0x1C,
0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
0x70,0x78,0x18,0x1C,0x18,0x18,0x78,0x70,
0x00,0x00,0x00,0x64,0x98,0x00,0x00,0x00,
0x18,0x30,0x60,0xFC,0x60,0x30,0x18,0x00,
};
