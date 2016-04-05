/***********************************************************
             CSC418, FALL 2009
 
                 penguin.cpp
                 author: Mike Pratscher
                 based on code by: Eron Steger, J. Radulovich

		Main source file for assignment 2
		Uses OpenGL, GLUT and GLUI libraries
  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design. In particular, see lines marked 'README'.
		
		Be sure to also look over keyframe.h and vector.h.
		While no changes are necessary to these files, looking
		them over will allow you to better understand their
		functionality and capabilites.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "keyframe.h"
#include "timer.h"
#include "vector.h"
#include "groundmap.h"

GLUquadricObj * IDquadric;
GLUquadricObj * IDquadric2;

GroundMap map;
// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

const float SPINNER_SPEED = 1.5;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

GLUI* glui_joints;			// Glui window with joint controls
GLUI* glui_keyframe;		// Glui window with keyframe controls
GLUI* glui_render;			// Glui window for render style

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")

float spark_X[24];			// direction of spark on x-axis
float spark_Y[24];			// direction of spark on y-axis
float spark_Z[24];			// direction of spark on z-axis

// ---------------- ANIMATION VARIABLES ---------------------

// Camera settings
bool updateCamZPos = false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.01;

GLdouble camXPos =  0.0;
GLdouble camYPos =  0.0;
GLdouble camZPos = -7.5;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.1;
const GLdouble FAR_CLIP    = 1000.0;

// Render settings
enum { WIREFRAME, SOLID, OUTLINED, METALLIC, MATTE };	// README: the different render styles
int renderStyle = WIREFRAME;			// README: the selected render style

// Animation settings
int animate_mode = 0;			// 0 = no anim, 1 = animate

// Keyframe settings
const char filenameKF[] = "keyframes.txt";	// file for loading / saving keyframes

Keyframe* keyframes;			// list of keyframes

int maxValidKeyframe   = 0;		// index of max VALID keyframe (in keyframe list)
const int KEYFRAME_MIN = 0;
const int KEYFRAME_MAX = 90;	// README: specifies the max number of keyframes

// Frame settings
char filenameF[128];			// storage for frame filename

int frameNumber = 0;			// current frame being dumped
int frameToFile = 0;			// flag for dumping frames to file

const float DUMP_FRAME_PER_SEC = 24.0;		// frame rate for dumped frames
const float DUMP_SEC_PER_FRAME = 1.0 / DUMP_FRAME_PER_SEC;

// Time settings
Timer* animationTimer;
Timer* frameRateTimer;

const float TIME_MIN = 0.0;
const float TIME_MAX = 180.0;	// README: specifies the max time of the animation
const float SEC_PER_FRAME = 1.0 / 60.0;

// Joint settings

// README: This is the key data structure for
// updating keyframes in the keyframe list and
// for driving the animation.
//   i) When updating a keyframe, use the values
//      in this data structure to update the
//      appropriate keyframe in the keyframe list.
//  ii) When calculating the interpolated pose,
//      the resulting pose vector is placed into
//      this data structure. (This code is already
//      in place - see the animate() function)
// iii) When drawing the scene, use the values in
//      this data structure (which are set in the
//      animate() function as described above) to
//      specify the appropriate transformations.
Keyframe* joint_ui_data;

// README: To change the range of a particular DOF,
// simply change the appropriate min/max values below
const float ROOT_TRANSLATE_X_MIN = -5.0;
const float ROOT_TRANSLATE_X_MAX =  5.0;
const float ROOT_TRANSLATE_Y_MIN = -5.0;
const float ROOT_TRANSLATE_Y_MAX =  5.0;
const float ROOT_TRANSLATE_Z_MIN = -10.0;
const float ROOT_TRANSLATE_Z_MAX =  10.0;
const float ROOT_ROTATE_X_MIN    = -180.0;
const float ROOT_ROTATE_X_MAX    =  180.0;
const float ROOT_ROTATE_Y_MIN    = -180.0;
const float ROOT_ROTATE_Y_MAX    =  180.0;
const float ROOT_ROTATE_Z_MIN    = -180.0;
const float ROOT_ROTATE_Z_MAX    =  180.0;
const float HEAD_MIN             = -78.0;
const float HEAD_MAX             =  78.0;
const float SHOULDER_PITCH_MIN   = -45.0;
const float SHOULDER_PITCH_MAX   =  45.0;
const float SHOULDER_YAW_MIN     = -15.0;
const float SHOULDER_YAW_MAX     =  65.0;
const float SHOULDER_ROLL_MIN    = -45.0;
const float SHOULDER_ROLL_MAX    =  45.0;
const float HIP_PITCH_MIN        = -45.0;
const float HIP_PITCH_MAX        =  45.0;
const float HIP_YAW_MIN          = -45.0;
const float HIP_YAW_MAX          =  45.0;
const float HIP_ROLL_MIN         = -45.0;
const float HIP_ROLL_MAX         =  45.0;
const float BEAK_MIN             =  0.0;
const float BEAK_MAX             =  0.25;
const float ELBOW_MIN            =  0.0;
const float ELBOW_MAX            = 75.0;
const float KNEE_MIN             =  0.0;
const float KNEE_MAX             = 75.0;




// Size declearations

int MAP_SIZE1 = 64;
int WALL_SIZE = 4;

// MIN / MAX declearations

const float TRANSLATE_MIN = -64.0;
const float TRANSLATE_MAX =  64.0;
const float ROTATE_MIN    = -360.0;
const float ROTATE_MAX    =  360.0;

const float GUN_ROTATE_MIN = -25;
const float GUN_ROTATE_MAX = 8;

const float TD_ROTATE_MIN = -15;
const float TD_ROTATE_MAX = 15;

const float SPARK_MIN = -15;
const float SPARK_MAX = 15;

int nmb = 0;

// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initDS();
void initGlut(int argc, char** argv);
void initGlui();
void initGl();


// Callbacks for handling events in glut

float mo[16];
void init_mo();
void update_mo( float angle, float x, float y, float z );

void reshape(int w, int h);
void animate();
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);


// Functions to help draw the object
Vector getInterpolatedJointDOFS(float time);

void drawnmb();
void drawCube(float x, float y, float z);
void drawCircle(float r, int num, int is_front);
void drawcy(float r1, float r2, float length, int num);

void drawSpark(float size);
void drawBall(float r);
void drawShell1();
void drawShell2();
void drawAll();

void drawMap();
void drawPlayer(int not_black);
void drawE1(int not_black);
void drawE2(int not_black);
void drawE3(int not_black);
void drawTD(int not_black);
void drawEnemyPlane(int not_black);

void drawHead_Player();
void drawBody_Player();
void drawTrack_Player();
void drawFender_Player();
void drawGunshild_Player();

void drawHead_E();
void drawBody_E();
void drawFender_E();

void drawHead_TD();
void drawBody_TD();
void drawTrack_TD();
void drawFender_TD();
void drawGunshild_TD();

void drawHead_EnemyPlane();
void drawBody_EnemyPlane();
void drawTrack_EnemyPlane();
void drawFender_EnemyPlane();
void drawGunshild_EnemyPlane();

// Image functions
void writeFrame(char* filename, bool pgm, bool frontBuffer);


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 640x480 window by default...\n");
        Win[0] = 640;
        Win[1] = 480;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }
	// Initialize spark direction
	spark_X[0] = -0.49267513271384755;
	spark_X[1] = -0.6372090188882323;
	spark_X[2] = -0.6413451075922252;
	spark_X[3] = 0.5833071419458141;
	spark_X[4] = 0.9908995501079033;
	spark_X[5] = 0.05545937774511256;
	spark_X[6] = 0.8046307145499476;
	spark_X[7] = 0.9404898993565531;
	spark_X[8] = 0.5247613298025012;
	spark_X[9] = 0.6723293813152816;
	spark_X[10] = -0.653699794304371;
	spark_X[11] = 0.828736589576834;
	spark_X[12] = 0.3868004231650872;
	spark_X[13] = 0.21454836267615485;
	spark_X[14] = -0.5252578029591035;
	spark_X[15] = -0.07044126082402191;
	spark_X[16] = 0.391615242577968;
	spark_X[17] = 0.9680233810333937;
	spark_X[18] = 0.808867401825532;
	spark_X[19] = 0.6932466547521463;
	spark_X[20] = -0.7223628397169777;
	spark_X[21] = 0.784508190325453;
	spark_X[22] = -0.6226794263077511;
	spark_X[23] = 0.39543540320417414;
	
	spark_Y[0] = 0.3868004231650872;
	spark_Y[1] = 0.21454836267615485;
	spark_Y[2] = -0.5252578029591035;
	spark_Y[3] = -0.07044126082402191;
	spark_Y[4] = 0.391615242577968;
	spark_Y[5] = 0.9680233810333937;
	spark_Y[6] = 0.808867401825532;
	spark_Y[7] = 0.6932466547521463;
	spark_Y[8] = -0.7223628397169777;
	spark_Y[9] = 0.784508190325453;
	spark_Y[10] = -0.6226794263077511;
	spark_Y[11] = 0.39543540320417414;
	spark_Y[12] = 0.17077203711829814;
	spark_Y[13] = 0.20476064678007055;
	spark_Y[14] = -0.6410339028974958;
	spark_Y[15] = 0.19058891449967652;
	spark_Y[16] = -0.5133759189117724;
	spark_Y[17] = 0.6102827406828671;
	spark_Y[18] = 0.0383162584576594;
	spark_Y[19] = -0.4488193808338202;
	spark_Y[20] = -0.8621785464487116;
	spark_Y[21] = 0.7285406067567;
	spark_Y[22] = -0.5492604749049776;
	spark_Y[23] = -0.41078367259270043;
	
	spark_Z[0] = 0.17077203711829814;
	spark_Z[1] = 0.20476064678007055;
	spark_Z[2] = -0.6410339028974958;
	spark_Z[3] = 0.19058891449967652;
	spark_Z[4] = -0.5133759189117724;
	spark_Z[5] = 0.6102827406828671;
	spark_Z[6] = 0.0383162584576594;
	spark_Z[7] = -0.4488193808338202;
	spark_Z[8] = -0.8621785464487116;
	spark_Z[9] = 0.7285406067567;
	spark_Z[10] = -0.5492604749049776;
	spark_Z[11] = -0.41078367259270043;
	spark_Z[12] = -0.49267513271384755;
	spark_Z[13] = -0.6372090188882323;
	spark_Z[14] = -0.6413451075922252;
	spark_Z[15] = 0.5833071419458141;
	spark_Z[16] = 0.9908995501079033;
	spark_Z[17] = 0.05545937774511256;
	spark_Z[18] = 0.8046307145499476;
	spark_Z[19] = 0.9404898993565531;
	spark_Z[20] = 0.5247613298025012;
	spark_Z[21] = 0.6723293813152816;
	spark_Z[22] = -0.653699794304371;
	spark_Z[23] = 0.828736589576834;
	
    // Initialize data structs, glut, glui, and opengl
	initDS();
    initGlut(argc, argv);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    
	init_mo();
    glutMainLoop();

    return 0;         // never reached
}


// Create / initialize global data structures
void initDS()
{
	keyframes = new Keyframe[KEYFRAME_MAX];
	for( int i = 0; i < KEYFRAME_MAX; i++ )
		keyframes[i].setID(i);

	animationTimer = new Timer();
	frameRateTimer = new Timer();
	joint_ui_data  = new Keyframe();
}


// Initialize glut and create a window with the specified caption 
void initGlut(int argc, char** argv)
{
	// Init GLUT
	glutInit(&argc, argv);

    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(argv[0]);

    // Setup callback functions to handle events
    glutReshapeFunc(reshape);	// Call reshape whenever window resized
    glutDisplayFunc(display);	// Call display whenever new frame needed
	glutMouseFunc(mouse);		// Call mouse whenever mouse button pressed
	glutMotionFunc(motion);		// Call motion whenever mouse moves while button pressed
}


// Load Keyframe button handler. Called when the "load keyframe" button is pressed
void loadKeyframeButton(int)
{
	// Get the keyframe ID from the UI
	int keyframeID = joint_ui_data->getID();

	// Update the 'joint_ui_data' variable with the appropriate
	// entry from the 'keyframes' array (the list of keyframes)
	*joint_ui_data = keyframes[keyframeID];

	// Sync the UI with the 'joint_ui_data' values
	glui_joints->sync_live();
	glui_keyframe->sync_live();

	// Let the user know the values have been loaded
	sprintf(msg, "Status: Keyframe %d loaded successfully", keyframeID);
	status->set_text(msg);
}

// Update Keyframe button handler. Called when the "update keyframe" button is pressed
void updateKeyframeButton(int)
{
	///////////////////////////////////////////////////////////
	// TODO:
	//   Modify this function to save the UI joint values into
	//   the appropriate keyframe entry in the keyframe list
	//   when the user clicks on the 'Update Keyframe' button.
	//   Refer to the 'loadKeyframeButton' function for help.
	///////////////////////////////////////////////////////////

	// Get the keyframe ID from the UI
	int keyframeID = 0;
	keyframeID = joint_ui_data->getID();

	// Update the 'maxValidKeyframe' index variable
	// (it will be needed when doing the interpolation)
	if(keyframeID > maxValidKeyframe){
		maxValidKeyframe = keyframeID;
	}
	
	keyframes[keyframeID].setTime(joint_ui_data->getTime());
	keyframes[keyframeID].setID(keyframeID);
	// Update the appropriate entry in the 'keyframes' array
	// with the 'joint_ui_data' data
	int i;
	for (i = 0; i<87; i++){
		keyframes[keyframeID].setDOF(i, joint_ui_data->getDOF(i));
	}

	// Let the user know the values have been updated
	sprintf(msg, "Status: Keyframe %d updated successfully", keyframeID);
	status->set_text(msg);
}

// Load Keyframes From File button handler. Called when the "load keyframes from file" button is pressed
//
// ASSUMES THAT THE FILE FORMAT IS CORRECT, ie, there is no error checking!
//
void loadKeyframesFromFileButton(int)
{
	// Open file for reading
	FILE* file = fopen(filenameKF, "r");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Read in maxValidKeyframe first
	fscanf(file, "%d", &maxValidKeyframe);

	// Now read in all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fscanf(file, "%d", keyframes[i].getIDPtr());
		fscanf(file, "%f", keyframes[i].getTimePtr());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fscanf(file, "%f", keyframes[i].getDOFPtr(j));
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been loaded
	sprintf(msg, "Status: Keyframes loaded successfully");
	status->set_text(msg);
}

// Save Keyframes To File button handler. Called when the "save keyframes to file" button is pressed
void saveKeyframesToFileButton(int)
{
	// Open file for writing
	FILE* file = fopen(filenameKF, "w");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Write out maxValidKeyframe first
	fprintf(file, "%d\n", maxValidKeyframe);
	fprintf(file, "\n");

	// Now write out all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fprintf(file, "%d\n", keyframes[i].getID());
		fprintf(file, "%f\n", keyframes[i].getTime());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fprintf(file, "%f\n", keyframes[i].getDOF(j));

		fprintf(file, "\n");
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been saved
	sprintf(msg, "Status: Keyframes saved successfully");
	status->set_text(msg);
}

// Animate button handler.  Called when the "animate" button is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui_keyframe->sync_live();

  // toggle animation mode and set idle function appropriately
  if( animate_mode == 0 )
  {
	// start animation
	frameRateTimer->reset();
	animationTimer->reset();

	animate_mode = 1;
	GLUI_Master.set_glutIdleFunc(animate);

	// Let the user know the animation is running
	sprintf(msg, "Status: Animating...");
	status->set_text(msg);
  }
  else
  {
	// stop animation
	animate_mode = 0;
	GLUI_Master.set_glutIdleFunc(NULL);

	// Let the user know the animation has stopped
	sprintf(msg, "Status: Animation stopped");
	status->set_text(msg);
  }
}

// Render Frames To File button handler. Called when the "Render Frames To File" button is pressed.
void renderFramesToFileButton(int)
{
	// Calculate number of frames to generate based on dump frame rate
	int numFrames = int(keyframes[maxValidKeyframe].getTime() * DUMP_FRAME_PER_SEC) + 1;

	// Generate frames and save to file
	frameToFile = 1;
	for( frameNumber = 0; frameNumber < numFrames; frameNumber++ )
	{
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(frameNumber * DUMP_SEC_PER_FRAME) );

		// Let the user know which frame is being rendered
		sprintf(msg, "Status: Rendering frame %d...", frameNumber);
		status->set_text(msg);

		// Render the frame
		display();
	}
	frameToFile = 0;

	// Let the user know how many frames were generated
	sprintf(msg, "Status: %d frame(s) rendered to file", numFrames);
	status->set_text(msg);
}

// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Initialize GLUI and the user interface
void initGlui()
{
	GLUI_Panel* glui_panel;
	GLUI_Spinner* glui_spinner;
	GLUI_RadioGroup* glui_radio_group;

    GLUI_Master.set_glutIdleFunc(NULL);


	// Create GLUI window (joint controls) ***************
	//
	glui_joints = GLUI_Master.create_glui("Joint Control", 0, Win[0]+12, 0);

    // Create controls to specify root position and orientation
	glui_panel = glui_joints->add_panel("Root");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify player rotation
	glui_panel = glui_joints->add_panel("Player");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_TURRET));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::PLAYER_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_joints->add_column(false);
	
	// Create controls to specify E1 rotation
	glui_panel = glui_joints->add_panel("E1");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_TURRET));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E1_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify E2 rotation
	glui_panel = glui_joints->add_panel("E2");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_TURRET));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E2_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify E3 rotation
	glui_panel = glui_joints->add_panel("E3");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_TURRET));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::E3_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_joints->add_column(false);
	
	// Create controls to specify TD rotation
	glui_panel = glui_joints->add_panel("TD");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_TURRET));
	glui_spinner->set_float_limits(TD_ROTATE_MIN, TD_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::TD_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	
	// Create controls to specify EnemyPlane rotation
	glui_panel = glui_joints->add_panel("Enemy Fighter");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "turret:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_TURRET));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "gun:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_GUN));
	glui_spinner->set_float_limits(GUN_ROTATE_MIN, GUN_ROTATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels R:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_WHEELS_R));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Wheels L:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::EnemyPlane_WHEELS_L));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_joints->add_column(false);
	
	glui_panel = glui_joints->add_panel("Spark");
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Spark Frame:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_FRAME));
	glui_spinner->set_float_limits(SPARK_MIN, SPARK_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(0.1);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Spark Scale:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_SCALE));
	glui_spinner->set_speed(0.1);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Light:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SPARK_LIGHT));
	glui_spinner->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(0.1);
	
	glui_panel = glui_joints->add_panel("Shell1");
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL1_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_panel = glui_joints->add_panel("Shell2");
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_TRANSLATE_X));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_TRANSLATE_Y));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_TRANSLATE_Z));
	glui_spinner->set_float_limits(TRANSLATE_MIN, TRANSLATE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_ROTATE_X));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_ROTATE_Y));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::SHELL2_ROTATE_Z));
	glui_spinner->set_float_limits(ROTATE_MIN, ROTATE_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
	///////////////////////////////////////////////////////////
	// TODO (for controlling light source position & additional
	//      rendering styles):
	//   Add more UI spinner elements here. Be sure to also
	//   add the appropriate min/max range values to this
	//   file, and to also add the appropriate enums to the
	//   enumeration in the Keyframe class (keyframe.h).
	///////////////////////////////////////////////////////////

	//
	// ***************************************************


	// Create GLUI window (keyframe controls) ************
	//
	glui_keyframe = GLUI_Master.create_glui("Keyframe Control", 0, 0, Win[1]+64);

	// Create a control to specify the time (for setting a keyframe)
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Time:", GLUI_SPINNER_FLOAT, joint_ui_data->getTimePtr());
	glui_spinner->set_float_limits(TIME_MIN, TIME_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create a control to specify a keyframe (for updating / loading a keyframe)
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Keyframe ID:", GLUI_SPINNER_INT, joint_ui_data->getIDPtr());
	glui_spinner->set_int_limits(KEYFRAME_MIN, KEYFRAME_MAX-1, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_keyframe->add_separator();

	// Add buttons to load and update keyframes
	// Add buttons to load and save keyframes from a file
	// Add buttons to start / stop animation and to render frames to file
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframe", 0, loadKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframes From File", 0, loadKeyframesFromFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Start / Stop Animation", 0, animateButton);
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_keyframe->add_button_to_panel(glui_panel, "Update Keyframe", 0, updateKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Save Keyframes To File", 0, saveKeyframesToFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Render Frames To File", 0, renderFramesToFileButton);

	glui_keyframe->add_separator();

	// Add status line
	glui_panel = glui_keyframe->add_panel("");
	status = glui_keyframe->add_statictext_to_panel(glui_panel, "Status: Ready");

	// Add button to quit
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Quit", 0, quitButton);
	//
	// ***************************************************


	// Create GLUI window (render controls) ************
	//
	glui_render = GLUI_Master.create_glui("Render Control", 0, 367, Win[1]+64);

	// Create control to specify the render style
	glui_panel = glui_render->add_panel("Render Style");
	glui_radio_group = glui_render->add_radiogroup_to_panel(glui_panel, &renderStyle);
	glui_render->add_radiobutton_to_group(glui_radio_group, "Wireframe");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid w/ outlines");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Metallic");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Matte");
	//
	// ***************************************************


	// Tell GLUI windows which window is main graphics window
	glui_joints->set_main_gfx_window(windowID);
	glui_keyframe->set_main_gfx_window(windowID);
	glui_render->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}


// Calculates the interpolated joint DOF vector
// using Catmull-Rom interpolation of the keyframes
Vector getInterpolatedJointDOFS(float time)
{
	// Need to find the keyframes bewteen which
	// the supplied time lies.
	// At the end of the loop we have:
	//    keyframes[i-1].getTime() < time <= keyframes[i].getTime()
	//
	int i = 0;
	while( i <= maxValidKeyframe && keyframes[i].getTime() < time )
		i++;

	// If time is before or at first defined keyframe, then
	// just use first keyframe pose
	if( i == 0 )
		return keyframes[0].getDOFVector();

	// If time is beyond last defined keyframe, then just
	// use last keyframe pose
	if( i > maxValidKeyframe )
		return keyframes[maxValidKeyframe].getDOFVector();

	// Need to normalize time to (0, 1]
	float alpha = (time - keyframes[i-1].getTime()) / (keyframes[i].getTime() - keyframes[i-1].getTime());

	// Get appropriate data points
	// for computing the interpolation
	Vector p1 = keyframes[i-1].getDOFVector();
	Vector p2 = keyframes[i].getDOFVector();

	///////////////////////////////////////////////////////////
	// TODO (animation using Catmull-Rom):
	//   Currently the code operates using linear interpolation
    //   Modify this function so it uses Catmull-Rom instead.
	///////////////////////////////////////////////////////////

	// Return the linearly interpolated Vector
	//return p0 * (1-alpha) + p1 * alpha;
	
	// citation: this part referenced code on github and old assignment
	Vector t0, t1;
	// case when i is the start point
	if (i == 1){
		Vector p3 = keyframes[i + 1].getDOFVector();
		t0 = p2 - p1;
		t1 = (p3 - p2) / 2;
	}
	// case when i is the last point
	else if (i == maxValidKeyframe){
		Vector p0 = keyframes[i - 2].getDOFVector();
		t0 = (p2 - p0) / 2;
		t1 = p2 - p1;
	}
	// normal case
	else{
		Vector p0 = keyframes[i - 2].getDOFVector();
		Vector p3 = keyframes[i + 1].getDOFVector();
		t0 = (p2 - p0) / 2;
		t1 = (p3 - p1) / 2;
	}
	
	Vector a0, a1, a2, a3;
	a0 = p1;
	a1 = t0;
	a2 = p1 * (-3) + p2 * 3 + t0 * (-2) + t1 * (-1);
	a3 = p1 * 2 + p2 * (-2) + t0 + t1;
	
	return (((a3 * alpha + a2) * alpha + a1) * alpha + a0);
}


// Callback idle function for animating the scene
void animate()
{
	// Only update if enough time has passed
	// (This locks the display to a certain frame rate rather
	//  than updating as fast as possible. The effect is that
	//  the animation should run at about the same rate
	//  whether being run on a fast machine or slow machine)
	if( frameRateTimer->elapsed() > SEC_PER_FRAME )
	{
		// Tell glut window to update itself. This will cause the display()
		// callback to be called, which renders the object (once you've written
		// the callback).
		glutSetWindow(windowID);
		glutPostRedisplay();

		// Restart the frame rate timer
		// for the next frame
		frameRateTimer->reset();
	}
}


// Handles the window being resized by updating the viewport
// and projection matrices
void reshape(int w, int h)
{
	// Update internal variables and OpenGL viewport
	Win[0] = w;
	Win[1] = h;
	glViewport(0, 0, (GLsizei)Win[0], (GLsizei)Win[1]);

    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(CAMERA_FOVY, (GLdouble)Win[0]/(GLdouble)Win[1], NEAR_CLIP, FAR_CLIP);
}

void init_mo()
{
    memset( mo, 0, sizeof(mo) );
    mo[0]=mo[5]=mo[10]=mo[15]=1;
    glutPostRedisplay();
}

void update_mo( float angle, float x, float y, float z )
{
    glPushMatrix();
    glLoadIdentity();
    glRotatef( angle, x,y,z );
    glMultMatrixf(mo);
    glGetFloatv( GL_MODELVIEW_MATRIX, mo );
    glPopMatrix();
    
    glutPostRedisplay();
}

// display callback
//
// README: This gets called by the event handler
// to draw the scene, so this is where you need
// to build your scene -- make your changes and
// additions here. All rendering happens in this
// function. For Assignment 2, updates to the
// joint DOFs (joint_ui_data) happen in the
// animate() function.
void display(void)
{	
    // Clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// Specify camera transformation
	glTranslatef(camXPos, camYPos, camZPos);


	// Get the time for the current animation step, if necessary
	if( animate_mode )
	{
		float curTime = animationTimer->elapsed();

		if( curTime >= keyframes[maxValidKeyframe].getTime() )
		{
			// Restart the animation
			animationTimer->reset();
			curTime = animationTimer->elapsed();
		}

		///////////////////////////////////////////////////////////
		// README:
		//   This statement loads the interpolated joint DOF vector
		//   into the global 'joint_ui_data' variable. Use the
		//   'joint_ui_data' variable below in your model code to
		//   drive the model for animation.
		///////////////////////////////////////////////////////////
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(curTime) );

		// Update user interface
		joint_ui_data->setTime(curTime);
		glui_keyframe->sync_live();
	}

	
    ///////////////////////////////////////////////////////////
    // TODO:
	//   Modify this function to draw the scene.
	//   This should include function calls that apply
	//   the appropriate transformation matrices and render
	//   the individual body parts.
	//   Use the 'joint_ui_data' data structure to obtain
	//   the joint DOFs to specify your transformations.
	//   Sample code is provided below and demonstrates how
	//   to access the joint DOF values. This sample code
	//   should be replaced with your own.
	//   Use the 'renderStyle' variable and the associated
	//   enumeration to determine how the geometry should be
	//   rendered.
    ///////////////////////////////////////////////////////////d
	// SAMPLE CODE **********
	//
	
	glEnable(GL_DEPTH_TEST);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glEnable ( GL_COLOR_MATERIAL );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE , GL_TRUE );
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	// basic layer
	glPushMatrix();
		// determine render style and set glPolygonMode appropriately
		// wireframe render
		//glTranslatef(0, 0, 5);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z), 0.0, 0.0, 1.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y), 0.0, 1.0, 0.0);
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawAll();
		
	glPopMatrix();
    // Execute any GL functions that are in the queue just to be safe
    glFlush();

	// Dump frame to file, if requested
	if( frameToFile )
	{
		sprintf(filenameF, "frame%03d.ppm", frameNumber);
		writeFrame(filenameF, false, false);
	}

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


///////////////////////////////////////////////////////////////
//					Drawing 	Part						 //
//															 //
///////////////////////////////////////////////////////////////

// DRAW OBJECTS
void drawAll(){
	//Light matrix
		glPushMatrix();
			glEnable(GL_LIGHTING);
			GLfloat posLight0[4] = {-1.0f, 1.0f, 1.0f, 0.0f};
			GLfloat ambLight0[4] = {0.5f, 0.5f, 0.5f, 0.5f};
			GLfloat difLight0[4] = {0.5f, 0.5f, 0.5f, 1.0f};
			GLfloat specLight0[4] = {0.5f, 0.5f, 0.5f, 1.0f};
			glLightfv(GL_LIGHT0, GL_POSITION, posLight0);
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambLight0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, difLight0);
			glLightfv(GL_LIGHT0, GL_SPECULAR, specLight0);
			glEnable(GL_LIGHT0);
		glPopMatrix();
		
		glPushMatrix();
			glColor3f(1,1,1);
			glTranslatef(-32, -0.49, -32);
			map.Render("air.tga");
		glPopMatrix();
		//drawing stuff
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		drawMap();
		drawPlayer(1);

		glPushMatrix();
			glTranslatef(0.0, 0.21, 0.0);
			drawE1(1);
			drawE2(1);
			drawE3(1);
			glTranslatef(0.0, -0.05, 0.0);
			drawTD(1);
			glTranslatef(0.0, 0.35, 0.0);
			drawEnemyPlane(1);
		glPopMatrix();
		drawShell1();
		drawShell2();
		
		glPushMatrix();
			glTranslatef(joint_ui_data->getDOF(Keyframe::SPARK_TRANSLATE_X),
					joint_ui_data->getDOF(Keyframe::SPARK_TRANSLATE_Y),
					joint_ui_data->getDOF(Keyframe::SPARK_TRANSLATE_Z));
			glRotatef(joint_ui_data->getDOF(Keyframe::SPARK_ROTATE_X), 1.0, 0.0, 0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::SPARK_ROTATE_Z), 0.0, 0.0, 1.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::SPARK_ROTATE_Y), 0.0, 1.0, 0.0);
			
			if(joint_ui_data->getDOF(Keyframe::SPARK_LIGHT) > 0.5){
				GLfloat posLight1[4] = {-1.0f, 1.0f, 1.0f, 0.0f};
				GLfloat ambLight1[4] = {0.5f, 0.5f, 0.0f, 1.0f};
				GLfloat difLight1[4] = {0.5f, 0.5f, 0.0f, 0.5f};
				GLfloat specLight1[4] = {0.5f, 0.5f, 0.0f, 0.5f};
				glLightfv(GL_LIGHT1, GL_POSITION, posLight1);
				glLightfv(GL_LIGHT1, GL_AMBIENT, ambLight1);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, difLight1);
				glLightfv(GL_LIGHT1, GL_SPECULAR, specLight1);
				glEnable(GL_LIGHT1);
			}
			else{
				glDisable(GL_LIGHT1);
			}
			drawSpark(joint_ui_data->getDOF(Keyframe::SPARK_SCALE));
			glRotatef(180,0,0,1);
			drawSpark(joint_ui_data->getDOF(Keyframe::SPARK_SCALE));
		glPopMatrix();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);
		
		drawPlayer(0);
		
		glPushMatrix();
			glTranslatef(0.0, 0.21, 0.0);
			drawE1(0);
			drawE2(0);
			drawE3(0);
			glTranslatef(0.0, -0.05, 0.0);
			drawTD(0);
			glTranslatef(0.0, 0.35, 0.0);
			drawEnemyPlane(0);
		glPopMatrix();
}


void drawMap(){
	//Draw air
	glPushMatrix();
		glTranslatef(0, -0.6, 0);
		glPushMatrix();
			glColor3f(5.5, 3, 1);
			drawCube(MAP_SIZE1, 0.2, MAP_SIZE1);
		glPopMatrix();
	glPopMatrix();
}

void drawPlayer(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.0078125, 0.3671875, 0.12890625);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::PLAYER_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::PLAYER_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::PLAYER_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::PLAYER_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::PLAYER_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::PLAYER_ROTATE_Z), 0.0, 0.0, 1.0);
	  // Fuselage
	    glPushMatrix();
	    glTranslatef(-0.5f, 0, 0);
	    glScalef(2.0f, 0.8f, 1.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rear Fuselage
	    glPushMatrix();
	    glTranslatef(1.0f, 0.15f, 0);
	    glScalef(2.75f, 0.5f, 0.5f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Wings
	    glPushMatrix();
	    glTranslatef(0, 0.3f, 0);
	    glScalef(0.8f, 0.1f, 6.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rudder
	    glPushMatrix();
	    glTranslatef(2.0f, 0.775f, 0);
	    glScalef(0.75f, 1.15f, 0.1f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Tail-plane
	    glPushMatrix();
	    glTranslatef(1.9f, 0, 0);
	    glScalef(0.85f, 0.1f, 2.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();
	glPopMatrix();
}

void drawE1(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.0098125, 0.5671875, 0.22890625);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::E1_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::E1_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::E1_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::E1_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E1_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E1_ROTATE_Z), 0.0, 0.0, 1.0);
		// Fuselage
	    glPushMatrix();
	    glTranslatef(-0.5f, 0, 0);
	    glScalef(2.0f, 0.4f, 0.5f); // x for width, y for thickness, z for length
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rear Fuselage
	    glPushMatrix();
	    glTranslatef(1.0f, 0.15f, 0);
	    glScalef(2.75f, 0.25f, 0.5f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Wings
	    glPushMatrix();
	    glTranslatef(0, 0.3f, 0);
	    glScalef(0.5f, 0.07f, 8.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rudder
	    glPushMatrix();
	    glTranslatef(2.0f, 0.775f, 0);
	    glScalef(0.375f, 1.05f, 0.02f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Tail-plane
	    glPushMatrix();
	    glTranslatef(1.9f, 0, 0);
	    glScalef(0.275f, 0.1f, 2.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();
	glPopMatrix();
}

void drawE2(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.0048125, 0.6671875, 0.82890625);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::E2_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::E2_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::E2_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::E2_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E2_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E2_ROTATE_Z), 0.0, 0.0, 1.0);
		// Fuselage
	    glPushMatrix();
	    glTranslatef(-0.5f, 0, 0);
	    glScalef(2.0f, 0.4f, 0.5f); // x for width, y for thickness, z for length
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rear Fuselage
	    glPushMatrix();
	    glTranslatef(1.0f, 0.15f, 0);
	    glScalef(2.75f, 0.25f, 0.5f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Wings
	    glPushMatrix();
	    glTranslatef(0, 0.3f, 0);
	    glScalef(0.5f, 0.07f, 8.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rudder
	    glPushMatrix();
	    glTranslatef(2.0f, 0.775f, 0);
	    glScalef(0.375f, 1.05f, 0.02f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Tail-plane
	    glPushMatrix();
	    glTranslatef(1.9f, 0, 0);
	    glScalef(0.275f, 0.1f, 2.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();
	glPopMatrix();
}

void drawE3(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.0148125, 0.1671875, 0.42890625);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::E3_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::E3_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::E3_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::E3_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E3_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::E3_ROTATE_Z), 0.0, 0.0, 1.0);
		// Fuselage
	    glPushMatrix();
	    glTranslatef(-0.5f, 0, 0);
	    glScalef(2.0f, 0.4f, 0.5f); // x for width, y for thickness, z for length
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rear Fuselage
	    glPushMatrix();
	    glTranslatef(1.0f, 0.15f, 0);
	    glScalef(2.75f, 0.25f, 0.5f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Wings
	    glPushMatrix();
	    glTranslatef(0, 0.3f, 0);
	    glScalef(0.5f, 0.07f, 8.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Rudder
	    glPushMatrix();
	    glTranslatef(2.0f, 0.775f, 0);
	    glScalef(0.375f, 1.05f, 0.02f);
	    glutSolidCube(1.0f);
	    glPopMatrix();

	    // Tail-plane
	    glPushMatrix();
	    glTranslatef(1.9f, 0, 0);
	    glScalef(0.275f, 0.1f, 2.0f);
	    glutSolidCube(1.0f);
	    glPopMatrix();
	glPopMatrix();
}

void drawTD(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.6, 0.6, 0.65);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::TD_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::TD_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::TD_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::TD_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::TD_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::TD_ROTATE_Z), 0.0, 0.0, 1.0);
		//Body
		drawBody_TD();
		
		//Head
		glPushMatrix();
			glTranslatef(0.0, 0.0, -0.5);
			drawHead_TD();
			
			//hatch
			glPushMatrix();
				glTranslatef(0.15, 0.25, -0.3);
				glRotatef(90, 0, 0, 1);
				drawcy(0.2, 0.18, 0.15, 12);
			glPopMatrix();
			
			//Gun
			glPushMatrix();
				glTranslatef(0.0, 0.175, 0.1);
				drawGunshild_TD();
				
				glRotatef(joint_ui_data->getDOF(Keyframe::TD_TURRET), 0, 1, 0);
				glRotatef(joint_ui_data->getDOF(Keyframe::TD_GUN), 1, 0, 0);
				glRotatef(-90, 0, 1, 0);
				
				glTranslatef(0.52, 0.05, 0);
				drawCube(0.05,0.3,0.35);
				glTranslatef(-0.5, 0, 0);
				
				drawcy(0.06, 0.06, 2.2, 12);
			glPopMatrix();
		glPopMatrix();
		
		//Right side
		glPushMatrix();
			//Right track
			glTranslatef(0.5, -0.27, -0.07);
			if(not_black){
				glColor3f(0.5, 0.5, 0.5);
			}
			drawTrack_Player();
			
			//Right fender
			glTranslatef(0, 0.10, 0);
			if(not_black){
				glColor3f(0.6, 0.6, 0.65);
			}
			drawFender_TD();
			
			//Right wheels
			glPushMatrix();
				if(not_black){
					glColor3f(0.6, 0.6, 0.65);
				}
				glTranslatef(0.0, -0.2, 1.34);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, -0.1, -0.3);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.35);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.35);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0.1, -0.3);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_R), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
		
		//Left track
		glPushMatrix();
			glTranslatef(-0.76, -0.27, -0.07);
			if(not_black){
				glColor3f(0.5, 0.5, 0.5);
			}
			drawTrack_Player();
			
			//Left fender
			glTranslatef(0, 0.10, 0);
			if(not_black){
				glColor3f(0.6, 0.6, 0.65);
			}
			drawFender_TD();
			
			//Left wheels
			glPushMatrix();
				if(not_black){
					glColor3f(0.6, 0.6, 0.65);
				}
				glTranslatef(0.0, -0.15, 1.34);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, -0.1, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.35);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.35);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.15, 0.15, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0.1, -0.32);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::TD_WHEELS_L), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void drawEnemyPlane(int not_black){
	glPushMatrix();
		if(not_black){
			glColor3f(0.6, 0.6, 0.65);
		}
		glTranslatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_TRANSLATE_X),
				joint_ui_data->getDOF(Keyframe::EnemyPlane_TRANSLATE_Y),
				joint_ui_data->getDOF(Keyframe::EnemyPlane_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_ROTATE_Z), 0.0, 0.0, 1.0);
		//Body
		drawBody_EnemyPlane();
		
		//Head
		glPushMatrix();
			glTranslatef(0.0, 0.0, -0.5);
			glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_TURRET), 0, 1, 0);
			drawHead_EnemyPlane();
			
			//Gun
			glPushMatrix();
				drawGunshild_EnemyPlane();
				glTranslatef(0.0, 0.3, 0.6);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_GUN), 1, 0, 0);
				glRotatef(-90, 0, 1, 0);
				glTranslatef(0.52, 0.05, 0);
				glTranslatef(-0.5, 0, 0);
				drawcy(0.07, 0.07, 2.2, 12);
				glTranslatef(2.2, 0, 0);
				
				drawcy(0.1, 0.1, 0.4, 12);
			glPopMatrix();
		glPopMatrix();
		
		//Right side
		glPushMatrix();
			//Right track
			glTranslatef(0.35, -0.6, -0.1);
			if(not_black){
				glColor3f(0.5, 0.5, 0.5);
			}
			drawTrack_EnemyPlane();
			
			//Right fender
			glTranslatef(0, 0.10, 0);
			if(not_black){
				glColor3f(0.6, 0.6, 0.65);
			}
			drawFender_EnemyPlane();
			
			//Right wheels
			glPushMatrix();
				if(not_black){
					glColor3f(0.6, 0.6, 0.65);
				}
				glTranslatef(0.0, -0.2, 1.8);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, -0.06, -0.4);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0.1, -0.4);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_R), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
		
		//Left track
		glPushMatrix();
			glTranslatef(-0.6, -0.6, -0.1);
			if(not_black){
				glColor3f(0.5, 0.5, 0.5);
			}
			drawTrack_EnemyPlane();
			
			//Left fender
			glTranslatef(0, 0.10, 0);
			if(not_black){
				glColor3f(0.6, 0.6, 0.65);
			}
			drawFender_EnemyPlane();
			
			//Left wheels
			glPushMatrix();
				if(not_black){
					glColor3f(0.6, 0.6, 0.65);
				}
				glTranslatef(0.0, -0.2, 1.8);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, -0.06, -0.4);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0, -0.47);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.2, 0.2, 0.22, 12);
				glPopMatrix();
				
				glTranslatef(0, 0.1, -0.4);
				glPushMatrix();
					glRotatef(joint_ui_data->getDOF(Keyframe::EnemyPlane_WHEELS_L), 1, 0, 0);
					drawcy(0.1, 0.1, 0.22, 12);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void drawSpark(float size){
	int i;
	float f;
	glPushMatrix();
		glColor4f(1.0, 0.6431372549019608, 0.3764705882352941,1);
		for(i=0; i<24; i++){
			glPushMatrix();
				f = joint_ui_data->getDOF(Keyframe::SPARK_FRAME);
				glTranslatef(f * spark_X[i], f * spark_Y[i], f * spark_Z[i]);
				drawCube(size,size,size);
			glPopMatrix();
		}
	glPopMatrix();
}

void drawBall(float r){
	IDquadric2 = gluNewQuadric();
	gluSphere(IDquadric2, r, 16,16);
}

// PLAYER PARTS
void drawBody_Player(){
	glBegin(GL_QUADS);
	 glPushMatrix();
      glScalef(3.0, 3.0, 3.0);
      glTranslatef(3, 4, 5);

      // body
      glPushMatrix();
         glScalef(1.0, 1.0, 10.0);
      glPopMatrix();

      // tail
      glPushMatrix();
         glTranslatef(0, 0, -8.5);
         glScalef(3.0, 0.2, 0.5);
         glPushMatrix();
            glRotatef(90, 0, 0, 1);
         glPopMatrix();
      glPopMatrix();

      // wing
      glPushMatrix();
         glTranslatef(0, 0, 3.0);
         glScalef(10.0, 0.3, 1.0);
      glPopMatrix();
	glEnd();
}

void drawHead_Player(){
	glBegin(GL_QUADS);
	//L1
		//Left
		glNormal3f(0.0, -0.624695, -0.780869);
		glVertex3f(-0.1,  0.0, 0.8);
		glVertex3f(-0.3,  0.0, 0.8);
		glVertex3f(-0.25, 0.25, 0.6);
		glVertex3f(-0.1, 0.25, 0.6);
		
		//not possible
		glNormal3f(0.392534, -0.706562, -0.588802);
		glVertex3f(-0.3,  0.0, 0.8);
		glVertex3f(-0.6,  0.0, 0.6);
		glVertex3f(-0.45, 0.25, 0.4);
		glVertex3f(-0.25, 0.25, 0.6);
		
		glNormal3f(0.766402, -0.613121, -0.1916);
		glVertex3f(-0.6,  0.0, 0.6);
		glVertex3f(-0.7,  0.0, 0.2);
		glVertex3f(-0.5, 0.25, 0.2);
		glVertex3f(-0.45, 0.25, 0.4);
		
		glNormal3f(0.780869, -0.624695, 0.0);
		glVertex3f(-0.7,  0.0, 0.2);
		glVertex3f(-0.7,  0.0, -0.2);
		glVertex3f(-0.5, 0.25, -0.2);
		glVertex3f(-0.5, 0.25, 0.2);
		
		//same
		glNormal3f(0.780869, -0.624695, 0.0);
		glVertex3f(-0.7,  0.0, 0.2);
		glVertex3f(-0.7,  0.0, -0.2);
		glVertex3f(-0.5, 0.25, -0.2);
		glVertex3f(-0.5, 0.25, 0.2);
		glVertex3f(-0.7,  0.0, 0.2);
		
		//not possible
		glNormal3f(0.680414, -0.272166, 0.680414);
		glVertex3f(-0.7,  0.0, -0.2);
		glVertex3f(-0.5,  0.0, -0.4);
		glVertex3f(-0.4, 0.25, -0.4);
		glVertex3f(-0.5, 0.25, -0.2);
		
		//not possible
		glNormal3f(0.238095, -0.190476, 0.952381);
		glVertex3f(-0.5,  0.0, -0.4);
		glVertex3f(-0.1,  0.0, -0.5);
		glVertex3f(-0.1, 0.25, -0.45);
		glVertex3f(-0.4, 0.25, -0.4);
		
		//Back
		glNormal3f(0.0, -0.196116, 0.980581);
		glVertex3f(-0.1,  0.0, -0.5);
		glVertex3f( 0.1,  0.0, -0.5);
		glVertex3f( 0.1, 0.25, -0.45);
		glVertex3f(-0.1, 0.25, -0.45);
		
		//Right
		//not possible
		glNormal3f(-0.241402, -0.0965609, 0.965609);
		glVertex3f(0.1,  0.0, -0.5);
		glVertex3f(0.5,  0.0, -0.4);
		glVertex3f(0.4, 0.25, -0.4);
		glVertex3f(0.1, 0.25, -0.45);
		
		//not possible
		glNormal3f(-0.615457, -0.492366, 0.615457);
		glVertex3f(0.5,  0.0, -0.4);
		glVertex3f(0.7,  0.0, -0.2);
		glVertex3f(0.5, 0.25, -0.2);
		glVertex3f(0.4, 0.25, -0.4);
		
		
		glNormal3f(-0.780869, -0.624695, 0.0);
		glVertex3f(0.7,  0.0, -0.2);
		glVertex3f(0.7,  0.0, 0.2);
		glVertex3f(0.5, 0.25, 0.2);
		glVertex3f(0.5, 0.25, -0.2);
		
		//same
		glNormal3f(-0.780869, -0.624695, 0.0);
		glVertex3f(0.7,  0.0, -0.2);
		glVertex3f(0.7,  0.0, 0.2);
		glVertex3f(0.5, 0.25, 0.2);
		glVertex3f(0.5, 0.25, -0.2);

		glNormal3f(-0.766402, -0.613121, -0.1916);
		glVertex3f(0.7,  0.0, 0.2);
		glVertex3f(0.6,  0.0, 0.6);
		glVertex3f(0.45, 0.25, 0.4);
		glVertex3f(0.5, 0.25, 0.2);
		
		//not possible
		glNormal3f(-0.438108, -0.613351, -0.657162);
		glVertex3f(0.6,  0.0, 0.6);
		glVertex3f(0.3,  0.0, 0.8);
		glVertex3f(0.25, 0.25, 0.6);
		glVertex3f(0.45, 0.25, 0.4);
		
		glNormal3f(0.0, -0.624695, -0.780869);
		glVertex3f(0.3,  0.0, 0.8);
		glVertex3f(0.1,  0.0, 0.8);
		glVertex3f(0.1, 0.25, 0.6);
		glVertex3f(0.25, 0.25, 0.6);
		
		//Front
		glNormal3f(0.0, -0.624695, -0.780869);
		glVertex3f(0.1,  0.0, 0.8);
		glVertex3f(-0.1,  0.0, 0.8);
		glVertex3f(-0.1, 0.25, 0.6);
		glVertex3f(0.1, 0.25, 0.6);
	//L2
		//Right
		glNormal3f(0.0, 0.874157, 0.485643);
		glVertex3f(0.1, 0.25, 0.6);
		glVertex3f(0.25, 0.25, 0.6);
		glVertex3f(0.25, 1.0/3.0, 0.45);
		glVertex3f(0.2, 1.0/3.0, 0.45);
		
		//not possible
		glNormal3f(0.237815, 0.941747, 0.237815);
		glVertex3f(0.25, 0.25, 0.6);
		glVertex3f(0.45, 0.25, 0.4);
		glVertex3f(0.32, 1.0/3.0, 0.2);
		glVertex3f(0.25, 1.0/3.0, 0.45);
		
		//not possible
		glNormal3f(0.352847, 0.931514, 0.0882116);
		glVertex3f(0.45, 0.25, 0.4);
		glVertex3f(0.5, 0.25, 0.2);
		glVertex3f(0.33, 1.0/3.0, 0.0);
		glVertex3f(0.32, 1.0/3.0, 0.2);
		
		glNormal3f(0.440157, 0.897921, 0.0);
		glVertex3f(0.5, 0.25, 0.2);
		glVertex3f(0.5, 0.25, -0.2);
		glVertex3f(0.33, 1.0/3.0, -0.2);
		glVertex3f(0.33, 1.0/3.0, 0.0);
		
		//not possible
		glNormal3f(0.460796, 0.857079, -0.230397);
		glVertex3f(0.5, 0.25, -0.2);
		glVertex3f(0.4, 0.25, -0.4);
		glVertex3f(0.32, 1.0/3.0, -0.25);
		glVertex3f(0.33, 1.0/3.0, -0.2);
		
		//not possible
		glNormal3f(0.0920575, 0.828517, -0.552345);
		glVertex3f(0.4, 0.25, -0.4);
		glVertex3f(0.1, 0.25, -0.45);
		glVertex3f(0.25, 1.0/3.0, -0.3);
		glVertex3f(0.32, 1.0/3.0, -0.25);
		
		//Back
		glNormal3f(0.0, 0.874157, -0.485644);
		glVertex3f(0.1, 0.25, -0.45);
		glVertex3f(-0.1, 0.25, -0.45);
		glVertex3f(-0.25, 1.0/3.0, -0.3);
		glVertex3f(0.25, 1.0/3.0, -0.3);
		
		//Left
		//not possible
		glNormal3f(-0.0755288, 0.888218, -0.453172);
		glVertex3f(-0.1, 0.25, -0.45);
		glVertex3f(-0.4, 0.25, -0.4);
		glVertex3f(-0.32, 1.0/3.0, -0.25);
		glVertex3f(-0.25, 1.0/3.0, -0.3);
		
		//not possible
		glNormal3f(-0.429871, 0.876935, -0.214935);
		glVertex3f(-0.4, 0.25, -0.4);
		glVertex3f(-0.5, 0.25, -0.2);
		glVertex3f(-0.33, 1.0/3.0, -0.2);
		glVertex3f(-0.32, 1.0/3.0, -0.25);
		
		glNormal3f(-0.440157, 0.897921, 0.0);
		glVertex3f(-0.5, 0.25, -0.2);
		glVertex3f(-0.5, 0.25, 0.2);
		glVertex3f(-0.33, 1.0/3.0, 0.0);
		glVertex3f(-0.33, 1.0/3.0, -0.2);
		
		//not possible
		glNormal3f(-0.417826, 0.902502, 0.104456);
		glVertex3f(-0.5, 0.25, 0.2);
		glVertex3f(-0.45, 0.25, 0.4);
		glVertex3f(-0.32, 1.0/3.0, 0.2);
		glVertex3f(-0.33, 1.0/3.0, 0.0);
		
		//not possibe
		glNormal3f(-0.436853, 0.786333, 0.436853);
		glVertex3f(-0.45, 0.25, 0.4);
		glVertex3f(-0.25, 0.25, 0.6);
		glVertex3f(-0.25, 1.0/3.0, 0.45);
		glVertex3f(-0.32, 1.0/3.0, 0.2);
		
		glNormal3f(0.0, 0.874157, 0.485643);
		glVertex3f(-0.25, 0.25, 0.6);
		glVertex3f(-0.1, 0.25, 0.6);
		glVertex3f(-0.1, 1.0/3.0, 0.45);
		glVertex3f(-0.25, 1.0/3.0, 0.45);
		
		//Front
		glNormal3f(0.0, 0.874157, 0.485644);
		glVertex3f(-0.1, 0.25, 0.6);
		glVertex3f( 0.1, 0.25, 0.6);
		glVertex3f( 0.1, 1.0/3.0, 0.45);
		glVertex3f(-0.1, 1.0/3.0, 0.45);
	
	//Top
		glNormal3f(0,1,0);
		glVertex3f(-0.1, 1.0/3.0, 0.45);
		glVertex3f( 0.1, 1.0/3.0, 0.45);
		glVertex3f(0.25, 1.0/3.0, 0.45);
		glVertex3f(-0.25, 1.0/3.0, 0.45);
		
		glNormal3f(0,1,0);
		glVertex3f(0.33, 1.0/3.0, 0.0);
		glVertex3f(0.33, 1.0/3.0, -0.2);
		glVertex3f(-0.33, 1.0/3.0, -0.2);
		glVertex3f(-0.33, 1.0/3.0, 0.0);
		
		glNormal3f(0,1,0);
		glVertex3f(0.25, 1.0/3.0, -0.3);
		glVertex3f(-0.25, 1.0/3.0, -0.3);
		glVertex3f(-0.32, 1.0/3.0, -0.25);
		glVertex3f(0.32, 1.0/3.0, -0.25);
		
		glNormal3f(0,1,0);
		glVertex3f(-0.32, 1.0/3.0, 0.2);
		glVertex3f(0.32, 1.0/3.0, 0.2);
		glVertex3f(-0.1, 1.0/3.0, 0.45);
		glVertex3f(-0.1, 1.0/3.0, 0.45);
	glEnd();
}

void drawTrack_Player(){
	glPushMatrix();
		//Upside
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.25, 0.02, 2.3);
		
		//Front Up
		glTranslatef(0, 0, 1.15);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front lower
		glTranslatef(0, 0, 0.05);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.25, 0.02, 0.25);
		
		//Front bottom
		glTranslatef(0, 0, 0.125);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Bottom
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 1.0);
		drawCube(0.25, 0.02, 2.0);
		
		//Back bottom
		glTranslatef(0, 0, 1.0);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Back lower
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.25, 0.02, 0.25);
		
		//Back mid
		glTranslatef(0, 0, 0.125);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Back Up
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
	glPopMatrix();
}

void drawFender_Player(){
	glPushMatrix();
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.25, 0.02, 2.4);
		
		//Front Up
		glTranslatef(0, 0, 1.2);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
	glPopMatrix();
}

void drawGunshild_Player(){
	
	glBegin(GL_QUADS);
	
		glVertex3f(-0.3, -0.05, 0.05);
		glVertex3f( 0.0, -0.1, 0.1);
		glVertex3f( 0.0,  0.1, 0.1);
		glVertex3f(-0.3,  0.05, 0.05);
		
		// draw back leg
		
		glVertex3f(-0.3, 0.05, -0.05);
		glVertex3f( 0.0,  0.1, -0.1);
		glVertex3f( 0.0, -0.1, -0.1);
		glVertex3f(-0.3, -0.05, -0.05);
		//left
		glVertex3f(-0.3, -0.05, 0.05);
		glVertex3f(-0.3,  0.05, 0.05);
		glVertex3f(-0.3,  0.05, -0.05);
		glVertex3f(-0.3, -0.05, -0.05);
		
		//right
		glVertex3f( 0.0, -0.1, -0.1);
		glVertex3f( 0.0,  0.1, -0.1);
		glVertex3f( 0.0,  0.1, 0.1);
		glVertex3f( 0.0, -0.1, 0.1);
		
		// draw up leg
		glVertex3f( 0.0,  0.1, -0.1);
		glVertex3f( -0.3,  0.05, -0.05);
		glVertex3f( -0.3,  0.05, 0.05);
		glVertex3f(-0.0,  0.1, 0.1);

		// draw bottom leg
		glVertex3f(-0.0,  -0.1, 0.1);
		glVertex3f(-0.0,  -0.1, -0.1);
		glVertex3f( -0.3, -0.05, -0.05);
		glVertex3f( -0.3, -0.05, 0.05);
	glEnd();
}

// E PARTS
void drawBody_E(){
	glBegin(GL_QUADS);
		// draw front upper face
		glNormal3f(0.0, 0.7432941462471663, 0.6689647316224496);
		glVertex3f( 0.5,  0.0, 0.9);
		glVertex3f(-0.5,  0.0, 0.9);
		glVertex3f(-0.5, -1.0/3.0, 1.3);
		glVertex3f( 0.5, -1.0/3.0, 1.3);
		
		// draw front lower face
		glNormal3f(0.0, -0.7071067811865475, -0.7071067811865475);
		glVertex3f( 0.5, -1.0/3.0, 1.3);
		glVertex3f(-0.5, -1.0/3.0, 1.3);
		glVertex3f(-0.5, -1.0/2.0, 1.15);
		glVertex3f( 0.5, -1.0/2.0, 1.15);
		
		// draw back face
		glNormal3f(-0.9805806756909201, -0.19611613513818402, 0.0);
		glVertex3f( 0.5, -1.0/2.0, -0.9);
		glVertex3f( 0.5,  0.0, -1.1);
		glVertex3f(-0.5,  0.0, -1.1);
		glVertex3f(-0.5, -1.0/2.0, -0.9);

		// draw right face
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.5, -1.0/2.0, -0.9);
		glVertex3f( 0.5, -1.0/2.0,  1.15);
		glVertex3f( 0.5,  0.0,  0.9);
		glVertex3f( 0.5,  0.0, -1.1);
		
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.5, -1.0/2.0,  1.15);
		glVertex3f( 0.5,  0.0,  0.9);
		glVertex3f( 0.5, -1.0/3.0,  1.3);
		glVertex3f( 0.5, -1.0/3.0,  1.3);
		
		// draw left face
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.5, -1.0/2.0, -0.9);
		glVertex3f( -0.5, -1.0/2.0,  1.15);
		glVertex3f( -0.5,  0.0,  0.9);
		glVertex3f( -0.5,  0.0, -1.1);
		
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.5, -1.0/2.0,  1.15);
		glVertex3f( -0.5,  0.0,  0.9);
		glVertex3f( -0.5, -1.0/3.0,  1.3);
		glVertex3f( -0.5, -1.0/3.0,  1.3);


		// draw top
		glNormal3f(  0.0, 1.0,  0.0);
		glVertex3f(  0.5, 0.0, -1.1);
		glVertex3f( -0.5, 0.0, -1.1);
		glVertex3f( -0.5, 0.0,  0.9);
		glVertex3f(  0.5, 0.0,  0.9);

		// draw bottom
		glNormal3f(  0.0, -1.0,  0.0);
		glVertex3f(  0.5, -1.0/3.0,  1.15);
		glVertex3f( -0.5, -1.0/3.0,  1.15);
		glVertex3f( -0.5, -1.0/3.0, -0.9);
		glVertex3f(  0.5, -1.0/3.0, -0.9);
	glEnd();
}

void drawHead_E(){
	glBegin(GL_QUADS);
		//Left front
		glNormal3f(-0.7001400420140049, 0.140028008402801, 0.7001400420140049);
		glVertex3f(-0.25, 0.0, 0.5);
		glVertex3f(-0.4,  0.0, -0.15);
		glVertex3f(-0.38, 1.0/3.0, -0.13);
		glVertex3f(-0.23, 1.0/3.0, 0.48);
		
		//Left back
		glNormal3f(-0.7001400420140049, 0.140028008402801, -0.7001400420140049);
		glVertex3f(-0.4,  0.0, -0.15);
		glVertex3f(-0.35, 0.0, -0.5);
		glVertex3f(-0.33, 1.0/3.0, -0.48);
		glVertex3f(-0.38, 1.0/3.0, -0.13);
		
		//Back
		glNormal3f(0, 0, -1);
		glVertex3f(-0.35, 0.0, -0.5);
		glVertex3f(0.35, 0.0, -0.5);
		glVertex3f(0.33, 1.0/3.0, -0.48);
		glVertex3f(-0.33, 1.0/3.0, -0.48);
		
		//Right back
		glNormal3f(0.7001400420140049, 0.140028008402801, -0.7001400420140049);
		glVertex3f(0.35, 0.0, -0.5);
		glVertex3f(0.4, 0.0, -0.15);
		glVertex3f(0.38, 1.0/3.0, -0.13);
		glVertex3f(0.33, 1.0/3.0, -0.48);
		
		//Right front
		glNormal3f(0.7001400420140049, 0.140028008402801, 0.7001400420140049);
		glVertex3f(0.4, 0.0, -0.15);
		glVertex3f(0.25, 0.0, 0.5);
		glVertex3f(0.22, 1.0/3.0, 0.48);
		glVertex3f(0.38, 1.0/3.0, -0.13);
		
		//Front
		glNormal3f(0, 0, 1);
		glVertex3f(0.25, 0.0, 0.5);
		glVertex3f(-0.25, 0.0, 0.5);
		glVertex3f(-0.22, 1.0/3.0, 0.48);
		glVertex3f(0.22, 1.0/3.0, 0.48);
		
		//Up
		glNormal3f(0, 1, 0);
		glVertex3f(-0.22, 1.0/3.0, 0.48);
		glVertex3f(0.22, 1.0/3.0, 0.48);
		glVertex3f(0.33, 1.0/3.0, -0.48);
		glVertex3f(-0.33, 1.0/3.0, -0.48);
		
		glNormal3f(0, 1, 0);
		glVertex3f(0.38, 1.0/3.0, -0.13);
		glVertex3f(-0.38, 1.0/3.0, -0.13);
		glVertex3f(-0.33, 1.0/3.0, -0.48);
		glVertex3f(0.33, 1.0/3.0, -0.48);
	glEnd();
}

void drawFender_E(){
	glPushMatrix();
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.25, 0.02, 2.4);
		
		//Front Up
		glTranslatef(0, 0, 1.2);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
	glPopMatrix();
}


// TD PARTS
void drawBody_TD(){
	glBegin(GL_QUADS);
		// draw front upper face
		glNormal3f(0.0, 0.7432941462471663, 0.6689647316224496);
		glVertex3f( 0.5,  0.0, 0.9);
		glVertex3f(-0.5,  0.0, 0.9);
		glVertex3f(-0.5, -1.0/3.0, 1.3);
		glVertex3f( 0.5, -1.0/3.0, 1.3);
		
		// draw front lower face
		glNormal3f(0.0, -0.7071067811865475, -0.7071067811865475);
		glVertex3f( 0.5, -1.0/3.0, 1.3);
		glVertex3f(-0.5, -1.0/3.0, 1.3);
		glVertex3f(-0.5, -1.0/2.0, 1.15);
		glVertex3f( 0.5, -1.0/2.0, 1.15);
		
		// draw back face
		glNormal3f(-0.9805806756909201, -0.19611613513818402, 0.0);
		glVertex3f( 0.5, -1.0/2.0, -0.9);
		glVertex3f( 0.5,  0.0, -1.1);
		glVertex3f(-0.5,  0.0, -1.1);
		glVertex3f(-0.5, -1.0/2.0, -0.9);

		// draw right face
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.5, -1.0/2.0, -0.9);
		glVertex3f( 0.5, -1.0/2.0,  1.15);
		glVertex3f( 0.5,  0.0,  0.9);
		glVertex3f( 0.5,  0.0, -1.1);
		
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.5, -1.0/2.0,  1.15);
		glVertex3f( 0.5,  0.0,  0.9);
		glVertex3f( 0.5, -1.0/3.0,  1.3);
		glVertex3f( 0.5, -1.0/3.0,  1.3);
		
		// draw left face
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.5, -1.0/2.0, -0.9);
		glVertex3f( -0.5, -1.0/2.0,  1.15);
		glVertex3f( -0.5,  0.0,  0.9);
		glVertex3f( -0.5,  0.0, -1.1);
		
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.5, -1.0/2.0,  1.15);
		glVertex3f( -0.5,  0.0,  0.9);
		glVertex3f( -0.5, -1.0/3.0,  1.3);
		glVertex3f( -0.5, -1.0/3.0,  1.3);


		// draw top
		glNormal3f(  0.0, 1.0,  0.0);
		glVertex3f(  0.5, 0.0, -1.1);
		glVertex3f( -0.5, 0.0, -1.1);
		glVertex3f( -0.5, 0.0,  0.9);
		glVertex3f(  0.5, 0.0,  0.9);

		// draw bottom
		glNormal3f(  0.0, -1.0,  0.0);
		glVertex3f(  0.5, -1.0/3.0,  1.15);
		glVertex3f( -0.5, -1.0/3.0,  1.15);
		glVertex3f( -0.5, -1.0/3.0, -0.9);
		glVertex3f(  0.5, -1.0/3.0, -0.9);
	glEnd();
}

void drawHead_TD(){
	glBegin(GL_QUADS);
		// draw front face
		glNormal3f(0.0, 0.500444341617353, 0.8657687109980206);
		glVertex3f(-0.5,  0.0, 0.6);
		glVertex3f( 0.5,  0.0, 0.6);
		glVertex3f( 0.35,  0.35, 0.1);
		glVertex3f(-0.35,  0.35, 0.1);

		// draw back face
		glNormal3f(0.0, 0.19611613513818402, -0.9805806756909201);
		glVertex3f(-0.5,  0.0, -0.6);
		glVertex3f( 0.5,  0.0, -0.6);
		glVertex3f( 0.35,  0.35, -0.5);
		glVertex3f(-0.35,  0.35, -0.5);

		// draw left face
		glNormal3f(-0.9805806756909201, 0.19611613513818402, 0.0);
		glVertex3f(-0.5,  0.0, 0.6);
		glVertex3f(-0.35,  0.35, 0.1);
		glVertex3f(-0.35,  0.35, -0.5);
		glVertex3f(-0.5,  0.0, -0.6);

		// draw right face
		glNormal3f(0.9805806756909201, 0.19611613513818402, 0.0);
		glVertex3f(0.5,  0.0, 0.6);
		glVertex3f(0.35,  0.35, 0.1);
		glVertex3f(0.35,  0.35, -0.5);
		glVertex3f(0.5,  0.0, -0.6);

		// draw top
		glNormal3f(0, 1, 0);
		glVertex3f(-0.35,  0.35, 0.1);
		glVertex3f( 0.35,  0.35, 0.1);
		glVertex3f( 0.35,  0.35, -0.5);
		glVertex3f(-0.35,  0.35, -0.5);
		

		// draw bottom
		glNormal3f(0, -1, 0);
		glVertex3f(-0.5,  0.0, 0.6);
		glVertex3f( 0.5,  0.0, 0.6);
		glVertex3f( 0.5,  0.0, -0.6);
		glVertex3f(-0.5,  0.0, -0.6);
	glEnd();
}

void drawFender_TD(){
	glPushMatrix();
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.25, 0.02, 2.4);
		
		//Front Up
		glTranslatef(0, 0, 1.2);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
	glPopMatrix();
}

void drawGunshild_TD(){
	glBegin(GL_QUADS);
		// draw front face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-0.15,  -0.10, 0.5);
		glVertex3f( 0.15,  -0.10, 0.5);
		glVertex3f( 0.15,  0.14, 0.5);
		glVertex3f(-0.15,  0.14, 0.5);

		// draw back face
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(-0.175,  -0.145, 0.0);
		glVertex3f( 0.175,  -0.145, 0.0);
		glVertex3f( 0.175,  0.145, 0.0);
		glVertex3f(-0.175,  0.145, 0.0);

		// draw left face
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-0.175,  -0.145, 0.0);
		glVertex3f(-0.15,  -0.10, 0.5);
		glVertex3f(-0.15,  0.14, 0.5);
		glVertex3f(-0.175,  0.145, 0.0);

		// draw right face
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(0.175,  -0.145, 0.0);
		glVertex3f(0.15,  -0.10, 0.5);
		glVertex3f(0.15,  0.14, 0.5);
		glVertex3f(0.175,  0.145, 0.0);

		// draw top
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f( 0.175,  0.145, 0.0);
		glVertex3f(-0.175,  0.145, 0.0);
		glVertex3f(-0.15,  0.14, 0.5);
		glVertex3f( 0.15,  0.14, 0.5);
		
		// draw bottom
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-0.175,  -0.145, 0.0);
		glVertex3f( 0.175,  -0.145, 0.0);
		glVertex3f( 0.15,  -0.10, 0.5);
		glVertex3f(-0.15,  -0.10, 0.5);
	glEnd();
}

void drawTrack_TD(){
	glPushMatrix();
		//Upside
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.25, 0.02, 2.3);
		
		//Front Up
		glTranslatef(0, 0, 1.15);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front lower
		glTranslatef(0, 0, 0.05);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.25, 0.02, 0.25);
		
		//Front bottom
		glTranslatef(0, 0, 0.125);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Bottom
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 1.0);
		drawCube(0.25, 0.02, 2.0);
		
		//Back bottom
		glTranslatef(0, 0, 1.0);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Back lower
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.25, 0.02, 0.25);
		
		//Back mid
		glTranslatef(0, 0, 0.125);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Back Up
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
	glPopMatrix();
}

// EnemyPlane PART
void drawBody_EnemyPlane(){
	glBegin(GL_QUADS);
		// draw front upper face
		glNormal3f(0.0, 0.7432941462471663, 0.6689647316224496);
		glVertex3f( 0.75,  0.0, 1.5);
		glVertex3f(-0.75,  0.0, 1.5);
		glVertex3f(-0.75, -1.0/2.0, 1.9);
		glVertex3f( 0.75, -1.0/2.0, 1.9);
		
		// draw front lower face
		glNormal3f(0.0, -0.7071067811865475, 0.7071067811865475);
		glVertex3f(-0.75, -1.0/2.0, 1.9);
		glVertex3f( 0.75, -1.0/2.0, 1.9);
		glVertex3f( 0.75, -0.8, 1.7);
		glVertex3f(-0.75, -0.8, 1.7);
		
		// draw back upper face
		glNormal3f(0.0, 0.7432941462471663, -0.6689647316224496);
		glVertex3f( 0.75,  0.0, -1.6);
		glVertex3f(-0.75,  0.0, -1.6);
		glVertex3f(-0.75, -1.0/2.0, -1.7);
		glVertex3f( 0.75, -1.0/2.0, -1.7);
		
		// draw back lower face
		glNormal3f(0.0, -0.7071067811865475, -0.7071067811865475);
		glVertex3f(-0.75, -1.0/2.0, -1.7);
		glVertex3f( 0.75, -1.0/2.0, -1.7);
		glVertex3f( 0.75, -0.8, -1.6);
		glVertex3f(-0.75, -0.8, -1.6);

		// draw right face
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.75,  0.0, -1.6);
		glVertex3f( 0.75, -1.0/2.0, -1.7);
		glVertex3f( 0.75, -1.0/2.0, 1.9);
		glVertex3f( 0.75,  0.0, 1.5);
		
		glNormal3f( 1.0,  0.0,  0.0);
		glVertex3f( 0.75, -0.8, 1.7);
		glVertex3f( 0.75, -1.0/2.0, 1.9);
		glVertex3f( 0.75, -1.0/2.0, -1.7);
		glVertex3f( 0.75, -0.8, -1.6);
		// draw left face
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.75,  0.0, -1.6);
		glVertex3f( -0.75, -1.0/2.0, -1.7);
		glVertex3f( -0.75, -1.0/2.0, 1.9);
		glVertex3f( -0.75,  0.0, 1.5);
		
		glNormal3f( -1.0,  0.0,  0.0);
		glVertex3f( -0.75, -0.8, 1.7);
		glVertex3f( -0.75, -1.0/2.0, 1.9);
		glVertex3f( -0.75, -1.0/2.0, -1.7);
		glVertex3f( -0.75, -0.8, -1.6);

		// draw top
		glNormal3f(  0.0, 1.0,  0.0);
		glVertex3f( 0.75,  0.0, 1.5);
		glVertex3f(-0.75,  0.0, 1.5);
		glVertex3f(-0.75,  0.0, -1.6);
		glVertex3f( 0.75,  0.0, -1.6);
		
		// draw bottom
		glNormal3f(  0.0, -1.0,  0.0);
		glVertex3f( 0.75, -0.8, 1.7);
		glVertex3f(-0.75, -0.8, 1.7);
		glVertex3f(-0.75, -0.8, -1.6);
		glVertex3f( 0.75, -0.8, -1.6);
	glEnd();
}

void drawHead_EnemyPlane(){
	glBegin(GL_QUADS);
		// draw front upper face
		glNormal3f(0.0, 0.7432941462471663, 0.6689647316224496);
		glVertex3f( 0.60, 0.6, 0.8);
		glVertex3f(-0.60, 0.6, 0.8);
		glVertex3f(-0.65, 0.4, 1.0);
		glVertex3f( 0.65, 0.4, 1.0);
		
		// draw front mid face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-0.65, 0.4, 1.0);
		glVertex3f( 0.65, 0.4, 1.0);
		glVertex3f( 0.70, 0.2, 1.0);
		glVertex3f(-0.70, 0.2, 1.0);
		
		// draw front lower face
		glNormal3f(0.0, -0.7432941462471663, 0.6689647316224496);
		glVertex3f(-0.70, 0.2, 1.0);
		glVertex3f( 0.70, 0.2, 1.0);
		glVertex3f( 0.75, 0.0, 0.8);
		glVertex3f(-0.75, 0.0, 0.8);
		
		// draw back face
		glNormal3f(0.0, 0.7432941462471663, -0.6689647316224496);
		glVertex3f(-0.60,  0.6, -0.8);
		glVertex3f( 0.60,  0.6, -0.8);
		glVertex3f( 0.75,  0.0, -1.0);
		glVertex3f(-0.75,  0.0, -1.0);

		// draw left face
		glNormal3f( -0.6689647316224496, 0.7432941462471663, 0.0);
		glVertex3f( -0.60, 0.6, 0.8);
		glVertex3f( -0.65, 0.4, 1.0);
		glVertex3f( -0.70, 0.2, 1.0);
		glVertex3f( -0.75, 0.0, 0.8);
		
		glNormal3f( -0.6689647316224496, 0.7432941462471663, 0.0);
		glVertex3f( -0.60, 0.6, 0.8);
		glVertex3f( -0.60, 0.6, -0.8);
		glVertex3f( -0.75, 0.0, -1.0);
		glVertex3f( -0.75, 0.0, 0.8);

		// draw right face
		glNormal3f( 0.6689647316224496, 0.7432941462471663, 0.0);
		glVertex3f( 0.60, 0.6, 0.8);
		glVertex3f( 0.65, 0.4, 1.0);
		glVertex3f( 0.70, 0.2, 1.0);
		glVertex3f( 0.75, 0.0, 0.8);
		
		glNormal3f( 0.6689647316224496, 0.7432941462471663, 0.0);
		glVertex3f( 0.60, 0.6, 0.8);
		glVertex3f( 0.60, 0.6, -0.8);
		glVertex3f( 0.75, 0.0, -1.0);
		glVertex3f( 0.75, 0.0, 0.8);
		
		// draw top
		glNormal3f(0, 1, 0);
		glVertex3f( 0.60, 0.6, 0.8);
		glVertex3f(-0.60, 0.6, 0.8);
		glVertex3f(-0.60, 0.6, -0.8);
		glVertex3f( 0.60, 0.6, -0.8);

		// draw bottom
		glNormal3f(0, -1, 0);
		glVertex3f( 0.75, 0.0, -1.0);
		glVertex3f(-0.75, 0.0, -1.0);
		glVertex3f(-0.75, 0.0, 0.8);
		glVertex3f( 0.75, 0.0, 0.8);
	glEnd();
}

void drawFender_EnemyPlane(){
	glPushMatrix();
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.45, 0.02, 2.8);
		
		//Front Up
		glTranslatef(0, 0, 1.4);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.25, 0.02, 0.1);
	glPopMatrix();
}

void drawTrack_EnemyPlane(){
	glPushMatrix();
		//Upside
		glTranslatef(0.13, 0, 0.2);
		drawCube(0.5, 0.02, 3.3);
		
		//Front Up
		glTranslatef(0, 0, 1.65);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
		//Front mid
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
		//Front lower
		glTranslatef(0, 0, 0.05);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.5, 0.02, 0.25);
		
		//Front bottom
		glTranslatef(0, 0, 0.125);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
		//Bottom
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 1.5);
		drawCube(0.5, 0.02, 3);
		
		//Back bottom
		glTranslatef(0, 0, 1.5);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
		//Back lower
		glTranslatef(0, 0, 0.05);
		glRotatef(22.5, 1, 0, 0);
		glTranslatef(0, 0, 0.125);
		drawCube(0.5, 0.02, 0.25);
		
		//Back mid
		glTranslatef(0, 0, 0.125);
		glRotatef(75, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
		//Back Up
		glTranslatef(0, 0, 0.05);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0, 0, 0.05);
		drawCube(0.5, 0.02, 0.1);
		
	glPopMatrix();
}

void drawGunshild_EnemyPlane(){
	glBegin(GL_QUADS);
		// draw front upper face
		glNormal3f(0.0, 0.7432941462471663, 0.6689647316224496);
		glVertex3f( 0.3, 0.5, 1);
		glVertex3f(-0.3, 0.5, 1);
		glVertex3f(-0.3, 0.4, 1.1);
		glVertex3f( 0.3, 0.4, 1.1);
		
		// draw front mid face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-0.3, 0.4, 1.1);
		glVertex3f( 0.3, 0.4, 1.1);
		glVertex3f( 0.3, 0.3, 1.1);
		glVertex3f(-0.3, 0.3, 1.1);
		
		// draw front lower face
		glNormal3f(0.0, -0.7432941462471663, 0.6689647316224496);
		glVertex3f(-0.3, 0.3, 1.1);
		glVertex3f( 0.3, 0.3, 1.1);
		glVertex3f( 0.3, 0.2, 1);
		glVertex3f(-0.3, 0.2, 1);
		
		// draw up side
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f( 0.3, 0.5, 0.8);
		glVertex3f(-0.3, 0.5, 0.8);
		glVertex3f(-0.3, 0.5, 1);
		glVertex3f( 0.3, 0.5, 1);
		
		// draw down side
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f( 0.3, 0.2, 0.8);
		glVertex3f(-0.3, 0.2, 0.8);
		glVertex3f(-0.3, 0.2, 1);
		glVertex3f( 0.3, 0.2, 1);
		
		// draw right side
		glNormal3f( 1.0, 0.0, 0.0);
		glVertex3f( 0.3, 0.2, 1);
		glVertex3f( 0.3, 0.3, 1.1);
		glVertex3f( 0.3, 0.4, 1.1);
		glVertex3f( 0.3, 0.5, 1);
		
		// draw left side
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-0.3, 0.2, 1);
		glVertex3f(-0.3, 0.3, 1.1);
		glVertex3f(-0.3, 0.4, 1.1);
		glVertex3f(-0.3, 0.5, 1);
	glEnd();
}

void drawShell1(){
	glPushMatrix();
		glTranslatef(joint_ui_data->getDOF(Keyframe::SHELL1_TRANSLATE_X),
					joint_ui_data->getDOF(Keyframe::SHELL1_TRANSLATE_Y),
					joint_ui_data->getDOF(Keyframe::SHELL1_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL1_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL1_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL1_ROTATE_Z), 0.0, 0.0, 1.0);
		glColor3f(0.8,0.8,0.8);
		drawCube(0.07, 0.07, 0.4);
	glPopMatrix();
}

void drawShell2(){
	glPushMatrix();
		glTranslatef(joint_ui_data->getDOF(Keyframe::SHELL2_TRANSLATE_X),
					joint_ui_data->getDOF(Keyframe::SHELL2_TRANSLATE_Y),
					joint_ui_data->getDOF(Keyframe::SHELL2_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL2_ROTATE_Y), 0.0, 1.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL2_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::SHELL2_ROTATE_Z), 0.0, 0.0, 1.0);
		glColor3f(0.8,0.8,0.8);
		drawCube(0.07, 0.07, 0.4);
	glPopMatrix();
}
// COMMON PARTS

void drawcy(float r1, float r2, float length, int num){
	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		IDquadric = gluNewQuadric();
		gluCylinder(IDquadric,r1,r2,length,num,1);
		drawCircle(r1,num, 0);
		glTranslatef(0, 0, length);
		drawCircle(r2,num, 1);
	glPopMatrix();
}

void drawCircle(float r, int num, int is_front){
    glBegin(GL_POLYGON);
    if(is_front == 1){
		glNormal3f(0,0,1);
	}
	else{
		glNormal3f(0,0,-1);
	}
    for(int i = 0; i < num; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num);//get the current angle

        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component
        glVertex3f(x, y, 0);//output vertex

    }
    glEnd();
}

void drawCube(float x, float y, float z){
	glBegin(GL_QUADS);
		// draw front face
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(-x / 2.0, -y / 2.0, z / 2.0);
		glVertex3f( x / 2.0, -y / 2.0, z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0, z / 2.0);
		glVertex3f(-x / 2.0,  y / 2.0, z / 2.0);

		// draw back face
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f( x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f(-x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f(-x / 2.0,  y / 2.0, -z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0, -z / 2.0);

		// draw left face
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(-x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f(-x / 2.0, -y / 2.0,  z / 2.0);
		glVertex3f(-x / 2.0,  y / 2.0,  z / 2.0);
		glVertex3f(-x / 2.0,  y / 2.0, -z / 2.0);

		// draw right face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f( x / 2.0, -y / 2.0,  z / 2.0);
		glVertex3f( x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0, -z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0,  z / 2.0);

		// draw top
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-x / 2.0,  y / 2.0,  z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0,  z / 2.0);
		glVertex3f( x / 2.0,  y / 2.0, -z / 2.0);
		glVertex3f(-x / 2.0,  y / 2.0, -z / 2.0);

		// draw bottom
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f( x / 2.0, -y / 2.0, -z / 2.0);
		glVertex3f( x / 2.0, -y / 2.0,  z / 2.0);
		glVertex3f(-x / 2.0, -y / 2.0,  z / 2.0);
	glEnd();
}







// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( button == GLUT_RIGHT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateCamZPos = true;
		}
		else
		{
			updateCamZPos = false;
		}
	}
}

// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( updateCamZPos )
	{
		// Update camera z position
		camZPos += (x - lastX) * ZOOM_SCALE;
		lastX = x;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}

///////////////////////////////////////////////////////////
//
// BELOW ARE FUNCTIONS FOR GENERATING IMAGE FILES (PPM/PGM)
//
///////////////////////////////////////////////////////////

void writePGM(char* filename, GLubyte* buffer, int width, int height, bool raw=true)
{
	FILE* fp = fopen(filename,"wt");

	if( fp == NULL )
	{
		printf("WARNING: Can't open output file %s\n",filename);
		return;
	}

	if( raw )
	{
		fprintf(fp,"P5\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			fwrite(&buffer[y*width],sizeof(GLubyte),width,fp);
			/*
			for(int x=0;x<width;x++)
			{
				fprintf(fp,"%c",int(buffer[x+y*width];
			}
			*/
		}
	}
	else
	{
		fprintf(fp,"P2\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				fprintf(fp,"%d ",int(buffer[x+y*width]));
			}
			fprintf(fp,"\n");
		}
	}

	fclose(fp);
}

#define RED_OFFSET   0
#define GREEN_OFFSET 1
#define BLUE_OFFSET  2

void writePPM(char* filename, GLubyte* buffer, int width, int height, bool raw=true)
{
	FILE* fp = fopen(filename,"wt");

	if( fp == NULL )
	{
		printf("WARNING: Can't open output file %s\n",filename);
		return;
	}

	if( raw )
	{
		fprintf(fp,"P6\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				GLubyte* pix = &buffer[4*(x+y*width)];

				fprintf(fp,"%c%c%c",int(pix[RED_OFFSET]),
									int(pix[GREEN_OFFSET]),
									int(pix[BLUE_OFFSET]));
			}
		}
	}
	else
	{
		fprintf(fp,"P3\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				GLubyte* pix = &buffer[4*(x+y*width)];

				fprintf(fp,"%d %d %d ",int(pix[RED_OFFSET]),
									   int(pix[GREEN_OFFSET]),
									   int(pix[BLUE_OFFSET]));
			}
			fprintf(fp,"\n");
		}
	}

	fclose(fp);
}

void writeFrame(char* filename, bool pgm, bool frontBuffer)
{
	static GLubyte* frameData = NULL;
	static int currentSize = -1;

	int size = (pgm ? 1 : 4);

	if( frameData == NULL || currentSize != size*Win[0]*Win[1] )
	{
		if (frameData != NULL)
			delete [] frameData;

		currentSize = size*Win[0]*Win[1];

		frameData = new GLubyte[currentSize];
	}

	glReadBuffer(frontBuffer ? GL_FRONT : GL_BACK);

	if( pgm )
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_LUMINANCE, GL_UNSIGNED_BYTE, frameData);
		writePGM(filename, frameData, Win[0], Win[1]);
	}
	else
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_RGBA, GL_UNSIGNED_BYTE, frameData);
		writePPM(filename, frameData, Win[0], Win[1]);
	}
}
