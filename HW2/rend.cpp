#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    <string>

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

#define RGBA_DIMEMSION	4	/* RGBA -> 4D color */
#define RGB_DIMEMSION	3	/* RGB -> 3D color */
#define COLOR_LIMIT		4095	/* Clamping the color value into 0~4095. */

GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 */
	xres = xRes; // user input
	yres = yRes; // user input
	int screenResolution = xres * yres;
	//--allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	//-- allocate memory for pixel buffer
	framebuffer = new char[(3) * screenResolution];
	pixelbuffer = new GzPixel[screenResolution];
}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel genericPixel = { 222, 333, 444, 1, MAXINT };

	int screenResolution = xres * yres;
	for (int i = 0; i < screenResolution; i++)
	{
		pixelbuffer[i] = genericPixel;
		framebuffer[i] = genericPixel.red;
		framebuffer[(3 * i) + 1] = genericPixel.blue;
		framebuffer[(3 * i) + 2] = genericPixel.green;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		if (z < pixelbuffer[index].z) {
			GzPixel currentPixel = { r, g, b, a, z };
			pixelbuffer[index] = currentPixel;
		}
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

float GzRender::GzPixelDistance(float x1, float x2)
{
	return x1 - x2;
}

/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/
	if (numAttributes == 1) {
		GzToken token = nameList[0];
		float* color = (float*)valueList[0];
		flatcolor[0] = color[0];
		flatcolor[1] = color[1];
		flatcolor[2] = color[2];
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken* nameList, GzPointer* valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/
	if (numParts == 1) {
		GzCoord* ptr = (GzCoord*)valueList[0];
		GzCoord vert[3], sortedVertices[3];

		// V1:
		vert[0][0] = ptr[0][0];
		vert[0][1] = ptr[0][1];
		vert[0][2] = ptr[0][2];
		// V2:
		vert[1][0] = ptr[1][0];
		vert[1][1] = ptr[1][1];
		vert[1][2] = ptr[1][2];
		// V3:
		vert[2][0] = ptr[2][0];
		vert[2][1] = ptr[2][1];
		vert[2][2] = ptr[2][2];

		//sorting and swapping
		if (vert[0][1] > vert[1][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vert[0][i], vert[1][i]);
			}
		}
		//sorting and swapping
		if (vert[0][1] > vert[2][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vert[0][i], vert[2][i]);
			}
		}
		//sorting and swapping
		if (vert[1][1] > vert[2][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vert[1][i], vert[2][i]);
			}
		}

		// sorting by Y
		if ((int)(vert[0][1] + 0.5) == (int)(vert[1][1] + 0.5)) {
			if (vert[0][0] > vert[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vert[1][i], vert[2][i]);
				}
			}
		}
		else if ((int)(vert[1][1] + 0.5) == (int)(vert[2][1] + 0.5)) {
			if (vert[2][0] > vert[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vert[1][i], vert[2][i]);
				}
			}
		}
		else {
			float slope, midX;
			if ((int)(vert[0][0] + 0.5) == (int)(vert[2][0] + 0.5)) {
				midX = vert[0][0];
			}
			else {
				slope = (vert[0][1] - vert[2][1]) / (vert[0][0] - vert[2][0]);
				midX = (vert[1][1] - vert[0][1]) / slope + vert[0][0];
			}

			if (midX > vert[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vert[1][i], vert[2][i]);
				}
			}
		}
		//sorted
		// dda parameters
		float dx12, dy12, dx23;
		float dy23, dx31, dy31;

		dx12 = GzPixelDistance(vert[1][0], vert[0][0]);
		dy12 = GzPixelDistance(vert[1][1], vert[0][1]);
		dx23 = GzPixelDistance(vert[2][0], vert[1][0]);
		dy23 = GzPixelDistance(vert[2][1], vert[1][1]);
		dx31 = GzPixelDistance(vert[0][0], vert[2][0]);
		dy31 = GzPixelDistance(vert[0][1], vert[2][1]);

		//edges
		float A12, B12, C12, A23, B23, C23, A31, B31, C31;

		A12 = dy12;
		B12 = -1.0f * dx12;
		C12 = dx12 * vert[0][1] - dy12 * vert[0][0];
		A23 = dy23;
		B23 = -1.0f * dx23;
		C23 = dx23 * vert[1][1] - dy23 * vert[1][0];
		A31 = dy31;
		B31 = -1.0f * dx31;
		C31 = dx31 * vert[2][1] - dy31 * vert[2][0];

		float X1 = vert[1][0] - vert[0][0];
		float Y1 = vert[1][1] - vert[0][1];
		float Z1 = vert[1][2] - vert[0][2];
		float X2 = vert[2][0] - vert[0][0];
		float Y2 = vert[2][1] - vert[0][1];
		float Z2 = vert[2][2] - vert[0][2];

		//
		float A = (Y1 * Z2) - (Z1 * Y2);
		float B = -((X1 * Z2) - (Z1 * X2));
		float C = (X1 * Y2) - (Y1 * X2);
		float D = -1.0f * (A * vert[0][0] + B * vert[0][1] + C * vert[0][2]);

		// the boundary of the figure
		float minxX = min(min(vert[0][0], vert[1][0]), vert[2][0]);
		float maxxX = max(max(vert[0][0], vert[1][0]), vert[2][0]);
		float minyY = min(min(vert[0][1], vert[1][1]), vert[2][1]);
		float maxyY = max(max(vert[0][1], vert[1][1]), vert[2][1]);
		int minX = (int)(minxX + 0.5);
		int maxX = (int)(maxxX + 0.5);
		int minY = (int)(minyY + 0.5);
		int maxY = (int)(maxyY + 0.5);

		// drawing on screeeeeeeen
		for (int i = minxX; i <= maxxX; i++) {
			for (int j = minyY; j <= maxyY; j++) {
				float l12 = A12 * (float)i + B12 * (float)j + C12;
				float l23 = A23 * (float)i + B23 * (float)j + C23;
				float l31 = A31 * (float)i + B31 * (float)j + C31;

				if ((l12 > 0 && l23 > 0 && l31 > 0 && C != 0)
					|| (l12 < 0 && l23 < 0 && l31 < 0 && C != 0)
					|| l12 == 0 || l23 == 0 || l31 == 0) {
					float interpolatedZ = -1.0f * (A * (float)i + B * (float)j + D) / C;
					int currentZ = (int)(interpolatedZ + 0.5);
					GzIntensity redOfRBG = ctoi(flatcolor[0]);
					GzIntensity greenofRBG = ctoi(flatcolor[1]);
					GzIntensity blueofRGB = ctoi(flatcolor[2]);
					// print on screeeeeeen
					GzPut(i, j, redOfRBG, greenofRBG, blueofRGB, 1, currentZ);
				}
			}
		}
	}
	return GZ_SUCCESS;
}