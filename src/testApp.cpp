#include "testApp.h"
using namespace std;

string picture = "chanel";
//--------------------------------------------------------------
void testApp::setup(){

    // fill our palette with colours modigliani used! (http://www.colorhunter.com/tag/modigliani/)
    colourPalette.push_back(ofVec3f(0,0,0)); // black
    colourPalette.push_back(ofVec3f(63,18,37)); // purple
    colourPalette.push_back(ofVec3f(56,73,81)); // blue ish
    colourPalette.push_back(ofVec3f(140,132,47)); // olive
    colourPalette.push_back(ofVec3f(152,46,4)); // red orange lips
    colourPalette.push_back(ofVec3f(70,50,6)); // a yellowy brown
    colourPalette.push_back(ofVec3f(60,23,16)); // redder brown
    colourPalette.push_back(ofVec3f(180,0,0)); // deep red
    colourPalette.push_back(ofVec3f(235,200,193)); // pink
    colourPalette.push_back(ofVec3f(229,211,140)); // beige
    colourPalette.push_back(ofVec3f(217,135,53)); // medium unsaturated orange
    colourPalette.push_back(ofVec3f(241,182,91)); // light orange
    colourPalette.push_back(ofVec3f(255,147,0)); // bright orange
    colourPalette.push_back(ofVec3f(255,247,227)); // off-white

    inputImage.loadImage(picture + ".jpg");
    height = inputImage.height;
	width = inputImage.width;
	ofSetWindowShape(width, height);
	edgeImage = inputImage;
    referenceImage.loadImage("composite.jpg");
    pointFile = (picture + ".txt");
    ourImagePixels = inputImage.getPixels();
    ourReferencePixels = referenceImage.getPixels();

    ourResultTexture = *  new ofTexture();
    ourResultTexture.allocate(width, height, GL_RGB);
	edgePixels = new unsigned char[width*height*3];
	ourResultPixels = new unsigned char[width*height*4];
    morph(); // morphs, outputs to input again!!

    vectorField.setup(width, height, 10);
    vectorField.setFromImage(inputImage);
    vectorField.normalize();
	vectorField.scale(5);
	vectorField.bias(0, 0.5);
	vectorField.blur();

	coarse.loadImage("large.png");
	coarse.resize(200,90);

	medium.loadImage("medium.png");
	medium.resize(100,45);

	fine.loadImage("small.png");
    fine.resize(17,8);

	edge.loadImage("edge.png");
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

    vector< pair <ofPoint, ofPoint> > srcFeatures; // THIS IS MODIGLIANI COMPOSITE
    vector< pair <ofPoint, ofPoint> > destFeatures; // THIS IS INPUT

    ofBuffer featureBuffer = ofBufferFromFile(pointFile);
    double asx, asy, aex, aey, bsx, bsy, bex, bey;
    int featureNum;

    string line;
    stringstream lineStream;
    line = featureBuffer.getFirstLine();
    while (!featureBuffer.isLastLine()) {
        lineStream << line;
        lineStream >> asx >> asy >> aex >> aey >> bsx >> bsy >> bex >> bey;
        ofPoint p1=ofPoint(asx,asy);
        ofPoint p2=ofPoint(aex,aey);
        srcFeatures.push_back(make_pair(p1,p2));
        ofPoint p3=ofPoint(bsx,bsy);
        ofPoint p4=ofPoint(bex,bey);
        destFeatures.push_back(make_pair(p3,p4));
        line = featureBuffer.getNextLine();
    }
    featureNum = destFeatures.size();
    vector< pair <ofPoint, ofPoint> > targetFeatures(featureNum);

    // calculate intermediate feature line
    for (int i=0; i<featureNum; i++) {
        pair <ofPoint, ofPoint> sp = srcFeatures[i];
        pair <ofPoint, ofPoint> dp = destFeatures[i];
        ofPoint t1 = ofPoint( (0.5F)*sp.first.x + 0.5F*dp.first.x, (0.5F)*sp.first.y + 0.5F*dp.first.y );
        ofPoint t2 = ofPoint( (0.5F)*sp.second.x + 0.5F*dp.second.x, (0.5F)*sp.second.y + 0.5F*dp.second.y );
        targetFeatures[i] = make_pair(t1, t2);
    }
    double u,vu,v,rawv,weight,vx,vy,hx,hy,tx,ty;
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            double sumsdx=0, sumsdy=0, sumddx=0, sumddy=0, sumweight=0;
            for (int i=0; i<featureNum; i++) {

                pair<ofPoint,ofPoint> tp = targetFeatures[i];

                vx =  tp.second.y - tp.first.y;
                vy = -tp.second.x + tp.first.x;
                hx =  tp.second.x - tp.first.x;
                hy =  tp.second.y - tp.first.y;
                tx =  x - tp.first.x;
                ty =  y - tp.first.y;

                // calc u
                u = (tx*hx + ty*hy) / lengthSquare(hx,hy);
                vu = length(vx, vy);
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
                weight = lineLength/(0.1+v);
                if (weight < 0) weight = 0;

                pair<ofPoint, ofPoint> sf = srcFeatures[i];
                ofPoint sp = getMappingPoint(sf.first, sf.second, u, rawv);
                double sdx = x - sp.x;
                double sdy = y - sp.y;
                sumsdx += sdx*weight;
                sumsdy += sdy*weight;

                pair<ofPoint, ofPoint> df = destFeatures[i];
                ofPoint dp = getMappingPoint(df.first, df.second, u, rawv);
                double ddx = x - dp.x;
                double ddy = y - dp.y;
                sumddx += ddx*weight;
                sumddy += ddy*weight;

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
            if (sx < 0 || sx > referenceImage.width || sy < 0 || sy > referenceImage.height) {
              continue;
            }
            if (dx < 0 || dx > inputImage.width || dy < 0 || dy > inputImage.height) {
              continue;
            }
            unsigned char dR,dG,dB, sR,sG,sB, tR,tG,tB;

            ourGetPixel(dx,dy,&dR,&dG,&dB,width,ourImagePixels);
            ourGetPixel(sx,sy,&sR,&sG,&sB,width,ourReferencePixels);
            tR = sR+(dR-sR)/2;
            tG = sG+(dG-sG)/2;
            tB = sB+(dB-sB)/2;
            CLIP(tR,0,255);
            CLIP(tG,0,255);
            CLIP(tB,0,255);
            ourSetPixel(x,y,tR,tG,tB,width,ourImagePixels);
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

// Use a limited palette! Minimize cartesian distance in the palette
void testApp::getPaletteColour(double* R, double* G, double* B){
	int numColours;
	numColours = (int) colourPalette.size();
    float dist, tempdist;
    int sR, sG, sB;
    list<ofVec3f>::iterator it;
    ofVec3f temp;
    it=colourPalette.begin();
    temp = *it;
    sR = temp.x;
    sG = temp.y;
    sB = temp.z;
    dist = pow(abs(*R-sR),2) + pow(abs(*G-sG),2) + pow(abs(*B-sB),2);
    it++;
    for (; it != colourPalette.end(); it++) {
        temp = *it;
        tempdist = pow(abs(*R-temp.x),2) + pow(abs(*G-temp.y),2) + pow(abs(*B-temp.z),2);
        if (dist > tempdist) {
            sR = temp.x;
            sG = temp.y;
            sB = temp.z;
        }
    }
    // meet it half way so it looks like you've mixed colours
	*R= (*R*2+sR)/3;
	*G= (*G*2+sG)/3;
	*B= (*B*2+sB)/3;
}

////--------------------------------------------------------------
//// INPUT STUFF, DEFAULT WITH THIS
////--------------------------------------------------------------
void testApp::keyPressed  (int key){
    if(key == '1') {
        picture = "anne";
    }
    else if(key == '2') {
        picture = "carey";
    }
    else if(key == '3') {
        picture = "chanel";
    }
    else {
        return;
    }
	height = inputImage.height;
    width = inputImage.width;
    inputImage.loadImage(picture+".jpg");
    pointFile = (picture + ".txt");
	ourImagePixels = inputImage.getPixels();
    morph(); // morphs, outputs to input again!!
    ourResultTexture = *  new ofTexture();
    ourResultTexture.allocate(width, height, GL_RGB);
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

	getPaletteColour(&sumR, &sumG, &sumB);

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

double testApp::lengthSquare (double x, double y) {
  return pow(x,2)+pow(y,2);
}

double testApp::length (double x, double y) {
  return pow(lengthSquare(x,y),0.5);
}

ofPoint testApp::getMappingPoint (ofPoint p, ofPoint q, double u, double v) {
  double vx =  q.y - p.y;
  double vy = -q.x + p.x;
  double hx =  q.x - p.x;
  double hy =  q.y - p.y;
  double vu = length(vx, vy);

  double sx = p.x + u*hx + (vx/vu)*v;
  double sy = p.y + u*hy + (vy/vu)*v;

  return ofPoint(sx, sy);
}
