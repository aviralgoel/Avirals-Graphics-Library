/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
	/* HW 3.1
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	*/

	// degree to radian conversion
	float angleInRadians = degree * PI / 180.0f;
	// initialize the 4x4 matrix with zeros


	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[1][1] = (float)cos((double)angleInRadians);
	mat[1][2] = -1.0f * (float)sin((double)angleInRadians);
	mat[2][1] = (float)sin((double)angleInRadians);
	mat[2][2] = (float)cos((double)angleInRadians);
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
	/* HW 3.2
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	*/

	// degree to radian conversion
	float angleInRadians = degree * PI / 180.0f;
	// initialize the 4x4 matrix with zeros
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}


	mat[0][0] = (float)cos((double)angleInRadians);
	mat[0][2] = (float)sin((double)angleInRadians);
	mat[1][1] = 1.0;
	mat[2][0] = -1.0f * (float)sin((double)angleInRadians);
	mat[2][2] = (float)cos((double)angleInRadians);
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
	/* HW 3.3
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	*/

	// degree to radian conversion
	float angleInRadians = degree * PI / 180.0f;
	// initialize the 4x4 matrix with zeros
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}


	mat[0][0] = (float)cos((double)angleInRadians);
	mat[0][1] = -1.0f * (float)sin((double)angleInRadians);
	mat[1][0] = (float)sin((double)angleInRadians);
	mat[1][1] = (float)cos((double)angleInRadians);
	mat[2][2] = 1.0;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	/* HW 3.4
	// Create translation matrix
	// Pass back the matrix using mat value
	*/

	// initialize the 4x4 matrix with zeros
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}


	mat[0][0] = 1.0; mat[1][1] = 1.0; mat[2][2] = 1.0; mat[3][3] = 1.0;
	float tx = translate[0]; float ty = translate[1]; float tz = translate[2];
	mat[0][3] = tx;
	mat[1][3] = ty;
	mat[2][3] = tz;

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	/* HW 3.5
	// Create scaling matrix
	// Pass back the matrix using mat value
	*/

	// initialize the 4x4 matrix with zeros
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}

	float Sx = scale[0]; float Sy = scale[1]; float Sz = scale[2];
	mat[0][0] = Sx;
	mat[1][1] = Sy;
	mat[2][2] = Sz;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */
	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;
	int resolution = xres * yres;

	framebuffer = (char*)calloc(resolution * 3, sizeof(char));
	pixelbuffer = (GzPixel*)calloc(resolution, sizeof(GzPixel));

	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/


	// initialize the 4x4 matrix with zeros
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			Xsp[i][j] = 0;
		}
	}



	// mapping NDC image to frame buffer per frame
	Xsp[0][0] = (float)xres / 2.0f;
	Xsp[0][3] = (float)xres / 2.0f;
	Xsp[1][1] = -1.0f * (float)yres / 2.0f;
	Xsp[1][3] = (float)yres / 2.0f;
	Xsp[2][2] = (float)MAXINT;
	Xsp[3][3] = 1.0;

	// Setting the default camera
	CameraInit();
	// begin with empty stack
	matlevel = -1;
}

void GzRender::CameraInit()
{
	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;
	m_camera.lookat[3] = { 0 };
	m_camera.worldup[3] = { 0 };
	m_camera.worldup[1] = 1.0;
	m_camera.FOV = DEFAULT_FOV;
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
	GzPixel defaultPixel = { 800,1400,1600,0, INT_MAX };
	// iterate over each pixel in pixelarray
	for (int i = 0; i < resolution; i++)
	{
		pixelbuffer[i] = defaultPixel;
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	// Compute Xiw:
	GzCoord cl;
	for (int i = 0; i < 3; i++) {
		cl[i] = m_camera.lookat[i] - m_camera.position[i];
	}

	GzCoord XofXiw, YofXiw, ZofXiw;
	ZofXiw[0] = cl[0] / MagnitudeOfVector(cl);
	ZofXiw[1] = cl[1] / MagnitudeOfVector(cl);
	ZofXiw[2] = cl[2] / MagnitudeOfVector(cl);
	// up' = up - (up Dot Z)*Z
	GzCoord upPrime;
	float upDotZ = DotProduct3v(m_camera.worldup, ZofXiw);
	upPrime[0] = m_camera.worldup[0] - upDotZ * ZofXiw[0];
	upPrime[1] = m_camera.worldup[1] - upDotZ * ZofXiw[1];
	upPrime[2] = m_camera.worldup[2] - upDotZ * ZofXiw[2];
	// Y = up' / ||| up'||
	YofXiw[0] = upPrime[0] / MagnitudeOfVector(upPrime);
	YofXiw[1] = upPrime[1] / MagnitudeOfVector(upPrime);
	YofXiw[2] = upPrime[2] / MagnitudeOfVector(upPrime);

	// X = Y x Z

	XofXiw[0] = YofXiw[1] * ZofXiw[2] - YofXiw[2] * ZofXiw[1];
	XofXiw[1] = YofXiw[2] * ZofXiw[0] - YofXiw[0] * ZofXiw[2];
	XofXiw[2] = YofXiw[0] * ZofXiw[1] - YofXiw[1] * ZofXiw[0];


	// Building Matrix Xiw
	m_camera.Xiw[0][0] = XofXiw[0]; 	m_camera.Xiw[0][1] = XofXiw[1]; 	m_camera.Xiw[0][2] = XofXiw[2]; m_camera.Xiw[0][3] = -1.0f * (DotProduct3v(XofXiw, m_camera.position));
	m_camera.Xiw[1][0] = YofXiw[0]; 	m_camera.Xiw[1][1] = YofXiw[1]; 	m_camera.Xiw[1][2] = YofXiw[2]; m_camera.Xiw[1][3] = -1.0f * (DotProduct3v(YofXiw, m_camera.position));
	m_camera.Xiw[2][0] = ZofXiw[0]; 	m_camera.Xiw[2][1] = ZofXiw[1]; 	m_camera.Xiw[2][2] = ZofXiw[2]; m_camera.Xiw[2][3] = -1.0f * (DotProduct3v(ZofXiw, m_camera.position));
	m_camera.Xiw[3][0] = 0; 	m_camera.Xiw[3][1] = 0; 	m_camera.Xiw[3][2] = 0; 	m_camera.Xiw[3][3] = 1;

	// Compute Xpi:
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			m_camera.Xpi[i][j] = 0;
		}
	}
	m_camera.Xpi[0][0] = 1;
	m_camera.Xpi[1][1] = 1;
	m_camera.Xpi[2][2] = (float)tan((double)(m_camera.FOV * PI / 180.0) / 2.0);
	m_camera.Xpi[3][2] = (float)tan((double)(m_camera.FOV * PI / 180.0) / 2.0);
	m_camera.Xpi[3][3] = 1;

	// Stack Operations
	int stack = GzPushMatrix(Xsp) || GzPushMatrix(m_camera.Xpi) || GzPushMatrix(m_camera.Xiw);
	if (!stack)
	{
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;

}


float GzRender::MagnitudeOfVector(GzCoord& c)
{
	return (float)sqrt((double)(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]));
}

float GzRender::DotProduct3v(GzCoord& a, GzCoord& b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

int GzRender::GzPutCamera(GzCamera camera)
{
	/* HW 3.8
	/*- overwrite renderer camera structure with new camera definition
	*/
	// copying p,v, up and FOX
	for (int i = 0; i < 3; i++) {
		m_camera.position[i] = camera.position[i];
		m_camera.lookat[i] = camera.lookat[i];
		m_camera.worldup[i] = camera.worldup[i];
	}
	m_camera.FOV = camera.FOV;
	// copying Xiw
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			m_camera.Xiw[i][j] = camera.Xiw[i][j];
		}
	}
	// copying Xpi
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			m_camera.Xpi[i][j] = camera.Xpi[i][j];
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/

	// Stack has not overflowed
	if (matlevel < MATLEVELS) {
		// Stack is actually empty
		if (matlevel == -1) {
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					Ximage[0][i][j] = matrix[i][j];
				}
			}
		}
		else {
			// Multiply the Matrices
			for (int k = 0; k < 4; k++) {
				for (int j = 0; j < 4; j++) {
					float sum = 0;
					for (int i = 0; i < 4; i++) {
						sum += Ximage[matlevel][k][i] * matrix[i][j];
					}
					Ximage[matlevel + 1][k][j] = sum;
				}
			}
		}
		// increment the number of elements in the stack
		matlevel += 1;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
	/* HW 3.10
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	if (matlevel > -1) {
		matlevel--;
	}
	//underflow
	else
		return GZ_FAILURE;
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
		if (z < pixelbuffer[loc].z)
			pixelbuffer[loc] = thisPixel;
	}

	return GZ_SUCCESS;
}

int GzRender::GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z)
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


int GzRender::GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList)
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
			float* color = (float*)valueList[0];
			flatcolor[0] = color[0];
			flatcolor[1] = color[1];
			flatcolor[2] = color[2];
		}
	}
	return GZ_SUCCESS;
}
int GzRender::GzPutTriangle(int numParts, GzToken* nameList, GzPointer* valueList)
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
		GzCoord* ptrToData = (GzCoord*)valueList[0];
		GzCoord verts[3];

		// Construct XYZ vertices of Triangle 4D Matrix:
		float vertices4D[3][4];
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				vertices4D[j][i] = ptrToData[j][i];
			}
		}
		vertices4D[0][3] = 1.0;	vertices4D[1][3] = 1.0;	vertices4D[2][3] = 1.0;

		float finalVerticeMatrix[3][4];
		// Transformation M * Vertex Coord.
		for (int k = 0; k < 3; k++) {
			for (int j = 0; j < 4; j++) {
				float sum = 0;
				for (int i = 0; i < 4; i++) {
					sum += Ximage[matlevel][j][i] * vertices4D[k][i];
				}
				finalVerticeMatrix[k][j] = sum;
			}
		}
		// 4D => 3D divide by W (normalizing)
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				verts[j][i] = finalVerticeMatrix[j][i] / finalVerticeMatrix[j][3];
			}
		}

		// Y
		SortByY(verts);
		// Sort by X and check for Flat Top and Flat Bottom Edge Case
		SortByXSpecialCases(verts);
		// Create a bounding box around the triangle
		float minX = ceil(min(min(verts[0][0], verts[1][0]), verts[2][0]));
		float maxX = ceil(max(max(verts[0][0], verts[1][0]), verts[2][0]));
		float minY = ceil(min(min(verts[0][1], verts[1][1]), verts[2][1]));
		float maxY = ceil(max(max(verts[0][1], verts[1][1]), verts[2][1]));

		// Calculate co-efficient of Edge Equation
		// Compute ABC for each edge (slide 15)
		float A12 = CalculateCoEfficientA(verts[1][1], verts[0][1]); // y2-y1 = deltaY12
		float B12 = CalculateCoEfficientB(verts[1][0], verts[0][0]);	// - x2-x1 = -deltaX12
		// deltaX12 * Y2 - deltaY12 * X2
		float C12 = CalculateCoEfficientC(verts[0][0], verts[1][0], verts[0][1], verts[1][1]); // (x2-x1)*y2 + (y2-y1)* x2
		float A23 = CalculateCoEfficientA(verts[2][1], verts[1][1]);
		float B23 = CalculateCoEfficientB(verts[2][0], verts[1][0]);
		float C23 = CalculateCoEfficientC(verts[1][0], verts[2][0], verts[1][1], verts[2][1]);
		float A31 = CalculateCoEfficientA(verts[0][1], verts[2][1]);
		float B31 = CalculateCoEfficientB(verts[0][0], verts[2][0]);
		float C31 = CalculateCoEfficientC(verts[2][0], verts[0][0], verts[2][1], verts[0][1]);


		// Start Rasterization:
		for (int i = minX; i <= maxX; i++) {
			for (int j = minY; j <= maxY; j++) {
				float edge1To2 = A12 * (float)i + B12 * (float)j + C12;
				float edge2To3 = A23 * (float)i + B23 * (float)j + C23;
				float edge3to1 = A31 * (float)i + B31 * (float)j + C31;
				// if all signs -ve  or all edqge equation = 0
				if ((edge1To2 < 0 && edge2To3 < 0 && edge3to1 < 0) || edge1To2 == 0 || edge2To3 == 0 || edge3to1 == 0)
				{
					GzIntensity redIntensity = ctoi(flatcolor[0]);
					GzIntensity greenIntensity = ctoi(flatcolor[1]);
					GzIntensity blueIntensity = ctoi(flatcolor[2]);
					// Call GzPut to push the pixel to pixelbuffer.
					GzPut(i, j, redIntensity, greenIntensity, blueIntensity, 1, ceil(ZInterPolate(i, j, verts)));
				}

			}
		}
	}
	return GZ_SUCCESS;
}
void GzRender::SortByXSpecialCases(GzCoord  vertices[3])
{
	// V1y = V2y ( FLAT TOP )
	if (ceil(vertices[0][1]) == ceil(vertices[1][1])) {
		if (vertices[0][0] > vertices[1][0]) {
			SwapMatrixElements(vertices, 0, 1);
		}
	}
	// V2y = V3y (FLAT BOTTON)
	else if (ceil(vertices[1][1]) == ceil(vertices[2][1])) {
		if (vertices[2][0] > vertices[1][0]) {
			SwapMatrixElements(vertices, 2, 1);
		}
	}
	// Neither FB or FT
	else {
		float  X_mid;

		if (ceil(vertices[0][0]) == ceil(vertices[2][0])) {
			X_mid = vertices[0][0];
		}
		else {
			float slopeOfSideEdge = (vertices[0][1] - vertices[2][1]) / (vertices[0][0] - vertices[2][0]);
			X_mid = (vertices[1][1] - vertices[0][1]) / slopeOfSideEdge + vertices[0][0];
		}


		if (X_mid > vertices[1][0]) {
			SwapMatrixElements(vertices, 1, 2);
		}
	}
}
void GzRender::SortByY(GzCoord  vertices[3])
{
	if (vertices[0][1] > vertices[1][1]) {
		SwapMatrixElements(vertices, 0, 1);
	}
	if (vertices[0][1] > vertices[2][1]) {
		SwapMatrixElements(vertices, 0, 2);
	}
	if (vertices[1][1] > vertices[2][1]) {
		SwapMatrixElements(vertices, 1, 2);
	}
}
void GzRender::SwapMatrixElements(GzCoord vertices[3], int i, int j)
{
	float x = vertices[i][0];
	float y = vertices[i][1];
	float z = vertices[i][2];
	vertices[i][0] = vertices[j][0];
	vertices[i][1] = vertices[j][1];
	vertices[i][2] = vertices[j][2];
	vertices[j][0] = x;
	vertices[j][1] = y;
	vertices[j][2] = z;
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

float GzRender::CalculateCoEfficientA(float x, float y)
{
	return x - y;
}
float GzRender::CalculateCoEfficientB(float x, float y)
{
	return (-1.0f) * (x - y);
}
float GzRender::CalculateCoEfficientC(float x1, float x2, float y1, float y2)
{
	return (x2 - x1) * y2 - (y2 - y1) * x2;
}

float GzRender::ZInterPolate(float i, float j, GzCoord verts[3])
{	
	// slide 18
	// Ax + By + Cz + D = 0
	// we know x and y and z at vertex 0.
	// A = cross product of y and z terms and so on
	float crossBC = ((verts[1][1] - verts[0][1]) * (verts[2][2] - verts[0][2])) - ((verts[1][2] - verts[0][2]) * (verts[2][1] - verts[0][1]));
	float crossAC = -(((verts[1][0] - verts[0][0]) * (verts[2][2] - verts[0][2])) - ((verts[1][2] - verts[0][2]) * (verts[2][0] - verts[0][0])));
	float crossAB = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
	float D = -1.0f * (crossBC * verts[0][0] + crossAC * verts[0][1] + crossAB * verts[0][2]);
	// Ax + By + Cz + D = 0
	return	-(1.0f * (crossBC * (float)i + crossAC * (float)j + D) / crossAB);
}


