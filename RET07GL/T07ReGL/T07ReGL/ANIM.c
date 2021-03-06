/*
* FILE NAME: anim.c
* PROGRAMMER: bg3
* DATE: 06.06.2013
* PURPOSE: Animation system core
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "def.h"
#include "anim.h"
#include "units.h"
#include "keycodes.h"

#pragma comment(lib, "winmm")
#include <mmsystem.h>

/* Animation context */
static bg3ANIM BG3_ANIM;
static BOOL BG3_IsInit;

/* Animation system functions */
BOOL BG3_AnimInit( HWND hWnd )
{
  PIXELFORMATDESCRIPTOR pfd = {0};
  LARGE_INTEGER timeFreq;
  LARGE_INTEGER time;
  POINT cursorPoint;
  INT i;
  HDC winDC;
  // Create context
  BG3_ANIM.hWnd = hWnd;
  winDC = GetDC(hWnd);
  BG3_ANIM.hDC = winDC;

  /* Ñîçäàíèå ñöåíû */

  /* OPEN GL */
  /*** èíèöèàëèçàöèÿ OpenGL ***/
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 24;
  pfd.cStencilBits = 0;

  i = ChoosePixelFormat(BG3_ANIM.hDC, &pfd);
  DescribePixelFormat(BG3_ANIM.hDC, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  SetPixelFormat(BG3_ANIM.hDC, i, &pfd);

  BG3_ANIM.hGLRC = wglCreateContext(BG3_ANIM.hDC);
  wglMakeCurrent(BG3_ANIM.hDC, BG3_ANIM.hGLRC);
  ReleaseDC(BG3_ANIM.hWnd, BG3_ANIM.hDC);

  /* óñòàíîâêà áàçîâûõ ïàðàìåòðîâ OpenGL */
  if (glewInit() != GLEW_OK)
  {
    ReleaseDC(BG3_ANIM.hWnd, BG3_ANIM.hDC);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(BG3_ANIM.hGLRC);
    memset(&BG3_ANIM, 0, sizeof(BG3_ANIM));
    return FALSE;
  }
  glClearColor(0.3, 0.5, 0.7, 1);
  
  BG3_SceneCreate(&BG3_ANIM);

  /* Get input state */
  memset(&BG3_ANIM.keyState, 0, sizeof(BG3_ANIM.keyState));
  memset(&BG3_ANIM.mouse, 0, sizeof(BG3_ANIM.mouse));
  memset(&BG3_ANIM.timer, 0, sizeof(BG3_ANIM.timer));
  memset(&BG3_ANIM.joy, 0, sizeof(BG3_ANIM.joy));

  /* Camera */
  BG3_ANIM.camera.Wh = 2;
  BG3_ANIM.camera.Hh = 2;
  BG3_ANIM.camera.PD = 1;
  BG3_ANIM.camera.projMatr = MatrFrustum(-BG3_ANIM.camera.Wh/2, BG3_ANIM.camera.Wh/2, -BG3_ANIM.camera.Hh/2, BG3_ANIM.camera.Hh/2, 1, 10000);

  /* Mouse position */
  GetCursorPos(&cursorPoint);
  // Convert to window
  ScreenToClient(hWnd, &cursorPoint);
  BG3_ANIM.mouse.x = cursorPoint.x;
  BG3_ANIM.mouse.y = cursorPoint.y;

  /* Timer state */
  QueryPerformanceFrequency(&timeFreq);
  QueryPerformanceCounter(&time);
  BG3_ANIM.timer.animStart = ((DBL)time.QuadPart / timeFreq.QuadPart);

  memset(&BG3_ANIM.mouse, 0, sizeof(BG3_ANIM.mouse));

  // Init units
  for (i = 0; i < BG3_ANIM.unitsNum; i++)
  {
BG3_ANIM.units[i]->Init(BG3_ANIM.units[i], &BG3_ANIM);
  }

  BG3_IsInit = TRUE;
  return TRUE;
}

VOID BG3_AnimClose( VOID )
{
  INT i;
  // Destroy buffer
  ReleaseDC(BG3_ANIM.hWnd, BG3_ANIM.hDC);
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(BG3_ANIM.hGLRC);
  
  // Close units
  for (i = 0; i < BG3_ANIM.unitsNum; i++)
  {
    BG3_ANIM.units[i]->Close(BG3_ANIM.units[i]);
  }

  BG3_IsInit = FALSE;
}


VOID BG3_AnimWheel( INT delta )
{
  BG3_ANIM.mouse.deltaWheel += delta;
}

VOID BG3_AnimResize( INT W, INT H )
{
  if (!BG3_IsInit)
    return;

  BG3_ANIM.W = W;
  BG3_ANIM.H = H;
  glViewport(0, 0, W, H);

  BG3_AnimRender();
}

VOID BG3_AnimRender( VOID )
{
  char buffer[1000];
  POINT cursorPoint;
  DBL timeSec;
  LARGE_INTEGER time, timeFreq;
  INT i;

  if (!BG3_IsInit)
    return;

  /* Update timer */
  QueryPerformanceFrequency(&timeFreq);
  QueryPerformanceCounter(&time);
  timeSec = (DBL)time.QuadPart / timeFreq.QuadPart;
  BG3_ANIM.timer.deltaGlobalTime = timeSec - BG3_ANIM.timer.animStart - BG3_ANIM.timer.globalTime;
  BG3_ANIM.timer.deltaActiveTime = BG3_ANIM.isPaused ? 0 : BG3_ANIM.timer.deltaGlobalTime;
  BG3_ANIM.timer.activeTime += BG3_ANIM.timer.deltaActiveTime;
  BG3_ANIM.timer.globalTime = timeSec - BG3_ANIM.timer.animStart;
  /* FPS Counter update */
  BG3_ANIM.timer.fpsTime += BG3_ANIM.timer.deltaGlobalTime;
  if (BG3_ANIM.timer.fpsTime > 1)
  {
    BG3_ANIM.timer.fpsTime = 0;
    BG3_ANIM.lastFPS = BG3_ANIM.fpsCounter;
    BG3_ANIM.fpsCounter = 0;
  }

  /* Update input state */
  // Keyboard
  memcpy(BG3_ANIM.keyState.old, BG3_ANIM.keyState.actual, sizeof(BG3_ANIM.keyState.actual));
  memset(BG3_ANIM.keyState.actual, 0, sizeof(BG3_ANIM.keyState.actual));
  GetKeyboardState(BG3_ANIM.keyState.actual);
  for (i = 0; i < 256; i++)
    BG3_ANIM.keyState.actual[i] >>= 7;
  // Joy
  if (joyGetNumDevs() > 1)
  {
    // Joy exists
    JOYCAPS jc;
    if (joyGetDevCaps(JOYSTICKID1, &jc, sizeof(jc)) == JOYERR_NOERROR)
    {
      JOYINFOEX ji;
      ji.dwSize = sizeof(ji);
      ji.dwFlags = JOY_RETURNALL;

      if (joyGetPosEx(JOYSTICKID1, &ji) == JOYERR_NOERROR)
      {
        // Buttons
        memcpy(BG3_ANIM.joy.buttonsOld, BG3_ANIM.joy.buttons, sizeof(BG3_ANIM.joy.buttons));
        for (i = 0; i < 32; i++)
        {
          BG3_ANIM.joy.buttons[i] = (ji.dwButtons >> i) & 1;
        }
        // Axis
        BG3_ANIM.joy.x = 2.0 * (ji.dwXpos - jc.wXmin) / (jc.wXmax - jc.wXmin) - 1;
        BG3_ANIM.joy.y = 2.0 * (ji.dwYpos - jc.wYmin) / (jc.wYmax - jc.wYmin) - 1;
        BG3_ANIM.joy.z = 2.0 * (ji.dwZpos - jc.wZmin) / (jc.wZmax - jc.wZmin) - 1;
        BG3_ANIM.joy.r = 2.0 * (ji.dwRpos - jc.wRmin) / (jc.wRmax - jc.wRmin) - 1;

        // PoV
        if (ji.dwPOV == 0xFFFF)
          BG3_ANIM.joy.pov = 0;
        else
          BG3_ANIM.joy.pov = ji.dwPOV / 4500 + 1;
      }
    }
  }
  // Mouse
  GetCursorPos(&cursorPoint);
  ScreenToClient(BG3_ANIM.hWnd, &cursorPoint);
  BG3_ANIM.mouse.deltaX = cursorPoint.x - BG3_ANIM.mouse.x;
  BG3_ANIM.mouse.deltaY = cursorPoint.y - BG3_ANIM.mouse.y;
  BG3_ANIM.mouse.x = cursorPoint.x;
  BG3_ANIM.mouse.y = cursorPoint.y;
  BG3_ANIM.mouse.deltaWheel = 0;

  /* Update units state */
  BG3_SceneResponse(&BG3_ANIM);
  for (i = 0; i < BG3_ANIM.unitsNum; i++)
    BG3_ANIM.units[i]->Response(BG3_ANIM.units[i], &BG3_ANIM);

  /* Erase background */
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  /* Render units */
  for (i = 0; i < BG3_ANIM.unitsNum; i++)
  {
    BG3_ANIM.units[i]->Render(BG3_ANIM.units[i], &BG3_ANIM);
  }
  glFinish();

  sprintf(buffer, "FPS: %d Global Time: %lf Active Time: %lf Wheel: %d MX: %d MY: %d", BG3_ANIM.lastFPS,
    BG3_ANIM.timer.globalTime, BG3_ANIM.timer.activeTime, BG3_ANIM.mouse.deltaWheel, BG3_ANIM.mouse.x, BG3_ANIM.mouse.y);
  SetWindowText(BG3_ANIM.hWnd, buffer);

  //BG3_ANIM.fpsCounter++;
  //BG3_ANIM.mouse.deltaWheel = 0;
}

VOID BG3_AnimCopyFrame( VOID )
{
  HDC hDC;
   if (!BG3_IsInit)
    return;
  hDC = GetDC(BG3_ANIM.hWnd);
  SwapBuffers(hDC);
  //BitBlt(hDC, 0, 0, BG3_ANIM.W, BG3_ANIM.H, BG3_ANIM.hDC, 0, 0, SRCCOPY);
  ReleaseDC(BG3_ANIM.hWnd, hDC);
  BG3_ANIM.fpsCounter++;
}

 /*
* Default unit methods implementations
*/
static VOID bg3UNIT_DINIT( bg3UNIT *Unit, bg3ANIM *Ani )
{
}
static VOID bg3UNIT_DCLOSE( bg3UNIT *Unit )
{
}
static VOID bg3UNIT_DRESPONSE( bg3UNIT *Unit, bg3ANIM *Ani )
{
}
static VOID bg3UNIT_DRENDER( bg3UNIT *Unit, bg3ANIM *Ani )
{
}

/* Create default unit */
bg3UNIT * BG3_UnitCreate( INT Size )
{
  bg3UNIT * unit = malloc(Size);
  memset(unit, 0, Size);
  unit->Init = bg3UNIT_DINIT;
  unit->Close = bg3UNIT_DCLOSE;
  unit->Response = bg3UNIT_DRESPONSE;
  unit->Render = bg3UNIT_DRENDER;
  return unit;
}

/* Put the unit to scene */
VOID BG3_AnimAdd( bg3UNIT *Unit )
{
  BG3_ANIM.units[BG3_ANIM.unitsNum++] = Unit;
}

BYTE keytable[256];
VOID InitKeytable()
{
  INT i;
  for (i = 0; i < 256; i++)
  {
    keytable[i] = i;
  }
  
  keytable[KEY_CTRL] = VK_CONTROL;
  keytable[KEY_ALT] = VK_LMENU;
  keytable[KEY_PGUP] = VK_NEXT;
  keytable[KEY_ENTER] = VK_RETURN;
  keytable[KEY_SPACE] = VK_SPACE;
  keytable[KEY_TAB] = VK_TAB;
}

/* Is pressed now? */
BOOL BG3_KeyPressed(BYTE key)
{
  return BG3_ANIM.keyState.actual[keytable[key]];
}

/* Is pressed before? */
BOOL BG3_KeyOldPressed(BYTE key)
{
  return BG3_ANIM.keyState.old[keytable[key]];
}