#ifndef __graphics_H__
#define __graphics_H__

#include "lcd.h"
  

// a 2D vertex
typedef struct VERTEX2DI_TYP
{
	int x,y; // the vertex
} VERTEX2DI, *VERTEX2DI_PTR;

// a 2D vertex
typedef struct VERTEX2DF_TYP
{
	float x,y; // the vertex
} VERTEX2DF, *VERTEX2DF_PTR;



// a 2D polygon
typedef struct POLYGON2D_TYP
{
	unsigned char state;        // state of polygon
	unsigned char num_verts;    // number of vertices
	int x0,y0;        // position of center of polygon
	int xv,yv;        // initial velocity
	int color;      // could be index or PALETTENTRY
	VERTEX2DF vlist[8]; // pointer to vertex list
} POLYGON2D, *POLYGON2D_PTR;


#define WINDOW_WIDTH 239
#define WINDOW_HEIGHT 100
#define M_PI 3.145
#define HALF_SCREEN_WIDTH  119
#define HALF_SCREEN_HEIGHT  50
#define ASPECT_RATIO  (float)2.4
#define viewing_distance 250

// a 3D vertex
typedef struct VERTEX4DI_TYP
{
	int x, y; // the vertex
} VERTEX4DI, *VERTEX4DI_PTR;

// a 3D vertex
typedef struct VERTEX3DF_TYP
{
	float x, y, z; // the vertex
} VERTEX3DF, *VERTEX3DF_PTR;


typedef struct POLYGON3D_TYP
{
	unsigned char state;        // state of polygon
	unsigned char num_verts;    // number of vertices
	int x0, y0;        // position of center of polygon
	int world_x, world_y, world_z;
	int  color;      // could be index or PALETTENTRY
	VERTEX3DF vlist[8]; // pointer to vertex list
	VERTEX3DF world_vlist[8]; // transformed vertices 
	VERTEX3DF camera_vlist[8];

} POLYGON3D, *POLYGON3D_PTR;


int  Draw_Polygon2D(POLYGON2D_PTR poly);
int  Translate_Polygon2D(POLYGON2D_PTR poly, int dx, int dy);
int  Rotate_Polygon2D(POLYGON2D_PTR poly, int theta);
int  Rotate_Polygon3D_YAxis(POLYGON3D_PTR poly, int theta);
int  Draw_Polygon3D(POLYGON3D_PTR poly);
void DrawSolidTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color );
void Draw_Line(float x1, float y1, float x2, float y2, int color);
void Transform_LocalToWorld(POLYGON3D_PTR poly);
void Transform_WorldToCamera( POLYGON3D_PTR poly);

int fsin( short alpha);
int fcos( short alpha);

#endif
