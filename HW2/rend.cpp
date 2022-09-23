#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include <algorithm>

// write to file
#include <fstream>
//

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
 // Since xres and yres are internaly defined as unsigned shorts (16 bit integers)
	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;
	int resolution = xres * yres;

	framebuffer = (char*)calloc(resolution * 3, sizeof(char));
	pixelbuffer = (GzPixel*)calloc(resolution, sizeof(GzPixel));
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	free(framebuffer);
	free(pixelbuffer);

}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	int resolution = xres * yres;
	// default pixel value for every pixel
	GzPixel defaultPixel = { 4000,300,400,0, INT_MAX};
	// iterate over each pixel in pixelarray
	for (int i = 0; i < resolution; i++)
	{
		pixelbuffer[i] = defaultPixel;
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	//if the indices are in screen bound
	if (i >= 0 && j >= 0 && i < xres && j < yres)
	{
		// fetch its relative location in PixelBuffer
		int loc = ARRAY(i, j);
		GzPixel thisPixel = { r, g, b, a, z };
		// replace the current pixelbuffer info with the new one
		// if the new pixel to be drawn is closer than the original pixel
		if(z < pixelbuffer[loc].z)
			pixelbuffer[loc] = thisPixel;
	}

	return GZ_SUCCESS;

}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
		/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i < xres && j < yres && i >= 0 && j >= 0)
	{
		int loc = ARRAY(i, j);
		r = &pixelbuffer[loc].red;
		g = &pixelbuffer[loc].green;
		b = &pixelbuffer[loc].blue;
		a = &pixelbuffer[loc].alpha;
		z = &pixelbuffer[loc].z;
	}
	return GZ_SUCCESS;
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);
	// iterate over each pixel in pixelbuffer
	for (int i = 0; i < xres * yres; i++)
	{
		// fetch that pixel info
		GzPixel thisPixel = pixelbuffer[i];
		// Clamp value to 0 (if less than 0)
		thisPixel.red = GzClampPixelValueFloor(thisPixel.red);
		thisPixel.green = GzClampPixelValueFloor(thisPixel.green);
		thisPixel.blue = GzClampPixelValueFloor(thisPixel.blue);
		// Clamp value to 4095 (if more than 4095)
		thisPixel.red = GzClampPixelValueCeil(thisPixel.red);
		thisPixel.green = GzClampPixelValueCeil(thisPixel.green);
		thisPixel.blue = GzClampPixelValueCeil(thisPixel.blue);
		// drop the last 4 LSBs in bit representation
		thisPixel.red = GzBitShift4PixelValue(thisPixel.red);
		thisPixel.blue = GzBitShift4PixelValue(thisPixel.blue);
		thisPixel.green = GzBitShift4PixelValue(thisPixel.green);
		//write to file
		char color[3] = { (char)(thisPixel.red), (char)(thisPixel.green),(char)(thisPixel.blue) };
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

		// similar clamping checks and fixing
		thisPixel.red = GzClampPixelValueFloor(thisPixel.red);
		thisPixel.green = GzClampPixelValueFloor(thisPixel.green);
		thisPixel.blue = GzClampPixelValueFloor(thisPixel.blue);


		thisPixel.red = GzClampPixelValueCeil(thisPixel.red);
		thisPixel.green = GzClampPixelValueCeil(thisPixel.green);
		thisPixel.blue = GzClampPixelValueCeil(thisPixel.blue);

		thisPixel.red = GzBitShift4PixelValue(thisPixel.red);
		thisPixel.blue = GzBitShift4PixelValue(thisPixel.blue);
		thisPixel.green = GzBitShift4PixelValue(thisPixel.green);

		// update the frame buffer with precise values
		framebuffer[3 * i] = thisPixel.blue;
		framebuffer[3 * i + 1] = thisPixel.green;
		framebuffer[3 * i + 2] = thisPixel.red;

	}
	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/	
	// for every attribute
	for (int i = 0; i < numAttributes; i++)
	{	
		// fetch the type of the attribute
		if (nameList[i] == GZ_RGB_COLOR)
		{	
			// fetch the value of the attribute and cast to proper type
			GzColor* color = (GzColor*) valueList[0];
			flatcolor[0] = *color[0];
			flatcolor[1] = *color[1];
			flatcolor[2] = *color[2];

		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken *nameList, GzPointer *valueList) 
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	// for each part
	for (int i = 0; i < numParts; i++)
	{	
		// if we are talking about the position of the triangles
		if (nameList[i] == GZ_POSITION)
		{
			// define array of 3 GzCoords (where each GzCoord stores 3 numbers, say, xyz)
			GzCoord v0, v1, v2;
			// handle to pointer to our vertex data
			GzCoord* ptrToData = (GzCoord*)valueList[0];
			
			// First Vertex
			v0[0] = ptrToData[0][0]; // X
			v0[1] = ptrToData[0][1]; // Y
			v0[2] = ptrToData[0][2]; // Z
			// Second Vertex
			v1[0] = ptrToData[1][0]; // X
			v1[1] = ptrToData[1][1]; // Y
			v1[2] = ptrToData[1][2]; // Z
			// Third Vertex
			v2[0] = ptrToData[2][0]; // X
			v2[1] = ptrToData[2][1]; // Y
			v2[2] = ptrToData[2][2]; // Z
			
			// Y
			SortByY(v0, v1, v2);
			// Sort by X 


		}
		// if token is NULL, do nothing
		if (nameList[i] == GZ_NULL_TOKEN) {
			return GZ_FAILURE;
		}
	}
	return GZ_SUCCESS;
}

void GzRender::SortByY(GzCoord& v0, GzCoord& v1, GzCoord& v2)
{
	if (v0[1] > v1[1])
	{
		std::swap(v0, v1);
	}
	if (v0[1] > v2[1])
	{
		std::swap(v0, v2);
	}
	if (v1[1] > v2[1])
	{
		std::swap(v1, v2);
	}
}

// method to check and fix pixel values if lesser than 0
GzIntensity GzRender::GzClampPixelValueFloor(GzIntensity pixelValue)
{
	return (pixelValue < 0 ? 0 : pixelValue);
}

// method to check and fix pixel values if greater than 4095
GzIntensity GzRender::GzClampPixelValueCeil(GzIntensity pixelValue)
{
	return (pixelValue > 4095 ? 4095 : pixelValue);
}

// method to bit shift pixel bit value towards right by 4 bits
GzIntensity GzRender::GzBitShift4PixelValue(GzIntensity pixelValue)
{
	return pixelValue >> 4;
}

