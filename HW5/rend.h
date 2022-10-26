#include	"gz.h"
#ifndef GZRENDER_
#define GZRENDER_


/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

class GzRender{			/* define a renderer */
  

public:
	unsigned short	xres;
	unsigned short	yres;
	GzPixel		*pixelbuffer;		/* frame buffer array */
	char* framebuffer;

	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
	GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
	GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
	GzColor		flatcolor;          /* color state for flat shaded triangles */
	int			interp_mode;
	int			numlights;
	GzLight		lights[MAX_LIGHTS];
	GzLight		ambientlight;
	GzColor		Ka, Kd, Ks;
	float		    spec;		/* specular power */
	GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */

  	// Constructors
	GzRender(int xRes, int yRes);
	void CameraInit();
	~GzRender();

	// Function declaration

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	float MagnitudeOfVector(GzCoord& c);
	float DotProduct3v(GzCoord& a, GzCoord& b);
	int GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
	int GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth	*z);

	int GzFlushDisplay2File(FILE* outfile);
	GzIntensity GzClampPixelValueFloor(GzIntensity pixelValue);
	GzIntensity GzClampPixelValueCeil(GzIntensity pixelValue);
	GzIntensity GzBitShift4PixelValue(GzIntensity pixelValue);
	int GzFlushDisplay2FrameBuffer();

	// HW2: Render methods
	int GzPutAttribute(int numAttributes, GzToken *nameList, GzPointer *valueList);
	int GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList);

	void ClampR(float& R_E);

	void GzRender::SortByXSpecialCases(GzCoord  vertices[3], GzCoord normals[3], GzTextureIndex UVList[3]);

	void GzRender::SortByY(GzCoord  vertices[3], GzCoord normals[3], GzTextureIndex UVList[3]);

	void SwapMatrixElements(GzCoord vertices[3], int i, int j);

	void SwapMatrixElements(GzTextureIndex vertices[3], int i, int j);

	float CalculateCoEfficientA(float x, float y);

	float CalculateCoEfficientB(float x, float y);

	float CalculateCoEfficientC(float x1, float x2, float y1, float y2);

	float ZInterPolate(float i, float j, GzCoord verts[3]);

	float GInterPolate(float i, float j, float a, float b, float c, float d);

	// HW3
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix	matrix);
	int AddToStack(short& currentStackLevel, GzMatrix* matStack, GzMatrix mat);
	void IdentityMatrix(GzMatrix& matrix_I);
	int GzPopMatrix();

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y){return (x+y*xres);}	/* simplify fbuf indexing */
	inline short	ctoi(float color) {return(short)((int)(color * ((1 << 12) - 1)));}		/* convert float color to GzIntensity short */


	// Object Translation
	int GzRotXMat(float degree, GzMatrix mat);
	int GzRotYMat(float degree, GzMatrix mat);
	int GzRotZMat(float degree, GzMatrix mat);
	int GzTrxMat(GzCoord translate, GzMatrix mat);
	int GzScaleMat(GzCoord scale, GzMatrix mat);

};
#endif