/* FILE NAME: GOBJ.C
 * PROGRAMMER: bg3
 * DATE: 11.06.2013
 * PURPOSE: Geometry object handle.
 */

#include <stdio.h>
#include <stdlib.h>

#include "anim.h"

/* ������� �������� ��������������� �������.
 * ���������:
 *   - ��������� �� ��������� ������:
 *       bg3GOBJ *Go;
 * ������������ ��������: ���.
 */
VOID BG3_GeomClose( bg3GOBJ *Go )
{
  if (Go->Prims != NULL)
  {
    INT i;

    for (i = 0; i < Go->NumOfPrims; i++)
      BG3_PrimClose(&Go->Prims[i]);
    free(Go->Prims);
  }
  if (Go->Mats != NULL)
    free(Go->Mats);
  memset(Go, 0, sizeof(bg3GOBJ));
} /* End of 'BG3_GeomClose' function */

/* ������� ���������� ��������� � ��������������� �������.
 * ���������:
 *   - ��������� �� ������:
 *       bg3GOBJ *Go;
 *   - ��������� �� ��������:
 *       bg3PRIM *Prim;
 * ������������ ��������:
 *   (BOOL) TRUE ��� ������, FALSE ��� �������� ������.
 */
BOOL BG3_GeomAddPrim( bg3GOBJ *Go, bg3PRIM *Prim )
{
  bg3PRIM *newblock = malloc(sizeof(bg3PRIM) * (Go->NumOfPrims + 1));

  if (newblock == NULL)
    return FALSE;
  /* �������� ������ ��������� */
  memcpy(newblock, Go->Prims, sizeof(bg3PRIM) * Go->NumOfPrims);


  /* ����������� ������ ������ */
  free(Go->Prims);
  Go->Prims = newblock;

  /* ���������� ����� �������� */
  Go->Prims[Go->NumOfPrims++] = *Prim;
  return TRUE;
} /* End of 'BG3_GeomAddPrim' function */

/* ������� ���������� ��������� � ��������������� �������.
 * ���������:
 *   - ��������� �� ������:
 *       bg3GOBJ *Go;
 *   - ��������� �� ��������:
 *       bg3MATERIAL *Mat;
 * ������������ ��������:
 *   (BOOL) TRUE ��� ������, FALSE ��� �������� ������.
 */
BOOL BG3_GeomAddMat( bg3GOBJ *Go, bg3MATERIAL *Mat )
{
  bg3MATERIAL *newblock = malloc(sizeof(bg3MATERIAL) * (Go->NumOfMats + 1));

  if (newblock == NULL)
    return FALSE;
  /* �������� ������ ��������� */
  memcpy(newblock, Go->Mats, sizeof(bg3MATERIAL) * Go->NumOfMats);

  /* ����������� ������ ������ */
  free(Go->Mats);
  Go->Mats = newblock;

  /* ���������� ����� �������� */
  Mat->TexNo = -1;
  Go->Mats[Go->NumOfMats++] = *Mat;
  return TRUE;
} /* End of 'BG3_GeomAddPrim' function */

/* ������� ��������� ��������������� �������.
 * ���������:
 *   - ��������� �� ������:
 *       bg3GOBJ *Go;
 * ������������ ��������: ���.
 */
VOID BG3_GeomDraw( bg3GOBJ *Go )
{
  INT i;

  for (i = 0; i < Go->NumOfPrims; i++)
  {
    /* ��������� ��������� */
    if (Go->Prims[i].Mat != -1)
    {
      INT n = Go->Prims[i].Mat;
      if (Go->Mats[n].TexNo == -1)
      {
        /* �������� ��� */
        Go->Mats[n].TexNo = 0;
        if (Go->Mats[n].MapD[0] != 0)
        {
          HBITMAP hBm, hBmDib;
          BITMAP Bm;
          HDC hDC, hMemDC, hMemDCDib;
          BITMAPINFOHEADER bih;
          DWORD *Bits;

          if ((hBm = LoadImage(NULL, Go->Mats[n].MapD,
                 IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)) != NULL)
          {
            GetObject(hBm, sizeof(BITMAP), &Bm);

            /* ������� ����������� �������� ��������� */
            bih.biSize = sizeof(BITMAPINFOHEADER);
            bih.biWidth = Bm.bmWidth;
            bih.biHeight = Bm.bmHeight;
            bih.biPlanes = 1;    /* ������ */
            bih.biBitCount = 32; /* xRGB */
            bih.biCompression = BI_RGB;
            bih.biSizeImage = bih.biWidth * bih.biHeight * 4;
            bih.biClrImportant = 0;
            bih.biClrUsed = 0;
            bih.biXPelsPerMeter = bih.biYPelsPerMeter = 0;

            if ((hBmDib = CreateDIBSection(NULL, (BITMAPINFO *)&bih, DIB_RGB_COLORS,
                  (VOID *)&Bits, NULL, 0)) != NULL)
            {
              hDC = GetDC(NULL);
              hMemDC = CreateCompatibleDC(hDC);
              hMemDCDib = CreateCompatibleDC(hDC);
              ReleaseDC(NULL, hDC);

              /* �������� �������� */
              SelectObject(hMemDC, hBm);
              SelectObject(hMemDCDib, hBmDib);
              BitBlt(hMemDCDib, 0, 0, Bm.bmWidth, Bm.bmHeight,
                hMemDC, 0, 0, SRCCOPY);

              /* ������� ��������� �������� */
              glGenTextures(1, &Go->Mats[n].TexNo);
              /* ���������� �� */
              glBindTexture(GL_TEXTURE_2D, Go->Mats[n].TexNo);

              /* �������� ����������� � OpenGL */
              gluBuild2DMipmaps(GL_TEXTURE_2D, 4, Bm.bmWidth, Bm.bmHeight,
                GL_BGRA, GL_UNSIGNED_BYTE, Bits);

              /* ������������ ������� */
              glBindTexture(GL_TEXTURE_2D, 0);

              DeleteDC(hMemDC);
              DeleteDC(hMemDCDib);
              DeleteObject(hBmDib);
            }
            DeleteObject(hBm);
          }
        }
      }
      glBindTexture(GL_TEXTURE_2D, Go->Mats[n].TexNo);
    }

    /* ������ �������� */
    if (i != 14)
      BG3_PrimDraw(&Go->Prims[i]);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
} /* End of 'BG3_GeomDraw' function */

/* END OF 'GOBJ.C' FILE */
