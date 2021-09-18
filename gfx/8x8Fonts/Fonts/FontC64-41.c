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
// Orignial name : "Vidilores.fnt"
// --------------------------------
 
u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x30,0x30,0x30,0x30,0x30,0x00,0x30,0x00,
0xD8,0xD8,0xD8,0x00,0x00,0x00,0x00,0x00,
0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00,
0x20,0x78,0xC0,0x70,0x18,0xF0,0x20,0x00,
0x00,0xC0,0xD8,0x30,0x60,0xD8,0x18,0x00,
0x30,0x68,0x30,0x7C,0xD0,0xD8,0x6C,0x00,
0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,
0x30,0x60,0x60,0x60,0x60,0x60,0x30,0x00,
0x60,0x30,0x30,0x30,0x30,0x30,0x60,0x00,
0x00,0x50,0x70,0xF8,0x70,0x50,0x00,0x00,
0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x60,
0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,
0x00,0x00,0x18,0x30,0x60,0xC0,0x00,0x00,
0x70,0xD8,0xD8,0xD8,0xD8,0xD8,0x70,0x00,
0x30,0x70,0x30,0x30,0x30,0x30,0x30,0x00,
0x70,0xD8,0x18,0x30,0x60,0xC0,0xF8,0x00,
0x70,0xD8,0x18,0x30,0x18,0xD8,0x70,0x00,
0x18,0x38,0x78,0xD8,0xF8,0x18,0x18,0x00,
0xF8,0xC0,0xF0,0xD8,0x18,0xD8,0x70,0x00,
0x30,0x60,0xC0,0xF0,0xD8,0xD8,0x70,0x00,
0xF8,0x18,0x18,0x30,0x30,0x60,0x60,0x00,
0x70,0xD8,0xD8,0x70,0xD8,0xD8,0x70,0x00,
0x70,0xD8,0xD8,0x78,0x18,0x30,0x60,0x00,
0x00,0x30,0x30,0x00,0x30,0x30,0x00,0x00,
0x00,0x30,0x30,0x00,0x30,0x30,0x60,0x00,
0x00,0x18,0x30,0x60,0x30,0x18,0x00,0x00,
0x00,0x00,0x78,0x00,0x78,0x00,0x00,0x00,
0x00,0x60,0x30,0x18,0x30,0x60,0x00,0x00,
0xF0,0x18,0x18,0x30,0x60,0x00,0x60,0x00,
0x00,0x70,0x88,0xB8,0xB8,0x98,0x40,0x00,
0x70,0xD8,0xD8,0xD8,0xF8,0xD8,0xD8,0x00,
0xF0,0xD8,0xD8,0xF0,0xD8,0xD8,0xF0,0x00,
0x70,0xD8,0xC0,0xC0,0xC0,0xD8,0x70,0x00,
0xE0,0xD0,0xD8,0xD8,0xD8,0xD8,0xF0,0x00,
0xF8,0xC0,0xC0,0xF0,0xC0,0xC0,0xF8,0x00,
0xF8,0xC0,0xC0,0xF0,0xC0,0xC0,0xC0,0x00,
0x70,0xD8,0xC0,0xD8,0xD8,0xD8,0x78,0x00,
0xD8,0xD8,0xD8,0xF8,0xD8,0xD8,0xD8,0x00,
0xF0,0x60,0x60,0x60,0x60,0x60,0xF0,0x00,
0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x60,
0xD8,0xD8,0xF0,0xE0,0xF0,0xD8,0xD8,0x00,
0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xF8,0x00,
0x88,0xD8,0xF8,0xF8,0xD8,0xD8,0xD8,0x00,
0x98,0xD8,0xF8,0xF8,0xF8,0xD8,0xC8,0x00,
0x70,0xD8,0xD8,0xD8,0xD8,0xD8,0x70,0x00,
0xF0,0xD8,0xD8,0xF0,0xC0,0xC0,0xC0,0x00,
0x70,0xD8,0xD8,0xD8,0xD8,0xD8,0x70,0x18,
0xF0,0xD8,0xD8,0xF0,0xD8,0xD8,0xD8,0x00,
0x70,0xD8,0xC0,0x70,0x18,0xD8,0x70,0x00,
0xFC,0x30,0x30,0x30,0x30,0x30,0x30,0x00,
0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0x70,0x00,
0xD8,0xD8,0xD8,0xD8,0x70,0x70,0x20,0x00,
0xD8,0xD8,0xD8,0xF8,0xF8,0xD8,0x88,0x00,
0xD8,0xD8,0x70,0x70,0xD8,0xD8,0xD8,0x00,
0xCC,0xCC,0xCC,0x78,0x30,0x30,0x30,0x00,
0xF8,0x18,0x30,0x60,0xC0,0xC0,0xF8,0x00,
0x70,0x60,0x60,0x60,0x60,0x60,0x70,0x00,
0x00,0x00,0xC0,0x60,0x30,0x18,0x00,0x00,
0x70,0x30,0x30,0x30,0x30,0x30,0x70,0x00,
0x20,0x70,0xD8,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,
0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x78,0xD8,0xD8,0xD8,0x78,0x00,
0xC0,0xC0,0xF0,0xD8,0xD8,0xD8,0x70,0x00,
0x00,0x00,0x70,0xD8,0xC0,0xD8,0x70,0x00,
0x18,0x18,0x78,0xD8,0xD8,0xD8,0x78,0x00,
0x00,0x00,0x70,0xD8,0xF8,0xC0,0x70,0x00,
0x18,0x30,0x30,0x78,0x30,0x30,0x30,0x30,
0x00,0x00,0x78,0xD8,0xD8,0x78,0x18,0x70,
0xC0,0xC0,0xF0,0xD8,0xD8,0xD8,0xD8,0x00,
0x30,0x00,0x70,0x30,0x30,0x30,0x30,0x00,
0x30,0x00,0x70,0x30,0x30,0x30,0x30,0x60,
0xC0,0xC0,0xD8,0xF0,0xE0,0xF0,0xD8,0x00,
0x70,0x30,0x30,0x30,0x30,0x30,0x30,0x00,
0x00,0x00,0xD0,0xF8,0xF8,0xD8,0xD8,0x00,
0x00,0x00,0xF0,0xD8,0xD8,0xD8,0xD8,0x00,
0x00,0x00,0x70,0xD8,0xD8,0xD8,0x70,0x00,
0x00,0x00,0xF0,0xD8,0xD8,0xF0,0xC0,0xC0,
0x00,0x00,0x78,0xD8,0xD8,0x78,0x18,0x18,
0x00,0x00,0xD8,0xE0,0xC0,0xC0,0xC0,0x00,
0x00,0x00,0x78,0xC0,0x70,0x18,0xF0,0x00,
0x10,0x30,0x78,0x30,0x30,0x30,0x18,0x00,
0x00,0x00,0xD8,0xD8,0xD8,0xD8,0x78,0x00,
0x00,0x00,0xD8,0xD8,0xD8,0x70,0x20,0x00,
0x00,0x00,0xD8,0xD8,0xF8,0x70,0x50,0x00,
0x00,0x00,0xD8,0x70,0x70,0xD8,0xD8,0x00,
0x00,0x00,0xD8,0xD8,0xD8,0x78,0x30,0x60,
0x00,0x00,0xF8,0x30,0x60,0xC0,0xF8,0x00,
0x18,0x30,0x30,0x60,0x30,0x30,0x18,0x00,
0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,
0x60,0x30,0x30,0x18,0x30,0x30,0x60,0x00,
0x68,0xB0,0x00,0x00,0x00,0x00,0x00,0x00,
0xA8,0x54,0xA8,0x54,0xA8,0x54,0xA8,0x54,
};
