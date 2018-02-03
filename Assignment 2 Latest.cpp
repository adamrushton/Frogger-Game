/* Adam James Rushton G20700507
   Frogger Assignment           */

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>      // Writing to game screen
#include <vector>	    // Dynamic Arrays - Better than standard arrays if values in the sequence need changing
#include <time.h>	    // Used for random number generating and timers
#include <fstream>		// Read/write to files
#include <iostream>     // Display text on the console
#include <iomanip>      // Printing highscores neatly
#include <algorithm>    // Sort vectors, highscores in this program

// Namespaces
using namespace tle; // T-L Engine definitions (I guess)
using namespace std; // Standard definitions

// Structures
struct AddScore
{   // Group names and score
	string name;
	int score;
};
struct By_Score
{   // Sort by score (highest at the top)
	bool operator()(AddScore const &a, AddScore const &b)
	{
		return a.score > b.score; // Sort largest to smallest (largest at the top)
	}
};
// Classes 
class Countdown
{
private:
	clock_t ticks;			    // Number of clock ticks
	clock_t timeLeft;	   	    // Time Remaining
	clock_t totalTimeRemaining;	// Total time remaining
	bool isCountingDown;	    // Check to see if timer is counting down
public:
	int startingTime; // Store starting time to reset it
	Countdown(int time) // Countdown constructor 
	{
		isCountingDown = false;
		startingTime = time;
		timeLeft = time;
		ticks = -clock();
		totalTimeRemaining;
	}
	void stopCountdown() // Stop the count down
	{
		isCountingDown = false;
		totalTimeRemaining = -clock() - ticks;
	}
	void startCountdown() // Start the count down
	{
		ticks = -clock() - timeLeft;
		isCountingDown = true;
	}
	void initalize()
	{
		startCountdown();
		stopCountdown();
	}
	void resetCountdown() // Reset the count down
	{
		timeLeft = startingTime;
	}
	bool countingDown()
	{
		return isCountingDown;
	}
	int getTimeRemaining()
	{
		if (isCountingDown) timeLeft = -clock() - ticks;
		if (timeLeft > 0) return int(timeLeft) / CLOCKS_PER_SEC; // Return time remaining
		else return 0; // Time UP	
	}
};

// -------------------------------------------------------------------------------------
// Enum setup
enum carTypes { Audi, Caprice, CaddyEldorado, Jeep, Estate, Transit, Rover, RangeRover, PickUp }; // Car names (non-scaled)
enum currentState { MainMenu, Start, Highscores, About, Settings, BindNewKey, Quit };	  		  // Game states
enum currentCarState { CarRotate, CarUp, CarLeft, CarRight, CarDown, CarTimeOut };				  // Car states
enum tyreState { TyreUpLeft, TyreUpRight, TyreDownLeft, TyreDownRight, TyreResetPositions };	  // Tyre states
enum currentFrogState { Leaping, FrogMovingLeft, FrogMovingRight, FrogSinking, FrogSquashing, 
						FrogSafeZoneDeath, FrogStill };											  // Frog states
enum frogStates { Waiting, Crossing, Dead };													  // Frog thinking state
enum frogIdentifier { FirstFrog, SecondFrog, ThirdFrog };										  // Identifying frogs
enum settingsLeftList { PauseText = 2, QuitText, ResetCameraText, TiltUpText };					  // Settings list
enum settingsRightList { RestartLevelText, FrogUpText, FrogDownText, FrogLeftText, 
						 FrogRightText, TiltDownText };											  // Settings list 
enum menuNames
{
	StartButton, HighscoresButton, SettingsButton, AboutButton, QuitButton, BackButton, FullScreenKey, FPSCounterKey, 
	PauseKey, QuitKey, ResetCameraKey, TopViewKey, RestartLevelKey, FrogForwardsKey, FrogBackwardsKey, FrogLeftKey, FrogRightKey, 
	DownViewKey, CancelButton, ResetButton, Highscore1, Highscore2, Highscore3, Highscore4, Highscore5
}; // Buttons and Game keys
   /* Main menu and text settings */
const int optionsAmt = 25;			  // Amount of text to be highlighted.
const int hoverColour = 0xFFFF7400;   // 16-bit Hex Colour - Highlighting options colour
const int defaultColour = 0xFFFFED08; // 16-bit Hex Colour - Text colour for everything else
const int titleColour = 0xFFDACCFF;	  // 16-bit Hex Colour - Title text colour
const int titleFontSize = 100;		  // Font size for all page titles
const int optionFontSize = 50;		  // Font size for options
const int aboutFontSize = 30;		  // Font size for text on about screen
const int titleTextX = 405;			  // X Position for page titles for each screen
const int titleTextY = 35;			  // Y Position for page titles for each screen
int widthOfText[optionsAmt];		  // Measure width of all the options
int heightOfText[optionsAmt];		  // Measure height of all the options
int optionMinX[optionsAmt];			  // Lower boundary X (For bounding a box around the text, applies to all options)
int optionMaxX[optionsAmt];			  // Upper boundary X (For bounding a box around the text, applies to all options)
int optionMinY[optionsAmt];			  // Lower boundary Y (For bounding a box around the text, applies to all options)
int optionMaxY[optionsAmt];		  	  // Upper boundary Y (For bounding a box around the text, applies to all options)
int textPosX[optionsAmt];		  	  // Current Text Position on X-Axis for all options
int textPosY[optionsAmt];			  // Current Text Position on Y-Axis for all options
int toggledColour[optionsAmt];		  // Toggled colour, applies to all options
int currentLine = 0;				  // Position on the screen
									  // All text listings for the menu pages
bool keyInUse = false;			      // Store key in use or not
const int mainMenuOptions = 5;		  // Amount of options on the Main Menu
const int settingsLeft = 6;			  // Number of settings on the left side
const int settingsRight = 6;		  // Number of settings on the right side
const int leftSettingsStart = 6;	  // Starting number for left settings
const int leftSettingsEnd = 12;	      // End of settings
const int rightSettingsStart = 12;    // Starting number for right settings
const int rightSettingsEnd = 18;      // Ending number for right settings
string keyCodeNames[kMaxKeyCodes] =
{
	"0", "L Button", "R Button", "3", "M Button", "X Button1", "X Button2", "7", "Back", "Tab", "10", 
	"11", "Clear", "Return", "14", "15", "Shift", "Control", "Menu", "P", "C-L", "21", "22", "23", "24", 
	"25", "26", "Escape", "Conv", "Non-conv", "Accept", "ModeChange", "Space", "Prior", "Next", "End", "Home",
	"Left", "Up", "Right", "Down", "Select", "Print", "Execute", "Snapshot", "Insert", "Delete", "Help", 
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9","58", "59", "60", "61", "62", "63", "64", "A", "B", 
	"C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W","X", "Y", "Z", 
	"LWin", "RWin", "Apps", "94", "Sleep", "NP 0", "NP 1", "NP 2", "NP 3", "NP 4", "NP 5", "NP 6", "NP 7", "NP 8", "NP 9", 
	"*", "+","Seperator", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", 
	"F14", "F15", "F16", "F17", "F18", "F19","F20", "F21", "F22", "F23", "F24", "136", "137", "138", "139", "140", "141", 
	"142", "143", "N-Lock", "S-Lock", "144", "145", "146", "147", "148", "149", "150","151", "152", "153", "154", "155", 
	"156", "157", "LShift", "RShift", "LControl", "RControl", "LMenu", "RMenu", "<-", "->", "168", "169", "170", "Star",
	"172" ,"Mute", "- Vol", "+ Vol", "FastFwd", "177", "178", "Play/Pause", "180", "181", "182", "CalcKey", "184", "185", 
	";", "Plus", "Comma", "Minus", "Period", "/", "'", "193", "194", "195", "196", "197", "198", "199", "200", "201", "202",
	"203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214","215", "216", "217", "218", "[",
	"BackSlash", "]", "#", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236",
	"237", "238", "239", "240", "241", "242", "243", "244", "245", "Attn", "CrSel", "ExSel", "ErEof", "Play", "Zoom", "252", 
	"PA1", "Clear", "255"
}; // My dictionary of all game keys
// Main menu options, left and right settings
string mainMenuList[mainMenuOptions] = { "Start", "Highscores", "Settings", "About", "Quit" };											
string leftSettingsList[settingsLeft] = { "Full Screen:", "FPS Toggle:", "Pause:", "Back/Quit:", "Reset Camera:", "Tilt up:" };			
string rightSettingsList[settingsRight] = { "Restart Level:", "Frog Up: ", "Frog Down: ", "Frog Left:", "Frog Right: ", "Tilt down:" }; 

/* Sprite details */
bool pauseCreated = false;                             // Pause sprite creation state
const float pauseSpriteX = 400.0f;					   // X to create the pause sprite
const float pauseSpriteY = 125.0f;					   // Y to create the pause sprite
const string mainAboutBG = "menu/mainAboutBGFS.jpg";   // Locating the main menu and about screen background
const string highscoresBG = "menu/highscoresBGFS.jpg"; // Locating the highscores background sprite
const string settingsBG = "menu/settingsBGFS.jpg";     // Locating the settings background sprite
const string pauseName = "menu/pause.png";			   // Locating the pause sprite
ISprite* backdrop;									   // Background sprite
ISprite* pause;										   // Pause sprite
/* Tree details */
bool treeCollision = false;							   // Decide if frog has collided with tree
const float treeSize = 2.0f;						   // Tree scale size
const float treeRadius = 2.49749f * treeSize;		   // Tree width 
const float treeSpacing = 20.0f;					   // Tree spacing (trees at the end of the 2nd island)
const int endTreeAmt = 6;							   // Number of trees at the final island
const int totalNumOfTrees = 10;						   // Total number of trees
													   /* Bounding box declaration for trees */
float treeXBoundary = 50.0f;						   // Start X for creation of the trees
float treeMinX[totalNumOfTrees];					   // Tree Min X 
float treeMaxX[totalNumOfTrees];					   // Tree Max X
float treeMinZ[totalNumOfTrees];					   // Tree Min Z
float treeMaxZ[totalNumOfTrees];					   // Tree Max Z
/* Tyre details */
const int tyreAmount = 12;							   // 4 lanes of 3 tyres in each lane
const float tyreLane1MinZ = 70.0f;					   // Tyre lane 1 Z lower boundary 
const float tyreLane1MaxZ = 80.0f;					   // Tyre lane 1 Z upper bounary 
const float tyreLane2MinZ = 90.0f;					   // Tyre lane 2 Z lower boundary
const float tyreLane2MaxZ = 100.0f;					   // Tyre lane 2 Z upper boundary
const float tyreLane3MinZ = 80.0f;					   // Tyre lane 3 lower boundary
const float tyreLane3MaxZ = 90.0f;				       // Tyre lane 3 upper boundary
const float tyreLane4MinZ = 100.0f;					   // Tyre lane 4 lower boundary
const float tyreLane4MaxZ = 110.0f;					   // Tyre lane 4 upper boundary
const float frogITZMinZ = 72.0f;					   // Frog in tyre zone min Z
const float frogITZMaxZ = 110.0f;					   // Frog in tyre zone max Z
const float tyreDrowned = -10.0f;					   // Starting Level for tyres
const float tyreGroundLevel = -3.0f;				   // Tyre max Y
const float tyreSize = 10.0f;						   // Tyre scaling size
const float tyreSpeed = 0.25f;						   // Speed of tyres
const float tyreRadius = 0.45f * tyreSize;			   // Tyre radius calculation
const float tyreDiameter = tyreRadius * 2;			   // Tyre diameter calculation
IModel* tyre[tyreAmount];							   // Array storing number of tyres
int tyreState[tyreAmount];							   // Array of Directions for each tyre
int numOfCollisions = 0;							   // Count number of collisions
/* Bounding box for tyre */
float tyreMinX[tyreAmount];							   // Tyre Min X
float tyreMaxX[tyreAmount];							   // Tyre Max X 
float tyreMinZ[tyreAmount];							   // Tyre Min Z
float tyreMaxZ[tyreAmount];							   // Tyre Max Z
/* Checkpoint details */
const float checkPointRotation = 90.0f;				   // Rotation angle for checkpoint model
const float checkPointSize = 5.0f;					   // Checkpoint scale
IModel* checkPoint;									   // Model for checkpoint
/* User details */
bool onlyWhiteSpace = false;						   // See if user only enters white spaces as username
bool validUserName = false;							   // Valid username on load up
const int maxNumOfChars = 10;						   // Number of characters allowsed in username
string userName;									   // Player name
string userNameInfo;								   // Text next to player name
/* Game details */
bool gamePaused = false;							   // Game state
bool gameOver = false;								   // Decide if game over
bool inGame = false;								   // Ingame state 
const float floatZero = 0.0f;						   // Float equivalent to 0
const float sunkLevel = -10.0f;						   // Height of object before hidden
const float groundLevel = 0.0f;						   // Ground level for frog, trees and checkpoint
const float slightlyRisen = 1.0f;					   // Slightly rise objects off the round
const float kGameSpeed = 0.05f;						   // Game speed 
const float waterAndIslandFloor = -5.0f;			   // Ocean level
const float slopeAngle = 10.0f;						   // Slope rotation
const float slopeScale = 0.1f;						   // Flatten Slope
const float maxLeftBoundary = -55.0f;				   // Lower boundary X axis
const float maxRightBoundary = 55.0f;				   // Upper boundary X axis
const int twentyPoints = 20;						   // Number of points
int gameLives = 3;									   // Total Game Lives
int gameScore = 0;									   // Game Score, start at 0
int displayScore;									   // Showing score on screen after game finished
int tempScore = 0;									   // Temp score while loading highscores
int gameLevel = 1;									   // Game Level, start at 1
float speedModifier = gameLevel * kGameSpeed;		   // Speed modifier for tyre
/* Frog details */
const int frogAmount = 3;							   // Number of frogs
bool frogInTyreZone = false;						   // Decide if frog in tyre zone
bool frogOnTyre = false;							   // Decide if frog on a tyre
bool frogLeaping = false;							   // Has the frog started leaping yet
bool movingLeft = false;							   // Check if frog is on the moving left tyres
bool movingRight = false;							   // Check if frog is on the moving right tyres
const float kPi = 3.142f;							   // Storing Pi, used in frog jumping
const float framesPassed = 200.0f;					   // 0.1 (kgamespeed) * 1.0 (frog speed) = 100.0f
const float frogAngle = 90.0f;						   // Frog facing direction angle
const float sinkingSpeed = 1.0f;					   // Sinking Speed
const float frogSpeed = 1.0f;						   // Speed of Frog
const float frogMultiplier = 0.1f;					   // Frog jump multiplier
const float frogSize = 0.5f;						   // Frog scale size
const float frogRadius = 5.04487f * frogSize;		   // Frog radius
const float frogDiameter = frogRadius * 2;			   // Frog diameter
const float frogStartX = -10.0f;					   // Frog inital starting position (X-axis)
const float frogStartZ = 15.0f;						   // Frog inital starting position (Z-axis)
const float frogMaxLeft = -40.0f;					   // Far Left can move to 
const float frogMaxRight = 40.0f;					   // Far Right can move to
const float squashedFrog = 0.1f;					   // Squashed frog
const float unSquashFrog = squashedFrog * 100.0f;	   // Unsquash frog
const float levelUpMinZ = 115.0f;					   // Level up Z lower boundary
const float levelUpMazZ = 125.0f;					   // Level up Z upper boundary
const int frogTimer = 30000;						   // Time limit for frog to get to other side (measured in miliseconds)
const int fullLeap = 4;
float frogLoc;										   // Position calculation (change name)
float frogNewHeight;								   // Storing sin calculation
float lastX[frogAmount];							   // Store X before jump
float lastY[frogAmount];						       // Store Y before jump
float lastZ[frogAmount];							   // Store Z before jump
float biggestZ = frogStartZ + frogRadius;			   // Store the maximum position reached this level
int currentFrog = 0;								   // Current Frog
int secs[frogAmount];								   // Time left for each frog 
int frogState = FrogStill;							   // Frog moving state
int frogStateTracker[frogAmount];					   // Frog game state
IModel* frog[frogAmount];							   // Model for each frog
Countdown* cdFrog[frogAmount];						   // Create a countdown for all of the frogs
/* Camera details */
const float cameraMove = 10.0f;						   // How many steps to move the camera forwards / backwards by
const float cameraLookDown = 20.0f;					   // Angle Manual Counter tilts down at
const float cameraX = 0.0f;							   // Default X position
const float cameraHeight = 60.0f;					   // Default height position
const float cameraZ = -160.0f;						   // Default Z position
const float cameraRotateAmount = 0.2f;				   // Number to rotate camera each game loop
const float rotateLimitForwards = 90.0f;			   // Camera Rotation Upper bound limit
const float rotateLimitBackwards = 0.0f;			   // Camera Rotation Lower bound limit
float cameraRotating = 0.0f;						   // Angle of rotation start
ICamera* myCamera;									   // Declare camera name
IModel* dummy;										   // Dummy model to attach frog and camera to
/* Car details */
const int carAmount = 8;							   // Number of cars
const int totalCarTypes = 9;						   // Number of different car types
const int scaledAmount = 5;							   // Total custom cars
bool frogInCarZone;									   // Check if frog in car zone
bool carCollision;									   // Decide if Collision between frog and car
bool canRotate[carAmount];							   // Stores state to which cars can rotate their wheels
const float carSunkLevel = -15.0f;					   // Sunk level for cars
const float fICZMinZ = 20.0f;						   // Frog in car zone min Z
const float fICZMaxZ = 60.0f;						   // Frog in car zone max Z
const float carScale = 3.0f;						   // Scale amount for the smaller cars
const int estateCarNumber = 4;						   // Estate car number 
const int carTime = 2000;							   // Car Timer	(measured in miliseconds)							
const int normalCarSizeStartNum = 5;				   // Model number (upwards) where they do not require scaling
float carLengthRadius[carAmount];					   // Length of each car
float carWidthRadius[carAmount];					   // Width of each careach car
float moveSpeed[carAmount];							   // Create a movement speed for each car
int carState[carAmount];							   // Directions for each car
int randomCar = 0;									   // Random number store for cars custom cars
Countdown* cdCar[carAmount]; 						   // Countdown for each car
IModel* car[carAmount];								   // Create so many cars
/* Car number lanes */
const int lane1Start = 0;							   // Lane 1 cars (0 and 1)
const int lane2Start = 2;							   // Lane 2 cars (2 and 3)
const int lane3Start = 4;							   // Lane 3 cars (4 and 5)
const int lane4Start = 6;							   // Lane 4 cars (6 and 7)
const int lane4End = 8;								   // End of final lane
// Starting lane speeds. Increased based on level.
const float lane1Speed = 0.2f;						   // Lane 1 car speed
const float lane2Speed = 0.4f;					       // Lane 2 car speed
const float lane3Speed = lane2Speed*1.5f;			   // Lane 3 car speed
const float lane4Speed = lane2Speed*2.0f;			   // Lane 4 car speed
const float carSpacing = 50.0f;						   // Initial spacing between each car
const float carRotation = 90.0f;					   // Angle to face when created
const float carXSlopeRight = 90.0f;					   // X axis start of slope
const float carXSlopeLeft = -90.0f;					   // X axis start of slope
const float generalRotationSpeed = 40.0f;			   // Car wheel rotation speed
const int frontLeftWheelNodeNumber = 4;			       // Car wheel node
const int frontRightWheelNodeNumber = 5;			   // Car wheel node
const int backLeftWheelNodeNumber = 6;				   // Car wheel node
const int backRightWheelNodeNumber = 7;				   // Car wheel node
const int estateFrontLeftWheelNodeNumber = 3;	       // Estate wheel node
const int estateFrontRightWheelNodeNumber = 4;		   // Estate wheel node
const int estateBackLeftWheelNodeNumber = 5;	       // Estate wheel node
const int estateBackRightWheelNodeNumber = 6;		   // Estate wheel node
// Car node declaration - store the nodes for each cars wheels
ISceneNode* frontLeftWheelNode[carAmount];  // Rotating car wheels (front left)
ISceneNode* frontRightWheelNode[carAmount]; // Rotating car wheels (front right)
ISceneNode* backRightWheelNode[carAmount];  // Rotating car wheels (back right)
ISceneNode* backLeftWheelNode[carAmount];   // Rotating car wheels (back left)
// Rectangle around car
float carMinX[carAmount];
float carMaxX[carAmount];
float carMinZ[carAmount];
float carMaxZ[carAmount];
/* Highscores details */
const char spaceFiller = ' ';					  // Space empty gap with space
const int numOfScores = 5;						  // Number of highscores saved
const int posSpacing = 6;						  // Number of spaces after Position
const int nameSpacing = 35;						  // Number of spaces after Name
/* All Text displays on screen (stringstreams) */
stringstream fpsText;							  // Game FPS
stringstream scoreText;							  // Game Score 
stringstream livesText;							  // Lives Remaining 
stringstream levelText;							  // Game Level
stringstream fullScreenStateText;				  // Full screen
stringstream fpsToggleStateText;				  // FPS Toggle
stringstream timeElapsedText;					  // Time Remaining
stringstream userNameText;						  // Store display username
stringstream top5PosText[numOfScores];		      // Draw position for each person
stringstream top5NameText[numOfScores];			  // Draw names for each person
stringstream top5ScoreText[numOfScores];		  // Draw score for each person
stringstream posNameScoreText;					  // Headers for the display table
stringstream currentKeyText;                      // When user presses a key, display it to the screen
stringstream mainMenuText[mainMenuOptions];		  // Options on the main menu
stringstream allFrogStates[frogAmount];
stringstream gameOverScore;						  // Display score when game over
/* Player details */
const int lastPlace = 4;						  // Store last place number
const int scoreAmount = 5;						  // Total number of scores	
AddScore p[scoreAmount];						  // Array of scores to add
int playerScore[scoreAmount];					  // Array of player scores
string playerName[scoreAmount];					  // Array of player names
/* FPS details */
float frames;
float nextUpdate;
float overtime;
float fps;
/* Button names */
string backName = "Back";     // Back button, used throughout the menus. show either Back or Save(settings changing) 
string cancelName = "Cancel"; // Cancel, used on settings
string resetName = "Reset";   // Reset, used on highscores and settings
/* About Us Text Positions */
const int textLine1X = 430; // X position of text on about page
const int textLine1Y = 280; // Y position of text on about page
const int textLine2X = 310; // X position of text on about page
const int textLine2Y = 400; // Y position of text on about page
const int textLine3X = 690; // X position of text on about page
const int textLine3Y = 400; // Y position of text on about page
const int textLine4X = 180; // X position of text on about page
const int textLine4Y = 560; // Y position of text on about page
/* Settings page */
int currentKey;			     					  // Store current key hit
string keyName = "";							  // Store key name
string tempCurrentKeyName = "";					  // Current key hit display
string notInUseKeyName;							  // Store if not in use
string keyChangeName;							  // Store which key they clicked on
string currentGameKeysLeft[settingsLeft];		  // Game keys list (left side)
string currentGameKeysRight[settingsRight];		  // Game keys list (right side)
bool fullScreen = false;						  // Toggle Full Screen
bool fpsCounter = false;						  // Toggle FPS Counter
/* Store into string, to be converted into int */
string currentQuitKey;
string currentPauseKey;
string currentResetCamera;
string currentFrogForwards;
string currentFrogBackwards;
string currentFrogLeft;
string currentFrogRight;
string currentViewTiltUp;
string currentViewTiltDown;
string currentRestartLevel;
/* Key numbers */
bool changingKey = false;
int pauseKeyNumber;
int quitKeyNumber;
int resetCamKeyNumber;
int viewTiltUpNumber;
int restartLevelNumber;
int frogForwardsNumber;
int frogBackwardsNumber;
int frogLeftNumber;
int frogRightNumber;
int viewTiltDownNumber;
/* Files to read and write to */
string top5scores = "scores.dat";
string gameSettings = "settings.dat";
string loadFullScreen;
string toggleFpsCounter;
ofstream saveGameSettings;	  // Write to file
ofstream saveHighscores;      // Write to file
ifstream loadData;			  // Read and load settings from file
ifstream loadHighscores;	  // Read and load Highscores
/* Declare game keys and temp keys */
EKeyCode buttonClicker = Mouse_LButton; // None changable key
EKeyCode restartGameKey = Key_Space;	// None changable key
EKeyCode pauseGameKey;		  // Pause game
EKeyCode quitKey;			  // Quit program
EKeyCode resetCamera;		  // Reset position of camera 
EKeyCode viewTiltUp;		  // Look up camera 
EKeyCode restartLevel;		  // Restart level
EKeyCode frogForwards;		  // Jump Forwards
EKeyCode frogBackwards;		  // Jump Backwards
EKeyCode frogLeft;			  // Jump Left
EKeyCode frogRight;			  // Jump Right
EKeyCode viewTiltDown;		  // Tilt down
EKeyCode tempPauseGameKey;    // Temp Pause game
EKeyCode tempQuitKey;		  // Temp Quit program
EKeyCode tempResetCamera;     // Temp Reset position of camera 
EKeyCode tempViewTiltUp;	  // Temp Look up camera 
EKeyCode tempRestartLevel;	  // Temp Restart level
EKeyCode tempFrogForwards;    // Temp Jump Forwards
EKeyCode tempFrogBackwards;   // Temp Jump Backwards
EKeyCode tempFrogLeft;	      // Temp Jump Left
EKeyCode tempFrogRight;		  // Temp Jump Right
EKeyCode tempViewTiltDown;	  // Temp Tilt down

/* Function declarations */
void QuitGame(I3DEngine* myEngine);
int GameDetails(I3DEngine* myEngine, int gameState, IFont* optionFont);
int LoadMainMenu(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont);
int AboutGame(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* aboutFont, IFont* optionFont);
int GameSettings(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont);
int EnterNewKey(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont);
int GameScores(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont);
int ButtonHandler(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* optionFont);
ISprite* LeaveGame(I3DEngine* myEngine, int gameState);
void LoadHS();					// Loading highscores from file
void SaveHS();					// Saving highscores to file
void LoadSettings();			// Loading settings from file
void SaveFile();				// Saving settings to file
void LoadFrogStates();			// First frog crossing, second and third waiting
void SetCarSpeed(int &i);		// Assigning car speed each level
bool CollisionWithCar(int &i);  // Collision between frog and cars
bool CollisionWithTree(int &i); // Collision between frog and trees
bool ContactWithTyre(int &i);   // Contact between frog and tyres

/* Main setup */
void main()
{
	system("Title Adam Rushton - Frogger Console"); // Set TL Engine Title
	LoadHS();		// Attempt to load the highscores file
	LoadSettings(); // Attempt to load the settings file

	while (userName.empty() || userName.length() > maxNumOfChars || onlyWhiteSpace)
	{   
		cout << "Please enter a username (between 1 and 10 characters): ";
		getline(cin, userName);
		(userName.find_first_not_of(' ') == string::npos) ? onlyWhiteSpace = true : onlyWhiteSpace = false; 
	}

	// Create a 3D engine (using TLX engine here)
	I3DEngine* myEngine = New3DEngine(kTLX);
	(!fullScreen) ? myEngine->StartWindowed() : myEngine->StartFullscreen();	// Load in full screen or windowed
	myEngine->SetWindowCaption("Adam Rushton - Frogger");						// Set console window title
	myEngine->AddMediaFolder("./media");									    // Add media folder
	IFont* titleFont = myEngine->LoadFont("stencil std", titleFontSize);        // Stencil std font
	IFont* optionFont = myEngine->LoadFont("Monotype Corsiva", optionFontSize); // Monotype Corsiva font
	IFont* aboutFont = myEngine->LoadFont("Monotype Corsiva", aboutFontSize);   // Monotype Corsiva font
	IMesh* frogMesh = myEngine->LoadMesh("frog/frog.x");						// Load frog media
	for (int i = 0; i < frogAmount; i++)
	{
		frog[i] = frogMesh->CreateModel(frogStartX + (i*frogDiameter), groundLevel, frogStartZ); // Starting point
		frog[i]->Scale(frogSize);
		cdFrog[i] = new Countdown(frogTimer); // Create new countdown timer for frog
	}
	for (int i = 0; i < carAmount; i++) cdCar[i] = new Countdown(carTime); // Create a two second timer for each car

	LoadFrogStates(); // Load Starting frog variable states
	IMesh* dummyMesh = myEngine->LoadMesh("dummy.x");
	dummy = dummyMesh->CreateModel();
	// Sky
	IMesh* skyMesh = myEngine->LoadMesh("sky/Skybox.x");
	IModel* sky = skyMesh->CreateModel(floatZero, -1000.0f, floatZero);
	// Floor - water
	IMesh* floorMesh = myEngine->LoadMesh("ground/surface.x");
	IModel* surface = floorMesh->CreateModel(floatZero, waterAndIslandFloor, floatZero);
	// Island 1
	IMesh* island1Mesh = myEngine->LoadMesh("ground/island1.x");
	IModel* island1 = island1Mesh->CreateModel(floatZero, waterAndIslandFloor, 40.0f);
	// Slope 1
	IModel* slopeLeftSide = island1Mesh->CreateModel(-109.0f, -10.0f, 40.0f);
	slopeLeftSide->ScaleY(slopeScale);
	slopeLeftSide->RotateZ(slopeAngle);
	// Slope 2
	IModel* slopeRightSide = island1Mesh->CreateModel(109.0f, -10.0f, 40.0f);
	slopeRightSide->ScaleY(slopeScale);
	slopeRightSide->RotateZ(-slopeAngle);
	// Island 2
	IMesh* island2Mesh = myEngine->LoadMesh("ground/island2.x");
	IModel* island2 = island2Mesh->CreateModel(floatZero, waterAndIslandFloor, 115.0f);
	// Checkpoint marker
	IMesh* checkPointMesh = myEngine->LoadMesh("checkpoint/checkpoint.x");
	checkPoint = checkPointMesh->CreateModel(frogStartX, slightlyRisen, biggestZ);
	checkPoint->Scale(checkPointSize);
	checkPoint->RotateY(checkPointRotation);
	// TREE Bounding box of geometry = (-2.55367,-0.0745566,-3.63036) to (3.53489,6.85374,2.49749).
	IMesh* treeMesh = myEngine->LoadMesh("trees/plant.x");
	IModel* tree[totalNumOfTrees];
	// Bounding box of geometry = (-2.82654,0.0567475,-6.1216) to (2.82654,5.48442,6.1216).
	IMesh* transitCarMesh = myEngine->LoadMesh("cars/transit.x");
	// Bounding box of geometry = (-2.79333,0.039705,-5.9069) to (2.79333,3.91721,5.9069).
	IMesh* roverCarMesh = myEngine->LoadMesh("cars/rover.x");
	// Bounding box of geometry = (-2.68256,0.0144401,-5.53402) to (2.68256,4.64614,5.53402).
	IMesh* rangeRoverCarMesh = myEngine->LoadMesh("cars/rangeRover.x");
	// Bounding box of geometry = (-2.58098,-0.0245075,-6.14739) to (2.58098,4.41126,6.14739).
	IMesh* pickUpCarMesh = myEngine->LoadMesh("cars/pickUp.x");
	// AUDI Bounding box of geometry = (-1.05776,-2.86102e-006,-2.13928) to (1.05776,1.61014,2.13928).
	IMesh* audiCarMesh = myEngine->LoadMesh("cars/Audi.x");
	// CAPRICE Bounding box of geometry = (-0.926702,0.000792503,-2.59159) to (0.926704,1.5621,2.59159).
	IMesh* capriceCarMesh = myEngine->LoadMesh("cars/Caprice.x");
	// ESTATE Bounding box of geometry = (-0.946118,0.0065695,-1.97237) to (0.946118,1.50131,1.97237).
	IMesh* estateCarMesh = myEngine->LoadMesh("cars/Estate.x");
	// JEEP Bounding box of geometry = (-1.07302,0.00577545,-2.21361) to (1.07302,1.85845,2.21361).
	IMesh* jeepCarMesh = myEngine->LoadMesh("cars/4x4jeep.x");
	// CADDY Bounding box of geometry = (-1.02552,0.0150795,-2.93132) to (0.96917,1.2442,2.93132).
	IMesh* caddyEldoradoCarMesh = myEngine->LoadMesh("cars/Caddy_eldorado.x");
	IMesh* carTypes[totalCarTypes];	// Array of car types
	// Declaring car meshes
	carTypes[Audi] = audiCarMesh;
	carTypes[Caprice] = capriceCarMesh;
	carTypes[CaddyEldorado] = caddyEldoradoCarMesh;
	carTypes[Jeep] = jeepCarMesh;
	carTypes[Estate] = estateCarMesh;
	carTypes[Transit] = transitCarMesh;
	carTypes[Rover] = roverCarMesh;
	carTypes[RangeRover] = rangeRoverCarMesh;
	carTypes[PickUp] = pickUpCarMesh;
	/* Creation of cars */
	for (int i = 0; i < carAmount; i++)
	{
		srand(unsigned(time(NULL) + i));					   // Generate random seed based on current time		
		if (i < lane3Start) randomCar = rand() % scaledAmount; // Generate random model between 0 and 4 for lanes 1 and 2
		else randomCar = rand() % 3 + normalCarSizeStartNum;   // Generate random model between 5 and 8 for lanes 3 and 4
		if (i < lane2Start)									   // Generate random cars for lane 1
		{	
			car[i] = carTypes[randomCar]->CreateModel(carXSlopeRight + (carSpacing*i), waterAndIslandFloor, 25.0f);
		}
		else if (i >= lane2Start && i < lane3Start)
		{  
			car[i] = carTypes[randomCar]->CreateModel(carXSlopeRight + (carSpacing*(i - 2)), waterAndIslandFloor, 35.0f);
		}
		else if (i >= lane3Start && i < lane4Start)
		{	
			car[i] = carTypes[randomCar]->CreateModel(carXSlopeLeft + (-carSpacing*(i - 4)), waterAndIslandFloor, 45.0f);
		}
		else
		{	
			car[i] = carTypes[randomCar]->CreateModel(carXSlopeLeft + (-carSpacing*(i - 6)), waterAndIslandFloor, 55.0f);
		}
		SetCarSpeed(i); // Set all car speeds

		// Properties for each car according to box of geometry
		switch (randomCar)
		{
		case Audi:
			carLengthRadius[i] = 2.13928f * carScale;
			carWidthRadius[i] = 0.946118f * carScale;
			break;

		case Caprice:
			carLengthRadius[i] = 2.59159f * carScale;
			carWidthRadius[i] = 0.926704f * carScale;
			break;

		case CaddyEldorado:
			carLengthRadius[i] = 2.93132f * carScale;
			carWidthRadius[i] = 0.96917f * carScale;
			break;

		case Jeep:
			carLengthRadius[i] = 2.21361f * carScale;
			carWidthRadius[i] = 1.07302f * carScale;
			break;

		case Estate:
			carLengthRadius[i] = 1.97237f * carScale;
			carWidthRadius[i] = 1.05776f * carScale;
			break;

		case Transit:
			carLengthRadius[i] = 6.1216f;
			carWidthRadius[i] = 2.82654f;
			break;

		case Rover:
			carLengthRadius[i] = 5.9069f;
			carWidthRadius[i] = 2.79333f;
			break;

		case RangeRover:
			carLengthRadius[i] = 5.53402f;
			carWidthRadius[i] = 2.68256f;
			break;

		case PickUp:
			carLengthRadius[i] = 6.14739f;
			carWidthRadius[i] = 2.58098f;
			break;
		}
		if (carTypes[randomCar] == estateCarMesh) // Node numbers for estate
		{
			frontLeftWheelNode[i] = car[i]->GetNode(estateFrontLeftWheelNodeNumber);   // Assign front left node
			frontRightWheelNode[i] = car[i]->GetNode(estateFrontRightWheelNodeNumber); // Assign front right node
			backRightWheelNode[i] = car[i]->GetNode(estateBackLeftWheelNodeNumber);    // Assign back left node
			backLeftWheelNode[i] = car[i]->GetNode(estateBackRightWheelNodeNumber);    // Assign back right node
		}
		else if (randomCar < estateCarNumber) // Node numbers for rest of the cars
		{
			frontLeftWheelNode[i] = car[i]->GetNode(frontLeftWheelNodeNumber);   // Assign front left node
			frontRightWheelNode[i] = car[i]->GetNode(frontRightWheelNodeNumber); // Assign front right node
			backRightWheelNode[i] = car[i]->GetNode(backRightWheelNodeNumber);   // Assign back left node
			backLeftWheelNode[i] = car[i]->GetNode(backLeftWheelNodeNumber);     // Assign back right node
		}
		if (randomCar <= estateCarNumber)
		{
			car[i]->Scale(carScale); // Scale the smaller models		
			canRotate[i] = true;     // Allow these cars to rotate
			car[i]->RotateLocalY(-carRotation);
		}
		else car[i]->RotateLocalY(carRotation);
	}
	/* Tyre creation */
	IMesh* tyreMesh = myEngine->LoadMesh("tyres/tyre.x");
	for (int i = 0; i < tyreAmount; i++)
	{
		if (i < 3)	tyre[i] = tyreMesh->CreateModel(maxRightBoundary + i * tyreDiameter, tyreDrowned, 75.0f);
		else if (i < 6)	tyre[i] = tyreMesh->CreateModel(maxLeftBoundary - i * tyreDiameter, tyreDrowned, 85.0f);
		else if (i < 9) tyre[i] = tyreMesh->CreateModel(maxRightBoundary + i * tyreDiameter, tyreDrowned, 95.0f);
		else            tyre[i] = tyreMesh->CreateModel(maxLeftBoundary - i * tyreDiameter, tyreDrowned, 105.0f);
		tyre[i]->Scale(tyreSize);								  // Enlarge the tyres
		if (i < 3 || i >= 6 && i < 9) tyreState[i] = TyreUpLeft;  // Assign direction
		else						  tyreState[i] = TyreUpRight; // Assign direction
	}

	backdrop = myEngine->CreateSprite(mainAboutBG); // Load main menu background
													// Create far end trees with spacing
	for (int i = 0; i < endTreeAmt; i++)  tree[i] = treeMesh->CreateModel(-treeXBoundary + (treeSpacing*i), groundLevel, 115.0f);
	// Create trees in each corner of first island
	tree[6] = treeMesh->CreateModel(treeXBoundary, groundLevel, 15.0f);
	tree[7] = treeMesh->CreateModel(-treeXBoundary, groundLevel, 15.0f);
	tree[8] = treeMesh->CreateModel(treeXBoundary, groundLevel, 65.0f);
	tree[9] = treeMesh->CreateModel(-treeXBoundary, groundLevel, 65.0f);
	for (int i = 0; i < totalNumOfTrees; i++)
	{
		tree[i]->Scale(treeSize); // Size of Trees
		treeMinX[i] = tree[i]->GetX() - treeRadius - frogRadius;
		treeMaxX[i] = tree[i]->GetX() + treeRadius + frogRadius;
		treeMinZ[i] = tree[i]->GetZ() - treeRadius - frogRadius;
		treeMaxZ[i] = tree[i]->GetZ() + treeRadius + frogRadius;
	}

	for (int i = 0; i < carAmount; i++)  carState[i] = CarRotate; // Rotate cars to go up the slope

	int gameState = MainMenu; // Game state starts as main menu

	// Main Menu Buttons X and Y values
	textPosX[StartButton] = 615;
	textPosY[StartButton] = 275;
	textPosX[HighscoresButton] = 370;
	textPosY[HighscoresButton] = 405;
	textPosX[SettingsButton] = 770;
	textPosY[SettingsButton] = 405;
	textPosX[AboutButton] = 300;
	textPosY[AboutButton] = 550;
	textPosX[QuitButton] = 890;
	textPosY[QuitButton] = 545;
	textPosX[BackButton] = 890;
	textPosY[BackButton] = 545;

	// Settings text positions
	for (int i = leftSettingsStart; i < leftSettingsEnd; i++)
	{
		textPosX[i] = 100;
		textPosY[i] = 260 + currentLine * 75;
		currentLine++;
	}
	currentLine = 0;
	for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
	{
		textPosX[i] = 530;
		textPosY[i] = 260 + currentLine * 75;
		currentLine++;
	}

	textPosX[CancelButton] = 1040; // Cancel button X
	textPosY[CancelButton] = 545;  // Cancel button Y
	widthOfText[CancelButton] = optionFont->MeasureTextWidth(cancelName);   // Measure width of cancel button
	heightOfText[CancelButton] = optionFont->MeasureTextHeight(cancelName); // Measure height of cancel button
	textPosX[ResetButton] = 1040; // Reset button X
	textPosY[ResetButton] = 545;  // Reset button Y
	widthOfText[ResetButton] = optionFont->MeasureTextWidth(resetName);   // Measure width of reset button
	heightOfText[ResetButton] = optionFont->MeasureTextHeight(resetName); // Measure height of reset button

	for (int i = Highscore1; i < Highscore5 + 1; i++) // Measure all the highscores lines
	{
		widthOfText[i] = optionFont->MeasureTextWidth(posNameScoreText.str());
		heightOfText[i] = optionFont->MeasureTextHeight(posNameScoreText.str());
	}
	for (int i = 0; i < mainMenuOptions; i++) // Measure all the main menu options 
	{
		widthOfText[i] = optionFont->MeasureTextWidth(mainMenuList[i]);
		heightOfText[i] = optionFont->MeasureTextHeight(mainMenuList[i]);
	}
	widthOfText[BackButton] = optionFont->MeasureTextWidth(backName);   // Back button measuring width
	heightOfText[BackButton] = optionFont->MeasureTextHeight(backName); // Back button measuring height

	for (int i = leftSettingsStart; i < leftSettingsEnd; i++)
	{
		widthOfText[i] = optionFont->MeasureTextWidth(leftSettingsList[i - 6]);
		heightOfText[i] = optionFont->MeasureTextHeight(leftSettingsList[i - 6]);
	}
	for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
	{
		widthOfText[i] = optionFont->MeasureTextWidth(rightSettingsList[i - 12]);
		heightOfText[i] = optionFont->MeasureTextHeight(rightSettingsList[i - 12]);
	}
	for (int i = 0; i < optionsAmt; i++) // Bounding box around all of the options
	{
		optionMinX[i] = textPosX[i];
		optionMaxX[i] = textPosX[i] + widthOfText[i];
		optionMinY[i] = textPosY[i];
		optionMaxY[i] = textPosY[i] + heightOfText[i];
	}
	myCamera = myEngine->CreateCamera(kManual, cameraX, cameraHeight, cameraZ); // Create a camera
	myCamera->AttachToParent(dummy);											// Attach camera to dummy
	dummy->AttachToParent(frog[currentFrog]);									// Attach dummy to frog 
	myCamera->RotateLocalX(cameraLookDown);										// Tilt camera
																				// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		/**** Update your scene each frame here ****/
		int mouseX = myEngine->GetMouseX(); // Record mouse X
		int mouseY = myEngine->GetMouseY(); // Record mouse Y

		(inGame) ? userNameInfo = "Playing as: " : userNameInfo = "Username: "; // Text before username
		userNameText << userNameInfo << userName;
		optionFont->Draw(userNameText.str(), 0, 175, defaultColour); // Draw text before username
		userNameText.str("");

		if (gameState != Start)
		{
			// If mouse is in the box of one  of the menu options, Switch to highlight colour. 
			// Otherwise show default colour
			for (int i = 0; i < optionsAmt; i++) // Hovering for all options
			{
				if (mouseX >= optionMinX[i] && mouseX <= optionMaxX[i] &&
					mouseY >= optionMinY[i] && mouseY <= optionMaxY[i]) toggledColour[i] = hoverColour;
				else													toggledColour[i] = defaultColour;
			}
		}
		switch (gameState)
		{
		case MainMenu: // Load main menu 
			gameState = LoadMainMenu(myEngine, gameState, mouseX, mouseY, titleFont, optionFont);
			break;

		case Start: // Load the game
			GameDetails(myEngine, gameState, optionFont);
			break;

		case Highscores: // Load the highscores
			gameState = GameScores(myEngine, gameState, mouseX, mouseY, titleFont, optionFont);
			break;

		case About: // Load the about page
			gameState = AboutGame(myEngine, gameState, mouseX, mouseY, titleFont, aboutFont, optionFont);
			break;

		case Settings: // Load the settings
			gameState = GameSettings(myEngine, gameState, mouseX, mouseY, titleFont, optionFont);
			break;

		case BindNewKey: // Load the key bind modifier page
			gameState = EnterNewKey(myEngine, gameState, mouseX, mouseY, titleFont, optionFont);
			break;

		case Quit: // Quit the game
			QuitGame(myEngine);
			break;
		}
		if (gameState == Start && gameOver)
		{
			gameState = Highscores;					   // Load the highscores
			backdrop = LeaveGame(myEngine, gameState); // Leave game code
			gamePaused = false;						   // No longer a game to be paused
		}
		if (myEngine->KeyHit(quitKey))
		{
			if (inGame)
			{
				cdFrog[currentFrog]->stopCountdown();	   // Stop the clock
				backdrop = LeaveGame(myEngine, gameState); // Leave game code	
				gameState = MainMenu;					   // Load main menu
			}
			else
			{
				SaveHS();			// Save the highscores
				SaveFile();			// Save the settings
				QuitGame(myEngine);	// Quit the game
			}
		}
		if (gameOver && myEngine->KeyHit(restartGameKey))
		{
			SaveHS();						  // if in highscores and press back button Save highscores to file	
			inGame = true;					  // Now back ingame
			gameOver = false;				  // No longer game over
			myEngine->RemoveSprite(backdrop); // Remove highscores backdrop
			gameState = Start;				  // Return to start game state
		}
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

bool CollisionWithCar(int &i) // Collision with cars
{
	return (frog[currentFrog]->GetX() < carMaxX[i] && frog[currentFrog]->GetX() > carMinX[i] &&
		frog[currentFrog]->GetZ() < carMaxZ[i] && frog[currentFrog]->GetZ() > carMinZ[i]);
}

bool CollisionWithTree(int &i) // Collision with trees
{
	return (frog[currentFrog]->GetX() < treeMaxX[i] && frog[currentFrog]->GetX() > treeMinX[i] &&
		frog[currentFrog]->GetZ() < treeMaxZ[i] && frog[currentFrog]->GetZ() > treeMinZ[i]);
}

bool ContactWithTyre(int &i) // Collision with tyres
{
	if (frogState == Leaping) return true;
	else if (frog[currentFrog]->GetX() < tyreMaxX[i] && frog[currentFrog]->GetX() > tyreMinX[i] &&
		frog[currentFrog]->GetZ() < tyreMaxZ[i] && frog[currentFrog]->GetZ() > tyreMinZ[i])
	{
		numOfCollisions++; // Count the number of collisions
		return true;	   // Return true if there is any collisions
	}
	else if (i == tyreAmount - 1 && numOfCollisions == 0)  return false; // No collisions occured
	return frogOnTyre;
}

void FrogLastPosition()
{	// Storing frog coordinates
	lastX[currentFrog] = frog[currentFrog]->GetX();
	lastY[currentFrog] = frog[currentFrog]->GetY();
	lastZ[currentFrog] = frog[currentFrog]->GetZ();
}

void RemoveFrogTyreMovement()
{	// Detaching from tyre
	movingLeft = false;
	movingRight = false;
}

void DummyRotation()
{	// Camera arc movement
	dummy->ResetOrientation();
	dummy->RotateLocalX(cameraRotating);
}

void EnterLeapingPhase()
{	// Activate frog jumping
	frogLeaping = true;
	frogState = Leaping;
}

void DownTheSlope(int &i)
{	// Rotate car ready to go down the slope
	car[i]->RotateLocalX(slopeAngle); // Look down
	carState[i] = CarDown;			  // Change direction
}

int AssignDirection()
{
	if (frog[currentFrog]->GetY() > groundLevel) frog[currentFrog]->SetY(groundLevel); // Set frog on ground 
	frogLeaping = false;
	if (frog[currentFrog]->GetZ() >= tyreLane1MinZ && frog[currentFrog]->GetZ() < tyreLane1MaxZ ||
		frog[currentFrog]->GetZ() >= tyreLane2MinZ && frog[currentFrog]->GetZ() < tyreLane2MaxZ)
	{	// Moving left with tyres
		movingRight = false;
		movingLeft = true;
		return FrogMovingLeft;
	}
	else if (frog[currentFrog]->GetZ() >= tyreLane3MinZ && frog[currentFrog]->GetZ() < tyreLane3MaxZ ||
		frog[currentFrog]->GetZ() >= tyreLane4MinZ && frog[currentFrog]->GetZ() < tyreLane4MaxZ)
	{	// Moving right with tyres
		movingLeft = false;
		movingRight = true;
		return FrogMovingRight;
	}
	return frogState;
}

void LoadFrogStates()
{	// Frog starting states
	frogStateTracker[FirstFrog] = Crossing;
	frogStateTracker[SecondFrog] = Waiting;
	frogStateTracker[ThirdFrog] = Waiting;
}

void SetCarSpeed(int &i)
{	// Initialising movement speed for cars in each lane
	if (i < lane2Start)      moveSpeed[i] = lane1Speed * speedModifier;
	else if (i < lane3Start) moveSpeed[i] = lane2Speed * speedModifier;
	else if (i < lane4Start) moveSpeed[i] = lane3Speed * speedModifier;
	else                     moveSpeed[i] = lane4Speed * speedModifier;
}

void SessionOver()
{   // Game over
	tempScore = gameScore;				 // Store game score in temp score
	displayScore = tempScore;			 // The score to be shown on the screen
	gameLevel = 1;						 // Reset level
	currentFrog = FirstFrog;			 // Reset current frog
	gameLives = 3;						 // Reset lives
	gameScore = 0;						 // Reset score
	frogState = FrogStill;				 // Reset frog state 
	biggestZ = frogStartZ + frogRadius;	 // Reset maximum position
	for (int i = 0; i < frogAmount; i++)
	{
		frog[i]->ResetOrientation(); // All frogs look forwards
		cdFrog[i]->resetCountdown(); // Reset the timers for all frogs
		frog[i]->SetSkin("frog/frog.bmp");	// Reset skins of frogs
		if (frog[i]->GetY() >= slightlyRisen) frog[i]->ScaleY(unSquashFrog); // Unsquash the squashed frogs
		frog[i]->SetPosition(frogStartX + (i*frogDiameter), groundLevel, frogStartZ);
	}
	speedModifier = gameLevel * kGameSpeed;			    // Reset movement speed by level
	for (int i = 0; i < carAmount; i++) SetCarSpeed(i); // Reset car movement speeds by level
	LoadFrogStates();									// Reset frog variable states
	FrogLastPosition();									// Update position
	gameOver = true;									// All lives used
}

void TakeALife(IModel* frog[frogAmount]) // Take a life
{
	frogStateTracker[currentFrog] = Dead;				   // Record that frog is now dead
	myCamera->DetachFromParent();						   // Detach from dummy
	dummy->DetachFromParent();							   // Detach from frog
	myCamera->SetPosition(cameraX, cameraHeight, cameraZ); // Set position
	gameLives--;										   // Take a life away
	cdFrog[currentFrog]->stopCountdown();				   // Stop the clock that frog is dead
	if (gameScore > twentyPoints - 1) gameScore -= twentyPoints; // Take 20 if score 20 or more
	myCamera->AttachToParent(dummy);					   // Attach camera to dummy
	(currentFrog < 2) ? currentFrog++ : SessionOver();	   // Next frog
	frogStateTracker[currentFrog] = Crossing;			   // Set next frog to crossing state
	dummy->ResetOrientation();							   // Reset orientation
	dummy->RotateLocalX(cameraRotating);				   // Rotate using current arc settings
	dummy->AttachToParent(frog[currentFrog]);			   // Attach to next frog
	frogState = FrogStill;								   // Frog no longer moving
}
// Level up
void LevelUp()
{
	myCamera->DetachFromParent();																	  // Detach from parent (dummy)
	gameLevel++;																					  // Level up
	biggestZ = frogStartZ + frogRadius;																  // Reset check point
	checkPoint->SetZ(biggestZ);																		  // Reset check point model
	frogState = FrogStill;																			  // Frog no longer moving
	frog[currentFrog]->SetPosition(frogStartX + (currentFrog*frogDiameter), groundLevel, frogStartZ); // Reset Position (current frog)
	myCamera->SetPosition(cameraX, cameraHeight, cameraZ);											  // Reset Position of Camera
	myCamera->AttachToParent(dummy);																  // Re-attach to dummy
	cdFrog[currentFrog]->stopCountdown();															  // Stop countdown
	cdFrog[currentFrog]->resetCountdown();															  // Reset countdown
	for (int i = 0; i < carAmount; i++) SetCarSpeed(i);												  // Inc car speeds
	speedModifier = gameLevel * kGameSpeed;															  // Change speed modifier
}
// Game text on screen
void GameTextOnScreen(IFont* optionFont)
{	// Displaying all game text on the screen
	scoreText << "Score: " << gameScore;
	optionFont->Draw(scoreText.str(), 450, 0, defaultColour);
	scoreText.str("");

	livesText << "Lives: " << gameLives;
	optionFont->Draw(livesText.str(), 450, 35, defaultColour);
	livesText.str("");

	levelText << "Level: " << gameLevel;
	optionFont->Draw(levelText.str(), 450, 70, defaultColour);
	levelText.str("");

	for (int i = 0; i < frogAmount; i++)
	{
		secs[i] = cdFrog[i]->getTimeRemaining();
		if (secs[i] > 0) timeElapsedText << "Frog " << i + 1 << " Time: " << secs[i] << "s";
		else             timeElapsedText << "Frog " << i + 1 << ": Time UP!";
		optionFont->Draw(timeElapsedText.str(), 0, 50 * i, defaultColour);
		timeElapsedText.str("");
	}
	if (secs[currentFrog] == 0 && gameLives > 0) // When time up for the current crossing frog
	{
		if (frogInTyreZone)     frogState = FrogSinking;	   // Uuse the sinking death effect
		else if (frogInCarZone) frogState = FrogSquashing;	   // Use the squashing death effect
		else					frogState = FrogSafeZoneDeath; // Use safe zone death, not in a danger zone,
	}
}
// FPS Counter
void FPSCounter(IFont* optionFont)
{
	if (fpsCounter) // If enabled
	{
		++frames; // Keep adding frames
		overtime = clock() - nextUpdate;
		if (overtime > 0)
		{
			fps = frames / (1.0f + overtime / (float(CLOCKS_PER_SEC)));
			frames = 0; // Reset frames
			nextUpdate = clock() + 1 * (float(CLOCKS_PER_SEC));
		}
		fpsText << roundf(fps); // Display FPS
		optionFont->Draw(fpsText.str(), 1180, 0, defaultColour);
		fpsText.str("");
	}
}
// Rotate car wheels
void RotateWheels(int &i)
{
	frontLeftWheelNode[i]->RotateLocalX(moveSpeed[i] * generalRotationSpeed);  // Rotate front left wheel
	frontRightWheelNode[i]->RotateLocalX(moveSpeed[i] * generalRotationSpeed); // Rotate front right wheel
	backLeftWheelNode[i]->RotateLocalX(moveSpeed[i] * generalRotationSpeed);   // Rotate back left wheel
	backRightWheelNode[i]->RotateLocalX(moveSpeed[i] * generalRotationSpeed);  // Rotate back right wheel}
}
// Reset highscores
void ResetHS()
{
	// Default names and scores until the user beats them
	for (int i = 0; i < scoreAmount; i++)
	{
		playerName[i] = "Ajrushton1";
		playerScore[i] = 100 + (i * 100);
	}
}

void HighscoresResetMSG()
{
	cout << "Highscores reset successfully." << endl;
}

void SettingsResetMSG()
{
	cout << "Settings reset successfully." << endl;
}

void FileLog()
{
	cout << "[LOG] Full Screen Enabled: " << boolalpha << loadFullScreen << "(Requires restart for changes to take effect)" << endl;
	cout << "[LOG] FPS Display Enabled: " << boolalpha << toggleFpsCounter << endl;
	cout << "[LOG] Pause Key: " << currentGameKeysLeft[PauseText] << endl;
	cout << "[LOG] Quit Key: " << currentGameKeysLeft[QuitText] << endl;
	cout << "[LOG] Reset Camera Key: " << currentGameKeysLeft[ResetCameraText] << endl;
	cout << "[LOG] View Tilt Up Key: " << currentGameKeysLeft[TiltUpText] << endl;
	cout << "[LOG] Restart Level Key: " << currentGameKeysRight[RestartLevelText] << endl;
	cout << "[LOG] Frog Forwards Key: " << currentGameKeysRight[FrogUpText] << endl;
	cout << "[LOG] Frog Backwards Key: " << currentGameKeysRight[FrogDownText] << endl;
	cout << "[LOG] Frog Left Key: " << currentGameKeysRight[FrogLeftText] << endl;
	cout << "[LOG] Frog Right Key: " << currentGameKeysRight[FrogRightText] << endl;
	cout << "[LOG] View Tilt Down Key: " << currentGameKeysRight[TiltDownText] << endl;
}

ISprite* LeaveGame(I3DEngine* myEngine, int gameState)
{	// Leaving the game sprite creation
	(gameState == Highscores) ? backdrop = myEngine->CreateSprite(highscoresBG) : backdrop = myEngine->CreateSprite(mainAboutBG);
	gamePaused = true; // Pause the game
	inGame = false;    // No longer in a game
	return backdrop;
}

void LoadTempKeys()
{	// Store keys in temporary variables
	tempQuitKey = quitKey;
	tempPauseGameKey = pauseGameKey;
	tempResetCamera = resetCamera;
	tempViewTiltUp = viewTiltUp;
	tempRestartLevel = restartLevel;
	tempFrogForwards = frogForwards;
	tempFrogBackwards = frogBackwards;
	tempFrogLeft = frogLeft;
	tempFrogRight = frogRight;
	tempViewTiltDown = viewTiltDown;
}
// Update keys (When pressing after changing key or if ResetKeys is called
void UpdateKeys()
{   // Update controls
	cout << "[LOG] Game keys updated" << endl;
	quitKey = tempQuitKey;
	pauseGameKey = tempPauseGameKey;
	resetCamera = tempResetCamera;
	viewTiltUp = tempViewTiltUp;
	restartLevel = tempRestartLevel;
	frogForwards = tempFrogForwards;
	frogBackwards = tempFrogBackwards;
	frogLeft = tempFrogLeft;
	frogRight = tempFrogRight;
	viewTiltDown = tempViewTiltDown;
}
// Reset Game Keys
void ResetKeys()
{
	const int fwdNum = 192;
	const int bkwdNum = 191;
	fullScreen = false;				   // Full Screen
	fpsCounter = false;				   // FPS Counter
	pauseGameKey = Key_P;			   // Pause game
	quitKey = Key_Escape;			   // Quit program
	resetCamera = Key_C;			   // Reset position of camera 
	viewTiltUp = Key_Up;			   // Look up camera 
	restartLevel = Key_R;		       // Restart level
	frogForwards = EKeyCode(fwdNum);   // Jump Forwards
	frogBackwards = EKeyCode(bkwdNum); // Jump Backwards
	frogLeft = Key_Z;			 	   // Jump Left
	frogRight = Key_X;				   // Jump Right
	viewTiltDown = Key_Down;	       // Tilt down
	LoadTempKeys();					   // Set temp keys equal to the default keys
									   // Reset key names on settings screen
	currentGameKeysLeft[PauseText] = "P";         // Default pause key
	currentGameKeysLeft[QuitText] = "Escape";     // Default quit key
	currentGameKeysLeft[ResetCameraText] = "C";   // Default reset camera key
	currentGameKeysLeft[TiltUpText] = "Up";        // Default camera view 1 key
	currentGameKeysRight[RestartLevelText] = "R"; // Default restart level key
	currentGameKeysRight[FrogUpText] = "'";       // Default Frog Leaping Forwards key
	currentGameKeysRight[FrogDownText] = "/";     // Default Frog Leaping Backwards key
	currentGameKeysRight[FrogLeftText] = "Z";     // Default Frog Leaping Left key
	currentGameKeysRight[FrogRightText] = "X";    // Default Frog Leaping Right key
	currentGameKeysRight[TiltDownText] = "Down";     // Default camera view 2 key
}

void SaveHS()
{
	saveHighscores.open(top5scores); // Open the file
	if (saveHighscores.fail()) perror("[LOG] scores.dat");// Error handling

	cout << "[LOG] Saving the scores..." << endl;
	for (int i = 0; i < scoreAmount; i++)
	{
		saveHighscores << playerName[i] << endl;  // Save each name
		saveHighscores << playerScore[i] << endl; // Save each score
	}
	saveHighscores.close(); // close file we are done with it
}

void LoadHS()
{
	loadHighscores.open(top5scores); // Open the file
	string line; // Temp store each line
	if (loadHighscores.fail()) // Error handling
	{
		perror("[LOG] scores.dat");
		cout << "[LOG] No highscores found. Using defaults." << endl;
		ResetHS();
		SaveHS();
		loadHighscores.open(top5scores);
	}
	vector<string> hsVector; // Dynamic array to store highscores in

	while (getline(loadHighscores, line)) hsVector.push_back(line); // Add each line to the dynamic array							  

	int ln = 0;
	for (int i = 0; i < scoreAmount; i++)
	{
		playerName[i] = hsVector[ln];					  // Add the name to the vector
		ln++;											  // Next line
		playerScore[i] = (int)atof(hsVector[ln].c_str()); // Add the score (whilst converting it)
		ln++;											  // Next line
	}
	loadHighscores.close(); // CLOSE we are done with it
}

void SaveFile()
{
	saveGameSettings.open(gameSettings); // Open the file
	if (saveGameSettings.fail()) perror("[LOG]settings.dat");// Error handling

	cout << "[LOG] SAVING ALL KEYS..." << endl;
	saveGameSettings << boolalpha << fullScreen << endl; // Save Full Screen State
	saveGameSettings << boolalpha << fpsCounter << endl; // Save FPS Counter State
	saveGameSettings << pauseGameKey << endl;
	saveGameSettings << quitKey << endl;
	saveGameSettings << resetCamera << endl;
	saveGameSettings << viewTiltUp << endl;
	saveGameSettings << restartLevel << endl;
	saveGameSettings << frogForwards << endl;
	saveGameSettings << frogBackwards << endl;
	saveGameSettings << frogLeft << endl;
	saveGameSettings << frogRight << endl;
	saveGameSettings << viewTiltDown << endl;
	cout << "[LOG] Save Complete" << endl;

	saveGameSettings.close(); // CLOSE we are done with it
}

string GetKeyName(int keyNumber) // Send the key number over
{
	keyName = keyCodeNames[keyNumber]; // Match key name with number
	return keyName;					   // Return the key name
}

void FindAllKeyNames()
{	// Left side menu key names
	currentGameKeysLeft[PauseText] = GetKeyName(pauseGameKey);
	currentGameKeysLeft[QuitText] = GetKeyName(quitKey);
	currentGameKeysLeft[ResetCameraText] = GetKeyName(resetCamera);
	currentGameKeysLeft[TiltUpText] = GetKeyName(viewTiltUp);
	// Right side menu key names
	currentGameKeysRight[RestartLevelText] = GetKeyName(restartLevel);
	currentGameKeysRight[FrogUpText] = GetKeyName(frogForwards);
	currentGameKeysRight[FrogDownText] = GetKeyName(frogBackwards);
	currentGameKeysRight[FrogLeftText] = GetKeyName(frogLeft);
	currentGameKeysRight[FrogRightText] = GetKeyName(frogRight);
	currentGameKeysRight[TiltDownText] = GetKeyName(viewTiltDown);
}

EKeyCode ReturnNewKey(I3DEngine* myEngine, int gameState)
{   //
	for (int i = 2; i < kMaxKeyCodes; i++) // For all the keys
	{
		if (myEngine->KeyHit(EKeyCode(i))) // If they hit a key in the list
		{
			notInUseKeyName = GetKeyName(i); // Display the temp name of key chosen
			if (notInUseKeyName == currentGameKeysLeft[PauseText] ||
				notInUseKeyName == currentGameKeysLeft[QuitText] ||
				notInUseKeyName == currentGameKeysLeft[ResetCameraText] ||
				notInUseKeyName == currentGameKeysLeft[TiltUpText] ||
				notInUseKeyName == currentGameKeysRight[RestartLevelText] ||
				notInUseKeyName == currentGameKeysRight[FrogUpText] ||
				notInUseKeyName == currentGameKeysRight[FrogDownText] ||
				notInUseKeyName == currentGameKeysRight[FrogLeftText] ||
				notInUseKeyName == currentGameKeysRight[FrogRightText] ||
				notInUseKeyName == currentGameKeysRight[TiltDownText] ||
				i == restartGameKey) keyInUse = true;
			else
			{
				keyInUse = false;
				tempCurrentKeyName = notInUseKeyName; // Set new key as temp
				return EKeyCode(i);                   // Put the key in temp
			}
		}
	}
	return EKeyCode(currentKey); //return current key
}

void ResetKeyChanger()
{	// No longer changing keys
	tempCurrentKeyName = "";
	notInUseKeyName = "";
	changingKey = false;
}

void SaveAndPrint()
{	// Reset and save keys to file
	ResetKeys();
	SaveFile();
	cout << "[LOG] Error loading game file." << endl;
}

void LoadSettings()
{
	loadData.open(gameSettings); // Open the file
	if (loadData.fail()) // Error handling
	{
		perror("[LOG] settings.dat");
		cout << "[LOG] Using default settings..." << endl;
	}
	// Load user settings from game file
	loadData >> loadFullScreen >> toggleFpsCounter >> currentPauseKey >> currentQuitKey 
		     >> currentResetCamera >> currentViewTiltUp >> currentRestartLevel >> currentFrogForwards 
		     >> currentFrogBackwards >> currentFrogLeft >> currentFrogRight >> currentViewTiltDown;
	(loadFullScreen == "true") ? fullScreen = true : fullScreen = false; 
	(toggleFpsCounter == "true") ? fpsCounter = true : fpsCounter = false; 
																		   
	if (currentPauseKey != "" &&
		currentQuitKey != "" &&
		currentResetCamera != "" &&
		currentViewTiltUp != "" &&
		currentRestartLevel != "" &&
		currentFrogForwards != "" &&
		currentFrogBackwards != "" &&
		currentFrogLeft != "" &&
		currentFrogRight != "" &&
		currentViewTiltDown != "")
	{   // Convert string to int
		pauseKeyNumber = stoi(currentPauseKey);
		quitKeyNumber = stoi(currentQuitKey);
		resetCamKeyNumber = stoi(currentResetCamera);
		viewTiltUpNumber = stoi(currentViewTiltUp);
		restartLevelNumber = stoi(currentRestartLevel);
		frogForwardsNumber = stoi(currentFrogForwards);
		frogBackwardsNumber = stoi(currentFrogBackwards);
		frogLeftNumber = stoi(currentFrogLeft);
		frogRightNumber = stoi(currentFrogRight);
		viewTiltDownNumber = stoi(currentViewTiltDown);
		// If they havent been tampered with
		if (pauseKeyNumber      > 0 && pauseKeyNumber      < kMaxKeyCodes &&
			quitKeyNumber       > 0 && quitKeyNumber       < kMaxKeyCodes &&
			resetCamKeyNumber   > 0 && resetCamKeyNumber   < kMaxKeyCodes &&
			viewTiltUpNumber    > 0 && viewTiltUpNumber    < kMaxKeyCodes &&
			restartLevelNumber  > 0 && restartLevelNumber  < kMaxKeyCodes &&
			frogForwardsNumber  > 0 && frogForwardsNumber  < kMaxKeyCodes &&
			frogBackwardsNumber > 0 && frogBackwardsNumber < kMaxKeyCodes &&
			frogLeftNumber      > 0 && frogLeftNumber      < kMaxKeyCodes &&
			frogRightNumber     > 0 && frogRightNumber     < kMaxKeyCodes &&
			viewTiltDownNumber  > 0 && viewTiltDownNumber  < kMaxKeyCodes)
		{   // Assign all key numbers and retrieve their names
			pauseGameKey = EKeyCode(pauseKeyNumber);
			quitKey = EKeyCode(quitKeyNumber);
			resetCamera = EKeyCode(resetCamKeyNumber);
			viewTiltUp = EKeyCode(viewTiltUpNumber);
			restartLevel = EKeyCode(restartLevelNumber);
			frogForwards = EKeyCode(frogForwardsNumber);
			frogBackwards = EKeyCode(frogBackwardsNumber);
			frogLeft = EKeyCode(frogLeftNumber);
			frogRight = EKeyCode(frogRightNumber);
			viewTiltDown = EKeyCode(viewTiltDownNumber);

			FindAllKeyNames(); // Find and assign all key names to the above keys
			LoadTempKeys(); // Store the newly assigned keys into temporary keys
		}
		else SaveAndPrint(); // If any numbers have been tampered with, they will use default
	}
	else SaveAndPrint(); // If nothing exists use default

	FileLog();		  // Write current key list on the console
	loadData.close(); // CLOSE we are done with it
}

int LoadMainMenu(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont)
{
	titleFont->Draw("Main Menu", titleTextX, titleTextY, titleColour); // Draw main menu text

	for (int i = 0; i < mainMenuOptions; i++) 	// Write out all the main menus
	{
		mainMenuText[i] << mainMenuList[i];
		optionFont->Draw(mainMenuText[i].str(), textPosX[i], textPosY[i], toggledColour[i]);
		mainMenuText[i].str("");
	}
	// Clicking Main Menu Buttons
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, optionFont);
	return gameState;
}

int GameScores(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont)
{
	titleFont->Draw("Highscores", titleTextX, titleTextY, titleColour);
	if (gameOver)
	{
		gameOverScore << "Game Over\n\nYour score: " << displayScore << "\n\n\nPress Space Bar \nto play again.";
		optionFont->Draw(gameOverScore.str(), 0, 250, kGreen);
		gameOverScore.str("");
	}
	// Add all the highscores
	for (int i = 0; i < scoreAmount; i++) p[i] = { playerName[i], playerScore[i] };
	// Create a vector filled with AddScore Entry objects.
	vector<AddScore> scoreboard;
	//Add our Entries to the vector
	for (int i = 0; i < scoreAmount; i++) scoreboard.push_back(p[i]);

	sort(scoreboard.begin(), scoreboard.end(), By_Score()); // Sort them now the last is the smallest
	if (tempScore > scoreboard[lastPlace].score) // If the user scored higher than the lowest score, replace it.
	{
		for (int i = 0; i < scoreAmount; i++)
		{
			if (playerScore[i] == scoreboard[lastPlace].score)
			{   // If any of the scores is the last score, replace it
				playerName[i] = userName;
				playerScore[i] = tempScore;
			}
		}
		tempScore = 0; // Reset it
	}
	sort(scoreboard.begin(), scoreboard.end(), By_Score()); // Re-sort them
															// Display column headers
	posNameScoreText << "#" << setw(posSpacing) << setfill(spaceFiller) << "Name" << setw(nameSpacing) << setfill(spaceFiller) << "Score";
	optionFont->Draw(posNameScoreText.str(), 300, 160, defaultColour);
	posNameScoreText.str("");

	for (int i = 0; i < numOfScores; i++) // Draw positions
	{
		top5PosText[i] << left << i + 1;
		optionFont->Draw(top5PosText[i].str(), 300, 260 + (i * 80), toggledColour[i + 20]);
		top5PosText[i].str("");
	}
	for (int i = 0; i < numOfScores; i++) // Draw names
	{
		top5NameText[i] << left << scoreboard[i].name;
		optionFont->Draw(top5NameText[i].str(), 350, 260 + (i * 80), toggledColour[i + 20]);
		top5NameText[i].str("");
	}
	for (int i = 0; i < numOfScores; i++) // Draw scores
	{
		top5ScoreText[i] << left << scoreboard[i].score;
		optionFont->Draw(top5ScoreText[i].str(), 750, 260 + (i * 80), toggledColour[i + 20]);
		top5ScoreText[i].str("");
	}
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, optionFont);
	return gameState;
}

int GameSettings(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont)
{
	titleFont->Draw("Settings", titleTextX, titleTextY, titleColour);
	changingKey = false;
	// Drawing the menus
	for (int i = leftSettingsStart; i < 12; i++)
	{   // Left hand side
		optionFont->Draw(leftSettingsList[i - 6], textPosX[i], textPosY[i], toggledColour[i]);
		optionFont->Draw(currentGameKeysLeft[i - 6], textPosX[i] + 225, textPosY[i], toggledColour[i]);
	}
	optionFont->Draw("For Full Screen to take effect, you must restart the game.", 150, 215, defaultColour);
	for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
	{   // Right hand side
		optionFont->Draw(rightSettingsList[i - 12], textPosX[i], textPosY[i], toggledColour[i]);
		optionFont->Draw(currentGameKeysRight[i - 12], textPosX[i] + 250, textPosY[i], toggledColour[i]);
	}

	fullScreenStateText << boolalpha << fullScreen;
	optionFont->Draw(fullScreenStateText.str(), textPosX[6] + 225, textPosY[6], toggledColour[6]);
	fullScreenStateText.str("");

	fpsToggleStateText << boolalpha << fpsCounter;
	optionFont->Draw(fpsToggleStateText.str(), textPosX[7] + 225, textPosY[7], toggledColour[7]);
	fpsToggleStateText.str("");

	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, optionFont);
	return gameState;
}

int EnterNewKey(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* optionFont)
{
	titleFont->Draw("Enter a New Key", titleTextX, titleTextY, titleColour);

	if (keyInUse) optionFont->Draw("Key in use", textPosX[7] + 150, textPosY[7] - 100, kRed);
	else if (!keyInUse && tempCurrentKeyName != "") optionFont->Draw("Key available", textPosX[7] + 150, textPosY[7] - 100, kGreen);
	// Change key bindings or stay the same
	currentKeyText << "New key: " << notInUseKeyName;
	optionFont->Draw(currentKeyText.str(), textPosX[7] + 150, textPosY[7], defaultColour);
	currentKeyText.str("");

	if (keyChangeName == leftSettingsList[PauseText])
	{
		currentKey = tempPauseGameKey;						  // Send current key in
		tempPauseGameKey = ReturnNewKey(myEngine, gameState); // Return a new key (temp)
	}
	else if (keyChangeName == leftSettingsList[QuitText])
	{
		currentKey = tempQuitKey;
		tempQuitKey = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == leftSettingsList[ResetCameraText])
	{
		currentKey = tempResetCamera;
		tempResetCamera = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == leftSettingsList[TiltUpText])
	{
		currentKey = tempViewTiltUp;
		tempViewTiltUp = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[RestartLevelText])
	{
		currentKey = tempRestartLevel;
		tempRestartLevel = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[FrogUpText])
	{
		currentKey = tempFrogForwards;
		tempFrogForwards = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[FrogDownText])
	{
		currentKey = tempFrogBackwards;
		tempFrogBackwards = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[FrogLeftText])
	{
		currentKey = tempFrogLeft;
		tempFrogLeft = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[FrogRightText])
	{
		currentKey = tempFrogRight;
		tempFrogRight = ReturnNewKey(myEngine, gameState);
	}
	else if (keyChangeName == rightSettingsList[TiltDownText])
	{
		currentKey = tempViewTiltDown;
		tempViewTiltDown = ReturnNewKey(myEngine, gameState);
	}
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, optionFont);
	return gameState;
}

int AboutGame(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* titleFont, IFont* aboutFont, IFont* optionFont)
{
	titleFont->Draw("About", titleTextX, titleTextY, titleColour);
	aboutFont->Draw("3D Frogger - 2nd UCLan Games Assignment", textLine1X, textLine1Y, defaultColour);
	aboutFont->Draw("Programmed by \n Adam Rushton", textLine2X, textLine2Y, defaultColour);
	aboutFont->Draw("First Year Software\nEngineering Student UCLan", textLine3X, textLine3Y, defaultColour);
	aboutFont->Draw("Made with TL-Engine using C++", textLine4X, textLine4Y, defaultColour);
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, optionFont);
	return gameState;
}

int ButtonHandler(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* optionFont)
{
	(gameState == BindNewKey) ? changingKey = true : changingKey = false;
	(changingKey) ? backName = "Save" : backName = "Back";

	// Draw buttons 
	if (gameState == Highscores || gameState == Settings) optionFont->Draw(resetName, textPosX[ResetButton], textPosY[ResetButton], toggledColour[ResetButton]);
	if (gameState != MainMenu) optionFont->Draw(backName, textPosX[BackButton], textPosY[BackButton], toggledColour[BackButton]); 
	if (gameState == BindNewKey) optionFont->Draw(cancelName, textPosX[CancelButton], textPosY[CancelButton], toggledColour[CancelButton]); 

	if (myEngine->KeyHit(buttonClicker)) // When you click the mouse button
	{
		// Main Menu Buttons Mouse click whilst hovering option 1
		if (gameState == MainMenu)
		{
			if (mouseX >= optionMinX[StartButton] && mouseX <= optionMaxX[StartButton] &&
				mouseY >= optionMinY[StartButton] && mouseY <= optionMaxY[StartButton])
			{
				inGame = true;
				if (!gamePaused && pauseCreated)
				{
					myEngine->RemoveSprite(pause);
					pauseCreated = false;
				}
				else if (gamePaused && !pauseCreated)
				{
					pause = myEngine->CreateSprite(pauseName, pauseSpriteX, pauseSpriteY);
					pauseCreated = true;
				}
				myEngine->RemoveSprite(backdrop); // Remove background if starting game
				return Start;
			}
			// Mouse click whilst hovering option 2
			if (mouseX >= optionMinX[HighscoresButton] && mouseX <= optionMaxX[HighscoresButton] &&
				mouseY >= optionMinY[HighscoresButton] && mouseY <= optionMaxY[HighscoresButton])
			{
				myEngine->RemoveSprite(backdrop);                // Remove main menu background
				backdrop = myEngine->CreateSprite(highscoresBG); // Create highscores background
				LoadHS();										 // Open highscores file									 
				return Highscores;								 // Go to highscores screen
			}
			// Mouse click whilst hovering option 3
			if (mouseX >= optionMinX[SettingsButton] && mouseX <= optionMaxX[SettingsButton] &&
				mouseY >= optionMinY[SettingsButton] && mouseY <= optionMaxY[SettingsButton])
			{
				myEngine->RemoveSprite(backdrop);              // Remove main menu background
				backdrop = myEngine->CreateSprite(settingsBG); // Create settings background
				return Settings;
			}
			// Mouse click whilst hovering option 4
			if (mouseX >= optionMinX[AboutButton] && mouseX <= optionMaxX[AboutButton] &&
				mouseY >= optionMinY[AboutButton] && mouseY <= optionMaxY[AboutButton])	return About;
			// Mouse click whilst hovering option 5
			if (mouseX >= optionMinX[QuitButton] && mouseX <= optionMaxX[QuitButton] &&
				mouseY >= optionMinY[QuitButton] && mouseY <= optionMaxY[QuitButton]) return Quit;
		}
		if (gameState == Highscores || gameState == Settings) // Check the gamestate
		{
			if (mouseX >= optionMinX[ResetButton] && mouseX <= optionMaxX[ResetButton] &&
				mouseY >= optionMinY[ResetButton] && mouseY <= optionMaxY[ResetButton]) // Click reset button
			{
				(gameState == Highscores) ? ResetHS() : ResetKeys(); // In HS? - ResetHS. Otherwise they are in settings, so reset settings.
				(gameState == Highscores) ? HighscoresResetMSG() : SettingsResetMSG(); // Display reset message on console
			}
		}
		if (gameState == BindNewKey)
		{
			if (mouseX >= optionMinX[CancelButton] && mouseX <= optionMaxX[CancelButton] &&
				mouseY >= optionMinY[CancelButton] && mouseY <= optionMaxY[CancelButton]) // Click cancel button
			{
				ResetKeyChanger(); // Reset temp key name, not in use key name and change state of changing key 
				gameState = Settings;
			}
		}
		if (mouseX >= optionMinX[BackButton] && mouseX <= optionMaxX[BackButton] &&
			mouseY >= optionMinY[BackButton] && mouseY <= optionMaxY[BackButton]) // Click back button
		{
			if (gameState == Highscores) SaveHS();	// if in highscores and press back button Save highscores to file		    
			if (gameOver) gameOver = false;
			if (gameState == Highscores || gameState == Settings)
			{
				myEngine->RemoveSprite(backdrop); // Remove background
				backdrop = myEngine->CreateSprite(mainAboutBG); // Create main menu background
			}
			(!changingKey) ? gameState = MainMenu : gameState = Settings;
			if (changingKey)
			{
				UpdateKeys();      // Update key numbers
				FindAllKeyNames(); // Update key names
				SaveFile();		   // Save updated keys
				ResetKeyChanger(); // Reset temp key name, not in use key name and change state of changing key  
			}
		}
		if (gameState == Settings) // When in the settings
		{
			if (mouseX >= optionMinX[FullScreenKey] && mouseX <= optionMaxX[FullScreenKey] &&
				mouseY >= optionMinY[FullScreenKey] && mouseY <= optionMaxY[FullScreenKey])
			{										 // Full screen button hit
				fullScreen = !fullScreen;			 // Switch between the two states
				SaveFile();							 // Save keys 
			}
			if (mouseX >= optionMinX[FPSCounterKey] && mouseX <= optionMaxX[FPSCounterKey] &&
				mouseY >= optionMinY[FPSCounterKey] && mouseY <= optionMaxY[FPSCounterKey])
			{										 // FPS Counter button hit
				fpsCounter = !fpsCounter;			 // Switch between the two states	
				SaveFile();							 // Save keys
			}
			if (mouseX >= optionMinX[PauseKey] && mouseX <= optionMaxX[PauseKey] &&
				mouseY >= optionMinY[PauseKey] && mouseY <= optionMaxY[PauseKey])
			{												 // Pause key button hit
				keyChangeName = leftSettingsList[PauseText]; // Changing the pause game key
				return BindNewKey;							 // Changing game state
			}
			if (mouseX >= optionMinX[QuitKey] && mouseX <= optionMaxX[QuitKey] &&
				mouseY >= optionMinY[QuitKey] && mouseY <= optionMaxY[QuitKey])
			{												// Back/Quit key
				keyChangeName = leftSettingsList[QuitText]; // Changing the back/quit game key
				return BindNewKey;							// Changing game state
			}
			if (mouseX >= optionMinX[ResetCameraKey] && mouseX <= optionMaxX[ResetCameraKey] &&
				mouseY >= optionMinY[ResetCameraKey] && mouseY <= optionMaxY[ResetCameraKey])
			{													   // Reset Camera Key
				keyChangeName = leftSettingsList[ResetCameraText]; // Changing the reset camera key
				return BindNewKey;								   // Changing game state
			}
			if (mouseX >= optionMinX[TopViewKey] && mouseX <= optionMaxX[TopViewKey] &&
				mouseY >= optionMinY[TopViewKey] && mouseY <= optionMaxY[TopViewKey])
			{												  // View Tilt Up Key
				keyChangeName = leftSettingsList[TiltUpText]; // Changing the top view key
				return BindNewKey;							  // Changing game state
			}
			if (mouseX >= optionMinX[RestartLevelKey] && mouseX <= optionMaxX[RestartLevelKey] &&
				mouseY >= optionMinY[RestartLevelKey] && mouseY <= optionMaxY[RestartLevelKey])
			{														 // Restart Level Key
				keyChangeName = rightSettingsList[RestartLevelText]; // Changing the restart level key
				return BindNewKey;									 // Changing game state
			}
			if (mouseX >= optionMinX[FrogForwardsKey] && mouseX <= optionMaxX[FrogForwardsKey] &&
				mouseY >= optionMinY[FrogForwardsKey] && mouseY <= optionMaxY[FrogForwardsKey])
			{												   // Frog Up Key
				keyChangeName = rightSettingsList[FrogUpText]; // Changing the frog up key
				return BindNewKey;							   // Changing game state
			}
			if (mouseX >= optionMinX[FrogBackwardsKey] && mouseX <= optionMaxX[FrogBackwardsKey] &&
				mouseY >= optionMinY[FrogBackwardsKey] && mouseY <= optionMaxY[FrogBackwardsKey])
			{													 // Frog Down Key
				keyChangeName = rightSettingsList[FrogDownText]; // Changing the frog down key
				return BindNewKey;								 // Changing game state
			}
			if (mouseX >= optionMinX[FrogLeftKey] && mouseX <= optionMaxX[FrogLeftKey] &&
				mouseY >= optionMinY[FrogLeftKey] && mouseY <= optionMaxY[FrogLeftKey])
			{													 // Frog Left key
				keyChangeName = rightSettingsList[FrogLeftText]; // Changing the frog left key
				return BindNewKey;								 // Changing game state
			}
			if (mouseX >= optionMinX[FrogRightKey] && mouseX <= optionMaxX[FrogRightKey] &&
				mouseY >= optionMinY[FrogRightKey] && mouseY <= optionMaxY[FrogRightKey])
			{													  // Frog Right key
				keyChangeName = rightSettingsList[FrogRightText]; // Changing the frog right key
				return BindNewKey;								  // Changing game state
			}
			if (mouseX >= optionMinX[DownViewKey] && mouseX <= optionMaxX[DownViewKey] &&
				mouseY >= optionMinY[DownViewKey] && mouseY <= optionMaxY[DownViewKey])
			{													 // View Tilt Down
				keyChangeName = rightSettingsList[TiltDownText]; // Changing the top back key
				return BindNewKey;								 // Changing game state
			}
		}
	}
	return gameState;
}

int GameDetails(I3DEngine* myEngine, int gameState, IFont* optionFont)
{
	GameTextOnScreen(optionFont); // Load all game text on the screen

	if (!gamePaused)
	{
		FPSCounter(optionFont); // Work out FPS
		for (int i = 0; i < frogAmount; i++)
		{
			switch (frogStateTracker[i]) // Switching between frog states
			{
			case Waiting:
				allFrogStates[i] << " Frog " << i + 1 << ": Waiting";
				break;
			case Crossing:
				allFrogStates[i] << " Frog " << i + 1 << ": Crossing";
				break;
			case Dead:
				allFrogStates[i] << " Frog " << i + 1 << ": Dead";
				break;
			}
			optionFont->Draw(allFrogStates[i].str(), 0 + (i * 300), 665, defaultColour);
			allFrogStates[i].str("");
		}
		switch (frogState)
		{
		case Leaping: // Frog jumping
			if (frogLoc < kPi)
			{
				frogNewHeight = fullLeap * sin(frogLoc);
				frog[currentFrog]->MoveLocalZ(frogMultiplier*frogSpeed);
				frog[currentFrog]->SetY(frogNewHeight);
				frogLoc += kPi / framesPassed; // 200 loops to complete wave and travel 10 units 
				if (frogInTyreZone && movingLeft) frog[currentFrog]->MoveX(-tyreSpeed * speedModifier);
				else if (frogInTyreZone && movingRight) frog[currentFrog]->MoveX(tyreSpeed * speedModifier);
				else RemoveFrogTyreMovement(); // Not moving left or right on tyres anymore
			}
			else // Frog has landed
			{	// Level up if in the level up zone
				if (frog[currentFrog]->GetZ() >= levelUpMinZ && frog[currentFrog]->GetZ() <= levelUpMazZ) LevelUp(); 
				if (frog[currentFrog]->GetZ() > biggestZ) 
				{	// Add score update checkpoint
					gameScore += twentyPoints / 2;
					biggestZ = frog[currentFrog]->GetLocalZ() + frogRadius;
					checkPoint->SetPosition(frog[currentFrog]->GetX(), slightlyRisen, biggestZ);
				}
				frogLoc = floatZero;
				FrogLastPosition();
				(frogOnTyre) ? frogState = AssignDirection() : frogState = FrogStill;
			}
			break;

		case FrogMovingLeft: // Frog moving left with tyres
			FrogLastPosition();
			if (frog[currentFrog]->GetY() > groundLevel) frog[currentFrog]->SetY(groundLevel);
			if (frogOnTyre && frog[currentFrog]->GetX() > maxLeftBoundary) frog[currentFrog]->MoveX(-tyreSpeed * speedModifier); // Keep moving left or change direction
			else frogState = FrogSinking;
			break;

		case FrogMovingRight: // Frog moving right with tyres
			FrogLastPosition();
			if (frog[currentFrog]->GetY() > groundLevel) frog[currentFrog]->SetY(groundLevel);
			if (frogOnTyre && frog[currentFrog]->GetX() < maxRightBoundary) frog[currentFrog]->MoveX(tyreSpeed * speedModifier); // Keep moving right or change direction
			else frogState = FrogSinking;
			break;

		case FrogSquashing: // Frog squashing after being hit by a car or time up in car zone.
			frog[currentFrog]->ScaleY(squashedFrog);		 // Squash him
			frog[currentFrog]->SetY(slightlyRisen);			 // Raise a little bit
			frog[currentFrog]->SetSkin("frog/frog_red.bmp"); // Make the frog bleed
			if (carCollision) carCollision = false;			 // Remove the collision
			TakeALife(frog);								 // Take a life	
			break;

		case FrogSafeZoneDeath: // Frog safe zone death if they die while not in car or tyre zone
			frog[currentFrog]->SetY(groundLevel); // Set frog to ground level
			frog[currentFrog]->SetSkin("frog/frog_red.bmp");
			TakeALife(frog);
			break;

		case FrogSinking: // Frog sinking if they fall off tyres
			if (frog[currentFrog]->GetY() == groundLevel) frog[currentFrog]->SetSkin("frog/frog_blue.bmp");
			if (frog[currentFrog]->GetY() >= sunkLevel) frog[currentFrog]->MoveY(kGameSpeed*-sinkingSpeed);
			else TakeALife(frog);
			break;

		case FrogStill: // Frog still to reset the jump calculation
			frogLoc = floatZero;
			frog[currentFrog]->SetY(groundLevel);
			frogLeaping = false;
			break;
		}
		for (int i = lane1Start; i < lane4End; i++) // For all of the cars
		{
			switch (carState[i])
			{
			case CarRotate:
				car[i]->RotateLocalX(-slopeAngle); // Set the slope angle
				carState[i] = CarUp;			   // Start direction of travel
				break;

			case CarUp:
				if (canRotate[i] == true) RotateWheels(i); // If the car is a custom car, rotate wheels
				if (car[i]->GetY() < groundLevel) car[i]->MoveLocalZ(moveSpeed[i]); // Move all the cars up till on surface			
				else
				{
					car[i]->RotateLocalX(slopeAngle); // Even the car's rotation angle
					(i < lane3Start) ? carState[i] = CarLeft : carState[i] = CarRight; // First two lanes go left, other two go right		
				}
				break;

			case CarDown:
				if (canRotate[i] == true) RotateWheels(i); // If the car is a custom car, rotate wheels
				if (car[i]->GetY() > carSunkLevel) car[i]->MoveLocalZ(moveSpeed[i]); // Move cars using local axis
				else carState[i] = CarTimeOut;
				break;

			case CarTimeOut:
				if (cdCar[i]->countingDown() == false) cdCar[i]->startCountdown(); // Start countdown if not already running
				if (cdCar[i]->getTimeRemaining() == 0)     // When time runs out
				{   // Reset Car Lane Positions
					if (i < lane2Start) car[i]->SetPosition(carXSlopeRight, waterAndIslandFloor, 25.0f);
					else if (i < lane3Start) car[i]->SetPosition(carXSlopeRight, waterAndIslandFloor, 35.0f);
					else if (i < lane4Start) car[i]->SetPosition(carXSlopeLeft, waterAndIslandFloor, 45.0f);
					else car[i]->SetPosition(carXSlopeLeft, waterAndIslandFloor, 55.0f);
					car[i]->RotateLocalX(-slopeAngle); // Put straight
					cdCar[i]->stopCountdown();    // Stop countdown
					cdCar[i]->resetCountdown();   // Reset countdown
				}
				if (cdCar[i]->countingDown() == false) carState[i] = CarRotate; // If not counting down rotate
				break;

			case CarLeft:
				if (canRotate[i] == true) RotateWheels(i);
				if (car[i]->GetX() > maxLeftBoundary) car[i]->MoveX(-moveSpeed[i]); // Move cars left until reach lower bound
				else DownTheSlope(i);
				break;

			case CarRight:
				if (canRotate[i] == true) RotateWheels(i);
				if (car[i]->GetX() < maxRightBoundary) car[i]->MoveX(moveSpeed[i]); // Move all the cars right till reach upper bound
				else DownTheSlope(i);
				break;
			}
		}

		for (int i = 0; i < tyreAmount; i++)
		{
			switch (tyreState[i])
			{
			case TyreUpLeft: // Up and left	and keep moving left or change direction
				if (tyre[i]->GetY() < tyreGroundLevel) tyre[i]->Move(-tyreSpeed * speedModifier, tyreSpeed * speedModifier, floatZero); 														  
				(tyre[i]->GetX() > maxLeftBoundary) ? tyre[i]->MoveX(-tyreSpeed * speedModifier) : tyreState[i] = TyreDownLeft; 
				break;

			case TyreDownLeft: // Sink till at min level
				if (tyre[i]->GetY() >= tyreDrowned) tyre[i]->Move(-tyreSpeed * speedModifier, -tyreSpeed * speedModifier, floatZero); 
				else tyreState[i] = TyreResetPositions;
				break;

			case TyreResetPositions: // Reset tyres to their starting positions
				if (i < 3)	tyre[i]->SetPosition(maxRightBoundary, tyreDrowned, 75.0f);
				else if (i < 6)	tyre[i]->SetPosition(maxLeftBoundary, tyreDrowned, 85.0f);
				else if (i < 9) tyre[i]->SetPosition(maxRightBoundary, tyreDrowned, 95.0f);
				else            tyre[i]->SetPosition(maxLeftBoundary, tyreDrowned, 105.0f);
				if (i < 3 || i >= 6 && i < 9) tyreState[i] = TyreUpLeft;  // Assign direction
				else						  tyreState[i] = TyreUpRight; // Assign direction
				break;

			case TyreUpRight: // Up and right
				if (tyre[i]->GetY() < tyreGroundLevel) tyre[i]->Move(tyreSpeed * speedModifier, tyreSpeed * speedModifier, floatZero); 
				(tyre[i]->GetX() < maxRightBoundary) ? tyre[i]->MoveX(tyreSpeed * speedModifier) : tyreState[i] = TyreDownRight;
				break;

			case TyreDownRight: // Sink till at min level 
				if (tyre[i]->GetY() >= tyreDrowned) tyre[i]->Move(tyreSpeed * speedModifier, -tyreSpeed * speedModifier, floatZero); 
				else tyreState[i] = TyreResetPositions;
				break;
			}
		}
		frogInCarZone = frog[currentFrog]->GetZ() >= fICZMinZ && frog[currentFrog]->GetZ() <= fICZMaxZ; // Check if frog in car zone
		if (frogInCarZone)
		{
			frogStateTracker[currentFrog] = Crossing;
			for (int i = 0; i < carAmount; i++)
			{   // Put a bounding box around all of the cars
				carMinX[i] = car[i]->GetX() - carLengthRadius[i] - frogRadius;
				carMaxX[i] = car[i]->GetX() + carLengthRadius[i] + frogRadius;
				carMinZ[i] = car[i]->GetZ() - carWidthRadius[i] - frogRadius;
				carMaxZ[i] = car[i]->GetZ() + carWidthRadius[i] + frogRadius;

				carCollision = CollisionWithCar(i);
				if (carCollision) frogState = FrogSquashing;
			}
		}
		frogInTyreZone = frog[currentFrog]->GetZ() >= frogITZMinZ && frog[currentFrog]->GetZ() <= frogITZMaxZ; // Check if frog in tyre zone

		if (frogInTyreZone) // Check if frog is in the tyre zone (save processing power)
		{
			for (int i = 0; i < tyreAmount; i++)
			{   // Put a boundary around the tyres
				tyreMinX[i] = tyre[i]->GetX() - tyreRadius;
				tyreMaxX[i] = tyre[i]->GetX() + tyreRadius;
				tyreMinZ[i] = tyre[i]->GetZ() - tyreRadius;
				tyreMaxZ[i] = tyre[i]->GetZ() + tyreRadius;

				frogOnTyre = ContactWithTyre(i);
				switch (frogOnTyre)
				{
				case false: // Not on tyre and in tyre zone
					frogState = FrogSinking;
					break;

				case true: //  On tyre and in tyre zone
					if (frogState == FrogStill) frogState = AssignDirection();
					break;
				}
			}
		}
		else if (frogOnTyre) frogOnTyre = false; // No longer in zone so no longer on tyre

		numOfCollisions = 0; // Reset collisions
		for (int i = 0; i < totalNumOfTrees; i++) // Detecting tree collision
		{
			treeCollision = CollisionWithTree(i);
			if (treeCollision)
			{
				frogState = FrogStill; // Set frog to still to stop it spazzing out
				frogOnTyre = true;	   // Keep them on tyre
				frog[currentFrog]->SetPosition(lastX[currentFrog], lastY[currentFrog], lastZ[currentFrog]);
			}
		}

		if (!carCollision)
		{   // Frog leaping forwards
			if (myEngine->KeyHit(frogForwards))
			{   // Start the clock if not already running
				if (cdFrog[currentFrog]->countingDown() == false) cdFrog[currentFrog]->startCountdown();
				if (!treeCollision && !frogLeaping && frogState != FrogSinking)	
				{
					DummyRotation();
					RemoveFrogTyreMovement();
					frog[currentFrog]->ResetOrientation();
					EnterLeapingPhase();
				}
			}

			if (myEngine->KeyHit(frogBackwards))
			{	// Frog leaping backwards
				if (frog[currentFrog]->GetZ() > frogStartZ && !treeCollision && !frogLeaping && frogState != FrogSinking)
				{
					DummyRotation();
					RemoveFrogTyreMovement();
					dummy->RotateY(frogAngle * 2);
					frog[currentFrog]->ResetOrientation();
					frog[currentFrog]->RotateLocalY(frogAngle * 2);
					EnterLeapingPhase();
				}
			}

			if (myEngine->KeyHit(frogLeft))
			{	// Frog leaping left
				if (frog[currentFrog]->GetX() > frogMaxLeft && !frogLeaping && frogState != FrogSinking)
				{
					DummyRotation();
					dummy->RotateY(frogAngle);
					frog[currentFrog]->ResetOrientation();
					frog[currentFrog]->RotateLocalY(-frogAngle);
					EnterLeapingPhase();
				}
			}

			if (myEngine->KeyHit(frogRight))
			{	// Frog leaping right
				if (frog[currentFrog]->GetX() < frogMaxRight && !frogLeaping && frogState != FrogSinking)
				{
					DummyRotation();
					dummy->RotateY(-frogAngle);
					frog[currentFrog]->ResetOrientation();
					frog[currentFrog]->RotateLocalY(frogAngle);
					EnterLeapingPhase();
				}
			}

			if (myEngine->KeyHit(restartLevel))
			{	// Reset current frog to its starting position
				if (!frogLeaping && frogState != FrogSinking)
				{
					cdFrog[currentFrog]->stopCountdown();
					cdFrog[currentFrog]->resetCountdown();
					frog[currentFrog]->SetPosition(frogStartX + (currentFrog*frogDiameter), groundLevel, frogStartZ);
					DummyRotation();
				}
			}

			if (myEngine->KeyHit(resetCamera))
			{	// Reset camera rotation angle
				if (frogState == FrogStill || frogState == FrogMovingLeft || frogState == FrogMovingRight)
				{
					cameraRotating = cameraLookDown;	   // Reset rotation factor
					frog[currentFrog]->ResetOrientation(); // Reset frog positioning
					dummy->ResetOrientation();			   // Reset Orientation
					dummy->RotateLocalX(cameraLookDown);   // Look down slightly
				}
			}

			if (myEngine->KeyHeld(viewTiltUp)) // Camera arc movement
			{	// Tilt camera arc down
				if (cameraRotating < rotateLimitForwards) dummy->RotateLocalX(cameraRotateAmount);
				if (cameraRotating < rotateLimitForwards) cameraRotating += cameraRotateAmount;
			}
			if (myEngine->KeyHeld(viewTiltDown)) // Camera arc movement
			{	// Tilt camera arc up
				if (cameraRotating > rotateLimitBackwards) dummy->RotateLocalX(-cameraRotateAmount);
				if (cameraRotating > rotateLimitBackwards) cameraRotating -= cameraRotateAmount;
			}
		}
	}
	// Pause key
	if (myEngine->KeyHit(pauseGameKey))
	{
		gamePaused = !gamePaused; // Toggle pause
								  /* Stop counting down if has started */
		if (gamePaused && cdFrog[currentFrog]->countingDown() == true) cdFrog[currentFrog]->stopCountdown();
		// Return counting down when unpaused if it was started in first place
		else if (!gamePaused && cdFrog[currentFrog]->getTimeRemaining() < frogTimer)
		{
			cdFrog[currentFrog]->startCountdown();
		}
		if (gamePaused && !pauseCreated)
		{
			pause = myEngine->CreateSprite(pauseName, pauseSpriteX, pauseSpriteY);
			pauseCreated = true;
		}
		else if (!gamePaused && pauseCreated)
		{
			myEngine->RemoveSprite(pause);
			pauseCreated = false;
		} // Create and remove pause sprite when pressing P
	}
	return gameState;
}

void QuitGame(I3DEngine* myEngine)
{
	myEngine->Stop();
}