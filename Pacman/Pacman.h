#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define MUNCHIECOUNT 50
#define GHOSTCOUNT 4
// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
struct player
{
	bool dead;
	Vector2* playerPosition;
	Rect* playerSourceRect;
	Texture2D* playerTexture;
	int playerDirection;
	int playerFrame;
	int playerCurrentFrameTime;
	float speedMultiplier;
};
struct collectable
{
	Vector2* collectablePosition;
	Rect* collectableSourceRect;
	Texture2D* collectableTexture;
	int collectableFrameCount;
	int collectableFrame;
	int collectableCurrentFrameTime;
};
struct movingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

class Pacman : public Game
{
private:
	//structs stuff
	player* pacman;
	collectable* munchie[MUNCHIECOUNT];
	collectable* cherry;
	movingEnemy* ghosts[GHOSTCOUNT];
	SoundEffect* pop;
	SoundEffect* death;
	SoundEffect* teleport;
	//Input methods
	void Input(int elapsedTime, Input::KeyboardState* state /*, Input::MouseState* mouseState*/);
	//Check methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();
	void CheckGhostCollisions();
	void CheckMunchieCollsions();
	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateCollectables(int elapsedTime);
	void UpdateGhost(movingEnemy*, int elapsedTime);
	//Constant data for Game Variables
	const float playerSpeed;
	const int playerFrameTime;
	const int _cMunchieFrameTime;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool paused;
	bool pkeyDown;
	int score;
	bool startMenu;
	//other things
	Vector2* _stringPosition;

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};

