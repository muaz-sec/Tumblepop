#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}
}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;
	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

	if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
	{
		onGround = true;
	}
	else
	{
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}
	else
	{
		velocityY = 0;
	}
}


int main()
{
	cout << "=== Program Starting ===" << endl;
	cout << "Creating window..." << endl;

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	cout << "Window created successfully!" << endl;

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;
	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 500;
	float player_y = 150;
	float speed = 5;

	const float jumpStrength = -20;
	const float gravity = 1;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;
	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;
	float terminal_Velocity = 20;

	int PlayerHeight = 102;
	int PlayerWidth = 96;

	PlayerTexture.loadFromFile("Data/player.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(3,3);
	PlayerSprite.setPosition(player_x, player_y);

	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
		for (int j = 0; j < width; j += 1)
		{
			lvl[i][j] = ' ';
		}
	}

	// Create ground floor
	for (int j = 0; j < width; j++)
	{
		lvl[13][j] = '#';
	}

	// Create platforms
	for (int j = 3; j < 8; j++)
	{
		lvl[10][j] = '#';
	}

	for (int j = 10; j < 15; j++)
	{
		lvl[10][j] = '#';
	}

	for (int j = 5; j < 13; j++)
	{
		lvl[7][j] = '#';
	}

	// Create walls
	for (int i = 0; i < height; i++)
	{
		lvl[i][0] = '#';
		lvl[i][width-1] = '#';
	}

	Event ev;
	cout << "Entering main game loop..." << endl;

	//main loop
	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}
		}

		//pressing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Apply gravity
		player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);

		// LEFT MOVEMENT
		if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
		{
			offset_x = player_x - speed;
			
			char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
			char left_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(offset_x) / cell_size];
			char left_bottom = lvl[(int)(player_y + PlayerHeight) / cell_size][(int)(offset_x) / cell_size];
			
			if (left_top != '#' && left_mid != '#' && left_bottom != '#')
			{
				player_x = offset_x;
			}
		}

		// RIGHT MOVEMENT
		if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
		{
			offset_x = player_x + speed;
			
			char right_top = lvl[(int)(player_y) / cell_size][(int)(offset_x + PlayerWidth) / cell_size];
			char right_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(offset_x + PlayerWidth) / cell_size];
			char right_bottom = lvl[(int)(player_y + PlayerHeight) / cell_size][(int)(offset_x + PlayerWidth) / cell_size];
			
			if (right_top != '#' && right_mid != '#' && right_bottom != '#')
			{
				player_x = offset_x;
			}
		}

		// JUMPING
		if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Space)) && onGround)
		{
			velocityY = jumpStrength;
			onGround = false;
		}

		// Keep player within screen bounds
		if (player_x < 0) player_x = 0;
		if (player_x + PlayerWidth > screen_x) player_x = screen_x - PlayerWidth;

		// Rendering
		window.clear();
		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		PlayerSprite.setPosition(player_x, player_y);
		window.draw(PlayerSprite);
		window.display();
	}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	cout << "Game loop ended" << endl;
	cout << "Press Enter to exit..." << endl;
	cin.ignore();
	cin.get();

	return 0;
}
