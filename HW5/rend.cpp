/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include	<algorithm>

#define PI (float) 3.14159265358979323846

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

#define RGBA_DIMEMSION	4	/* RGBA -> 4D color */
#define RGB_DIMEMSION	3	/* RGB -> 3D color */
#define COLOR_LIMIT		4095	/* Clamping the color value into 0~4095. */

static short matlevelNormal;	/* Global variable of the head of matrix stack of normals */
int pushMatToStack(short& head, GzMatrix* matStack, GzMatrix mat);
int popMatToStack(short& head, GzMatrix* matStack);

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
	// mistake from previous solution
	// requesting for regrade
	mat[0][0] = 1.0;

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

	// lights
	numlights = 0;
	matlevelNormal = -1;
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
	GzCoord cl, newUp, the_X, the_Y;
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
	// Push Xsp:
	int stack = GzPushMatrix(Xsp) || GzPushMatrix(m_camera.Xpi) || GzPushMatrix(m_camera.Xiw);

	if (stack)
		return GZ_FAILURE;
	else
		return GZ_SUCCESS;

}
// helper function for calculating magnitude
float GzRender::MagnitudeOfVector(GzCoord& c)
{
	return (float)sqrt((double)(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]));
}
// helper function for calculating dot product
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

	int success = 0;


	GzMatrix matrix_I;
	IdentityMatrix(matrix_I);
	GzMatrix Xnorm_iw;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			Xnorm_iw[j][i] = m_camera.Xiw[j][i];
		}
	}
	Xnorm_iw[0][3] = 0;
	Xnorm_iw[1][3] = 0;
	Xnorm_iw[2][3] = 0;
	success |= AddToStack(matlevel, Ximage, matrix);
	// Pushing the corrext matrix among Identity, XNorm, and recent matrix
	if (matlevelNormal < 2) {	// Xsp and Xpi, Push I
		success |= AddToStack(matlevelNormal, Xnorm, matrix_I);
	}
	else if (matlevelNormal == 2) {	// Xiw.


		success |= AddToStack(matlevelNormal, Xnorm, Xnorm_iw);
	}
	else {
		if (matrix[0][1] == 0 && matrix[0][2] == 0 && matrix[1][0] == 0
			&& matrix[1][2] == 0 && matrix[2][0] == 0 && matrix[2][1] == 0) {
			success |= AddToStack(matlevelNormal, Xnorm, matrix_I);
		}
		else {
			success |= AddToStack(matlevelNormal, Xnorm, matrix);
		}
	}

	if (success)
		return GZ_FAILURE;
	return GZ_SUCCESS;

}
int GzRender::AddToStack(short& currentStackLevel, GzMatrix* matStack, GzMatrix mat) {

	// Stack has not overflowed
	if (currentStackLevel < MATLEVELS) {
		// Stack is actually empty
		if (currentStackLevel == -1) {
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					matStack[0][i][j] = mat[i][j];
				}
			}
		}
		else {
			// Multiply the Matrices
			for (int k = 0; k < 4; k++) {
				for (int j = 0; j < 4; j++) {
					float sum = 0;
					for (int i = 0; i < 4; i++) {
						sum += matStack[currentStackLevel][k][i] * mat[i][j];
					}
					matStack[currentStackLevel + 1][k][j] = sum;
				}
			}
		}
		// increment the number of elements in the stack
		currentStackLevel++;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
}
void GzRender::IdentityMatrix(GzMatrix& matrix_I)
{
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			matrix_I[j][i] = 0;
		}
	}
	matrix_I[0][0] = 1.0f;
	matrix_I[1][1] = 1.0f;
	matrix_I[2][2] = 1.0f;
	matrix_I[3][3] = 1.0f;
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
	/* HW1.5 retrieve a pixel information from the pixel buffer*/
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
int GzRender::GzFlushDisplay2FrameBuffer()
{
	/* HW1.7 write pixels to framebuffer:
		- put the pixels into the frame buffer
		- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red
		- NOT red, green, and blue !!!
	*/
	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		GzPixel currentPixel = pixelbuffer[i];

		GzIntensity gotRed = currentPixel.red;
		GzIntensity gotGreen = currentPixel.green;
		GzIntensity gotBlue = currentPixel.blue;
		GzDepth gotZ = currentPixel.z;
		if (currentPixel.red < 0)
			gotRed = 0;
		if (currentPixel.red > COLOR_LIMIT)
			gotRed = COLOR_LIMIT;
		if (currentPixel.green < 0)
			gotGreen = 0;
		if (currentPixel.green > COLOR_LIMIT)
			gotGreen = COLOR_LIMIT;
		if (currentPixel.blue < 0)
			gotBlue = 0;
		if (currentPixel.blue > COLOR_LIMIT)
			gotBlue = COLOR_LIMIT;

		GzIntensity red2 = gotRed >> 4;
		char redValue = (char)(red2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 2] = redValue;

		GzIntensity green2 = gotGreen >> 4;
		char greenValue = (char)(green2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 1] = greenValue;

		GzIntensity blue2 = gotBlue >> 4;
		char blueValue = (char)(blue2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i] = blueValue;

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
			float* color = (float*)valueList[i];
			flatcolor[0] = color[0];
			flatcolor[1] = color[1];
			flatcolor[2] = color[2];
		}
		if (nameList[i] == GZ_INTERPOLATE)
		{
			int* interp = (int*)valueList[i];
			interp_mode = *interp;
		}
		if (nameList[i] == GZ_DIRECTIONAL_LIGHT)
		{
			GzLight* dir = (GzLight*)valueList[i];
			lights[numlights].direction[0] = dir->direction[0];
			lights[numlights].direction[1] = dir->direction[1];
			lights[numlights].direction[2] = dir->direction[2];
			lights[numlights].color[0] = dir->color[0];
			lights[numlights].color[1] = dir->color[1];
			lights[numlights].color[2] = dir->color[2];
			numlights++;
		}
		if (nameList[i] == GZ_AMBIENT_LIGHT)
		{
			GzLight* ambL = (GzLight*)valueList[i];
			ambientlight.direction[0] = ambL->direction[0];
			ambientlight.direction[1] = ambL->direction[1];
			ambientlight.direction[2] = ambL->direction[2];
			ambientlight.color[0] = ambL->color[0];
			ambientlight.color[1] = ambL->color[1];
			ambientlight.color[2] = ambL->color[2];
		}
		if (nameList[i] == GZ_AMBIENT_COEFFICIENT)
		{
			GzColor* ambA = (GzColor*)valueList[i];
			Ka[0] = (*ambA)[0]; Ka[1] = (*ambA)[1]; Ka[2] = (*ambA)[2];
		}
		if (nameList[i] == GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor* diff = (GzColor*)valueList[i];
			Kd[0] = (*diff)[0]; Kd[1] = (*diff)[1]; Kd[2] = (*diff)[2];
		}
		if (nameList[i] == GZ_SPECULAR_COEFFICIENT)
		{
			GzColor* spec = (GzColor*)valueList[i];
			Ks[0] = (*spec)[0]; Ks[1] = (*spec)[1]; Ks[2] = (*spec)[2];
		}
		if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT)
		{
			float* distribution = (float*)valueList[i];
			spec = *distribution;
		}
		if(nameList[i] == GZ_TEXTURE_MAP)
		{
			GzTexture tex = (GzTexture)valueList[i];
			if (tex != NULL)
				tex_fun = tex;
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

	GzCoord* ptrToVertices = (GzCoord*)valueList[0];
	GzCoord verts[3];
	GzCoord* ptrToNormals = (GzCoord*)valueList[1];
	GzCoord normals[3];
	// Zero Initialization of Intensities
	GzColor S_I[3], D_I[3], A_I[3], T_I[3];
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			S_I[j][i] = 0;
			D_I[j][i] = 0;
			A_I[j][i] = 0;
			T_I[j][i] = 0;
		}
	}

	GzTextureIndex* UV = (GzTextureIndex*)valueList[2];
	GzTextureIndex UVData[3];
	// Fetch UV Data
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 2; i++) {
			UVData[j][i] = UV[j][i];
		}
	}


	// Construct XYZ vertices of Triangle 4D Matrix:
	float vertices4D[3][4];
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			vertices4D[j][i] = ptrToVertices[j][i];
		}
	}
	vertices4D[0][3] = 1.0;	vertices4D[1][3] = 1.0;	vertices4D[2][3] = 1.0;

	// Construct the Normals Matrix
	float normals4D[3][4];
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			normals4D[j][i] = ptrToNormals[j][i];
		}
	}
	normals4D[0][3] = 1.0;	normals4D[1][3] = 1.0;	normals4D[2][3] = 1.0;

	float finalVerticeMatrix[3][4];
	// Transformation M * Vertex Coord.
	for (int k = 0; k < 3; k++) {
		for (int j = 0; j < 4; j++) {
			float sum = 0, sum2 = 0;
			for (int i = 0; i < 4; i++) {
				sum += Ximage[matlevel][j][i] * vertices4D[k][i];
			}
			finalVerticeMatrix[k][j] = sum;
		}
	}
	float finalNormalMatrix[3][4];
	// Transformation M * Vertex Coord.
	for (int k = 0; k < 3; k++) {
		for (int j = 0; j < 4; j++) {
			float sum = 0, sum2 = 0;
			for (int i = 0; i < 4; i++) {
				sum += Xnorm[matlevel][j][i] * normals4D[k][i];
			}
			finalNormalMatrix[k][j] = sum;
		}
	}


	// 4D => 3D
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			verts[j][i] = finalVerticeMatrix[j][i] / finalVerticeMatrix[j][3];
			normals[j][i] = finalNormalMatrix[j][i] / finalNormalMatrix[j][3];
		}
	}

	SortByY(verts, normals, UVData);
	// Sort by X and check for Flat Top and Flat Bottom Edge Case
	SortByXSpecialCases(verts, normals, UVData);


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


	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < numlights; i++) {
			GzCoord R, EyeVector;

			EyeVector[0] = 0; EyeVector[1] = 0; EyeVector[2] = -1.0f;

			float N_L = normals[j][0] * (lights[i]).direction[0]
				+ normals[j][1] * (lights[i]).direction[1]
				+ normals[j][2] * (lights[i]).direction[2];
			float N_E = normals[j][0] * EyeVector[0] + normals[j][1] * EyeVector[1] + normals[j][2] * EyeVector[2];
			// both positive or both negative
			// compute lighting model
			if (N_L * N_E > 0) {
				for (int p = 0; p < 3; p++) {
					R[p] = 2.0f * N_L * normals[j][p] - (lights[i]).direction[p];
				}
				R[0] /= MagnitudeOfVector(R);
				R[1] /= MagnitudeOfVector(R);
				R[2] /= MagnitudeOfVector(R);

				for (int k = 0; k < 3; k++) {
					float R_E = R[0] * EyeVector[0] + R[1] * EyeVector[1] + R[2] * EyeVector[2];
					ClampR(R_E);

					S_I[j][k] += Ks[k] * (float)pow((double)(R_E), (double)spec) * (lights[i]).color[k];
					// Both positive
					if (N_L > 0 && N_E > 0) {
						D_I[j][k] += Kd[k] * (normals[j][0] * (lights[i]).direction[0] + normals[j][1] * (lights[i]).direction[1] + normals[j][2] * (lights[i]).direction[2]) * (lights[i]).color[k];
					}
					// flip normal
					else {
						D_I[j][k] += Kd[k] * (-1.0f * normals[j][0] * (lights[i]).direction[0] - 1.0f * normals[j][1] * (lights[i]).direction[1] - 1.0f * normals[j][2] * (lights[i]).direction[2]) * (lights[i]).color[k];
					}
				}
			}
		}


		A_I[j][0] += Ka[0] * ambientlight.color[0];
		A_I[j][1] += Ka[1] * ambientlight.color[1];
		A_I[j][2] += Ka[2] * ambientlight.color[2];

	}
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			T_I[j][i] = S_I[j][i] + D_I[j][i] + A_I[j][i];
			ClampR(T_I[i][j]);
		}
	}

	// Interpolation of the colors
	float redA = (verts[1][1] - verts[0][1] * T_I[2][0] - T_I[0][0]) - (T_I[1][0] - T_I[0][0] * verts[2][1] - verts[0][1]);
	float greenA = (verts[1][1] - verts[0][1] * T_I[2][1] - T_I[0][1]) - (T_I[1][1] - T_I[0][1] * verts[2][1] - verts[0][1]);
	float blueA = (verts[1][1] - verts[0][1] * T_I[2][2] - T_I[0][2]) - (T_I[1][2] - T_I[0][2] * verts[2][1] - verts[0][1]);
	float NormXA = ((verts[1][1] - verts[0][1]) * (normals[2][0] - normals[0][0])) - ((normals[1][0] - normals[0][0]) * (verts[2][1] - verts[0][1]));
	float normYA = ((verts[1][1] - verts[0][1]) * (normals[2][1] - normals[0][1])) - ((normals[1][1] - normals[0][1]) * (verts[2][1] - verts[0][1]));
	float normZA = ((verts[1][1] - verts[0][1]) * (normals[2][2] - normals[0][2])) - ((normals[1][2] - normals[0][2]) * (verts[2][1] - verts[0][1]));

	float redB = -((verts[1][0] - verts[0][0] * T_I[2][0] - T_I[0][0]) - (T_I[1][0] - T_I[0][0] * verts[2][0] - verts[0][0]));
	float greenB = -((verts[1][0] - verts[0][0] * T_I[2][1] - T_I[0][1]) - (T_I[1][1] - T_I[0][1] * verts[2][0] - verts[0][0]));
	float blueB = -((verts[1][0] - verts[0][0] * T_I[2][2] - T_I[0][2]) - (T_I[1][2] - T_I[0][2] * verts[2][0] - verts[0][0]));
	float normalXB = -(((verts[1][0] - verts[0][0]) * (normals[2][0] - normals[0][0])) - ((normals[1][0] - normals[0][0]) * (verts[2][0] - verts[0][0])));
	float normalYB = -(((verts[1][0] - verts[0][0]) * (normals[2][1] - normals[0][1])) - ((normals[1][1] - normals[0][1]) * (verts[2][0] - verts[0][0])));
	float normalZB = -(((verts[1][0] - verts[0][0]) * (normals[2][2] - normals[0][2])) - ((normals[1][2] - normals[0][2]) * (verts[2][0] - verts[0][0])));

	float redC = (verts[1][0] - verts[0][0] * verts[2][1] - verts[0][1]) - (verts[1][1] - verts[0][1] * verts[2][0] - verts[0][0]);
	float greenC = (verts[1][0] - verts[0][0] * verts[2][1] - verts[0][1]) - (verts[1][1] - verts[0][1] * verts[2][0] - verts[0][0]);
	float blueC = (verts[1][0] - verts[0][0] * verts[2][1] - verts[0][1]) - (verts[1][1] - verts[0][1] * verts[2][0] - verts[0][0]);
	float normalXC = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
	float normalYC = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
	float normalZC = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));

	float redD = -1.0f * (redA * verts[0][0] + redB * verts[0][1] + redC * T_I[0][0]);
	float greenD = -1.0f * (greenA * verts[0][0] + greenB * verts[0][1] + greenC * T_I[0][1]);
	float blueD = -1.0f * (blueA * verts[0][0] + blueB * verts[0][1] + blueC * T_I[0][2]);
	float normalXD = -1.0f * (NormXA * verts[0][0] + normalXB * verts[0][1] + normalXC * normals[0][0]);
	float normalYD = -1.0f * (normYA * verts[0][0] + normalYB * verts[0][1] + normalYC * normals[0][1]);
	float normalZD = -1.0f * (normZA * verts[0][0] + normalZB * verts[0][1] + normalZC * normals[0][2]);


	GzTextureIndex correctedUVData[3];
	for (int j = 0; j < 3; j++) {
		float ZPrime = (verts[j][2] / ((float)MAXINT - verts[j][2])) + 1.0f;
		correctedUVData[j][U] = UVData[j][U] / (ZPrime); // U
		correctedUVData[j][V] = UVData[j][V] / (ZPrime); // V
	}

	float uA = ((verts[1][1] - verts[0][1]) * (correctedUVData[2][0] - correctedUVData[0][0])) - ((correctedUVData[1][0] - correctedUVData[0][0]) * (verts[2][1] - verts[0][1]));
	float uB = -(((verts[1][0] - verts[0][0]) * (correctedUVData[2][0] - correctedUVData[0][0])) - ((correctedUVData[1][0] - correctedUVData[0][0]) * (verts[2][0] - verts[0][0])));
	float uC = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
	float interPolateduD = -1.0f * (uA * verts[0][0] + uB * verts[0][1] + uC * correctedUVData[0][0]);

	float vA = ((verts[1][1] - verts[0][1]) * (correctedUVData[2][1] - correctedUVData[0][1])) - ((correctedUVData[1][1] - correctedUVData[0][1]) * (verts[2][1] - verts[0][1]));
	float vB = -(((verts[1][0] - verts[0][0]) * (correctedUVData[2][1] - correctedUVData[0][1])) - ((correctedUVData[1][1] - correctedUVData[0][1]) * (verts[2][0] - verts[0][0])));
	float vC = ((verts[1][0] - verts[0][0])* (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
	float interPolatedvD = -1.0f * (vA * verts[0][0] + vB * verts[0][1] + vC * correctedUVData[0][1]);

	// Get Bounding Box:
	float minX = ceil(min(min(verts[0][0], verts[1][0]), verts[2][0]));
	float maxX = ceil(max(max(verts[0][0], verts[1][0]), verts[2][0]));
	float minY = ceil(min(min(verts[0][1], verts[1][1]), verts[2][1]));
	float maxY = ceil(max(max(verts[0][1], verts[1][1]), verts[2][1]));
	// Start Rasterization The TeApot
	for (int i = minX; i <= maxX; i++) {
		for (int j = minY; j <= maxY; j++) {
			float edge1To2 = A12 * (float)i + B12 * (float)j + C12;
			float edge2To3 = A23 * (float)i + B23 * (float)j + C23;
			float edge3to1 = A31 * (float)i + B31 * (float)j + C31;

			if (((edge1To2 < 0 && edge2To3 < 0 && edge3to1 < 0) || edge1To2 == 0 || edge2To3 == 0 || edge3to1 == 0)
				&& redC * greenC * blueC != 0
				&& normalXC * normalYC * normalZC != 0) {

				int currentZ = ceil(ZInterPolate(i, j, verts));
				if (currentZ >= 0) {
					// Color Calculation:
					GzIntensity redIntensity = 0, greenIntensity = 0, blueIntensity = 0;
					if (interp_mode == GZ_FLAT) {	// No Shading
						redIntensity = ctoi(flatcolor[0]);
						greenIntensity = ctoi(flatcolor[1]);
						blueIntensity = ctoi(flatcolor[2]);
					}
					else if (interp_mode == GZ_COLOR) {	//Gauroud Shading
						GzColor intensity;
						intensity[0] = ctoi(GInterPolate(i, j, redA, redB, redC, redD));
						intensity[1] = ctoi(GInterPolate(i, j, greenA, greenB, greenC, greenD));
						intensity[2] = ctoi(GInterPolate(i, j, blueA, blueB, blueC, blueD));

						if (tex_fun != NULL) {
							GzTextureIndex tempUV;
							GzColor uvColor;
							tempUV[U] = GInterPolate(i,j,uA,uB,uC, interPolateduD) * (((float)currentZ / ((float)MAXINT - (float)currentZ)) + 1.0f);
							tempUV[V] = GInterPolate(i, j, vA, vB, vC, interPolatedvD) * (((float)currentZ / ((float)MAXINT - (float)currentZ)) + 1.0f);

							int status = tex_fun(tempUV[U], tempUV[V], uvColor);
							intensity[0] *= uvColor[0]; intensity[1] *= uvColor[1]; intensity[1] *= uvColor[1];
						}

						redIntensity = ctoi(intensity[0]);
						greenIntensity = ctoi(intensity[1]);
						blueIntensity = ctoi(intensity[2]);

					}
					else if (interp_mode == GZ_NORMALS) {	// Phong Shading
						GzColor intensity;
						GzCoord InterpolatedN;
						InterpolatedN[0] = GInterPolate(i, j, NormXA, normalXB, normalXC, normalXD);
						InterpolatedN[1] = GInterPolate(i, j, normYA, normalYB, normalYC, normalYD);;
						InterpolatedN[2] = GInterPolate(i, j, normZA, normalZB, normalZC, normalZD);;		
						if (tex_fun != NULL) {
							GzTextureIndex tempUV;
							GzColor uvColor;
							tempUV[U] = GInterPolate(i, j, uA, uB, uC, interPolateduD) * (((float)currentZ / ((float)MAXINT - (float)currentZ)) + 1.0f);
							tempUV[V] = GInterPolate(i, j, vA, vB, vC, interPolatedvD) * (((float)currentZ / ((float)MAXINT - (float)currentZ)) + 1.0f);
							int status = tex_fun(tempUV[U], tempUV[V], uvColor);
							
							for (int m = 0; m < 3; m++) {
								Kd[m] = uvColor[m];
								Ka[m] = uvColor[m];
							}

						}

						GzColor specularIntensity, diffuseIntensity, ambientIntensity;
						for (int m = 0; m < 3; m++) {
							specularIntensity[m] = 0;
							diffuseIntensity[m] = 0;
							ambientIntensity[m] = 0;
						}

						for (int x = 0; x < numlights; x++) {
							GzCoord R, EyeVector;

							EyeVector[0] = 0; EyeVector[1] = 0; EyeVector[2] = -1.0f;

							float N_L = InterpolatedN[0] * (lights[x]).direction[0]
								+ InterpolatedN[1] * (lights[x]).direction[1]
								+ InterpolatedN[2] * (lights[x]).direction[2];
							float N_E = InterpolatedN[0] * EyeVector[0] + InterpolatedN[1] * EyeVector[1] + InterpolatedN[2] * EyeVector[2];
							// both positive or both negative
							// compute lighting model
							if (N_L * N_E > 0) {
								for (int p = 0; p < 3; p++) {
									R[p] = 2.0f * N_L * InterpolatedN[p] - (lights[x]).direction[p];
								}
								R[0] /= MagnitudeOfVector(R);
								R[1] /= MagnitudeOfVector(R);
								R[2] /= MagnitudeOfVector(R);

								for (int k = 0; k < 3; k++) {
									float R_E = R[0] * EyeVector[0] + R[1] * EyeVector[1] + R[2] * EyeVector[2];
									ClampR(R_E);

									specularIntensity[k] += Ks[k] * (float)pow((double)(R_E), (double)spec) * (lights[x]).color[k];
									if (N_L > 0 && N_E > 0) {
										diffuseIntensity[k] += Kd[k] * (InterpolatedN[0] * (lights[x]).direction[0] + InterpolatedN[1] * (lights[x]).direction[1] + InterpolatedN[2] * (lights[x]).direction[2]) * (lights[x]).color[k];
									}
									else {
										diffuseIntensity[k] += Kd[k] * (-1.0f * InterpolatedN[0] * (lights[x]).direction[0] - 1.0f * InterpolatedN[1] * (lights[x]).direction[1] - 1.0f * InterpolatedN[2] * (lights[x]).direction[2]) * (lights[x]).color[k];
									}
								}
							}
						}

						ambientIntensity[0] += Ka[0] * ambientlight.color[0];
						ambientIntensity[1] += Ka[1] * ambientlight.color[1];
						ambientIntensity[2] += Ka[2] * ambientlight.color[2];


						for (int m = 0; m < 3; m++) {
							intensity[m] = specularIntensity[m] + diffuseIntensity[m] + ambientIntensity[m];
							ClampR(intensity[m]);
						}

						redIntensity = ctoi(intensity[0]);
						greenIntensity = ctoi(intensity[1]);
						blueIntensity = ctoi(intensity[2]);
					}
					GzPut(i, j, redIntensity, greenIntensity, blueIntensity, 1, currentZ);
				}
			}
		}
	}
	return GZ_SUCCESS;
}

void GzRender::ClampR(float& R_E)
{
	if (R_E < 0) {
		R_E = 0;
	}
	if (R_E > 1.0f) {
		R_E = 1.0f;
	}
}



int popMatToStack(short& head, GzMatrix* matStack) {
	if (head > -1) {
		head--;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

void GzRender::SortByXSpecialCases(GzCoord  vertices[3], GzCoord normals[3], GzTextureIndex UVList[3])
{
	// V1y = V2y ( FLAT TOP )
	if (ceil(vertices[0][1]) == ceil(vertices[1][1])) {
		if (vertices[0][0] > vertices[1][0]) {
			SwapMatrixElements(vertices, 0, 1);
			SwapMatrixElements(normals, 0, 1);
			SwapMatrixElements(UVList, 0, 1);

		}
	}
	// V2y = V3y (FLAT BOTTON)
	else if (ceil(vertices[1][1]) == ceil(vertices[2][1])) {
		if (vertices[2][0] > vertices[1][0]) {
			SwapMatrixElements(vertices, 2, 1);
			SwapMatrixElements(normals, 2, 1);
			SwapMatrixElements(UVList, 2, 1);

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
			SwapMatrixElements(normals, 1, 2);
			SwapMatrixElements(UVList, 1, 2);

		}
	}
}
void GzRender::SortByY(GzCoord  vertices[3], GzCoord normals[3], GzTextureIndex UVList[3])
{
	if (vertices[0][1] > vertices[1][1]) {
		SwapMatrixElements(vertices, 0, 1);
		SwapMatrixElements(normals, 0, 1);
		SwapMatrixElements(UVList, 0, 1);

	}
	if (vertices[0][1] > vertices[2][1]) {
		SwapMatrixElements(vertices, 0, 2);
		SwapMatrixElements(normals, 0, 2);
		SwapMatrixElements(UVList, 0, 2);
	}
	if (vertices[1][1] > vertices[2][1]) {
		SwapMatrixElements(vertices, 1, 2);
		SwapMatrixElements(normals, 1, 2);
		SwapMatrixElements(UVList, 1, 2);
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
void GzRender::SwapMatrixElements(GzTextureIndex vertices[3], int i, int j)
{
	float x = vertices[i][0];
	float y = vertices[i][1];

	vertices[i][0] = vertices[j][0];
	vertices[i][1] = vertices[j][1];

	vertices[j][0] = x;
	vertices[j][1] = y;

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

float GzRender::GInterPolate(float i, float j, float a, float b, float c, float d)
{
	// Ax + By + Cz + D = 0
	return	-(1.0f * (a * (float)i + b * (float)j + d) / c);
}

