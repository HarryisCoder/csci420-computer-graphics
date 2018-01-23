/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: <type your USC username here>
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

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
char windowTitle[512] = "CSCI 420 homework I";

ImageIO * heightmapImage;

//vertices of the triangle
float position[3][3] = {{0.0, 0.0, -1.0}, {1.0, 0.0, -1.0}, {0.0, 1.0, -1.0}};
//color to be assigned to vertices
float color[3][4] = {{1.0, 0.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}};

GLuint triangleVBO;
GLuint triangleVAO;
OpenGLMatrix * openGLMatrix;	
BasicPipelineProgram * pipelineProgram;

GLuint program; 
//PipelineProgram * pipelineProgram;

void initPipelineProgram()
{
  //initialize shader pipeline program

  pipelineProgram = new BasicPipelineProgram();

  //intialize uniform variable handles
  pipelineProgram->Init(shaderBasePath);
  //cout<<"print shaderBasePath" + shaderBasePath << endl;

  pipelineProgram->Bind();

  program = pipelineProgram->GetProgramHandle();

}

void initVBO()
{

	//Load shaders and use the resulting shader program

	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position) + sizeof(color), NULL, GL_STATIC_DRAW);
	//upload position data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	//upload color data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(position), sizeof(color), color);

  //Bind VBO...
  
  //glBindBuffer(GL_ARRAY_BUFFER, triangleVBO)
  //GLuint program = pipelineProgram->GetProgramHandle();
  /*
  GLuint loc = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc);
  const void* offset = (const void*) 0;
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, offset);

  GLuint loc2 = glGetAttribLocation(program, "color");
  glEnableVertexAttribArray(loc2);
  offset = (const void*) sizeof(position);
  glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0, offset);
  */
}

void initVAO()
{
  glGenVertexArrays(1, &triangleVAO);
  glBindVertexArray(triangleVAO); //bind the VAO

  glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

  GLuint loc = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc);
  const void* offset = (const void*) 0;
  GLboolean normalized = GL_FALSE;
  GLsizei stride = 0;
  //set the layout of the "position" attribute data
  glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);

  GLuint loc2 = glGetAttribLocation(program, "color");
  glEnableVertexAttribArray(loc2);
  offset = (const void*) sizeof(position);
  //set the layout of the "color" attribute data
  stride = 0;
  glVertexAttribPointer(loc2, 4, GL_FLOAT, normalized, stride, offset);

  glBindVertexArray(0); //unbind VAO

}


// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void renderTriangle()
{
  //cout << "begin render..." << endl;
  pipelineProgram->Bind();
  glBindVertexArray(triangleVAO);

	GLint first = 0;
	GLsizei count = 3;
	glDrawArrays(GL_TRIANGLES, first, count);

  //cout << "render successfully" << endl;

  glBindVertexArray(0); //unbind the VAO
}

/*void bindProgram(const float * m, const float * p)
{
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

  GLuint program = pipelineProgram->GetProgramHandle();

	GLuint loc = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(loc);
	const void* offset = (const void*) 0;
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, offset);

	GLuint loc2 = glGetAttribLocation(program, "color");
	glEnableVertexAttribArray(loc2);
	offset = (const void*) sizeof(position);
	glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0, offset);

}
*/

void displayFunc()
{
  // render some stuff...

  //GLuint program = pipelineProgram->GetProgramHandle();
  //glUseProgram(program);

  //cout << "begin display..." << endl;

  GLfloat zStudent = 3.4821354844;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
  openGLMatrix->LoadIdentity();
  openGLMatrix->LookAt(0, 0, zStudent, 0, 0, -1, 0, 1, 0);

  float m[16];
  openGLMatrix->GetMatrix(m);

  openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
  float p[16];
  openGLMatrix->GetMatrix(p);

  //initPipelineProgram(m, p);
  pipelineProgram->SetModelViewMatrix(m);
  pipelineProgram->SetProjectionMatrix(p);

  //bind VAO...
  //initVAO();


  renderTriangle();

  glutSwapBuffers();
}

void idleFunc()
{
  // do some stuff... 

  // for example, here, you can save the screenshots to disk (to make the animation)

  // make the screen update 
  glutPostRedisplay();
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
  }
}

void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // do additional initialization here...
  //cout << "test initVBO ... " << endl;

  openGLMatrix = new OpenGLMatrix();

  initVBO();
  initPipelineProgram();
  //cout << "begin initVAO..." << endl;
  initVAO();
  //cout << "FINISH initVAO..." << endl;
}

int main(int argc, char *argv[])
{
  //cout << "CreateWindow ... " << endl;
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

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
  //cout << "test initScene ... " << endl;
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}


