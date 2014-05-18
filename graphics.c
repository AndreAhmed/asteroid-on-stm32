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


/**
 * @brief  Displays a line.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Length: line length.
 * @param  Direction: line direction.
 *   This parameter can be one of the following values: LCD_LINE_VERTICAL or Horizontal.
 * @retval None
 */
void Draw_Line(float x1, float y1, float x2, float y2, u16 color) {
	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if (xdiff == 0.0f && ydiff == 0.0f) {
		LCD_WritetoFB((int) x1, (int) y1, color);
		return;
	}

	if (fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if (x1 < x2) {
			xmin = x1;
			xmax = x2;
		} else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		float x;
		for (x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			LCD_WritetoFB((int) x, (int) y, color);
		}
	} else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if (y1 < y2) {
			ymin = y1;
			ymax = y2;
		} else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for (float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			LCD_WritetoFB((int) x, (int) y, color);
		}
	}
}

void Draw_Rect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width,
                u16 color) {
        // must do clippign here
        for (int ymin = Ypos; ymin < Ypos + Height; ymin++) {
                for (int xmin = Xpos; xmin < Xpos + Width; xmin++) {

                        LCD_WritetoFB(xmin, ymin, color);
                }
        }
}


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
			DrawLine(poly->vlist[index].x + poly->x0,
					poly->vlist[index].y + poly->y0,
					poly->vlist[index + 1].x + poly->x0,
					poly->vlist[index + 1].y + poly->y0, poly->color);
		} // end for

		// now close up polygon
		// draw line from last vertex to 0th
		DrawLine(poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0,
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

void DrawSolidTriangle(int x0, int y0, int x1, int y1, int x2, int y2,
		int color) {
	// Sort our points into order of y
	// 0 top
	// 2 middle
	// 1 bottom
	if (y1 < y0) {
		swap(&y1, &y0);
		swap(&x1, &x0);
	}
	if (y2 < y0) {
		swap(&y2, &y0);
		swap(&x2, &x0);
	}
	if (y1 < y2) {
		swap(&y2, &y1);
		swap(&x2, &x1);
	}

	float xl_edge = (float) x0; // left edge
	float xr_edge = (float) x0; // right edge

	float dxldy;
	float dxrdy;

	float dxdy1 = (float) (x2 - x0) / (y2 - y0);
	float dxdy2 = (float) (x1 - x0) / (y1 - y0);

	if (dxdy1 < dxdy2) {
		dxldy = dxdy1;
		dxrdy = dxdy2;
	} else {
		dxldy = dxdy2;
		dxrdy = dxdy1;
	}

	// Top of the triangle
	for (int y = y0; y < y2; y++) {
		int x;
		for (x = xl_edge; x < xr_edge; x++) {
			LCD_WritetoFB(x, y, color);

		} //end for loop x

		xl_edge = xl_edge + dxldy;
		xr_edge = xr_edge + dxrdy;

	} // end for loop y

	  // Bottom half of the triangle

	if (dxdy1 < dxdy2) {
		dxldy = (float) (x2 - x1) / (y2 - y1);
	} else {
		dxrdy = (float) (x2 - x1) / (y2 - y1);
	}

	for (int y = y2; y < y1; y++) {
		int x;
		for (x = xl_edge; x < xr_edge; x++) {
			LCD_WritetoFB(x, y, color);
		} //end for loop x

		xl_edge = xl_edge + dxldy;
		xr_edge = xr_edge + dxrdy;

	} // end for loop y
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

