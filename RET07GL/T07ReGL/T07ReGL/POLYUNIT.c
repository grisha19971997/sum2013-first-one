/* FILE NAME: polyunit.c
* PROGRAMMER: bg3
* DATE: 06.06.2013
* PURPOSE: animation unit based on polygonal 3D model
*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "shader.h"
#include "anim.h"
#include "units.h"
#include "vec.h"

typedef struct tagbg3POLYUNIT
{
  /* Extend basic unit */
  BG3_UNIT_BASE_FUNCS;
  /* Is posable */
  BG3_UNIT_POS_FIELDS;
  /* Other transformations */
  MATR transformMatrix;
  /* 3D model */
  POLYGONAL_MODEL * model;

  /* Destination point */
  int destX, destY;

  /* Color */
  float r, g, b;
} POLYUNIT;

static INT CowProg = -2;

/* RenderPolyUnit
* Args:
* - "Self" pointer
* POLYUNIT *Unit;
* - Animation context pointer
* bg3ANIM *Ani;
*/
static VOID PolyUnitRender( POLYUNIT *Unit, bg3ANIM *Ani ,bg3GOBJ *Go)
{
  if (CowProg < -1)
  {
    CowProg = ShadProgInit("a.vert", "a.frag");
  }
  if (Unit->model != NULL && CowProg >= 0)
  {
    POLYGONAL_MODEL * model = Unit->model;
    VEC * vertexes = model->vertexes;
    FACET * facets = model->facets;
    MATR translateMatrix = MatrTranslate(Unit->X, Unit->Y, Unit->Z);

    MATR rotateMatrix = MatrRotate(0.01 * clock(),0,0,1);
    MATR transformMatrix = MatrMulMatr(MatrMulMatr(Unit->transformMatrix, rotateMatrix), translateMatrix);
    MATR fullMatr = MatrMulMatr(transformMatrix, Ani->camera.MVP);
    VEC * vertexesProj = Unit->model->vertProj;
    UINT locMatr, locTime;
        
    glUseProgram(CowProg);
    locMatr = glGetUniformLocation(CowProg, "Matr");
    locTime = glGetUniformLocation(CowProg, "Time");
    if (locMatr >= 0)
    {
      glUniformMatrix4fv(locMatr, 1, FALSE, &fullMatr.A[0][0]);
    }
    if (locTime >= 0)
    {
      glUniform1f(locTime, (float)(Ani->timer.activeTime));
    }

    // Draw model
    
    BG3_GeomDraw( Go );

    glEnd();
    glUseProgram(0);
  }
} /* End of 'CubeRender' function */


/* Create new polygonal unit
* Àrgs:
* - Location
* INT X, Y;
* - Model of the unit
* POLYGONAL_MODEL model
* Returns:
* (bg3UNIT *) crated unit pointer
*/
bg3UNIT * PolyUnitCreate( INT X, INT Y, INT Z, POLYGONAL_MODEL * model, MATR transformationMatrix )
{
  POLYUNIT *Unit;

  if ((Unit = (POLYUNIT *)BG3_UnitCreate(sizeof(POLYUNIT))) == NULL)
    return NULL;

  Unit->Render = (bg3UNIT_RENDER)PolyUnitRender;
  Unit->X = X;
  Unit->Y = Y;
  Unit->Z = Z;
  Unit->r = (rand() % 256) / 256.0;
  Unit->g = (rand() % 256) / 256.0;
  Unit->b = (rand() % 256) / 256.0;
  Unit->transformMatrix = transformationMatrix;
  Unit->model = model;
  return (bg3UNIT *)Unit;
} /* End of 'CubeCreate' function */

void PolyUnitMoveTo(bg3UNIT * unit, INT x, INT y, INT z) {
    POLYUNIT * polyUnit = (POLYUNIT *) unit;
    polyUnit->X = x;
    polyUnit->Y = y;
    polyUnit->Z = z;
}

void PolyUnitMove(bg3UNIT * unit, VEC delta)
{
POLYUNIT * polyUnit = (POLYUNIT *) unit;
polyUnit->X += delta.X;
polyUnit->Y += delta.Y;
polyUnit->Z += delta.Z;
}

void PolyUnitSetTransform(bg3UNIT * unit, MATR matr)
{
POLYUNIT * polyUnit = (POLYUNIT *) unit;
polyUnit->transformMatrix = matr;
}