#include <math.h>

#include "graphics.h"
#include "ili9320.h"


// precomputed sin table * 256
static const short tsin[91]= { 0,4,9,13,18,22,27,31,36,40,44,49,53,58,62,66,71,75,
                        79,83,88,92,96,100,104,108,112,116,120,124,128,132,
                        136,139,143,147,150,154,158,161,165,168,171,175,178,
                        181,184,187,190,193,196,199,202,204,207,210,212,215,
                        217,219,222,224,226,228,230,232,234,236,237,239,241,
                        242,243,245,246,247,248,249,250,251,252,253,254,254,
                        255,255,255,256,256,256,256
                        };


// this function draws a POLYGON2D based on
int Draw_Polygon2D(POLYGON2D_PTR poly)
{
	// test if the polygon is visible
	if (poly->state)
	{
		u8 index;
		// loop thru and draw a line from vertices 1 to n
		for (index=0; index < poly->num_verts-1; index++)
		{
			// draw line from ith to ith+1 vertex
			LCD_DrawLine(poly->vlist[index].x + poly->x0,
					poly->vlist[index].y + poly->y0,
					poly->vlist[index + 1].x + poly->x0,
					poly->vlist[index + 1].y + poly->y0, poly->color);
		} // end for

		// now close up polygon
		// draw line from last vertex to 0th
		LCD_DrawLine(poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0,
				poly->vlist[7].x + poly->x0, poly->vlist[7].y + poly->y0,
				poly->color);

		// return success
		return (1);
	} // end if
	else
		return(0);
}

// this function translates the center of a polygon
int Translate_Polygon2D(POLYGON2D_PTR poly, int dx, int dy)
{
	// test for valid pointer
	if (!poly)
		return (0);

	// translate
	poly->x0 += dx;
	poly->y0 += dy;

	// return success
	return (1);
}


// this function rotates the local coordinates of the polygon
int Rotate_Polygon2D(POLYGON2D_PTR poly, int theta)
{
	// test for valid pointer
	if (!poly)
	   return(0);

	float theta_Rad = (float)theta*3.14159 / 180; // trig functions work in rad

	u8 curr_vert;
	// loop and rotate each point, very crude, no lookup!!!
	for (curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// perform rotation
		float xr = (float) poly->vlist[curr_vert].x * cos(theta_Rad)
				- (float) poly->vlist[curr_vert].y * sin(theta_Rad);

		float yr = (float) poly->vlist[curr_vert].x * sin(theta_Rad)
				+ (float) poly->vlist[curr_vert].y * cos(theta_Rad);

		// store result back
		poly->vlist[curr_vert].x = xr;
		poly->vlist[curr_vert].y = yr;

	} // end for curr_vert

	// return success
	return(1);
}


int fsin( short alpha)
{
    int i = 1;

    // normalize the angle
    if ( alpha < 0)
        alpha = 360 - ((-alpha) % 360);
    else
        alpha %= 360;

	if ( alpha >= 180)
	{   // sin(a+180) == - sin(a)
		alpha -= 180;
        i = -1;
    }
    if ( alpha > 90)
    { // sin(180-a) == sin (a); when a<180
        alpha = 180 - alpha;
    }

    // use the table to approximate the angle
    return ( i * tsin[ alpha]);
}

int fcos( short alpha)
{   // cos(a ) == sin (a+90)
    return fsin( alpha + 90);
}

