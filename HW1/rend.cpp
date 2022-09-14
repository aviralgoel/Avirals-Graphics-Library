#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */
	xres = xRes;
	yres = yRes;
	framebuffer = (char*)calloc(3 * xres * yres, sizeof(char));
	pixelbuffer = (GzPixel*)calloc(xres * yres, sizeof(GzPixel));
}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */
	free(framebuffer);
	free(pixelbuffer);
}

int GzRender::GzDefault()
{
	GzPixel defaultPixel = { 2457, 433, 481, 0, 0 };
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < xres * yres; i++)
	{
		pixelbuffer[i] = defaultPixel;
		framebuffer[i] = defaultPixel.red;
		framebuffer[(3 * i) + 1] = defaultPixel.blue;
		framebuffer[(3 * i) + 2] = defaultPixel.green;

	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */
	if (i > 0 && i < xres && j > 0 && j < yres)
	{
		GzPixel tempPixel = { r,g,b,a,z };
		int index = ARRAY(i, j);
		pixelbuffer[index] = tempPixel;
	}

	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z)
{
	/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i > 0 && i < xres && j > 0 && j < yres)
	{
		int index = ARRAY(i, j);
		*r = pixelbuffer[index].red;
		*g = pixelbuffer[index].green;
		*b = pixelbuffer[index].blue;
		*a = pixelbuffer[index].alpha;
		*z = pixelbuffer[index].z;
	}

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);
	for (int i = 0; i < xres * yres; i++)
	{
		GzPixel thisPixel = pixelbuffer[i];
		thisPixel.red = GzClampPixelValueFloor(thisPixel.red);
		thisPixel.green = GzClampPixelValueFloor(thisPixel.green);
		thisPixel.blue = GzClampPixelValueFloor(thisPixel.blue);

		thisPixel.red = GzClampPixelValueCeil(thisPixel.red);
		thisPixel.green = GzClampPixelValueCeil(thisPixel.green);
		thisPixel.blue = GzClampPixelValueCeil(thisPixel.blue);

		thisPixel.red = GzBitShift4PixelValue(thisPixel.red);
		thisPixel.blue = GzBitShift4PixelValue(thisPixel.blue);
		thisPixel.green = GzBitShift4PixelValue(thisPixel.green);

		char color[3] = { (char)(thisPixel.red), (char)(thisPixel.green ),(char)(thisPixel.blue) };
		fwrite(color, 1, 3, outfile);

	}
	return GZ_SUCCESS;
}
	


int GzRender::GzFlushDisplay2FrameBuffer()
{
	/* HW1.7 write pixels to framebuffer:
		- put the pixels into the frame buffer
		- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red
		- NOT red, green, and blue !!!
	*/
	for (int i = 0; i < xres * yres; i++)
	{
		GzPixel thisPixel = pixelbuffer[i];
		
		//is this part called rendering?
		thisPixel.red = GzClampPixelValueFloor(thisPixel.red);
		thisPixel.green = GzClampPixelValueFloor(thisPixel.green);
		thisPixel.blue = GzClampPixelValueFloor(thisPixel.blue);
		

		thisPixel.red = GzClampPixelValueCeil(thisPixel.red);
		thisPixel.green = GzClampPixelValueCeil(thisPixel.green);
		thisPixel.blue = GzClampPixelValueCeil(thisPixel.blue);

		thisPixel.red = GzBitShift4PixelValue(thisPixel.red);
		thisPixel.blue = GzBitShift4PixelValue(thisPixel.blue);
		thisPixel.green = GzBitShift4PixelValue(thisPixel.green);
		// end of rendering

		framebuffer[3 * i] = thisPixel.blue;
		framebuffer[3 * i + 1] = thisPixel.green;
		framebuffer[3 * i + 2] = thisPixel.red;

	}

	return GZ_SUCCESS;
}

GzIntensity GzRender::GzClampPixelValueFloor(GzIntensity pixelValue)
{	
	return (pixelValue < 0 ? 0 : pixelValue);
}
GzIntensity GzRender::GzClampPixelValueCeil(GzIntensity pixelValue)
{
	return (pixelValue > 4095 ? 4095 : pixelValue);
}

GzIntensity GzRender::GzBitShift4PixelValue(GzIntensity pixelValue)
{
	return pixelValue >> 4;
}
