#include <png.h>
#include "malloc.h"
#ifdef WIN32
#include "Win32PSPSpecific.h"
#else
#include "PSPSpecific.h"
#endif
#include "PSPFileSystem.h" 

#include "PngImage.h"

extern unsigned int clut_face[16];
extern int PSPViceVideoMode;

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
  // ignore PNG warnings
} 

// ------------------------------------------------------------
// Name : PngLoadImage
// Load image sizex x sizey     (screenshots are 160x100 or 320x200)
// Result image is 8888 (32bits)
// ------------------------------------------------------------
int PngLoadImage(char* filename, unsigned int* dest, int* outsizex, int* outsizey)
{
  //int         bufferwidth;
  //int         pixelformat;
  int         unknown;
  png_structp png_ptr;
  png_infop   info_ptr;
  unsigned int sig_read = 0;
  png_uint_32 width, height;
  int         bit_depth, color_type, interlace_type, x, y;
  unsigned int* line;
  FILE*       fp;

  printf("Png image load %s\n", filename);

  if ((fp = fopen(filename, "rb")) == NULL) // Searching into big zip file can be long
    return 0;

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png_ptr == NULL)
  {
    fclose(fp);
    return 0;
  }

  png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
  info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == NULL)
  {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, sig_read);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

  //printf("Size of picture %d %d\n", width, height);

  // Check if size is ok
  if ( (width>320) || (height>200) ) // Max size is 320x200
  {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }

  *outsizex=width;
  *outsizey=height;

  png_set_strip_16(png_ptr);
  png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

  line = (unsigned int*) malloc(width * 4);
  if (!line)
  {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return 0;
  }

  // Read 160x100 picture
  if (width<256)
  {
    for (y = 0; y < height; y++)
    {
      png_read_row(png_ptr, (unsigned char*) line, png_bytep_NULL);
      for (x = 0; x < width; x++)
      {
        unsigned int color32 = line[x];
        int r = color32 & 0xff;
        int g = (color32 >> 8) & 0xff;
        int b = (color32 >> 16) & 0xff;
        int a = 0xff;
        color32 = r | (g << 8) | (b << 16) | (a<<24);
        dest[x + y * 160] = color32;
      }
      dest += 256-160; // jump over unused pixels on 256*256 texture
    }
  }
  else if (width==320 && height==200)
  {
    // Read 320x200 picture
    for (y = 0; y < height; y++)
    {
      png_read_row(png_ptr, (unsigned char*) line, png_bytep_NULL);
      for (x = 0; x < width; x++)
      {
        unsigned int color32 = line[x];
        int r = color32 & 0xff;
        int g = (color32 >> 8) & 0xff;
        int b = (color32 >> 16) & 0xff;
        int a = 0xff;
        color32 = r | (g << 8) | (b << 16) | (a<<24);
        dest[x + y * 320] = color32;
      }
      dest += 512-320; // jump over unused pixels on 256*256 texture
    }

    /*
    for (y = 0; y < height; y++)
    {
      png_read_row(png_ptr, (unsigned char*) line, png_bytep_NULL);
      if ( (y%2)== 0 )
      {
        for (x = 0; x < width; x++)
        {
          if ( (x%2)==0 )
          {
            unsigned int color32 = line[x];
            int r = color32 & 0xff;
            int g = (color32 >> 8) & 0xff;
            int b = (color32 >> 16) & 0xff;
            int a = 0xff;
            color32 = r | (g << 8) | (b << 16) | (a<<24);
            dest[x/2 + y/2 * sizex/2] = color32;
          }
        }
        dest += 256-sizex/2; // jump over unused pixels on 256*256 texture
      }
    }
    */
  }

  free(line);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
  fclose(fp);

  return 1; // true
}

// ------------------------------------------------------------
// Name : PngSaveImage
// Save frame buffer
// Size of screenshots must be 160x100 (half real size)
// (int*)clut_face)    // Colors 16 colors ARGB
// (byte*)ps2_videobuf // 16 colors, 2 pixels per bits. 512*512. 
// offsetx = 32.0f; offsety=35.0 (pal) 19 (ntsc)
// PSPViceVideoMode==0 (Means Pal) != 0 (Ntsc)
// 
// source ==> Should always be : ps2_videobuf	  
// ------------------------------------------------------------
void PngSaveImage(char* name, char* source, int sizex, int sizey)
{
  int unknown;
  int i, x, y;
  unsigned int color;
  unsigned int index;
  unsigned char r, g, b;
  int offsetx;
  int offsety;

  int* palette;

  png_structp png_ptr;
  png_infop info_ptr;

  FILE* fp;
  unsigned char* line;

  fp = fopen(name, "wb");
  if (!fp)
  {
    return;
  }
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    return;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    fclose(fp);
    return;
  }
  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, sizex, sizey,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  line = (unsigned char*) malloc(sizex * 3);

  palette = (int*) clut_face; // ARGB

  offsetx=32/2;
  if (PSPViceVideoMode==0)
    offsety=18; // 35/2
  else
    offsety=10; // 19/2

  for (y = 0; y < sizey; y++)
  {
    for (i = 0, x = 0; x < sizex; x++)
    {
      // Get pixel from buffer
      index=source[512*(offsety+y)+offsetx+x];
      color = palette[(index&0x000F)];
      r = color & 0xff; // Not logical but it works, so who cares :)
      g = (color >> 8) & 0xff;
      b = (color >> 16) & 0xff;
      line[i++] = r;
      line[i++] = g;
      line[i++] = b;
    }
    png_write_row(png_ptr, line);
  }
  free(line);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  fclose(fp);
}


// Debug

static int nbscreenshots=0;
// -----------------------------
void TakeScreenshot()
{
  // PNG Version, much faster
  int unknown;
  int i, x, y;
  png_structp png_ptr;
  png_infop info_ptr;
  FILE* fp;
  unsigned char* line;
  const int lineWidth = 512;	
  int sizex,sizey;
  unsigned int* vram;
  int bufferwidth;
  int pixelformat;
  unsigned int color;
  unsigned int index;
  unsigned char r, g, b;

  // Fill values
  char name[512];
  sprintf(name,"ms0:/PSPVice%d.png",nbscreenshots);
  nbscreenshots++;
  sizex=480;
  sizey=272;

  // Get Vram
  sceGuTexFlush();
  sceKernelDcacheWritebackAll();
  sceDisplayWaitVblankStart();
  sceDisplayGetFrameBuf((void**)&vram, &bufferwidth, &pixelformat, &unknown);

  // Save 
  fp = fopen(name, "wb");
  if (!fp)
  {
    return;
  }
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    return;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    fclose(fp);
    return;
  }
  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, sizex, sizey,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  line = (unsigned char*) malloc(sizex * 3);

  for (y = 0; y < sizey; y++)
  {
    for (i = 0, x = 0; x < sizex; x++)
    {
      // Get pixel from buffer
      color = vram[(y * lineWidth) + x];
      r = color & 0xff; // Not logical but it works, so who cares :)
      g = (color >> 8) & 0xff;
      b = (color >> 16) & 0xff;
      line[i++] = r;
      line[i++] = g;
      line[i++] = b;
    }
    png_write_row(png_ptr, line);
  }
  free(line);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  fclose(fp);
}


/*
#include <pspkernel.h>
// Take real PSP Screenshot
void writeByte(int fd, unsigned char data)
{
sceIoWrite(fd, &data, 1);
}
const char tgaHeader[] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int nbscreenshots=0;
// -----------------------------
void TakeScreenshot()
{
//A nice TGA screenshotter courtesy of
//Shine's snake.
const int width = 480;
const int lineWidth = 512;
const int height = 272;
int bufferwidth;
int pixelformat;
int unknown;
unsigned int* lineBuffer = (unsigned int*) malloc(width*4);
unsigned int* vram;
sceGuTexFlush();
sceKernelDcacheWritebackAll();
sceDisplayWaitVblankStart();
sceDisplayGetFrameBuf((void**)&vram, &bufferwidth, &pixelformat, &unknown);
int x, y;
char fullname[128];
sprintf(fullname,"ms0:/PSPVice%d.tga",nbscreenshots);
nbscreenshots++;
int fd = sceIoOpen(fullname, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
if (!fd)
return;
sceIoWrite(fd, tgaHeader, sizeof(tgaHeader));
writeByte(fd, width & 0xff);
writeByte(fd, width >> 8);
writeByte(fd, height & 0xff);
writeByte(fd, height >> 8);
writeByte(fd, 32);
writeByte(fd, 0);
unsigned char red = 0, green = 0, blue = 0;
for (y = height - 1; y >= 0; y--) {
int m =0;
for (x = 0; x < width; x++) {
unsigned int color = vram[(y * lineWidth) + x];
red = color & 0xff;
green = (color >> 8) & 0xff;
blue = (color >> 16) & 0xff;
lineBuffer[x] = (0xFF << 24) | (red<<16) | (green<<8) | blue;
}
sceIoWrite(fd, lineBuffer, width*4);
}
sceIoClose(fd);
} 
*/

