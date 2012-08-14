#include "testApp.h"
using namespace std;
int threshold = 30;

string picture = "anne";
//--------------------------------------------------------------
void testApp::setup(){
    inputImage.loadImage(picture + ".jpg");
    height = inputImage.height;
	width = inputImage.width;
	ofSetWindowShape(width, height);
	edgeImage = inputImage;
    referenceImage.loadImage("jeanne.jpg");
    pointFile = (picture + ".txt");
    morph(); // morphs, outputs to input again!!

    ourImagePixels = inputImage.getPixels();
    ourResultTexture = *  new ofTexture();								// create a ofTexture to hold the result info
	ourResultTexture.allocate(width, height, GL_RGB);					// allocate memory for the ofTextue (This is RGB no Alpha)
	edgePixels = new unsigned char[width*height*3];
	ourResultPixels = new unsigned char[width*height*4];

    vectorField.setup(1000, 1000, 10);
    vectorField.setFromImage(inputImage);
    vectorField.normalize();
	vectorField.scale(5);
	vectorField.bias(0, 0.5);
	vectorField.blur();

	coarse.loadImage("large.png");								// load brush images for big size brush stroke
	coarse.resize(200,90);										// set its size

	medium.loadImage("medium.png");							// middle size brush
	medium.resize(100,45);

	fine.loadImage("small.png");								// small brush
	fine.resize(17,8);

	edge.loadImage("edge.png");								// small brush
	edge.resize(2,2);

	ofEnableAlphaBlending();

}
//--------------------------------------------------------------
void testApp::update(){
    ofBackground(255,255,255);
}
//--------------------------------------------------------------
void testApp::draw(){
    unsigned char R,G,B, sR, sG, sB, iR, iG, iB;
	int x, y = 0,i=0,j=0;
	ofVec2f pointVector;
    ofSetupScreen();

    ourProcessImage();
	ourResultTexture.loadData(ourResultPixels, width, height, GL_RGB);

	for (int x=0; x < width+100; x+= 60 ) {
		for ( int y=0; y < height; y+= 60) {
			ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
			glPushMatrix();
			glTranslatef(x, y, 0);
			ofSetColor(R,G,B,255);

            ofVec2f pointVector;
            pointVector = vectorField.getVectorInterpolated(x,y, width, height);
            glRotatef(85, pointVector.x, pointVector.y, 1);
			coarse.draw(-200, -80);
			glPopMatrix();
		}
	}

	displayImage.grabScreen(0, 0, width, height);
	displayPixels = displayImage.getPixels();

	for (int x=0; x < width+100; x+= 20) {
		for ( int y=0; y < height; y+= 20) {
            ourGetPixel(x, y, &sR,&sG,&sB, width, displayPixels);
            if ((((sR+sG+sB) - (R+G+B)) > 70) || (((R+G+B) - (sR+sG+sB)) > 70)){
                ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
                glPushMatrix();
                glTranslatef(x, y, 0);
                ofSetColor(R,G,B,255);
                pointVector = vectorField.getVectorInterpolated(x,y, width, height);
                glRotatef(85, pointVector.x, pointVector.y, 1);
                medium.draw(-50, -20);
                glPopMatrix();
            } else {
                ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
                ofSetColor(R,G,B,0);
                medium.draw(-6, -3);
            }
		}
	}

	displayImage.grabScreen(0, 0, width, height);
	displayPixels = displayImage.getPixels();

	// third brush
	for (int x=0; x < width; x+= 2 ) {
		for ( int y=0; y < height; y+= 2) {

			ourGetPixel(x, y, &sR,&sG,&sB, width, displayPixels);

			if ((((sR+sG+sB) - (R+G+B)) > 70) || (((R+G+B) - (sR+sG+sB)) > 70)){
				ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
				glPushMatrix();
				glTranslatef(x, y, 0);
				ofSetColor(R,G,B,255);
                pointVector = vectorField.getVectorInterpolated(x,y, width, height);
                glRotatef(85, pointVector.x, pointVector.y, 1);
				fine.draw(-1, -1);
				glPopMatrix();

			}else{
				ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
				ofSetColor(R,G,B,0);
				fine.draw(-6, -3);
			}
		}
	}

    // calculates the edges
    edges();

	// edge brush
	for (int x=0; x < width; x+= 1 ) {
		for ( int y=0; y < height; y+= 1) {
			ourGetPixel(x, y, &sR,&sG,&sB, width, edgePixels);
			if (sR == 0 ){
				ourGetPixel(x, y, &R,&G,&B, width, ourResultPixels);
				glPushMatrix();
				glTranslatef(x, y, 0);
				ofSetColor(R,G,B,255);
                pointVector = vectorField.getVectorInterpolated(x,y, width, height);
                glRotatef(85, pointVector.x, pointVector.y, 1);
				edge.draw(2, -1);
				glPopMatrix();
			}
		}
	}

    ofSetHexColor(0xFF0000);

    // Text
    ofDrawBitmapString("Original image (" + picture + ".jpg)", 30, height - 50);
    ofDrawBitmapString("'1, 2, 3' change image",  30, 10);

}
////--------------------------------------------------------------
//// CUSTOM STUFF
////--------------------------------------------------------------
void testApp::morph() {
    // ahhhhhhhhh!!
    vector< pair <ofPoint, ofPoint> > srcFeatures;
    vector< pair <ofPoint, ofPoint> > destFeatures;

    ofBuffer featureBuffer = ofBufferFromFile(pointFile);
    double asx, asy, aex, aey, bsx, bsy, bex, bey;
    int featureNum;
    featureNum = featureBuffer.size();
    for (int i=0; i < featureNum; i++) {
      string line = featureBuffer.getFirstLine();
      line >> asx >> asy >> aex >> aey >> bsx >> bsy >> bex >> bey;
      ofPoint p1=ofPoint(asx,asy);
      ofPoint p2=ofPoint(aex,aey);
      srcFeatures.push_back(make_pair(p1,p2));
      ofPoint p3=ofPoint(bsx,bsy);
      ofPoint p4=ofPoint(bex,bey);
      destFeatures.push_back(make_pair(p3,p4));
    }

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            double sumsdx=0, sumsdy=0, sumddx=0, sumddy=0, sumweight=0;
            for (int i=0; i<featureNum; i++) {
                double u,v,rawv,weight;
                pair<ofPoint,ofPoint> tp = destFeatures[i];
            }
        }
    }

    vector< pair <CvPoint, CvPoint> > targetFeatures(destFeatureNum);
  int fileIndex = 0;

  // increase time parameter
  for (double t=0; t<=1.0F; t+=1.0F/FRAMENUM) {
    // calculate intermediate feature line
    for (int i=0; i<destFeatureNum; i++) {
      pair <CvPoint, CvPoint> sp = srcFeatures[i];
      pair <CvPoint, CvPoint> dp = destFeatures[i];
      CvPoint t1 = cvPoint( (1.0F-t)*sp.first.x + t*dp.first.x, (1.0F-t)*sp.first.y + t*dp.first.y );
      CvPoint t2 = cvPoint( (1.0F-t)*sp.second.x + t*dp.second.x, (1.0F-t)*sp.second.y + t*dp.second.y );
      targetFeatures[i] = mp(t1, t2);
    }

    // calculate warped images from src image and dest image, and cross-dissolve two warped images into target image
    for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
        // warping
        double sumsdx=0, sumsdy=0, sumddx=0, sumddy=0, sumweight=0;
        for (int i=0; i<destFeatureNum; i++) {
          // calculate weight for point(x,y) with line(i)
          double u,v,rawv,weight;
          {
            pair<CvPoint,CvPoint> tp = targetFeatures[i];
            // vertical vector is ps.second.y-ps.first.y, -ps.second.x+ps.first.x
            double vx =  tp.second.y - tp.first.y;
            double vy = -tp.second.x + tp.first.x;
            double hx =  tp.second.x - tp.first.x;
            double hy =  tp.second.y - tp.first.y;
            double tx =  x - tp.first.x;
            double ty =  y - tp.first.y;

            // calc u
            u = (tx*hx + ty*hy) / lengthSquare(hx,hy);
            double vu = length(vx, vy);
            rawv = (vx*tx + vy*ty) / vu;
            if (u <= 0) {
              // v = PX
              v = length(tx, ty);
            } else if (u >= 1) {
              // v = QX
              v = length(x - tp.second.x, y - tp.second.y);
            } else {
              // vertical vector length
              v = abs(rawv);
            }
            double lineLength = length(hx, hy);
            weight = pow ((pow(lineLength, P)/(A+v)), B);
            assert(weight >= 0);
          }

          {
            pair<CvPoint, CvPoint> sf = srcFeatures[i];
            CvPoint sp = getMappingPoint(sf.first, sf.second, u, rawv);
            double sdx = x - sp.x;
            double sdy = y - sp.y;
            sumsdx += sdx*weight;
            sumsdy += sdy*weight;
          }

          {
            pair<CvPoint, CvPoint> df = destFeatures[i];
            CvPoint dp = getMappingPoint(df.first, df.second, u, rawv);
            double ddx = x - dp.x;
            double ddy = y - dp.y;
            sumddx += ddx*weight;
            sumddy += ddy*weight;
          }

          sumweight += weight;
        }
        double avesdx = sumsdx/sumweight;
        double avesdy = sumsdy/sumweight;
        double aveddx = sumddx/sumweight;
        double aveddy = sumddy/sumweight;

        int sx = (int)(x - avesdx);
        int sy = (int)(y - avesdy);
        int dx = (int)(x - aveddx);
        int dy = (int)(y - aveddy);
        if (sx < 0 || sx > srcImage->width || sy < 0 || sy > srcImage->height) {
          continue;
        }
        if (dx < 0 || dx > destImage->width || dy < 0 || dy > destImage->height) {
          continue;
        }

        // cross-dissolve
        int destIndex = destImage->widthStep * dy + dx * destImage->nChannels;
        int srcIndex = srcImage->widthStep * sy + sx * srcImage->nChannels;
        int targetIndex = morphImage->widthStep * y + x * morphImage->nChannels;
        for (int i=0; i<morphImage->nChannels; i++) {
          uchar dp = (destImage->imageData[destIndex+i]);
          uchar sp = (srcImage->imageData[srcIndex+i]);
          int diff =  (int)dp - (int)sp;
          int newvalue = diff * sigmoid(t) + sp;
          assert(newvalue <= 255 && newvalue >= 0);
          morphImage->imageData[targetIndex+i] = (uchar)newvalue;
        }
      }
    }
  }

}
//--------------------------------------------------------------
void testApp::edges(){
	unsigned char R,G,B, R2,G2, B2;
	int cell = 1, threshold= 200;
	for (int y = cell; y < height-cell; y++) {													//visit all pixels, make sure not to go under 0 or over width and height...
		for (int x = cell; x < width-cell; x++) {												// that's why w start at cell and go to width-cell
			ourGetPixel(x,y,&R,&G,&B,width,ourImagePixels);									// get the RGB of the pixel
			int sum =0;																// for every pixel, sum will add up all the change of the neighborhood
			for (int cellY= y-cell; cellY<=y+cell; cellY++){										// go into a repeat loop of cell x cell around the pixel
				for (int cellX= x-cell; cellX<=x+cell; cellX++){
					ourGetPixel(cellX,cellY,&R2,&G2,&B2,width,ourImagePixels);				// our method that gets RGBs values at x,y position, note that it expects pointers for R,G,B
					int difference= sqrt((R-R2)*(R-R2)+(G-G2)*(G-G2)+(B-B2)*(B-B2));
					sum+= difference;													// accumulate all the change of the neighborhood into sum
				}
			}
			if (sum< threshold){															// if there is less change than we difine as threshold...
				ourSetPixel(x,y,255,255,255,width,edgePixels);							// set the pixel white...
			}else{
				ourSetPixel(x,y,0,0,0,width,edgePixels);									//otherwise set it black
			}
		}
	}
}
////--------------------------------------------------------------
//// INPUT STUFF, DEFAULT WITH THIS
////--------------------------------------------------------------
void testApp::keyPressed  (int key){
    if(key == '1') {
        picture = "anne";
    }
    else if(key == '2') {
        picture = "ryan";
    }
    else {
        return;
    }
    inputImage.loadImage(picture+".jpg");
    pointFile = (picture + ".txt");
    height = inputImage.height;
	width = inputImage.width;
	ourImagePixels = inputImage.getPixels();
    ourResultTexture = *  new ofTexture();								// create a ofTexture to hold the result info
	ourResultTexture.allocate(width, height, GL_RGB);					// allocate memory for the ofTextue (This is RGB no Alpha)
	edgePixels = new unsigned char[width*height*3];
	ourResultPixels = new unsigned char[width*height*4];
	ofSetWindowShape(width, height);
    /*
    vectorField.setFromImage(inputImage);
    vectorField.normalize();
	vectorField.scale(5);
	vectorField.bias(0, 0.5);
	vectorField.blur();*/
}
//--------------------------------------------------------------
void testApp::keyReleased  (int key){
}
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}
//--------------------------------------------------------------
void testApp::mouseReleased(){
}

//******************************* Pixel by Pixel course method, I think most of your code will go in here   **************************************
void testApp::ourProcessImage(){
	unsigned char R,G,B;
	double Matrix[121] ={-1,-1,-1,-1,9,-1,-1,-1,-1};// sharpen
	int MatrixSize = 2;																	// size of our matrix

	copyPixelArrays(ourImagePixels,ourResultPixels ,width,height, 3);

	for (int i = 0; i< MatrixSize*MatrixSize;i++){
		Matrix[i] = 1/(double)( MatrixSize*MatrixSize);											// filling our matrix with values in this case all equal to create a blur
	}

	for(int x=MatrixSize;x<width-MatrixSize;x++){
		for(int y=MatrixSize;y<height-MatrixSize;y++){
            Convolve(x,y,&R,&G,&B,MatrixSize,Matrix,width,ourImagePixels);					// calling the Convolve() method using our matrix
            ourSetPixel(x,y,R,G,B,width,ourResultPixels);									// setting  the new RGB values
		}
	}
}
//**********************  Pixel by Pixel course method that... convolves ***********************

void  testApp::Convolve(int horizontal,int vertical,unsigned char* R,unsigned char* G,unsigned char* B,int MatrixSize,double* Matrix,int width,unsigned char pixels[]){
	long x,y,halfMatrix ,countMatrix =0,sumMatrix=0;
	double sumR=0,sumG=0,sumB=0;
	unsigned char r,g,b;
	double temp;
	halfMatrix = MatrixSize/2;
	for (x=-halfMatrix;x<MatrixSize-halfMatrix;x++){										// visit every pixel in the neighborhood of the center pixel
		for (y=-halfMatrix;y<MatrixSize-halfMatrix;y++){
			ourGetPixel(horizontal+x,vertical+y,&r,&g,&b,width,pixels);
			temp =r;
			sumR += (temp * Matrix[countMatrix]);										// multiplying the originalvalue of each pixel by the value of the matrix

			temp =g;
			sumG += (temp * Matrix[countMatrix]);


			temp =b;
			sumB += (temp * Matrix[countMatrix]);

			countMatrix++;
		}
	}
	CLIP(sumR,0,255);															//calles the CLIP macro that makes sure that the value is between  0 and 255
	CLIP(sumG,0,255);
	CLIP(sumB,0,255);

	*R = sumR;																	// returning the new R,G,B of the center pixel
	*G = sumG;
	*B = sumB;

}
//**********************  Pixel by Pixel course method that sets a pixel RGB in an x, y position in a pixel array ***********************

void testApp::ourSetPixel(int horizontal,int vertical,unsigned char R,unsigned char G,unsigned char B,int w, unsigned char pixels[]){
	int thisPixel;
	thisPixel = 3*(w * vertical +horizontal);
	pixels[thisPixel]=R;
	pixels[thisPixel+1]=G;
	pixels[thisPixel+2]=B;
}

//**********************  Pixel by Pixel course method that gets a pixel RGB in an x, y position in a pixel array ************************

void testApp::ourGetPixel(int horizontal,int vertical,unsigned char* R,unsigned char* G,unsigned char* B,int w, unsigned char pixels[]){
	int thisPixel;
	thisPixel = 3*(w * vertical +horizontal);
	*R= pixels[thisPixel];
	*G= pixels[thisPixel+1];
	*B= pixels[thisPixel+2];
}

//**********************  Pixel by Pixel course method that copies the content of one pixel array to another ******************************
//*****************************  Note that both pixel arays must have the same height and width  ******************************************

void testApp::copyPixelArrays(unsigned char sourceArray[],unsigned char destArray[] ,int Width,int Height, int bpp){
	memcpy(destArray,sourceArray,Width*Height*bpp) ;
}


//**********************  Pixel by Pixel course method that copies the content of one pixel array to another ******************************
//********************  Note that both pixel arays can have different sizes and this will shrink or stretch  ******************************

void testApp::copyPixelArrays(unsigned char* sourceArray,unsigned char* destArray ,int sourceWidth,int sourceHeight,int destWidth,int destHeight ){
	int bpp = 3;
	int xFactor = (100*sourceWidth)/destWidth;
	int yFactor =( 100*sourceHeight)/destHeight;

	for (int y = 0 ;y< destHeight;y++){
		for (int x = 0 ;x< destWidth;x++){
			int destPixel = bpp*(y*destWidth+x);
			//int sourcePixel = bpp*(y*sourceWidth*yFactor+x*xFactor);
			int sourceX= (x*xFactor)/100;
			int sourceY = (y*yFactor)/100;
			int sourcePixel = bpp*(sourceY*sourceWidth+sourceX);
			destArray[destPixel]= sourceArray[sourcePixel];
			destArray[destPixel+1]= sourceArray[sourcePixel+1];
			destArray[destPixel+2]= sourceArray[sourcePixel+2];
		}
	}
}

//*******************  Pixel by Pixel course method that copies a region of pixels from one array to a region in anther  ***********************
//***************   Note that both pixel arrays must have the same height and widths and this will shrink or stretch the region ****************
//***************								This is uefull for cropping images												****************

void testApp::copyPixelArrays(unsigned char* sourceArray,unsigned char* destArray ,int Width,int Height,
							  int sourceRectL,int sourceRectT,int sourceRectR,int sourceRectB,
							  int destRectL,int destRectT,int destRectR,int destRectB){
	int bpp = 3;
	if (sourceRectR-sourceRectL==destRectR-destRectL && sourceRectT-sourceRectB==destRectT-destRectB){
		int deltaX= destRectL-sourceRectL;
		int deltaY= destRectT-sourceRectT;
		for (int y = destRectT ;y< destRectB;y++){
			for (int x = destRectL ;x< destRectR;x++){
				int destPixel = bpp*(y*Width+x);
				int sourcePixel = bpp*((y+deltaY)*Width+deltaX+x);
				destArray[destPixel]= sourceArray[sourcePixel];
				destArray[destPixel+1]= sourceArray[sourcePixel+1];
				destArray[destPixel+2]= sourceArray[sourcePixel+2];
			}
		}
	}else{
		int deltaX= destRectL-sourceRectL;
		int deltaY= destRectT-sourceRectT;
		int sourceWidth = sourceRectL-sourceRectR;
		int sourceHeight = sourceRectT-sourceRectB;
		int destWidth = destRectL-destRectR;
		int destHeight = destRectT-destRectB;
		int xFactor = (100*sourceWidth)/destWidth;
		int yFactor =( 100*sourceHeight)/destHeight;
		for (int y = destRectT ;y< destRectB;y++){
			int sourceY = deltaY+(y*yFactor)/100;
			for (int x = destRectL ;x< destRectR;x++){
				int destPixel = bpp*(y*Width+x);
				int sourceX= deltaX+(x*xFactor)/100;
				int sourcePixel = bpp*(sourceY*Width+sourceX);
				destArray[destPixel]= sourceArray[sourcePixel];
				destArray[destPixel+1]= sourceArray[sourcePixel+1];
				destArray[destPixel+2]= sourceArray[sourcePixel+2];
			}
		}
	}
}

//**********************  Pixel by Pixel course method that set all the pixel in pixel array to a certain R,G,B value  *****************************

void testApp::clearPixelArrays(unsigned char* pixelArray,int Width,int Height,unsigned char R ,unsigned char G,unsigned char B ){
	int bpp = 3;
	for (int y = 0 ;y< Height;y++){
		for (int x = 0 ;x< Width;x++){
			int Pixel = bpp*(y*Width+x);
			pixelArray[Pixel]= R;
			pixelArray[Pixel+1]= G;
			pixelArray[Pixel+2]= B;
		}
	}
}
