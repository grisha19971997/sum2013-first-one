#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "units.h"
#include "anim.h"
#include "def.h"
#include "vec.h"

#define WND_CLASS_NAME "My window class"

/* Ññûëêè âïåðåä */
LRESULT CALLBACK MyWindowFunc( HWND hWnd, UINT Msg,
                               WPARAM wParam, LPARAM lParam );
void redraw(HDC hdc);


INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    CHAR *CmdLine, INT ShowCmd )
{
  WNDCLASS wc;
  HWND hWnd;
  MSG msg;

  wc.style = CS_VREDRAW | CS_HREDRAW; /* Ñòèëü îêíà: ïîëíîñòüþ ïåðåðèñîâûâàòü
* ïðè èçìåíåíèè âåðòèêàëüíîãî èëè
* ãîðèçîíòàëüíîãî ðàçìåðîâ */
  wc.cbClsExtra = 0; /* Äîïîëíèòåëüíîå êîëè÷åñòâî áàéò äëÿ êëàññà */
  wc.cbWndExtra = 0; /* Äîïîëíèòåëüíîå êîëè÷åñòâî áàéò äëÿ îêíà */
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW; /* Ôîíîâûé öâåò - âûáðàííûé â ñèñòåìå */
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); /* Çàãðóçêà êóðñîðà (ñèñòåìíîãî) */
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Çàãðóçêà ïèêòîãðàììû (ñèñòåìíîé) */
  wc.lpszMenuName = NULL; /* Èìÿ ðåñóðñà ìåíþ */
  wc.hInstance = hInstance; /* Äåñêðèïòîð ïðèëîæåíèÿ, ðåãèñòðèðóþùåãî êëàññ */
  wc.lpfnWndProc = MyWindowFunc; /* Óêàçàòåëü íà ôóíêöèþ îáðàáîòêè */
  wc.lpszClassName = WND_CLASS_NAME; /* Èìÿ êëàññà */

  /* Ðåãèñòðàöèÿ êëàññà â ñèñòåìå */
  if (!RegisterClass(&wc))
  {
    MessageBox(NULL, "Error register window class", "ERROR", MB_OK);
    return 0;
  }
 
  hWnd = CreateWindowA(WND_CLASS_NAME, "> The cow <",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
    10, 10, 1920,1080 , NULL, NULL, hInstance, NULL);

  while (TRUE)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        break;
      DispatchMessage(&msg);
    }
    else
    {
      BG3_AnimRender();
      BG3_AnimCopyFrame();
      //PostMessage(hWnd, WM_TIMER, 30, 0);
    }
  }

  return msg.wParam;
} /* End of 'WinMain' function */

/*
* Ôóíêöèÿ îáðàáîòêè ñîîáùåíèÿ îêíà.
*/
LRESULT CALLBACK MyWindowFunc( HWND hWnd, UINT Msg,
                               WPARAM wParam, LPARAM lParam )
{
  switch (Msg)
  {
  case WM_CREATE:
    BG3_AnimInit(hWnd);
    return 0;
  case WM_SIZE:
    BG3_AnimResize(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_MOUSEWHEEL:
    BG3_AnimWheel(wParam);
    return 0;
  case WM_ERASEBKGND:
    return 0;
  case WM_TIMER:
    BG3_AnimRender();
    BG3_AnimCopyFrame();
    return 0;
  case WM_DESTROY:
    /* Close animation system */
    BG3_AnimClose();
    /* Send WM_QUIT */
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, Msg, wParam, lParam);
} 