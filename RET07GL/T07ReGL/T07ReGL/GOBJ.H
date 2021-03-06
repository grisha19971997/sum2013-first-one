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
  BG3_PRIM_TRIMESH, BG3_PRIM_GRID
} bg3PRIM_TYPE;

/* ��� ��������� - ������ ������ � �������� */
typedef struct
{
  bg3PRIM_TYPE Type; /* ��� ��������� */
  INT
    NumOfV,          /* ���������� ������ */
    NumOfI,          /* ���������� �������� */
    GW, GH,          /* ������ ����� ��� PRIM_GRID */
    Mat,             /* ����� ��������� */
    Size;            /* ������ ��������� � ������ */
  VERTEX *V;         /* ������ ������ */
  INT *I;            /* ������ �������� */
  UINT Buffs[2], VertBuf; /* OpenGL ������ */
} bg3PRIM;

/* ��� ������������� ��������� */
#define MAX_NAME 100
typedef struct
{
  VEC Ka, Kd, Ks;   /* ������������ ��������� */
  FLT Phong;        /* ����������� ����� */
  CHAR
    Name[MAX_NAME], /* ��� ��������� */
    MapD[MAX_NAME]; /* �������� ����� */     
} bg3MATERIAL;

/* ��� ������������� ��������������� ������� - ������������
 * ���������� � �� ���������� */
typedef struct
{
  INT NumOfPrims, NumOfMats; /* ������� �������� */
  bg3PRIM *Prims;            /* ������ ���������� */
  bg3MATERIAL *Mats;         /* ������ ���������� */
} bg3GOBJ;

/* ���������� ���������� - ���� ������� �� ��������� */
extern VEC BG3_PrimDefaultColor;

/* ������� �������� ��������� */
BOOL BG3_PrimCreate( bg3PRIM *Prim, bg3PRIM_TYPE Type,
                     INT NumOfV, INT NumOfI,
                     VERTEX *Vert, INT *Ind );

/* ������� �������� ��������� */
VOID BG3_PrimClose( bg3PRIM *Prim );

/* ������� ����������� ��������� */
BOOL BG3_PrimDup( bg3PRIM *Prim, bg3PRIM *PrimSrc );

/* ������� ��������� ��������� */
VOID BG3_PrimDraw( bg3PRIM *Prim );

BOOL IB1_GeomAddMat( IB1GOBJ *Go, IB1MATERIAL *Mat );

VOID IB1_GeomClose( IB1GOBJ *Go );


