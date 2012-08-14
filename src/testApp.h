#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxVectorField.h"

class testApp : public ofSimpleApp{

	public:
		//****************************************************** buit in OF method prototypes *****************************************************************
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

		//*********************************************** other **************************************************************
		void ourProcessImage();
		void ourSetPixel(int horizontal,int vertical,unsigned char R,unsigned char G,unsigned char B,int width, unsigned char pixels[]);
		void ourGetPixel(int horizontal,int vertical,unsigned char* R,unsigned char* G,unsigned char* B,int width, unsigned char  pixels[]);
		void copyPixelArrays(unsigned char sourceArray[],unsigned char destArray[] ,int Width,int Height, int bpp);
		void copyPixelArrays(unsigned char* sourceArray,unsigned char* destArray ,int sourceWidth,int sourceHeight,int destWidth,int destHeight );
		void copyPixelArrays(unsigned char* sourceArray,unsigned char* destArray ,int Width,int Height, int sourceRectL,int sourceRectT,int sourceRectR,int sourceRectB, int destRectL,int destRectT,int destRectR,int destRectB);
		void clearPixelArrays(unsigned char* pixelArray,int Width,int Height,unsigned char R ,unsigned char G,unsigned char B );
		void Convolve(int horizontal,int vertical,unsigned char* R,unsigned char* G,unsigned char* B,int MatrixSize,double* Matrix,int width,unsigned char pixels[]);
		void morph();
		void edges();

		//************** Pixel by Pixel course macros (works like a method but is precompiled with the rest of the code******************************************
		#define CLIP(val, low, high) {if(val<low) val=low; if(val>high)val=high;}


		//*********************************************** global variables (actually class variables) ***********************************************************

		int						    width, height;
		unsigned char				*ourImagePixels, *ourResultPixels, *displayPixels, *edgePixels;
		ofTexture					ourResultTexture;
		ofImage					    inputImage, referenceImage;
		ofImage                     resultImage, displayImage, edgeImage;
        ofImage                     coarse, medium, fine, edge;
        ofxVectorField              vectorField;
        string                      pointFile;
};

#endif
