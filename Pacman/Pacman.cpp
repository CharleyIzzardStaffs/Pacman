#include "Pacman.h"
#include <iostream>
#include <sstream>
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), playerSpeed(0.1f), playerFrameTime(250), _cMunchieFrameTime(500)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		munchie[i] = new collectable;
		munchie[i]->collectableCurrentFrameTime = 0;
		munchie[i]->collectableFrameCount = 0;
		munchie[i]->collectableTexture;
	}
	cherry = new collectable;
	cherry->collectableFrameCount = 0;
	cherry->collectableCurrentFrameTime = 0;
	pacman = new player;
	pacman->playerDirection = 0;
	pacman->playerCurrentFrameTime = 0;
	pacman->playerFrame = 0;
	pacman->speedMultiplier = 1.0f; 
	pacman->dead = false;
	
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		ghosts[i] = new movingEnemy();
		ghosts[i]->direction = 0;
		ghosts[i]->speed = 0.05f;
	}
	pop = new SoundEffect;
	death = new SoundEffect;
	teleport = new SoundEffect;
	paused = false;
	startMenu = true;
	score = 0;
	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}
Pacman::~Pacman()
{
	delete pacman->playerTexture;
	delete pacman->playerSourceRect;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete munchie[i]->collectableTexture;
		delete munchie[i]->collectableSourceRect;
		delete munchie[i];
	}
	delete[] munchie;
	delete cherry->collectableTexture;
	delete cherry->collectableSourceRect;
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		ghosts[i]->direction;
		ghosts[i]->speed;
		ghosts[i];
	}
	delete[]ghosts;
	delete pop;
}
void Pacman::LoadContent()
{
	// Load Pacman
	pacman->playerTexture = new Texture2D();
	pacman->playerTexture->Load("Textures/Pacman.tga", false);
	pacman->playerPosition = new Vector2(350.0f, 350.0f);
	pacman->playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	//sounds
	pop->Load("Sounds/pop.wav");
	death->Load("sounds/PacmanDeath.Wav");
	teleport->Load("sounds/teleport.wav");
	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		munchie[i]->collectableTexture = new Texture2D();
		munchie[i]->collectableTexture->Load("Textures/Munchie.png", true);
		munchie[i]->collectablePosition = new Vector2((rand() % Graphics::GetViewportHeight()), (rand() % Graphics::GetViewportHeight()));
		munchie[i]->collectableSourceRect = new Rect(0.0f, 0.0f, 12, 12);
	}

	//load cherry
	cherry->collectableTexture = new Texture2D();
	cherry->collectableTexture->Load("Textures/CherrySpriteSheet.png", true);
	cherry->collectablePosition = new Vector2(50.0f, 450.0f);
	cherry->collectableSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// other
	_stringPosition = new Vector2(10.0f, 25.0f);
	
	// Set Menu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(),
		Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f,
		Graphics::GetViewportHeight() / 2.0f);
	//load ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		ghosts[i]->texture = new Texture2D();
		ghosts[i]->texture->Load("Textures/GhostTextures.png", false);
		ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}
}
void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	//Input::MouseState* mouseState = Input::Mouse::GetState();
	if (keyboardState->IsKeyDown(Input::Keys::P) && !pkeyDown)
	{
		pkeyDown = true;
		paused = !paused;
	}
	if (keyboardState->IsKeyUp(Input::Keys::P))
		pkeyDown = false;

	if (!paused)
	{
		UpdateCollectables(elapsedTime);
		UpdatePacman(elapsedTime);
		CheckGhostCollisions();
		Input(elapsedTime, keyboardState /*, mouseState*/);
		for (int i = 0; i < GHOSTCOUNT; i++)
			UpdateGhost(ghosts[i], elapsedTime);
		for (int i = 0; i < MUNCHIECOUNT; i++)
			CheckMunchieCollsions();
	}
	if (!Audio::IsInitialised())
		std::cout << "audio is not initialised" << endl;
	if (!pop->IsLoaded())
		std::cout << "pop is not loaded" << endl;
}
void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score: " << score;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!pacman->dead)
	{
		SpriteBatch::Draw(pacman->playerTexture, pacman->playerPosition, pacman->playerSourceRect); // Draws Pacman
	}
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(munchie[i]->collectableTexture, munchie[i]->collectablePosition, munchie[i]->collectableSourceRect);
	}
	SpriteBatch::Draw(cherry->collectableTexture, cherry->collectablePosition, cherry->collectableSourceRect);
	if (paused) //draws pause screen
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}	
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(ghosts[i]->texture, ghosts[i]->position, ghosts[i]->sourceRect);
	}
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}
void Pacman::Input(int elapsedTime, Input::KeyboardState* state /*, Input::MouseState* mouseState*/)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	/*Input::MouseState* mouseState = Input::Mouse::GetState();*/
	float pacmanSpeed = playerSpeed * elapsedTime * pacman->speedMultiplier;
	if (!paused)
	{
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			pacman->playerPosition->X += pacmanSpeed;
			pacman->playerDirection = 0;
		}
		else if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			pacman->playerPosition->X -= pacmanSpeed;
			pacman->playerDirection = 2;
		}
		else if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			pacman->playerPosition->Y += pacmanSpeed;
			pacman->playerDirection = 1;
		}
		else if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			pacman->playerPosition->Y -= pacmanSpeed;
			pacman->playerDirection = 3;
		}
		if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
		{
			pacman->speedMultiplier = 2.0f;
		}
		else
		{
			pacman->speedMultiplier = 1.0f;
		}
		/*if ((keyboardState->IsKeyDown(Input::Keys::R)))
		{
			cherry->collectablePosition->X = (rand() % Graphics::GetViewportWidth());
			cherry->collectablePosition->Y = (rand() % Graphics::GetViewportHeight());
		}*/
	}
}
void Pacman::UpdateCollectables(int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		munchie[i]->collectableCurrentFrameTime += elapsedTime;
		if (munchie[i]->collectableCurrentFrameTime > _cMunchieFrameTime)
		{
			munchie[i]->collectableFrameCount++;
			if (munchie[i]->collectableFrameCount >= 2)
				munchie[i]->collectableFrameCount = 0;
			munchie[i]->collectableCurrentFrameTime = 0;
			munchie[i]->collectableSourceRect->X = munchie[i]->collectableSourceRect->Width * munchie[i]->collectableFrameCount;
		}
	}
	cherry->collectableCurrentFrameTime += elapsedTime;
	if (cherry->collectableCurrentFrameTime > _cMunchieFrameTime)
	{
		cherry->collectableFrameCount++;
		if (cherry->collectableFrameCount >= 2)
			cherry->collectableFrameCount = 0;
		cherry->collectableCurrentFrameTime = 0;
		cherry->collectableSourceRect->X = cherry->collectableSourceRect->Width * cherry->collectableFrameCount;
	}
}
void Pacman::UpdatePacman(int elapsedTime)
{
	pacman->playerCurrentFrameTime += elapsedTime;
	if (pacman->playerCurrentFrameTime > playerFrameTime)
	{
		pacman->playerFrame++;
		if (pacman->playerFrame >= 2)
			pacman->playerFrame = 0;
		pacman->playerCurrentFrameTime = 0;
		pacman->playerSourceRect->X = pacman->playerSourceRect->Width * pacman->playerFrame;
	}
	if (pacman->playerPosition->X > 1024)
	{
		pacman->playerPosition->X = 0;
		Audio::Play(teleport);
	}
	if (pacman->playerPosition->X < 0)
	{
		pacman->playerPosition->X = 1024;
		Audio::Play(teleport);
	}
	if (pacman->playerPosition->Y > 768)
	{
		pacman->playerPosition->Y = 0;
		Audio::Play(teleport);
	}
	if (pacman->playerPosition->Y < 0)
	{
		pacman->playerPosition->Y = 768;
		Audio::Play(teleport);
	}
	pacman->playerSourceRect->Y = pacman->playerSourceRect->Height * pacman->playerDirection;
}
void Pacman::UpdateGhost(movingEnemy* ghost, int elapsedTime)
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (ghost->direction == 0)
			ghost->position->X += ghost->speed * elapsedTime;

		else if (ghost->direction == 1)
			ghost->position->X -= ghost->speed * elapsedTime;

		if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
			ghost->direction = 1;

		else if (ghost->position->X <= 0)
			ghost->direction = 0;
	}
}
void Pacman::CheckGhostCollisions()
{
	int i = 0;
	int bottom1 = pacman->playerPosition->Y + pacman->playerSourceRect->Height;
	int bottom2 = 0;
	int top1 = pacman->playerPosition->Y;
	int top2 = 0;
	int left1 = pacman->playerPosition->X;
	int left2 = 0;
	int right1 = pacman->playerPosition->X + pacman->playerSourceRect->Width;
	int right2 = 0;
	for (i = 0; i < GHOSTCOUNT; i++)
	{
		bottom2 = ghosts[i]->position->Y + ghosts[i]->sourceRect->Height;
		left2 = ghosts[i]->position->X;
		right2 = ghosts[i]->position->X + ghosts[i]->sourceRect->Width;
		top2 = ghosts[i]->position->Y;
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			pacman->dead = true;
			Audio::Play(death);
			i = GHOSTCOUNT;
		}
	}
}
void Pacman::CheckMunchieCollsions()
{
	int i = 0;
	int bottom1 = pacman->playerPosition->Y + pacman->playerSourceRect->Height;
	int bottom2 = 0;
	int top1 = pacman->playerPosition->Y;
	int top2 = 0;
	int left1 = pacman->playerPosition->X;
	int left2 = 0;
	int right1 = pacman->playerPosition->X + pacman->playerSourceRect->Width;
	int right2 = 0;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		bottom2 = munchie[i]->collectablePosition->Y + munchie[i]->collectableSourceRect->Height;
		left2 = munchie[i]->collectablePosition->X;
		right2 = munchie[i]->collectablePosition->X + munchie[i]->collectableSourceRect->Width;
		top2 = munchie[i]->collectablePosition->Y;
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			munchie[i]->collectablePosition->X = -100;
			Audio::Play(pop);
			i = MUNCHIECOUNT;
			score += 1;
		}
	}
	bottom2 = cherry->collectablePosition->Y + cherry->collectableSourceRect->Height;
	left2 = cherry->collectablePosition->X;
	right2 = cherry->collectablePosition->X + cherry->collectableSourceRect->Width;
	top2 = cherry->collectablePosition->Y;
	if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
	{
		cherry->collectablePosition->X = (rand() % Graphics::GetViewportWidth());
		cherry->collectablePosition->Y = (rand() % Graphics::GetViewportHeight());
		Audio::Play(pop);
		score += 5;
	}
}