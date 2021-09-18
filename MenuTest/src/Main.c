// Main.cpp : Start point
//
//#include "GameManager.h"
#include "gl/glut.h"
#include "pad.h"

extern int menu_update();
extern int menu_active;
extern void scrolling_update();
extern void scrolling_display();
extern void set_machine_menu(int machine);
extern char path_root[2048];
extern char path_games[2048];
extern void file_explorer_frame();
extern int is_paused;
extern void draw_msg(char* a);
extern int aboutwin_active;
extern int infowin_active;
extern int file_explorer_active;
extern void aboutwin_frame();
extern void infowin_frame();

int INTERFACE_UPDATETIME = 15; // milliseconds
int TotalMillisecondSinceLastUpdate=0;

// -----------------------------------------------------------------
// Name: glutMainKeyboard
// catch button pressed
// -----------------------------------------------------------------
void glutMainKeyboard(unsigned char key, int x, int y)
{

/*
#define PAD_START    PSP_CTRL_START
#define PAD_SELECT   PSP_CTRL_SELECT
#define PAD_TRIANGLE PSP_CTRL_TRIANGLE
#define PAD_CROSS    PSP_CTRL_CROSS
#define PAD_SQUARE   PSP_CTRL_SQUARE
#define PAD_CIRCLE   PSP_CTRL_CIRCLE
*/
  //printf("%d\n", key);

  switch(key)
  {
    case 27:
      exit(0);
      break;

    //space
    case 32:
      paddata |= PAD_SELECT;
      break;
    // w || W
    case 119:
    case 87:
      paddata |= PAD_CROSS;
      break;
     // x || X
    case 120:
    case 88:
      paddata |= PAD_SQUARE;
      break;      
    // c || C
    case 99:
    case 67:
      paddata |= PAD_CIRCLE;
      break;
    // v
    case 118:
      paddata |= PAD_TRIANGLE;
    break;
  }
  
}

// -----------------------------------------------------------------
// Name: glutMainUpKeyboard
// catch current button released
// -----------------------------------------------------------------
void glutMainUpKeyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 32:
      paddata ^= PAD_SELECT;
      break;
    // w || W
    case 119:
    case 87:
      paddata ^= PAD_CROSS;
      break;
     // x || X
    case 120:
    case 88:
      paddata ^= PAD_SQUARE;
      break;      
    // c || C
    case 99:
    case 67:
      paddata ^= PAD_CIRCLE;
      break;
    // v
    case 118:
      paddata ^= PAD_TRIANGLE;
    break;
  }
  
}

// -----------------------------------------------------------------
// Name: glutSpecialKeyboard
// catch special key pressed
// -----------------------------------------------------------------
void glutSpecialKeyboard(int key, int x, int y)
{
  switch(key)
  {
    case GLUT_KEY_LEFT:
      paddata |= PAD_LEFT;
      break;
    case GLUT_KEY_RIGHT:
      paddata |= PAD_RIGHT;
      break;
    case GLUT_KEY_UP:
      paddata |= PAD_UP;
      break;
    case GLUT_KEY_DOWN:
      paddata |= PAD_DOWN;
      break;
  }
}

// -----------------------------------------------------------------
// Name: glutSpecialUpKeyboard
// Catch button released
// -----------------------------------------------------------------
void glutSpecialUpKeyboard(int key, int x, int y)
{
 
  switch(key)
  {
    case GLUT_KEY_LEFT:
      paddata ^= PAD_LEFT;
      break;
    case GLUT_KEY_RIGHT:
      paddata ^= PAD_RIGHT;
      break;
    case GLUT_KEY_UP:
      paddata ^= PAD_UP;
      break;
    case GLUT_KEY_DOWN:
      paddata ^= PAD_DOWN;
      break;
  }
  
}

// -----------------------------------------------------------------
// Name: glutMouseClick
// -----------------------------------------------------------------
void glutMouseClick(int button, int state, int x, int y)
{
  /*switch (button)
  {
  case GLUT_LEFT_BUTTON:
    {
      if (state == GLUT_DOWN)
        g_inputData.mFire1 = true;
      else if (state == GLUT_UP)
        g_inputData.mFire1 = false;
    break;
    }
  case GLUT_RIGHT_BUTTON:
    break;
  }*/
}

// -----------------------------------------------------------------
// Name: glutMouseMove
// 
// -----------------------------------------------------------------
void glutMouseMove(int x, int y)
{
  //g_inputData.mCursorPos.x = x;
  //g_inputData.mCursorPos.y = y - SCREENYRES;
}




// -----------------------------------------------------------------
// Name : 
// Update and ask for Display
// -----------------------------------------------------------------
void timerFunc(int nValue) // Updated each 17ms
{
  glutPostRedisplay();

  //if ( gameState > 4 )
  //  cGameManager::getInstance()->Update();

  glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);
}

// -----------------------------------------------------------------
// Name : 
// -----------------------------------------------------------------
void Display()
{
  long delta = TotalMillisecondSinceLastUpdate;
  TotalMillisecondSinceLastUpdate = glutGet(GLUT_ELAPSED_TIME);
  
  // Update Inputs
  new_pad = paddata & ~old_pad;
  new_rel = ~paddata & old_pad;
  old_pad = paddata; 

  // clear
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  if ( !file_explorer_active)
  {
    menu_update();
  }

  if ( menu_active )
  {
    scrolling_update();
    scrolling_display();
  }
  // Draw message "Pause"
  if ( (is_paused) && (menu_active==0) )
  {
    draw_msg("PAUSED");
    //pspaudio_clear();
  }
  else if (aboutwin_active)
  {
    aboutwin_frame();
  } 
  else if (infowin_active)
  {
    infowin_frame();
  }
  else if (file_explorer_active)
  {
    file_explorer_frame();
  } 

  // finish
  glutSwapBuffers();
}

// -----------------------------------------------------------------
// Name : 
// -----------------------------------------------------------------
void cleanUp()
{
}

// -----------------------------------------------------------------
// Name : 
// -----------------------------------------------------------------
void init()
{
  glClearColor(0.1f,0.1f,0.4f,1.0f);
  gluOrtho2D(0, 480, 272, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);   
  //glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  //glClearDepth(1.0f);                    // Configuration du tampon de profondeur
  //glDepthFunc(GL_LEQUAL);                // Le type de test de profondeur a faire

  // To enter menu, press SELECT
  set_machine_menu(0);

  paddata = 0;
  new_pad = 0;
  new_rel = 0;
  old_pad = 0; 

  strcpy(path_root,"D:/Donnees/Utils/Cygwin/usr/local/pspdev/psp/sdk/samples/PSPVice/__SCE__PSPVice/");
  strcpy(path_games,"D:/Donnees/Utils/Cygwin/usr/local/pspdev/psp/sdk/samples/PSPVice/__SCE__PSPVice/games/");
}


// -----------------------------------------------------------------
// Name : 
// -----------------------------------------------------------------
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  //glutInitWindowPosition(0,0);
  glutInitWindowSize(480, 272);
  glutCreateWindow("Menu PC");
  glutKeyboardFunc(glutMainKeyboard);
  glutKeyboardUpFunc(glutMainUpKeyboard);
  glutSpecialFunc(glutSpecialKeyboard);
  glutSpecialUpFunc(glutSpecialUpKeyboard);
  glutDisplayFunc(Display);
  init();
  glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);
  glutMainLoop();
	return 0;
}
