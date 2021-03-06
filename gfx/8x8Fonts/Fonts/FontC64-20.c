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
// Orignial name : "Metallion02.fnt"
// --------------------------------

u8 Font[] =  {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xEE,0xAA,0xAA,0xAA,0xEE,0x00,0xEE,0x00,
0xEE,0xAA,0xCC,0x88,0x00,0x00,0x00,0x00,
0x6C,0xFE,0xEE,0x44,0xEE,0xFE,0x6C,0x00,
0x18,0x7E,0x90,0x7C,0x12,0xFC,0x30,0x00,
0xC6,0xCA,0x14,0x28,0x50,0xA6,0xC6,0x00,
0x38,0x6C,0x68,0x76,0xDC,0xCC,0x76,0x00,
0x38,0x28,0x30,0x20,0x00,0x00,0x00,0x00,
0x1C,0x28,0x50,0x50,0x50,0x28,0x1C,0x00,
0x70,0x28,0x14,0x14,0x14,0x28,0x70,0x00,
0x92,0x6C,0x44,0x82,0x44,0x6C,0x92,0x00,
0x38,0x28,0xEE,0x82,0xEE,0x28,0x38,0x00,
0x00,0x00,0x00,0x00,0x38,0x28,0x30,0x20,
0x00,0x00,0x3E,0x42,0x7C,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x38,0x28,0x38,0x00,
0x06,0x0A,0x14,0x28,0x50,0xA0,0xC0,0x00,
0x1C,0x32,0x56,0x5A,0x52,0x52,0x3C,0x00,
0x18,0x18,0x28,0x28,0x28,0x38,0x7C,0x00,
0x7C,0x62,0x02,0x3C,0x50,0x50,0x7E,0x00,
0x7C,0x02,0x3C,0x0A,0x0A,0x0A,0x7C,0x00,
0xE4,0xA4,0xA4,0xFE,0x04,0x04,0x0E,0x00,
0x7E,0x50,0x7C,0x02,0x02,0x02,0x7C,0x00,
0x3E,0x50,0x5C,0x52,0x52,0x52,0x3C,0x00,
0x7E,0x66,0x0A,0x14,0x28,0x28,0x38,0x00,
0x3C,0x56,0x3C,0x52,0x52,0x52,0x3C,0x00,
0x3C,0x4A,0x4A,0x3A,0x0A,0x0A,0x7C,0x00,
0x38,0x28,0x38,0x00,0x38,0x28,0x38,0x00,
0x38,0x28,0x38,0x00,0x38,0x28,0x30,0x20,
0x1C,0x28,0x50,0xA0,0x50,0x28,0x1C,0x00,
0x00,0x7E,0x7E,0x00,0x7E,0x7E,0x00,0x00,
0x70,0x28,0x14,0x0A,0x14,0x28,0x70,0x00,
0xFC,0xEA,0x0A,0x34,0x38,0x00,0x38,0x00,
0x7C,0xA2,0xA2,0xAE,0xA0,0xA0,0x7E,0x00,
0x3E,0x52,0xA2,0xBE,0xA2,0xA2,0xE2,0x00,
0xF8,0xA4,0xA4,0xBE,0xA2,0xA2,0xFC,0x00,
0x3E,0x56,0xA0,0xA0,0xA0,0xA6,0x7E,0x00,
0xF8,0xAC,0xA6,0xA2,0xA2,0xA6,0xFC,0x00,
0x3E,0x56,0xA0,0xB8,0xA0,0xA6,0xFE,0x00,
0x3E,0x56,0xA0,0xB8,0xA0,0xA0,0xF0,0x00,
0x3E,0x56,0xA0,0xA6,0xA2,0xA2,0x7E,0x00,
0x22,0x62,0xA2,0xBE,0xA2,0xA2,0xE2,0x00,
0x7E,0x28,0x28,0x28,0x28,0x28,0xFC,0x00,
0x3E,0x2A,0x0A,0x0A,0x8A,0xCA,0x7C,0x00,
0x22,0x66,0xAC,0xB8,0xAC,0xA6,0xE2,0x00,
0x20,0x60,0xA0,0xA0,0xA0,0xA6,0xFE,0x00,
0x26,0x7E,0xAA,0xA2,0xA2,0xA2,0xE2,0x00,
0x22,0x62,0xB2,0xAA,0xA6,0xA2,0xE2,0x00,
0x3C,0x56,0xA2,0xA2,0xA2,0xA6,0x7C,0x00,
0x3C,0x62,0xA2,0xBC,0xA0,0xA0,0xF0,0x00,
0x3C,0x56,0xA2,0xA2,0xA2,0xA6,0x7C,0x06,
0x3C,0x52,0xA2,0xBC,0xA2,0xA2,0xE2,0x00,
0x3E,0x50,0xA0,0x7C,0x06,0x06,0xFC,0x00,
0xFE,0xAA,0x28,0x28,0x28,0x28,0x7C,0x00,
0x22,0x62,0xA2,0xA2,0xA2,0xA2,0x7E,0x00,
0x22,0x66,0xA4,0xAC,0xA8,0xB8,0x70,0x00,
0x22,0x62,0xA2,0xA2,0xAA,0xB6,0xE2,0x00,
0xC1,0xA2,0x54,0x28,0x54,0xA2,0xC1,0x00,
0x22,0x62,0xA2,0x7E,0x02,0x02,0xFC,0x00,
0xFE,0x8A,0x14,0x28,0x50,0xA2,0xFE,0x00,
0x3C,0x28,0x28,0x28,0x28,0x28,0x3C,0x00,
0xC0,0xA0,0x50,0x28,0x14,0x0A,0x06,0x00,
0x3C,0x14,0x14,0x14,0x14,0x14,0x3C,0x00,
0x10,0x28,0x54,0xEE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0x1C,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x3C,0x52,0x5E,0x52,0x72,0x00,
0x00,0xC0,0x7C,0x52,0x52,0x52,0x7C,0x00,
0x00,0x00,0x3E,0x52,0x50,0x52,0x3E,0x00,
0x00,0x03,0x3E,0x52,0x52,0x52,0x3E,0x00,
0x00,0x00,0x3C,0x52,0x5E,0x50,0x3E,0x00,
0x00,0x1E,0x28,0x28,0x6C,0x28,0x7C,0x00,
0x00,0x00,0x3E,0x52,0x52,0x3E,0x02,0x7C,
0x00,0xC0,0x7C,0x52,0x52,0x52,0x72,0x00,
0x00,0x30,0x00,0x78,0x28,0x28,0x38,0x00,
0x00,0x18,0x00,0x3C,0x14,0x14,0xD4,0x78,
0x00,0xE0,0xA6,0xAC,0xB8,0xAC,0xE6,0x00,
0x00,0x38,0x28,0x28,0x28,0x28,0x3C,0x00,
0x00,0x00,0xF4,0xBE,0xAA,0xA2,0xE2,0x00,
0x00,0x00,0x7C,0x52,0x52,0x52,0x72,0x00,
0x00,0x00,0x3C,0x52,0x52,0x52,0x3C,0x00,
0x00,0x00,0xFC,0x52,0x52,0x5C,0x50,0x70,
0x00,0x00,0x7E,0xA4,0xA4,0x7C,0x04,0x04,
0x00,0x00,0xFC,0x56,0x50,0x50,0x70,0x00,
0x00,0x00,0x3E,0x50,0x3C,0x0A,0x7C,0x00,
0x00,0x10,0x30,0x58,0x50,0x54,0x38,0x00,
0x00,0x00,0x72,0x52,0x52,0x52,0x3E,0x00,
0x00,0x00,0x72,0x56,0x5C,0x48,0x30,0x00,
0x00,0x00,0xE2,0xA2,0xAA,0xBE,0x74,0x00,
0x00,0x00,0xC6,0xAC,0x58,0xAC,0xC6,0x00,
0x00,0x00,0x72,0x52,0x52,0x2C,0x18,0x70,
0x00,0x00,0x7E,0x14,0x28,0x52,0x7E,0x00,
0x0E,0x14,0x14,0x28,0x14,0x14,0x0E,0x00,
0x38,0x28,0x28,0x28,0x28,0x28,0x38,0x00,
0x70,0x28,0x28,0x14,0x28,0x28,0x70,0x00,
0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00,
0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,
};
