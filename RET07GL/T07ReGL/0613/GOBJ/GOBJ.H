/***
 * ������� ������ � ����������� � ��������������� ���������
 ***/

/* ��� ������������� ���������� ��������� */
typedef struct
{
  FLT U, V; /* ���������� ���������� (� OpenGL -> st[vq]) */
} UV;
__inline UV UVSet( FLT U, FLT V )
{
  UV r;

  r.U = U;
  r.V = V;
  return r;
} /* End of 'UVSet' function */

/* ��� ������������� ������� */
typedef struct
{
  VEC P; /* ������� ������� */
  UV T;  /* ���������� ���������� */
  VEC N; /* ������� ������� */
  VEC C; /* ���� ������� */
} VERTEX;

/* ���� ���������� */
typedef enum
{
  VG4_PRIM_TRIMESH, VG4_PRIM_GRID
} vg4PRIM_TYPE;

/* ��� ��������� - ������ ������ � �������� */
typedef struct
{
  vg4PRIM_TYPE Type; /* ��� ��������� */
  INT
    NumOfV,          /* ���������� ������ */
    NumOfI,          /* ���������� �������� */
    GW, GH,          /* ������ ����� ��� PRIM_GRID */
    Mat,             /* ����� ��������� */
    Size;            /* ������ ��������� � ������ */
  VERTEX *V;         /* ������ ������ */
  INT *I;            /* ������ �������� */
  UINT Buffs[2], VertBuf; /* OpenGL ������ */
} vg4PRIM;

/* ��� ������������� ��������� */
#define MAX_NAME 100
typedef struct
{
  VEC Ka, Kd, Ks;   /* ������������ ��������� */
  FLT Phong;        /* ����������� ����� */
  CHAR
    Name[MAX_NAME], /* ��� ��������� */
    MapD[MAX_NAME]; /* �������� ����� */     
} vg4MATERIAL;

/* ��� ������������� ��������������� ������� - ������������
 * ���������� � �� ���������� */
typedef struct
{
  INT NumOfPrims, NumOfMats; /* ������� �������� */
  vg4PRIM *Prims;            /* ������ ���������� */
  vg4MATERIAL *Mats;         /* ������ ���������� */
} vg4GOBJ;

/* ���������� ���������� - ���� ������� �� ��������� */
extern VEC VG4_PrimDefaultColor;

/* ������� �������� ��������� */
BOOL VG4_PrimCreate( vg4PRIM *Prim, vg4PRIM_TYPE Type,
                     INT NumOfV, INT NumOfI,
                     VERTEX *Vert, INT *Ind );

/* ������� �������� ��������� */
VOID VG4_PrimClose( vg4PRIM *Prim );

/* ������� ����������� ��������� */
BOOL VG4_PrimDup( vg4PRIM *Prim, vg4PRIM *PrimSrc );

/* ������� ��������� ��������� */
VOID VG4_PrimDraw( vg4PRIM *Prim );

BOOL IB1_GeomAddMat( IB1GOBJ *Go, IB1MATERIAL *Mat );

VOID IB1_GeomClose( IB1GOBJ *Go );


