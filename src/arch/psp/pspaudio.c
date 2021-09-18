#include <pspkernel.h>
#include <pspaudio.h>

#include <stdlib.h>
#include <string.h>

#include "pspaudio.h"

#define NUM_AUDIO_BUFFERS 16
#define AUDIO_BUFFER_SIZE 1024	//	psp buffers must be multiples of 64

const int num_audio_buffs = NUM_AUDIO_BUFFERS;
const int audio_buff_size = AUDIO_BUFFER_SIZE;

//	the buffer
short audio_buff[2 * AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS];

//	Frequency is 44100, Only 44100 is supported
// 44100*2 bytes per second. 
// 90 bytes for a ms
// 6000 bytes for 66 ms (both 2 voices)
// One buffer is 2048 byte. So to create a stereo effect, we copy 3 buffers ahead.

int audio_handle = -1;
int audio_sema = -1;
static int audio_thread = -1;

int playing_buffer = 0;
int write_offset = 0;

int audio_active = 1;
int previous_audio_active;

static int buffered_samples = 0 ;

int audio_effect=0; // Software echo. 0=None 1=Stereo 2=Echo

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
void pspaudio_SetEffect(int value)
{
  audio_effect=value;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
int AudioThreadFunc(int args, void* argp)
{
  //int cacheflushasked=0;

	//while (audio_active)
	while (1) // Always active
	{
	  /*
 	  if ( cacheflushasked)
 	  {
 	    sceKernelDcacheWritebackAll();
 	    cacheflushasked = 0;
 	  }
 	  */
		//if (audio_active)
		
		if ( previous_audio_active == 1 && audio_active == 0 ) // Sound has been disable
		{
		  pspaudio_clear();
		  previous_audio_active=0;
		}
		
		if (1)
		{
  		sceAudioOutputPannedBlocking(audio_handle, PSP_AUDIO_VOLUME_MAX, PSP_AUDIO_VOLUME_MAX, &audio_buff[playing_buffer * 2 * AUDIO_BUFFER_SIZE]);

      // -- Check if buffered samples are more than 3 frames ahead
      if ( buffered_samples >= 4*AUDIO_BUFFER_SIZE )
      {
        buffered_samples -= 2*AUDIO_BUFFER_SIZE; // If yes, then skip one frame
        
        playing_buffer+=2;
        
    		if (playing_buffer >= NUM_AUDIO_BUFFERS)
    			playing_buffer -= NUM_AUDIO_BUFFERS;
    			
    		//cacheflushasked=1;
      }
      else if ( buffered_samples >= AUDIO_BUFFER_SIZE ) // If buffer is one or two frame ahead, then play next buffer
      {
        buffered_samples -= AUDIO_BUFFER_SIZE;
        
        playing_buffer++;
        
    		if (playing_buffer >= NUM_AUDIO_BUFFERS)
    			playing_buffer -= NUM_AUDIO_BUFFERS;
      }
  			
  	  //cacheflushasked=1;
    }
    // -- If not enough samples, then play again same buffer

	}
	sceKernelExitThread(0);
	return 0;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
void pspaudio_init(void)
{
  if ( audio_thread == -1)
  {
  	pspaudio_clear();

  	if (audio_handle == -1)
  		audio_handle = sceAudioChReserve(-1, AUDIO_BUFFER_SIZE, 0);

    audio_thread = sceKernelCreateThread("audio_thread", (void*)AudioThreadFunc, 0x12, 0x10000, 0, 0);
    if (audio_thread >= 0)
  		sceKernelStartThread(audio_thread, 0, 0);

  	if (audio_sema == -1)
  		audio_sema = sceKernelCreateSema("audio_buffer_1", 0, 1, 1, 0);	// breaks without this, why?..
  }

  audio_active=1;
  previous_audio_active=1;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
void pspaudio_clear(void)
{
	int buff;
	for (buff = 0; buff < NUM_AUDIO_BUFFERS; buff++)
		memset(&audio_buff[buff * 2 * AUDIO_BUFFER_SIZE], 0, 2 * AUDIO_BUFFER_SIZE * sizeof(short) );
}

// ------------------------------------------------------------------------------
// Input sound is 22K
// ------------------------------------------------------------------------------
void pspaudio_write(short* buffer, int num_samples)
{
	if (!audio_active)
	  return;
	  
	previous_audio_active=1; // Keep old state, to be able to get transition
	
	buffered_samples += num_samples;
		
	int i;

  if ( audio_effect == 0 ) // No stereo effect
  {
    // No effect
    unsigned int* bufint;
    bufint = (unsigned int*) (&audio_buff[write_offset]);

    // Half optimised version
  	for (i = 0; i < num_samples; i++)
  	{
  		unsigned short s = (unsigned short) buffer[i];
  		unsigned int sint;
  		sint = ( s | (s<<16) );
      *bufint=sint;
  		write_offset+=2;
  		bufint++;

  		if (write_offset == 2 * AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS)
  		{
  			write_offset = 0;
        bufint = (unsigned int*) &audio_buff[write_offset];			
  	  }
  	}
  }
  else
  {
    // Stereo effect
    unsigned short* bufintl;
    unsigned short* bufintr;
    int write_offsetr;
    bufintl = (unsigned short*) (&audio_buff[write_offset]);
    //write_offsetr = (write_offset+AUDIO_BUFFER_SIZE*2*2)+1;
    if ( audio_effect == 1)
      write_offsetr = (write_offset+AUDIO_BUFFER_SIZE*2*2)+1; // Stereo, 2 buffers ahead
    else if ( audio_effect == 2)
      write_offsetr = (write_offset+AUDIO_BUFFER_SIZE*5*2)+1; // Stereo, 5 buffers ahead
  	if (write_offsetr >= (2*AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS)+1)
  	{
  		write_offsetr -= ((2*AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS));
    }
    bufintr = (unsigned short*) (&audio_buff[write_offsetr]);
    // Half optimised version
  	for (i = 0; i < num_samples; i++)
  	{
  		unsigned short s = (unsigned short) buffer[i];
      *bufintl=s;
      *bufintr=s;
  		write_offset+=2;
  		write_offsetr+=2;
  		bufintl+=2;
  		bufintr+=2;
  		if (write_offset == 2 * AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS)
  		{
  			write_offset = 0;
        bufintl = (unsigned short*) &audio_buff[write_offset];			
  	  }
  		if (write_offsetr == ((2*AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS)+1))
  		{
  			write_offsetr = 1;
        bufintr = (unsigned short*) &audio_buff[write_offsetr];			
  	  }
  	}
  }


  // Unoptimised version
	/*
	for (i = 0; i < num_samples; i++)
	{
		short s = buffer[i];
		//	l+r
		audio_buff[write_offset++] = s;
		audio_buff[write_offset++] = s;

		if (write_offset == 2 * AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS)
			write_offset = 0;
	}
	*/

  // Optimisez version (do not work well)
  /* 
  int numsample1;
  int numsample2; // If this is !=0, then that means we reach end of buffer at first pass and need to copy again
  
  numsample1 = num_samples;
  numsample2=0;
  
  if ( write_offset+2*num_samples >= 2 * AUDIO_BUFFER_SIZE * NUM_AUDIO_BUFFERS )
  {
    numsample1 = ( (2*AUDIO_BUFFER_SIZE*NUM_AUDIO_BUFFERS)-write_offset)/2;
    numsample2 = num_samples - numsample1;
  }
  
  short* pSrc;
  short* pDst;
  
  pSrc = (short*)(((int)buffer) | 0x40000000);
  pDst = (short*)(((int)&audio_buff[write_offset]) | 0x40000000); 

  short s;

	for (i = 0; i < numsample1; i++)
	{
	  s = *pSrc++;
		*pDst++ = s;
		*pDst++ = s;
	}
	
	write_offset+=numsample1*2;
	
	if ( numsample2 != 0 )
	{
	  short* pSrc;
	  short* pDst;
	  write_offset=0;
	  pSrc = (short*)(((int)&buffer[numsample1]) | 0x40000000);
	  pDst = (short*)(((int)audio_buff) | 0x40000000); 
  
    short s;

		for (i = 0; i < numsample2; i++)
		{
		  s = *pSrc++;
			*pDst++ = s;
			*pDst++ = s;
		}
		write_offset+=numsample2*2;
	}
  */

	return;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
void pspaudio_shutdown(void)
{
	//	signal thread to end
	audio_active = 0;
	pspaudio_clear();
	//audio_thread = 0; // Never stop thread
}
