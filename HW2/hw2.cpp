/*
  CSCI 420 Computer Graphics, USC
  Assignment 2: Roller Coaster
  C++ starter code

  Student username: <type your USC username here>
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

//#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "openGLHeader.h"
#include "imageIO.h"

#include <vector>

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

using namespace std;

// represents one control point along the spline 
struct Point 
{
  double x;
  double y;
  double z;
};

// spline struct 
// contains how many control points the spline has, and an array of control points 
struct Spline 
{
  int numControlPoints;
  Point * points;
};

// the spline array 
Spline * splines;
// total number of splines 
int numSplines;

//--------------------------------------------------------

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework II";

//ImageIO * heightmapImage;
//const int SIZE_OF_DOUBLE = sizeof(double);
const int SIZE_OF_FLOAT = sizeof(float);
const int GROUND_WIDTH = 100;
const int CAMERA_MOVE_STEP = 20; // move one point per frame

const int TRACK_INTERVAL = 4;
const float TRACK_RADIUM = 0.5;
const int TRACK_POINTS_PER_UNIT = 100;
const int HEIGHT_ABOVE_TRACK = 3;
//const string SKY_BOX_NAME = "afterrain";
const int INIT_UP = 1;

bool stop_flag = false;

//vertices of the triangle
//float position[3][3] = {{0.0, 0.0, -1.0}, {1.0, 0.0, -1.0}, {0.0, 1.0, -1.0}};
//color to be assigned to vertices
//float color[3][4] = {{1.0, 0.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}};

//array to hold pixels of image
vector<float> pixels;
vector<float> position1; // for pointVBO
vector<float> vector_t;
vector<float> vector_n;
vector<float> vector_b;

//camera position
int pointLoc;

vector<float> groundPosition; 
vector<float> skyPosition_up;
vector<float> skyPosition_ft; 
vector<float> skyPosition_rt; 
vector<float> skyPosition_bk; 
vector<float> skyPosition_lf;  
vector<float> leftTrackPosition; 
vector<float> rightTrackPosition; 

vector<float> color1;
//vector<float> color2;
//vector<float> color2_strip;
//vector<float> color3;

vector<float> groundTexture; 
vector<float> skyTexture_rt;
vector<float> skyTexture_bk;
vector<float> skyTexture_lf;
vector<float> skyTexture_ft;
vector<float> skyTexture_up; 
vector<float> trackTexture; 


//scale to show pixel height
float scale = 0.2;
//int temp_max;
//width of image
int width;
//height of image
int height;
//mode lable
int mode = 1; 

GLuint groundTexHandle;
GLuint skyTexHandle_lf;
GLuint skyTexHandle_rt;
GLuint skyTexHandle_up;
GLuint skyTexHandle_ft;
GLuint skyTexHandle_bk;
GLuint trackTexHandle;

GLuint pointVBO;
GLuint groundVBO;
GLuint skyupVBO;
GLuint skylfVBO;
GLuint skyrtVBO;
GLuint skyftVBO;
GLuint skybkVBO;
GLuint leftTrackVBO;
GLuint rightTrackVBO;

GLuint pointVAO;
GLuint groundVAO;
GLuint skyupVAO;
GLuint skylfVAO;
GLuint skyrtVAO;
GLuint skyftVAO;
GLuint skybkVAO;
GLuint leftTrackVAO;
GLuint rightTrackVAO;

//GLfloat theta[3] = {0.0, 0.0, 0.0};
GLfloat delta = 0.1;
GLint axis = 1;
bool enableSpin = false;

OpenGLMatrix * openGLMatrix;	
BasicPipelineProgram * pipelineProgram;

GLuint program; 
//PipelineProgram * pipelineProgram;

//-------------------------------hw2 code-----------------------------------------
int loadSplines(char * argv) 
{
  char * cName = (char *) malloc(128 * sizeof(char));    // 128? 
  FILE * fileList;
  FILE * fileSpline;
  int iType, i = 0, j, iLength;

  // load the track file 
  fileList = fopen(argv, "r");
  if (fileList == NULL) 
  {
    printf ("can't open file\n");
    exit(1);
  }
  
  // stores the number of splines in a global variable 
  fscanf(fileList, "%d", &numSplines);

  splines = (Spline*) malloc(numSplines * sizeof(Spline));

  // reads through the spline files 
  for (j = 0; j < numSplines; j++) 
  {
    i = 0;
    fscanf(fileList, "%s", cName);
    fileSpline = fopen(cName, "r");

    if (fileSpline == NULL) 
    {
      printf ("can't open file\n");
      exit(1);
    }

    // gets length for spline file
    fscanf(fileSpline, "%d %d", &iLength, &iType);

    // allocate memory for all the points
    splines[j].points = (Point *)malloc(iLength * sizeof(Point));
    splines[j].numControlPoints = iLength;
    

    // saves the data to the struct
    while (fscanf(fileSpline, "%lf %lf %lf", 
     &splines[j].points[i].x, 
     &splines[j].points[i].y, 
     &splines[j].points[i].z) != EOF) 
    {
      i++;
    }
  }

  free(cName);

  return 0;
}

int initTexture(const char * imageFilename, GLuint textureHandle)
{
  // read the texture image
  ImageIO img;
  ImageIO::fileFormatType imgFormat;
  ImageIO::errorType err = img.load(imageFilename, &imgFormat);

  if (err != ImageIO::OK) 
  {
    printf("Loading texture from %s failed.\n", imageFilename);
    return -1;
  }

  // check that the number of bytes is a multiple of 4
  if (img.getWidth() * img.getBytesPerPixel() % 4) 
  {
    printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
    return -1;
  }

  // allocate space for an array of pixels
  int width = img.getWidth();
  int height = img.getHeight();
  unsigned char * pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA

  // fill the pixelsRGBA array with the image pixels
  memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++) 
    {
      // assign some default byte values (for the case where img.getBytesPerPixel() < 4)
      pixelsRGBA[4 * (h * width + w) + 0] = 0; // red
      pixelsRGBA[4 * (h * width + w) + 1] = 0; // green
      pixelsRGBA[4 * (h * width + w) + 2] = 0; // blue
      pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque

      // set the RGBA channels, based on the loaded image
      int numChannels = img.getBytesPerPixel();
      for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
        pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);
    }

  // bind the texture
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  // initialize the texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);

  // generate the mipmaps for this texture
  glGenerateMipmap(GL_TEXTURE_2D);

  // set the texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // query support for anisotropic texture filtering
  GLfloat fLargest;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
  printf("Max available anisotropic samples: %f\n", fLargest);
  // set anisotropic texture filtering
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);

  // query for any errors
  GLenum errCode = glGetError();
  if (errCode != 0) 
  {
    printf("Texture initialization error. Error code: %d.\n", errCode);
    return -1;
  }
  
  // de-allocate the pixel array -- it is no longer needed
  delete [] pixelsRGBA;

  return 0;
}
//----------------------------------hw1 code---------------------------------------

void initPipelineProgram()
{
  //initialize shader pipeline program

  pipelineProgram = new BasicPipelineProgram();

  //intialize uniform variable handles
  pipelineProgram->Init(shaderBasePath);
  //cout<<"print shaderBasePath" + shaderBasePath << endl;

  pipelineProgram->Bind();

  program = pipelineProgram->GetProgramHandle();
  cout<<glGetError()<<endl;

}

void initVBO()
{

  //------------------------groundVBO------------------------------
  glGenBuffers(1, &groundVBO);
  glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
  glBufferData(GL_ARRAY_BUFFER, groundPosition.size()*SIZE_OF_FLOAT + groundTexture.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, groundPosition.size()*SIZE_OF_FLOAT, groundPosition.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, groundPosition.size()*SIZE_OF_FLOAT, groundTexture.size()*SIZE_OF_FLOAT, groundTexture.data());
  
  //------------------------skyupVBO------------------------------
  glGenBuffers(1, &skyupVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyupVBO);
  glBufferData(GL_ARRAY_BUFFER, skyPosition_up.size()*SIZE_OF_FLOAT + skyTexture_up.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, skyPosition_up.size()*SIZE_OF_FLOAT, skyPosition_up.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, skyPosition_up.size()*SIZE_OF_FLOAT, skyTexture_up.size()*SIZE_OF_FLOAT, skyTexture_up.data());

  //------------------------skylfVBO------------------------------
  glGenBuffers(1, &skylfVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skylfVBO);
  glBufferData(GL_ARRAY_BUFFER, skyPosition_lf.size()*SIZE_OF_FLOAT + skyTexture_lf.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, skyPosition_lf.size()*SIZE_OF_FLOAT, skyPosition_lf.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, skyPosition_lf.size()*SIZE_OF_FLOAT, skyTexture_lf.size()*SIZE_OF_FLOAT, skyTexture_lf.data());

  //------------------------skyrtVBO------------------------------
  glGenBuffers(1, &skyrtVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyrtVBO);
  glBufferData(GL_ARRAY_BUFFER, skyPosition_rt.size()*SIZE_OF_FLOAT + skyTexture_rt.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, skyPosition_rt.size()*SIZE_OF_FLOAT, skyPosition_rt.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, skyPosition_rt.size()*SIZE_OF_FLOAT, skyTexture_rt.size()*SIZE_OF_FLOAT, skyTexture_rt.data());

  //------------------------skybkVBO------------------------------
  glGenBuffers(1, &skybkVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skybkVBO);
  glBufferData(GL_ARRAY_BUFFER, skyPosition_bk.size()*SIZE_OF_FLOAT + skyTexture_bk.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, skyPosition_bk.size()*SIZE_OF_FLOAT, skyPosition_bk.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, skyPosition_bk.size()*SIZE_OF_FLOAT, skyTexture_bk.size()*SIZE_OF_FLOAT, skyTexture_bk.data());

  //------------------------skyftVBO------------------------------
  glGenBuffers(1, &skyftVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyftVBO);
  glBufferData(GL_ARRAY_BUFFER, skyPosition_ft.size()*SIZE_OF_FLOAT + skyTexture_ft.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, skyPosition_ft.size()*SIZE_OF_FLOAT, skyPosition_ft.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, skyPosition_ft.size()*SIZE_OF_FLOAT, skyTexture_ft.size()*SIZE_OF_FLOAT, skyTexture_ft.data());

  //------------------------splineVBO------------------------------
  glGenBuffers(1, &pointVBO);
  glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
  glBufferData(GL_ARRAY_BUFFER, position1.size()*SIZE_OF_FLOAT + color1.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, position1.size()*SIZE_OF_FLOAT, position1.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, position1.size()*SIZE_OF_FLOAT, color1.size()*SIZE_OF_FLOAT, color1.data());

  //------------------------leftTrackVBO------------------------------
  glGenBuffers(1, &leftTrackVBO);
  glBindBuffer(GL_ARRAY_BUFFER, leftTrackVBO);
  glBufferData(GL_ARRAY_BUFFER, leftTrackPosition.size()*SIZE_OF_FLOAT + trackTexture.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, leftTrackPosition.size()*SIZE_OF_FLOAT, leftTrackPosition.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, leftTrackPosition.size()*SIZE_OF_FLOAT, trackTexture.size()*SIZE_OF_FLOAT, trackTexture.data());
  
  //------------------------rightTrackVBO------------------------------
  glGenBuffers(1, &rightTrackVBO);
  glBindBuffer(GL_ARRAY_BUFFER, rightTrackVBO);
  glBufferData(GL_ARRAY_BUFFER, rightTrackPosition.size()*SIZE_OF_FLOAT + trackTexture.size()*SIZE_OF_FLOAT, NULL, GL_STATIC_DRAW);
  //upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, rightTrackPosition.size()*SIZE_OF_FLOAT, rightTrackPosition.data());
  //upload color data
  glBufferSubData(GL_ARRAY_BUFFER, rightTrackPosition.size()*SIZE_OF_FLOAT, trackTexture.size()*SIZE_OF_FLOAT, trackTexture.data());
  
  cout<<glGetError()<<endl;
  
}

void initVAO()
{
 
  //------------------------ground VAO------------------------------
  glGenVertexArrays(1, &groundVAO);
  glBindVertexArray(groundVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, groundVBO);

  GLuint loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  const void* offset = (const void*) 0;
  GLboolean normalized = GL_FALSE;
  GLsizei stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  GLuint loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (groundPosition.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------skyupVAO------------------------------
  glGenVertexArrays(1, &skyupVAO);
  glBindVertexArray(skyupVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, skyupVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (skyPosition_up.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------skylfVAO------------------------------
  glGenVertexArrays(1, &skylfVAO);
  glBindVertexArray(skylfVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, skylfVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (skyPosition_lf.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------skyrtVAO------------------------------
  glGenVertexArrays(1, &skyrtVAO);
  glBindVertexArray(skyrtVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, skyrtVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (skyPosition_rt.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------skyftVAO------------------------------
  glGenVertexArrays(1, &skyftVAO);
  glBindVertexArray(skyftVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, skyftVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (skyPosition_ft.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------skybkVAO------------------------------
  glGenVertexArrays(1, &skybkVAO);
  glBindVertexArray(skybkVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, skybkVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (skyPosition_bk.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------splineVAO------------------------------
  glGenVertexArrays(1, &pointVAO);
  glBindVertexArray(pointVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (position1.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------left track VAO------------------------------
  glGenVertexArrays(1, &leftTrackVAO);
  glBindVertexArray(leftTrackVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, leftTrackVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (leftTrackPosition.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  //------------------------right track VAO------------------------------
  glGenVertexArrays(1, &rightTrackVAO);
  glBindVertexArray(rightTrackVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, rightTrackVBO);

  loc1 = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc1);
  offset = (const void*) 0;
  normalized = GL_FALSE;
  stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc1, 3, GL_FLOAT, normalized, stride, offset);

  // get location index of the “texCoord” shader variable
  loc2 = glGetAttribLocation(program, "texCoord"); 
  glEnableVertexAttribArray(loc2); 
  // enable the “texCoord” attribute
  // set the layout of the “texCoord” attribute data
  offset = (const void*) (rightTrackPosition.size()*SIZE_OF_FLOAT);
  stride = 0; 
  glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, stride, offset);
  glBindVertexArray(0); //unbind VAO

  cout<<glGetError()<<endl;
}


// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved succeedfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}
/*
void renderPoints()
{

  //cout << "begin rendering points..." << endl;
  //pipelineProgram->Bind();

  glBindVertexArray(groundVAO);

	GLint first = 0;
	GLsizei count = groundPosition.size()/3;
	glDrawArrays(GL_TRIANGLE_STRIP, first, count);

  glBindVertexArray(skyVAO);

  first = 0;
  count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, first, count);

  //cout << "render succeedfully" << endl;

  glBindVertexArray(0); //unbind the VAO
  //cout<<glGetError()<<endl;
  
}
*/
void setTextureUnit(GLint unit) {

  glActiveTexture(unit); // select the active texture unit
  // get a handle to the “textureImage” shader variable
  GLint h_textureImage = glGetUniformLocation(program, "textureImage"); 
  // deem the shader variable “textureImage” to read from texture unit “unit” 
  glUniform1i(h_textureImage, unit - GL_TEXTURE0);

}


void displayFunc()
{
  // render some stuff...

  //GLuint program = pipelineProgram->GetProgramHandle();
  //glUseProgram(program);

  //cout << "begin display..." << endl;

  //GLfloat zStudent = -10;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  pipelineProgram->Bind();

  /*
  openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
  openGLMatrix->LoadIdentity();
  openGLMatrix->LookAt(-GROUND_WIDTH/2, GROUND_WIDTH*2, -GROUND_WIDTH/2, GROUND_WIDTH/2, 0, GROUND_WIDTH/2, 0, 1, 0);
  */
  ///*
  int pointNum = vector_t.size()/3;
  if(pointLoc < pointNum - 1){  
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    openGLMatrix->LoadIdentity();
    openGLMatrix->LookAt(position1[pointLoc*6 + 0],
                         position1[pointLoc*6 + 1], 
                         position1[pointLoc*6 + 2],
                         position1[pointLoc*6 + 0] + vector_t[pointLoc*3 + 0], 
                         position1[pointLoc*6 + 1] + vector_t[pointLoc*3 + 1], 
                         position1[pointLoc*6 + 2] + vector_t[pointLoc*3 + 2],
                         vector_n[pointLoc*3 + 0], 
                         vector_n[pointLoc*3 + 1], 
                         vector_n[pointLoc*3 + 2]);

    //pointLoc = pointLoc + 60;
    int move_step = 2 + ceil(sqrt(abs(position1[1] - position1[(pointLoc + 1)*6 + 1])) * 8);
    pointLoc = pointLoc + move_step;
    //cout << "position: " << position1[(pointLoc + 1)*6 + 1] << endl;
    //cout << "position difference: " << position1[1] - position1[(pointLoc + 1)*6 + 1] << endl;
    //cout << "move_step: " << move_step << endl;
  }
  else{
    pointLoc = 0;}
  //*/
  openGLMatrix->Translate(landTranslate[0], landTranslate[1], landTranslate[2]);

  openGLMatrix->Scale(landScale[0], landScale[1], landScale[2]);

  openGLMatrix->Rotate(landRotate[0], 1.0, 0.0, 0.0);
  openGLMatrix->Rotate(landRotate[1], 0.0, 1.0, 0.0);
  openGLMatrix->Rotate(landRotate[2], 0.0, 0.0, 1.0);

  
  float m[16];
  openGLMatrix->GetMatrix(m);

  openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
  float p[16];
  openGLMatrix->GetMatrix(p);

  //initPipelineProgram(m, p);
  pipelineProgram->SetModelViewMatrix(m);  //pass m to GPU
  pipelineProgram->SetProjectionMatrix(p); //pass p to GPU

  // select the active texture unit
  //setTextureUnit(GL_TEXTURE0); // it is safe to always use GL_TEXTURE0 
  
  //render points
  
  //----------------------draw ground--------------------------------------
  // select the texture to use (“texHandle” was generated by glGenTextures) 
  glBindTexture(GL_TEXTURE_2D, groundTexHandle);
  glBindVertexArray(groundVAO);
  GLint first = 0;
  GLsizei count = groundPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, first, count);

  //----------------------draw skyft--------------------------------------
  ///*
  glBindTexture(GL_TEXTURE_2D, skyTexHandle_ft);
  glBindVertexArray(skyftVAO);
  first = 0;
  //count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0); //unbind the VAO
  
  //----------------------draw skyrt--------------------------------------

  glBindTexture(GL_TEXTURE_2D, skyTexHandle_rt);
  glBindVertexArray(skyrtVAO);
  first = 0;
  //count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0); //unbind the VAO

  
  //----------------------draw skybk--------------------------------------
  
  glBindTexture(GL_TEXTURE_2D, skyTexHandle_bk);
  glBindVertexArray(skybkVAO);
  first = 0;
  //count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0); //unbind the VAO
  //
  //----------------------draw skylf--------------------------------------
  
  glBindTexture(GL_TEXTURE_2D, skyTexHandle_lf);
  glBindVertexArray(skylfVAO);
  first = 0;
  //count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0); //unbind the VAO
  ///*
  //----------------------draw skyup--------------------------------------
  ///*
  glBindTexture(GL_TEXTURE_2D, skyTexHandle_up);
  glBindVertexArray(skyupVAO);
  first = 0;
  //count = skyPosition.size()/3;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0); //unbind the VAO
  //*/
  //----------------------draw spline--------------------------------------
  /*
  glBindTexture(GL_TEXTURE_2D, skyTexHandle_up);
  glBindVertexArray(pointVAO);
  first = 0;
  count = position1.size()/3;
  glDrawArrays(GL_LINES, first, count);
  glBindVertexArray(0); //unbind the VAO
  //*/
  //----------------------draw left track--------------------------------------
  
  glBindTexture(GL_TEXTURE_2D, trackTexHandle);
  glBindVertexArray(leftTrackVAO);
  first = 0;
  count = leftTrackPosition.size()/3;

  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

  for(int i = 0; i < count - 10; i += 10){
  //for(int i = 10; i < 21; i += 10){
    glDrawArrays(GL_TRIANGLE_STRIP, i, 10);
  }
  glBindVertexArray(0); //unbind the VAO

  //----------------------draw right track--------------------------------------
  
  glBindTexture(GL_TEXTURE_2D, trackTexHandle);
  glBindVertexArray(rightTrackVAO);
  first = 0;
  count = rightTrackPosition.size()/3;

  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

  for(int i = 0; i < count - 10; i += 10){
  //for(int i = 10; i < 21; i += 10){
    glDrawArrays(GL_TRIANGLE_STRIP, i, 10);
  }
  glBindVertexArray(0); //unbind the VAO


  glutSwapBuffers();

  //cout<<glGetError()<<endl;
}
void spin(){
  if(enableSpin){
    landRotate[axis] += delta;
    if(landRotate[axis] > 360.0){
      landRotate[axis] -= 360.0;
    }
  }

}

void idleFunc()
{
  // do some stuff... 
  //Rotate object

  spin();

  // for example, here, you can save the screenshots to disk (to make the animation)

  // make the screen update 
  if (stop_flag == false){
      glutPostRedisplay();
  }
}

void reshapeFunc(int w, int h)
{
  // setup perspective matrix...
  GLfloat aspectRatio = w/h;//1.0; //w/h
  GLfloat fieldOfView = 45.0;
  GLfloat near = 0.01;
  GLfloat far = 1000.0;
  glViewport(0, 0, w, h);
  openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
  openGLMatrix->LoadIdentity();
  openGLMatrix->Perspective(fieldOfView, aspectRatio, near, far);
  openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);

}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }
      break;

    // scale the landscape
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
    break;

    case 'p':
      // take a screenshot
      stop_flag = !stop_flag;
    break;

  }
}

vector<float> matrixMultiply(vector<float> matrixA, int rowA, vector<float> matrixB, int rowB)
{
  //int rowA = sizeof(matrixA)/sizeof(matrixA[0]);
  //int rowB = sizeof(matrixB)/sizeof(matrixB[0]);
  int colA = matrixA.size()/rowA;
  int colB = matrixB.size()/rowB;

  //cout << "matrixA: [" << rowA << ", "<< colA <<"]" << endl;
  //cout << "matrixB: [" << rowB << ", "<< colB <<"]" << endl;

  vector<float> matrixC;
  //matrixC = new double*[rowA];
  /*
  for (int i = 0; i < rowA; i++){
      matrixC[i] = new double[colB];
  }*/

  //cout << "sizeof(matrixC): " << matrixC.size() << endl;
  //cout << "matrixC: [" << sizeof(matrixC)/sizeof(matrixC[0]) << ", "<< sizeof(matrixC[0])/sizeof(matrixC[0][0]) <<"]" << endl;
  
  //memset(matrixC, 0, rowA * colB * sizeof(double));
  float temp = 0.0;
  if(colA == rowB){    
    for(int row = 0; row < rowA; row++){
      for(int col = 0; col < colB; col++){
        for(int inner = 0; inner < rowB; inner++){
          temp += matrixA[row * colA + inner] * matrixB[inner * colB + col];
        }
        matrixC.push_back(temp);
        temp = 0.0;
      }
    }
  }
  else{
    cout << "Error: matrix dismatch!" << endl;
  }
  return matrixC;
}

vector<float> calculateSpline(float u, Point p1, Point p2, Point p3, Point p4){

  float s = 0.5;
  float basisMatrixArray[] = {-s, 2-s, s-2, s, 2*s, s-3, 3-2*s, -s, -s, 0, s, 0, 0, 1, 0, 0};
  vector<float> basisMatrix(basisMatrixArray, basisMatrixArray + sizeof(basisMatrixArray) / sizeof(basisMatrixArray[0]) );
  float controlMatrixArray[] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z};
  vector<float> controlMatrix(controlMatrixArray, controlMatrixArray + sizeof(controlMatrixArray)/sizeof(controlMatrixArray[0]));
  //cout << "BasisMatrix: " << endl;
  /*
  for(int x=0; x < sizeof(basisMatrix)/sizeof(int); x++)  // loop 3 times for three lines
  {
      for(int y=0;y<sizeof(basisMatrix[0])/sizeof(int);y++)  // loop for the three elements on the line
      {
          cout<<basisMatrix[x][y]<<", ";  // display the current element out of the array
      }
  cout<<endl;  // when the inner loop is done, go to a new line
  }
  */
  //cout << "sizeof(basisMatrix): " << sizeof(basisMatrix) << " sizeof(basisMatrix[0]:" << sizeof(basisMatrix[0]) << endl;
  //cout << "basisMatrix: [" << sizeof(basisMatrix)/sizeof(basisMatrix[0]) << ", "<< sizeof(basisMatrix[0])/sizeof(basisMatrix[0][0]) <<"]" << endl;
  //cout << "controlmatrix: [" << rowA << ", "<< colA <<"]" << endl;
  vector<float> temp = matrixMultiply(basisMatrix, 4, controlMatrix, 4);
  float uMatrixArray[] = {u*u*u, u*u, u, 1.0};
  vector<float> uMatrix(uMatrixArray, uMatrixArray + sizeof(uMatrixArray)/sizeof(uMatrixArray[0]));;
  //double* temp1D = 2Dto1D(temp2D);
  vector<float> splinePoint = matrixMultiply(uMatrix, 1, temp, 4);

  //cout << "splinePoint: [" << sizeof(splinePoint)/sizeof(splinePoint[0]) << ", "<< sizeof(splinePoint[0])/sizeof(splinePoint[0][0]) <<"]" << endl;
  return splinePoint;

}


vector<float> normalizeVector(vector<float> v){

  vector<float> result;
  float magnitude = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  for(int i = 0; i < 3; i++){
    result.push_back(v[i]/magnitude);
  }
  return result;
}

void printV3(vector<float> v){

  for(int i = 0; i < v.size()/3; i++){
    cout << "("<<v[i*3+0]<<", "<<v[i*3+1]<<", "<<v[i*3+2]<<")" << endl;
  }
}

vector<float> calculateSplineTangent(float u, Point p1, Point p2, Point p3, Point p4){

  float s = 0.5;
  float basisMatrixArray[] = {-s, 2-s, s-2, s, 2*s, s-3, 3-2*s, -s, -s, 0, s, 0, 0, 1, 0, 0};
  vector<float> basisMatrix(basisMatrixArray, basisMatrixArray + sizeof(basisMatrixArray) / sizeof(basisMatrixArray[0]) );
  float controlMatrixArray[] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z};
  vector<float> controlMatrix(controlMatrixArray, controlMatrixArray + sizeof(controlMatrixArray)/sizeof(controlMatrixArray[0]));

  vector<float> temp = matrixMultiply(basisMatrix, 4, controlMatrix, 4);
  float uMatrixArray[] = {3*u*u, 2*u, 1, 0};
  vector<float> uMatrix(uMatrixArray, uMatrixArray + sizeof(uMatrixArray)/sizeof(uMatrixArray[0]));;
  //double* temp1D = 2Dto1D(temp2D);
  vector<float> splinePointTangent = matrixMultiply(uMatrix, 1, temp, 4);

  //cout << "splinePoint: [" << sizeof(splinePoint)/sizeof(splinePoint[0]) << ", "<< sizeof(splinePoint[0])/sizeof(splinePoint[0][0]) <<"]" << endl;
  return splinePointTangent;

}

vector<float> crossMultiply(vector<float> a, vector<float> b){
  vector<float> result;
  result.push_back(a[1] * b[2] - b[1] * a[2]);
  result.push_back(a[2] * b[0] - b[2] * a[0]);
  result.push_back(a[0] * b[1] - b[0] * a[1]);
  return result;
}

void initPointTNBs(){

  cout << "begin initPointTNBs ... " << endl;

  float uStep = 0.001;
  vector<float> point_t;
  vector<float> point_n;
  vector<float> point_b;

  for(int i = 0; i < numSplines; i++){
    for(int j = 0; j < splines[i].numControlPoints-3; j++){

      Point p1 = splines[i].points[j];
      Point p2 = splines[i].points[j+1];
      Point p3 = splines[i].points[j+2];
      Point p4 = splines[i].points[j+3];

      for(float u = 0; u <= 1.0; u += uStep){       
        point_t = normalizeVector(calculateSplineTangent(u, p1, p2, p3, p4));
        if(u == 0){
          point_n.push_back(0);
          point_n.push_back(INIT_UP);
          point_n.push_back(0);
        }
        else{
          point_n = normalizeVector(crossMultiply(point_b, point_t));
        }
        
        point_b = normalizeVector(crossMultiply(point_t, point_n));

        vector_t.push_back(point_t[0]);
        vector_t.push_back(point_t[1]);
        vector_t.push_back(point_t[2]);
        //cout << "vector_t: " << point_t[0] << "," << point_t[1] << "," << point_t[2] << endl;

        vector_n.push_back(point_n[0]);
        vector_n.push_back(point_n[1]);
        vector_n.push_back(point_n[2]);

        vector_b.push_back(point_b[0]);
        vector_b.push_back(point_b[1]);
        vector_b.push_back(point_b[2]);

        //cout << "print u: " << u << endl;
      }

    }
  }

  cout << "vector_t created! size = " << vector_t.size() << endl;
  cout << "vector_n created! size = " << vector_n.size() << endl;
  cout << "vector_b created! size = " << vector_b.size() << endl;

  cout << "initPointTNBs succeed! " << endl;


}

vector<float> v3_Add(vector<float> v1, vector<float> v2){

  vector<float> result;
  for(int i = 0; i < v1.size(); i++){
    result.push_back(v1[i] + v2[i]);
  }
  return result;
}

vector<float> v3_Multiply(vector<float> v1, float c){

  vector<float> result;
  for(int i = 0; i < v1.size(); i++){
    result.push_back(v1[i] * c);
  }
  return result;
}

vector<float> computeSquarePoints(vector<float> center, vector<float> b, vector<float> n){

  vector<float> left = v3_Add(v3_Multiply(b, -1 * TRACK_RADIUM), center);
  vector<float> right = v3_Add(v3_Multiply(b, 1 * TRACK_RADIUM), center);
  vector<float> leftTop = v3_Add(v3_Multiply(n, 1 * TRACK_RADIUM), left);
  vector<float> rightTop = v3_Add(v3_Multiply(n, 1 * TRACK_RADIUM), right);
  vector<float> leftBottom = v3_Add(v3_Multiply(n, -1 * TRACK_RADIUM), left);
  vector<float> rightBottom = v3_Add(v3_Multiply(n, -1 * TRACK_RADIUM), right);
  vector<float> squarePoints;
  squarePoints.push_back(rightTop[0]);
  squarePoints.push_back(rightTop[1]);
  squarePoints.push_back(rightTop[2]);
  squarePoints.push_back(leftTop[0]);
  squarePoints.push_back(leftTop[1]);
  squarePoints.push_back(leftTop[2]);  
  squarePoints.push_back(leftBottom[0]);
  squarePoints.push_back(leftBottom[1]);
  squarePoints.push_back(leftBottom[2]);
  squarePoints.push_back(rightBottom[0]);
  squarePoints.push_back(rightBottom[1]);
  squarePoints.push_back(rightBottom[2]);

  return squarePoints;
}

void initPoints()
{
  //------------------------position1------------------------------
  
  //double s = 0.5;
  float uStep = 0.001;
  vector<float> splinePoint;

  cout << "calculating splines... " << endl;
  cout << "numSplines = " << numSplines << endl;
  cout << "numControlPoints = " << splines[0].numControlPoints << endl;

  for(int i = 0; i < numSplines; i++){
    for(int j = 0; j < splines[i].numControlPoints-3; j++){
      Point p1 = splines[i].points[j];
      Point p2 = splines[i].points[j+1];
      Point p3 = splines[i].points[j+2];
      Point p4 = splines[i].points[j+3];

      float u = 0.0;
      splinePoint = calculateSpline(u, p1, p2, p3, p4);
      position1.push_back(splinePoint[0]);
      position1.push_back(splinePoint[1]);
      position1.push_back(splinePoint[2]);
      //cout << "print u: " << u << endl;

      for(u = uStep; u < 1.0; u += uStep){       
        splinePoint = calculateSpline(u, p1, p2, p3, p4);

        position1.push_back(splinePoint[0]);
        position1.push_back(splinePoint[1]);
        position1.push_back(splinePoint[2]);
        
        position1.push_back(splinePoint[0]);
        position1.push_back(splinePoint[1]);
        position1.push_back(splinePoint[2]);

        //cout << "print u: " << u << endl;
      }

      u = 1.0;
      splinePoint = calculateSpline(u, p1, p2, p3, p4);
      position1.push_back(splinePoint[0]);
      position1.push_back(splinePoint[1]);
      position1.push_back(splinePoint[2]);
      //cout << "print u: " << u << endl;

    }
  }
  cout << "position1 vector created! size = " << position1.size() << endl;

  // calculate T,N,B for each point
  initPointTNBs();

  //------------------------leftTrackPosition------------------------------
  

  for(int i = 0; i < position1.size()/6 - TRACK_POINTS_PER_UNIT; i += TRACK_POINTS_PER_UNIT){
  //for(int i = 0; i < 4 * TRACK_POINTS_PER_UNIT; i += TRACK_POINTS_PER_UNIT){
    //cout << "i = "<<i<<endl;
    vector<float> currentCenterPoint;
    vector<float> currentPosition;
    vector<float> currentB;
    vector<float> currentN;
    vector<float> currentSquare;
    vector<float> nextCenterPoint;
    vector<float> nextPosition;
    vector<float> nextB;
    vector<float> nextN;
    vector<float> nextSquare;

    currentPosition.push_back(position1[i*6 + 0]);
    currentPosition.push_back(position1[i*6 + 1]);
    currentPosition.push_back(position1[i*6 + 2]);

    currentB.push_back(vector_b[i*3 + 0]);
    currentB.push_back(vector_b[i*3 + 1]);
    currentB.push_back(vector_b[i*3 + 2]);

    currentN.push_back(vector_n[i*3 + 0]);
    currentN.push_back(vector_n[i*3 + 1]);
    currentN.push_back(vector_n[i*3 + 2]);

    //cout << "************currentPosition: " << endl;
    //printV3(currentPosition);

    //cout << "currentB: " << endl;
    //printV3(currentB);

    currentCenterPoint = v3_Add(v3_Multiply(currentB, -1 * TRACK_INTERVAL/2), currentPosition);
    currentCenterPoint = v3_Add(v3_Multiply(currentN, -1 * HEIGHT_ABOVE_TRACK), currentCenterPoint);
    //cout << "currentCenterPoint: " << endl;
    //printV3(currentCenterPoint);

    currentSquare = computeSquarePoints(currentCenterPoint, currentB, currentN);
    //cout << "current Square: " << endl;
    //printV3(currentSquare);

    int ii = i + TRACK_POINTS_PER_UNIT;

    nextPosition.push_back(position1[ii*6 + 0]);
    nextPosition.push_back(position1[ii*6 + 1]);
    nextPosition.push_back(position1[ii*6 + 2]);

    nextB.push_back(vector_b[ii*3 + 0]);
    nextB.push_back(vector_b[ii*3 + 1]);
    nextB.push_back(vector_b[ii*3 + 2]);

    nextN.push_back(vector_n[ii*3 + 0]);
    nextN.push_back(vector_n[ii*3 + 1]);
    nextN.push_back(vector_n[ii*3 + 2]);

    nextCenterPoint = v3_Add(v3_Multiply(nextB, -1 * TRACK_INTERVAL/2), nextPosition);
    nextCenterPoint = v3_Add(v3_Multiply(nextN, -1 * HEIGHT_ABOVE_TRACK), nextCenterPoint);
    //cout << "nextCenterPoint: " << endl;
    //printV3(nextCenterPoint);

    nextSquare = computeSquarePoints(nextCenterPoint, nextB, nextN);
    //cout << "next Square: " << endl;
    //printV3(nextSquare);

    for(int j = 0; j < currentSquare.size()/3; j++){
      //cout << "j = " << j << endl;
      leftTrackPosition.push_back(nextSquare[j*3 + 0]);
      leftTrackPosition.push_back(nextSquare[j*3 + 1]);
      leftTrackPosition.push_back(nextSquare[j*3 + 2]);

      leftTrackPosition.push_back(currentSquare[j*3 + 0]);
      leftTrackPosition.push_back(currentSquare[j*3 + 1]);
      leftTrackPosition.push_back(currentSquare[j*3 + 2]);
    }

    //last two points of one track unit
    leftTrackPosition.push_back(nextSquare[0]);
    leftTrackPosition.push_back(nextSquare[1]);
    leftTrackPosition.push_back(nextSquare[2]);

    leftTrackPosition.push_back(currentSquare[0]);
    leftTrackPosition.push_back(currentSquare[1]);
    leftTrackPosition.push_back(currentSquare[2]);

    //cout << "leftTrackPosition: " << endl;
    //printV3(leftTrackPosition);

  }

  cout << "leftTrackPosition vector created! size = " << leftTrackPosition.size() << endl;

  //------------------------rightTrackPosition------------------------------
  

  for(int i = 0; i < position1.size()/6 - TRACK_POINTS_PER_UNIT; i += TRACK_POINTS_PER_UNIT){
  //for(int i = 0; i < 4 * TRACK_POINTS_PER_UNIT; i += TRACK_POINTS_PER_UNIT){
    //cout << "i = "<<i<<endl;
    vector<float> currentCenterPoint;
    vector<float> currentPosition;
    vector<float> currentB;
    vector<float> currentN;
    vector<float> currentSquare;
    vector<float> nextCenterPoint;
    vector<float> nextPosition;
    vector<float> nextB;
    vector<float> nextN;
    vector<float> nextSquare;

    currentPosition.push_back(position1[i*6 + 0]);
    currentPosition.push_back(position1[i*6 + 1]);
    currentPosition.push_back(position1[i*6 + 2]);

    currentB.push_back(vector_b[i*3 + 0]);
    currentB.push_back(vector_b[i*3 + 1]);
    currentB.push_back(vector_b[i*3 + 2]);

    currentN.push_back(vector_n[i*3 + 0]);
    currentN.push_back(vector_n[i*3 + 1]);
    currentN.push_back(vector_n[i*3 + 2]);

    //cout << "************currentPosition: " << endl;
    //printV3(currentPosition);

    //cout << "currentB: " << endl;
    //printV3(currentB);

    currentCenterPoint = v3_Add(v3_Multiply(currentB, 1 * TRACK_INTERVAL/2), currentPosition);
    currentCenterPoint = v3_Add(v3_Multiply(currentN, -1 * HEIGHT_ABOVE_TRACK), currentCenterPoint);
    //cout << "currentCenterPoint: " << endl;
    //printV3(currentCenterPoint);

    currentSquare = computeSquarePoints(currentCenterPoint, currentB, currentN);
    //cout << "current Square: " << endl;
    //printV3(currentSquare);

    int ii = i + TRACK_POINTS_PER_UNIT;

    nextPosition.push_back(position1[ii*6 + 0]);
    nextPosition.push_back(position1[ii*6 + 1]);
    nextPosition.push_back(position1[ii*6 + 2]);

    nextB.push_back(vector_b[ii*3 + 0]);
    nextB.push_back(vector_b[ii*3 + 1]);
    nextB.push_back(vector_b[ii*3 + 2]);

    nextN.push_back(vector_n[ii*3 + 0]);
    nextN.push_back(vector_n[ii*3 + 1]);
    nextN.push_back(vector_n[ii*3 + 2]);

    nextCenterPoint = v3_Add(v3_Multiply(nextB, 1 * TRACK_INTERVAL/2), nextPosition);
    nextCenterPoint = v3_Add(v3_Multiply(nextN, -1 * HEIGHT_ABOVE_TRACK), nextCenterPoint);
    //cout << "nextCenterPoint: " << endl;
    //printV3(nextCenterPoint);

    nextSquare = computeSquarePoints(nextCenterPoint, nextB, nextN);
    //cout << "next Square: " << endl;
    //printV3(nextSquare);

    for(int j = 0; j < currentSquare.size()/3; j++){
      //cout << "j = " << j << endl;
      rightTrackPosition.push_back(nextSquare[j*3 + 0]);
      rightTrackPosition.push_back(nextSquare[j*3 + 1]);
      rightTrackPosition.push_back(nextSquare[j*3 + 2]);

      rightTrackPosition.push_back(currentSquare[j*3 + 0]);
      rightTrackPosition.push_back(currentSquare[j*3 + 1]);
      rightTrackPosition.push_back(currentSquare[j*3 + 2]);
    }

    //last two points of one track unit
    rightTrackPosition.push_back(nextSquare[0]);
    rightTrackPosition.push_back(nextSquare[1]);
    rightTrackPosition.push_back(nextSquare[2]);

    rightTrackPosition.push_back(currentSquare[0]);
    rightTrackPosition.push_back(currentSquare[1]);
    rightTrackPosition.push_back(currentSquare[2]);

    //cout << "rightTrackPosition: " << endl;
    //printV3(rightTrackPosition);

  }

  cout << "rightTrackPosition vector created! size = " << rightTrackPosition.size() << endl;

  //------------------------trackTexture------------------------------
  for(int i = 0; i < leftTrackPosition.size()/30; i++){

    trackTexture.push_back(0);
    trackTexture.push_back(0);

    trackTexture.push_back(1);
    trackTexture.push_back(0);

    trackTexture.push_back(0);
    trackTexture.push_back(1);

    trackTexture.push_back(1);
    trackTexture.push_back(1);

    trackTexture.push_back(0);
    trackTexture.push_back(0);

    trackTexture.push_back(1);
    trackTexture.push_back(0);

    trackTexture.push_back(0);
    trackTexture.push_back(1);

    trackTexture.push_back(1);
    trackTexture.push_back(1);

    trackTexture.push_back(0);
    trackTexture.push_back(0);

    trackTexture.push_back(1);
    trackTexture.push_back(0);

  }

  cout << "trackTexture vector created! size = " << trackTexture.size() << endl;

  //------------------------groundPosition------------------------------

  groundPosition.push_back(0);
  groundPosition.push_back(0);
  groundPosition.push_back(0);

  groundPosition.push_back(0);
  groundPosition.push_back(0);
  groundPosition.push_back(GROUND_WIDTH);

  groundPosition.push_back(GROUND_WIDTH);
  groundPosition.push_back(0);
  groundPosition.push_back(0);

  groundPosition.push_back(GROUND_WIDTH);
  groundPosition.push_back(0);
  groundPosition.push_back(GROUND_WIDTH);


  cout << "groundPosition vector created! size = " << groundPosition.size() << endl;

  //------------------------groundTexture------------------------------
  groundTexture.push_back(1);
  groundTexture.push_back(0);

  groundTexture.push_back(0);
  groundTexture.push_back(0);

  groundTexture.push_back(1);
  groundTexture.push_back(1);

  groundTexture.push_back(0);
  groundTexture.push_back(1);

  cout << "groundTexture vector created! size = " << groundTexture.size() << endl;

  //------------------------skyPosition_ft------------------------------

  skyPosition_ft.push_back(0);
  skyPosition_ft.push_back(0);
  skyPosition_ft.push_back(0);

  skyPosition_ft.push_back(0);
  skyPosition_ft.push_back(GROUND_WIDTH);
  skyPosition_ft.push_back(0);

  skyPosition_ft.push_back(GROUND_WIDTH);
  skyPosition_ft.push_back(0);
  skyPosition_ft.push_back(0);

  skyPosition_ft.push_back(GROUND_WIDTH);
  skyPosition_ft.push_back(GROUND_WIDTH);
  skyPosition_ft.push_back(0);

  //------------------------skyPosition_rt------------------------------

  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(0);
  skyPosition_rt.push_back(0);

  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(0);

  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(0);
  skyPosition_rt.push_back(GROUND_WIDTH);

  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(GROUND_WIDTH);
  skyPosition_rt.push_back(GROUND_WIDTH);

  //------------------------skyPosition_bk------------------------------

  skyPosition_bk.push_back(GROUND_WIDTH);
  skyPosition_bk.push_back(0);
  skyPosition_bk.push_back(GROUND_WIDTH);

  skyPosition_bk.push_back(GROUND_WIDTH);
  skyPosition_bk.push_back(GROUND_WIDTH);
  skyPosition_bk.push_back(GROUND_WIDTH);

  skyPosition_bk.push_back(0);
  skyPosition_bk.push_back(0);
  skyPosition_bk.push_back(GROUND_WIDTH);

  skyPosition_bk.push_back(0);
  skyPosition_bk.push_back(GROUND_WIDTH);
  skyPosition_bk.push_back(GROUND_WIDTH);

  //------------------------skyPosition_lf------------------------------

  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(GROUND_WIDTH);

  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(GROUND_WIDTH);
  skyPosition_lf.push_back(GROUND_WIDTH);

  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(0);

  skyPosition_lf.push_back(0);
  skyPosition_lf.push_back(GROUND_WIDTH);
  skyPosition_lf.push_back(0);


  //------------------------skyPosition_up------------------------------

  skyPosition_up.push_back(0);
  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(0);

  skyPosition_up.push_back(0);
  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(GROUND_WIDTH);

  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(0);

  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(GROUND_WIDTH);
  skyPosition_up.push_back(GROUND_WIDTH);
  


  cout << "skyPosition vector created! size = " << skyPosition_up.size() << endl;

  //------------------------skyTexture_ft------------------------------
  skyTexture_ft.push_back(1);
  skyTexture_ft.push_back(0);

  skyTexture_ft.push_back(1);
  skyTexture_ft.push_back(1);

  skyTexture_ft.push_back(0);
  skyTexture_ft.push_back(0);

  skyTexture_ft.push_back(0);
  skyTexture_ft.push_back(1);

  //------------------------skyTexture_rt------------------------------

  skyTexture_rt.push_back(1);
  skyTexture_rt.push_back(0);

  skyTexture_rt.push_back(1);
  skyTexture_rt.push_back(1);

  skyTexture_rt.push_back(0);
  skyTexture_rt.push_back(0);

  skyTexture_rt.push_back(0);
  skyTexture_rt.push_back(1);

  //------------------------skyTexture_bk------------------------------

  skyTexture_bk.push_back(1);
  skyTexture_bk.push_back(0);

  skyTexture_bk.push_back(1);
  skyTexture_bk.push_back(1);

  skyTexture_bk.push_back(0);
  skyTexture_bk.push_back(0);

  skyTexture_bk.push_back(0);
  skyTexture_bk.push_back(1);

  //------------------------skyTexture_lf------------------------------

  skyTexture_lf.push_back(1);
  skyTexture_lf.push_back(0);

  skyTexture_lf.push_back(1);
  skyTexture_lf.push_back(1);

  skyTexture_lf.push_back(0);
  skyTexture_lf.push_back(0);

  skyTexture_lf.push_back(0);
  skyTexture_lf.push_back(1);

  //------------------------skyTexture_up------------------------------

  skyTexture_up.push_back(1);
  skyTexture_up.push_back(1);

  skyTexture_up.push_back(0);
  skyTexture_up.push_back(1);

  skyTexture_up.push_back(1);
  skyTexture_up.push_back(0);

  skyTexture_up.push_back(0);
  skyTexture_up.push_back(0);

  cout << "skyTexture vector created! size = " << skyTexture_up.size() << endl;
  //return position1;

}

void initScene(int argc, char *argv[])
{

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // do additional initialization here...
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  openGLMatrix = new OpenGLMatrix();


  cout << "begin initPipeProgram ... " << endl;
  initPipelineProgram();
  cout << "initPipeProgram succeed! " << endl;
  

  //initialize texture
  cout << "begin initTexture ... " << endl;

  // create an integer handle for the texture
  glGenTextures(1, &groundTexHandle);

  char groundImageFile[] = "../hw2_texture/afterrain_dn.jpg";
  int code = initTexture(groundImageFile, groundTexHandle); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &skyTexHandle_lf);
  char skyImageFile_lf[] = "../hw2_texture/afterrain_lf.jpg";
  code = initTexture(skyImageFile_lf, skyTexHandle_lf); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &skyTexHandle_rt);
  char skyImageFile_rt[] = "../hw2_texture/afterrain_rt.jpg";
  code = initTexture(skyImageFile_rt, skyTexHandle_rt); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &skyTexHandle_ft);
  char skyImageFile_ft[] = "../hw2_texture/afterrain_ft.jpg";
  code = initTexture(skyImageFile_ft, skyTexHandle_ft); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &skyTexHandle_bk);
  char skyImageFile_bk[] = "../hw2_texture/afterrain_bk.jpg";
  code = initTexture(skyImageFile_bk, skyTexHandle_bk); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &skyTexHandle_up);
  char skyImageFile_up[] = "../hw2_texture/afterrain_up.jpg";
  code = initTexture(skyImageFile_up, skyTexHandle_up); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  glGenTextures(1, &trackTexHandle);
  char trackImageFile[] = "../hw2_texture/medaltrackwithedge.jpg";//medaltrackwithedge.jpg";
  code = initTexture(trackImageFile, trackTexHandle); 
  if (code != 0)
  {
    printf("Error loading the texture image.\n");
    exit(EXIT_FAILURE); 
  }

  cout << "initTexture succeed! " << endl;


  //initialize points
  initPoints();

  //initialize color
  cout << "begin initColor ... " << endl;
  
  //------------------------color1------------------------------
  
  for(int i = 0; i < position1.size()/3; i++){

        color1.push_back(1.0);
        color1.push_back(1.0);
        //color.push_back(double(heightmapImage->getPixel(i, j, 0))/255);
        color1.push_back(0.0);
        color1.push_back(1.0);
  }

  cout << "color1 vector created! size = " << color1.size() << endl;
  cout << "initColor succeed! " << endl;


  cout << "begin initVBO ... " << endl;
  initVBO();
  cout << "initVBO succeed! " << endl;

  cout << "begin initVAO..." << endl;
  initVAO();
  cout << "initVAO succeed!" << endl;

  cout<<glGetError()<<endl;

}


int main(int argc, char **argv)                          // ? argc, **argv
{
  if (argc<2)
  {  
    printf ("usage: %s <trackfile>\n", argv[0]);
    exit(0);
  }

  // load the splines from the provided filename
  loadSplines(argv[1]);

  printf("Loaded %d spline(s).\n", numSplines);
  for(int i=0; i<numSplines; i++)
    printf("Num control points in spline %d: %d.\n", i, splines[i].numControlPoints);


  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    //glEnable(GL_DEPTH_TEST); 
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  //glEnable(GL_DEPTH_TEST); 

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  //cout << "1.CreateWindow ... " << endl;
  glutCreateWindow(windowTitle);
  //cout << "2.CreateWindow ... " << endl;

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  //cout << "test display ... " << endl;
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif


  // do initialization
  //initPipelineProgram();
  cout << "begin initScene ... " << endl;
  initScene(argc, argv);
  cout << "initScene succeed! " << endl;

  // sink forever into the glut loop
  glutMainLoop();

  return 0;
}


