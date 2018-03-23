#include<iostream>
#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include<queue>
#include<string>
#include<RVO.h>
#include<RVOSimulator.h>
#include<Vector2.h>

using namespace std;

/*
Tasks to-do: 
1. BuildGrid and not use image. - Done
2.  . . . . .
	. . . O UP NOT UP-LEFT - Done
3. Make cells classes
*/

//SDL_Color textColor={0,0,0};

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 640
#define H_CELLS 32
#define V_CELLS 32
#define FONT_SIZE 18
#define CELL_HEIGHT SCREEN_HEIGHT/V_CELLS
#define CELL_WIDTH SCREEN_WIDTH/H_CELLS

//Some Global Algo. variables and classes
class point {
public:
	point(int a, int b);
	~point();
	int getx();
	int gety();
	void setx(int n);
	void sety(int n);
private:
	int x;
	int y;
};

class cell {
	public:
		int val;
		int dir;
};

cell Dijikastra[V_CELLS][H_CELLS];
queue<point> Queue;

void point::setx(int n) {
	x = n;
}
void point::sety(int n) {
	y = n;
}
point::point(int a, int b) {
	x = a;
	y = b;
}
point::~point() {

}
int point::getx() {
	return x;
}
int point::gety() {
	return y;
}
//Rendering Part Start

string goal_path = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\goal.png"; //Should be png
//string grid_path = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\grid.png"; //Should be png
string obstacle_path = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\obstacle.png"; //Should be png
string probe_path = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\probe.png";
string font_path = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\font.ttf";
string arrow_left = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_left.bmp";
string arrow_right = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_right.bmp";
string arrow_up = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_up.bmp";
string arrow_down = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_down.bmp";
string arrow_top_left = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_top_left.bmp";
string arrow_top_right = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_top_right.bmp";
string arrow_bottom_left = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_bottom_left.bmp";
string arrow_bottom_right = "C:\\Users\\ASUS\\Desktop\\Sem-4\\Openage\\Flow_field\\VStudio\\Project1\\Project1\\img\\arrow_bottom_right.bmp";


SDL_Window* gWindow = NULL;
SDL_Renderer* gBackground = NULL;
TTF_Font* gFont = NULL;

RVO::RVOSimulator* sim = new RVO::RVOSimulator();



/*SDL_Surface* gBackground = NULL;
SDL_Surface* gGoal = NULL;
SDL_Surface* gObstacle = NULL;

SDL_Renderer* gRenderer = NULL;
SDL_Texture* gGrid = NULL;*/

class myTexture {
	public:
		//Deallocates memory
		myTexture();

		//Destructor
		~myTexture();

		//PNG Loader
		bool pngTextureLoader(string path);

		//Text Loader
		bool textTextureLoader(string value, SDL_Color textColor);

		//BMP Loader
		bool bmpTextureLoader(string path);

		//Dellocates texture
		void free();

		//Renders texture at given point
		void render(int x, int y);

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//Actual Texture
		SDL_Texture* texture;

		//Image dimensions
		int width;
		int height;
};

myTexture::myTexture() {
	texture = NULL;
	width = 0;
	height = 0;
}

myTexture::~myTexture() {
	free();
}

bool myTexture::textTextureLoader(string value, SDL_Color textColor) {
	free();

	SDL_Surface* solidText = TTF_RenderText_Solid(gFont, value.c_str(), textColor);
	SDL_Texture* newTexture = NULL;

	if (!solidText) {
		cout << "Textloading failed: " << value.c_str() << " Error : " << TTF_GetError() << endl;
	}
	else {

		newTexture = SDL_CreateTextureFromSurface(gBackground, solidText);
		if (!newTexture) {
			cout << "Unable to Create Texture : " << font_path.c_str() << " Error : " << SDL_GetError() << endl;
		}
		else {
			width = solidText->w;
			height = solidText->h;
		}

		SDL_FreeSurface(solidText);
	}

	texture = newTexture;
	return texture != NULL;
}

bool myTexture::pngTextureLoader(string path) {

	free();

	SDL_Surface* loadPNG = IMG_Load(path.c_str());
	SDL_Texture* newTexture = NULL;

	if (!loadPNG) {
		cout << "SDL_image load failed: " << path.c_str() << " Error : " << IMG_GetError() << endl;
	}
	else {
		//Color Keying
		SDL_SetColorKey(loadPNG, SDL_TRUE, SDL_MapRGB(loadPNG->format, 0xFF, 0xFF, 0xFF));

		//Texture
		newTexture = SDL_CreateTextureFromSurface(gBackground, loadPNG);
		if (!newTexture) {
			cout << "Unable to Create Texture : " << path.c_str() << " Error : " << SDL_GetError() << endl;
		}
		else {
			width = loadPNG->w;
			height = loadPNG->h;
		}

		SDL_FreeSurface(loadPNG);
	}

	texture=newTexture;
	return texture != NULL;
}

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}

	return loadedSurface;
}

bool myTexture::bmpTextureLoader(string path) {

	free();

	SDL_Surface* loadBMP = loadSurface(path.c_str());
	SDL_Texture* newTexture = NULL;

	if (!loadBMP) {
		cout << "SDL BMP image load failed: " << path.c_str() << " Error : " << SDL_GetError() << endl;
	}
	else {
		//Color Keying
		SDL_SetColorKey(loadBMP, SDL_TRUE, SDL_MapRGB(loadBMP->format, 0xFF, 0xFF, 0xFF));

		//Texture
		newTexture = SDL_CreateTextureFromSurface(gBackground, loadBMP);
		if (!newTexture) {
			cout << "Unable to Create Texture : " << path.c_str() << " Error : " << SDL_GetError() << endl;
		}
		else {
			width = loadBMP->w;
			height = loadBMP->h;
		}

		SDL_FreeSurface(loadBMP);
	}

	texture = newTexture;
	return texture != NULL;
}

void myTexture::free() {
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void myTexture::render(int x, int y) {
	SDL_Rect renderQuad = { x,y,width,height };
	SDL_RenderCopy(gBackground, texture, NULL, &renderQuad);
}

int myTexture::getHeight() {
	return height;
}

int myTexture::getWidth() {
	return width;
}



myTexture gGoal;
myTexture gObstacle;
myTexture gProbe;
myTexture gText;
myTexture gArrow;

int goal_prev_x = -1;
int goal_prev_y = -1;//Stores last goal coordinates

vector<point> obstacle_list;
vector<point> probe_list;

//SDL_Surface* pngSurfaceLoader(string);
//SDL_Texture* pngTextureLoader(string);
void buildGrid(int S_H, int S_W, int num_horizontal, int num_vertical);
void resetDijikastra(point* goal_coordinates);

bool init() {
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gBackground = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_PRESENTVSYNC );
			if (!gBackground) {
				cout << "Renderer Initialization Failed : " << "Error : " << SDL_GetError() <<endl;
				success = false;
			}
			else {
				//Renderer Color
				SDL_SetRenderDrawColor(gBackground, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

			}
		}
	}
	return success;
}

bool loadMedia() {
	bool success = true;


	if (!gGoal.pngTextureLoader(goal_path)) {
		cout << "gGrid Load Failed, Error : " << SDL_GetError() << endl;
		success = false;
	}

	if (!gObstacle.pngTextureLoader(obstacle_path)) {
		cout << "gGrid Load Failed, Error : " << SDL_GetError() << endl;
		success = false;
	}

	if (!gProbe.pngTextureLoader(probe_path)) {
		cout << "gGrid Load Failed, Error : " << SDL_GetError() << endl;
		success = false;
	}

	gFont = TTF_OpenFont( font_path.c_str(), FONT_SIZE);
	if (!gFont) {
		cout << "Unable to load font Error : " << TTF_GetError() << endl;
		success = false;
	}
	

	return success;
}

void close() {
	
	//Free loaded images
	gArrow.free();
	gGoal.free();
	gObstacle.free();
	gProbe.free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window
	SDL_DestroyRenderer(gBackground);
	SDL_DestroyWindow(gWindow);

	gBackground = NULL;
	gWindow = NULL;

	//Quit
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void AddProbe(int x, int y) {

	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;

	x = (x / cell_width)*cell_width;
	y = (y / cell_height)*cell_height;

	point* temp = new point(x, y);

	probe_list.push_back(*temp);

	sim->addAgent(RVO::Vector2((float)x, (float)y));

	return;
}

void AddObstacle(int x, int y) {

	//cout << "x : " << x << " y : " << y << endl;

	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;

	x = (x / cell_width)*cell_width;
	y = (y / cell_height)*cell_height;

	//cout << "x : " << x << " y : " << y << endl;

	point* temp = new point(x, y);

	obstacle_list.push_back(*temp);

	std::vector<RVO::Vector2> obstacles;
	obstacles.push_back(RVO::Vector2((float)x, (float)y));

	sim->addObstacle(obstacles);

	sim->processObstacles();

	Dijikastra[x / cell_width][y / cell_height].val = -2;
	//buildGrid(SCREEN_HEIGHT, SCREEN_WIDTH, H_CELLS, V_CELLS);

	return;
}

void ChangeGoal(int new_x, int new_y) {

	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;

	new_x = (new_x / cell_width)*cell_width;
	new_y = (new_y / cell_height)*cell_height;

	goal_prev_x = new_x;
	goal_prev_y = new_y;

	return;
}

void mouseclick(SDL_MouseButtonEvent& e) {
	switch (e.button) {
		case SDL_BUTTON_LEFT:
			AddObstacle(e.x, e.y);
			break;
		case SDL_BUTTON_RIGHT:
			ChangeGoal(e.x, e.y);
			break;
	}
}

void buildGrid(int S_H, int S_W, int num_horizontal, int num_vertical) {
	SDL_SetRenderDrawColor(gBackground, 0x00, 0x00, 0x00, 0xFF);
	
	SDL_Rect outlineRect = { 0,0,S_W,S_H };
	SDL_RenderDrawRect(gBackground, &outlineRect);

	int cell_height = S_H / num_vertical;
	int cell_width = S_W / num_horizontal;

	int i;

	for (i = 1;i<num_horizontal;i++) {
		SDL_RenderDrawLine(gBackground, 0, i*cell_height, S_W, i*cell_height);
	}

	for (i = 1; i<num_vertical; i++) {
		SDL_RenderDrawLine(gBackground, i*cell_width, 0, i*cell_width, S_H);
	}

	return;

}

void renderDijikastra() {
	int i, j;
	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;
	for (i = 0; i<V_CELLS; i++) {
		for (j = 0; j<H_CELLS; j++) {
			string number = to_string(Dijikastra[i][j].val);

			SDL_Color textColor = { 0,0,0 };
			gText.textTextureLoader(number, textColor);
			gText.render(i*cell_width+(cell_width/4), j*cell_height);
		}
	}
	return;
}

bool isvalid(int a, int b) {
	bool success = true;
	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;
	int i = 0;
	int l = obstacle_list.size();
	//cout << l << endl;
	while (i < l) {
		//cout << "inside" << endl;
		if (obstacle_list[i].getx()/cell_width == a && obstacle_list[i].gety()/cell_height == b) {
			success = false;
			break;
		}
		i++;
	}
	/*if (a == 0 && b == 0) {
		cout << success << endl;
	}*/
	return success;
}

void giveDirection(int x, int y) {
	int minx = -1;
	int miny = -1;
	Dijikastra[x][y].dir = 0;
	
	int D[8];
	for (int y = 0;y<8;y++) {
		D[y] = 1;
	}

	if (!isvalid(x-1,y-1)) {
		D[7] = 0;
		D[0] = 0;
		D[1] = 0;
	}
	if (!isvalid(x - 1, y)) {
		//D[0] = 0;
		D[1] = 0;
		//D[2] = 0;
	}
	if (!isvalid(x - 1, y + 1)) {
		D[1] = 0;
		D[2] = 0;
		D[3] = 0;
	}
	if (!isvalid(x, y - 1)) {
		//D[2] = 0;
		D[3] = 0;
		//D[4] = 0;
	}
	if (!isvalid(x, y + 1)) {
		D[3] = 0;
		D[4] = 0;
		D[5] = 0;
	}
	if (!isvalid(x + 1, y - 1)) {
		//D[4] = 0;
		D[5] = 0;
		//D[6] = 0;
	}
	if (!isvalid(x + 1, y)) {
		D[5] = 0;
		D[6] = 0;
		D[7] = 0;
	}
	if (!isvalid(x + 1, y + 1)) {
		//D[6] = 0;
		D[7] = 0;
		//D[0] = 0;
	}

	if ((x - 1 >= 0) && (y - 1 >= 0) && D[0] && ((minx == -1 && miny == -1) || (Dijikastra[x - 1][y - 1].val < Dijikastra[minx][miny].val))) {
		minx = x - 1;
		miny = y - 1;
		Dijikastra[x][y].dir = 1;
	}
	if ((x - 1 >= 0) && (y  >= 0) && D[1] && ((minx == -1 && miny == -1) || (Dijikastra[x - 1][y].val<Dijikastra[minx][miny].val))) {
		minx = x - 1;
		miny = y;
		Dijikastra[x][y].dir = 0;
	}
	if((x - 1 >= 0) && (y + 1 >= 0) && D[2] && ((minx == -1 && miny == -1) || (Dijikastra[x - 1][y + 1].val<Dijikastra[minx][miny].val))) {
		minx = x - 1;
		miny = y + 1;
		Dijikastra[x][y].dir = 7;
	}
	if ((x>= 0) && (y - 1 >= 0) && D[3] && ((minx == -1 && miny == -1) || (Dijikastra[x][y - 1].val<Dijikastra[minx][miny].val))) {
		minx = x;
		miny = y - 1;
		Dijikastra[x][y].dir = 2;
	}
	if ((x>= 0) && (y + 1 >= 0) && D[4] && ((minx == -1 && miny == -1) || (Dijikastra[x][y + 1].val<Dijikastra[minx][miny].val))) {
		minx = x;
		miny = y + 1;
		Dijikastra[x][y].dir = 6;
	}
	if ((x + 1 >= 0) && (y + 1 >= 0) && D[5] && ((minx == -1 && miny == -1) || (Dijikastra[x + 1][y + 1].val<Dijikastra[minx][miny].val))) {
		minx = x + 1;
		miny = y + 1;
		Dijikastra[x][y].dir = 5;
	}
	if ((x + 1 >= 0) && (y >= 0) && D[6] && ((minx == -1 && miny == -1) || (Dijikastra[x + 1][y].val<Dijikastra[minx][miny].val))) {
		minx = x + 1;
		miny = y;
		Dijikastra[x][y].dir = 4;
	}
	if ((x + 1 >= 0) && (y - 1 >= 0) && D[7] && ((minx == -1 && miny == -1) || (Dijikastra[x + 1][y - 1].val<Dijikastra[minx][miny].val))) {
		minx = x + 1;
		miny = y - 1;
		Dijikastra[x][y].dir = 3;
	}

	switch (Dijikastra[x][y].dir) {
		case 0: gArrow.bmpTextureLoader(arrow_left); break;
		case 1: gArrow.bmpTextureLoader(arrow_top_left); break;
		case 2: gArrow.bmpTextureLoader(arrow_up); break;
		case 3: gArrow.bmpTextureLoader(arrow_top_right); break;
		case 4: gArrow.bmpTextureLoader(arrow_right); break;
		case 5: gArrow.bmpTextureLoader(arrow_bottom_right); break;
		case 6: gArrow.bmpTextureLoader(arrow_down); break;
		case 7: gArrow.bmpTextureLoader(arrow_bottom_left); break;
	}

	return;
}

void updateDirection() {
	int i, j;
	for (i = 0; i<V_CELLS; i++) {
		for (j = 0; j<H_CELLS; j++) {
			giveDirection(i, j);
		}
		//cout << ((float)i / (float)V_CELLS) << "%" << endl;
	}
	return;
}

void renderArrows() {
	int i, j;
	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;
	for (i = 0; i<V_CELLS; i++) {
		for (j = 0; j<H_CELLS; j++) {
			gArrow.free();
			gArrow.render(i*cell_width, j*cell_height);
		}
		//cout << ((float)i / (float)V_CELLS) << "%" << endl;
	}
	return;
}
//End of Renderer Part of the code


void resetDijikastra() {
	int cell_width = CELL_WIDTH;
	int cell_height = CELL_HEIGHT;
	int i,j;
	for (i = 0;i<V_CELLS;i++) {
		for (j = 0;j<H_CELLS;j++) {
			if (i == (goal_prev_x/cell_width) && j == (goal_prev_y/cell_height)) {
				Dijikastra[i][j].val = 0;
				//cout << "Dijikastra["<<i<<"]["<<j<<"] = "<< Dijikastra[i][j] << endl;
				while (!Queue.empty()) { Queue.pop(); };//clear the queue
				point* goal_coordinates = new point(i,j);
				Queue.push(*goal_coordinates);   
			}else if (Dijikastra[i][j].val!=-2) {
				Dijikastra[i][j].val = -1;
			}
		}
	}
	return;
}//Only resets the reachable positions.

void updatePoint(int x, int y, int distance) {
	if (x>=0 && x<=H_CELLS && y>=0 && y<=V_CELLS && Dijikastra[x][y].val==-1) {
		Dijikastra[x][y].val = distance + 1;
		point* temp = new point(x,y);
		Queue.push(*temp);
	}
	else {
		//cout << "x : "<<x<<" y : " << y << "point is not updatable" << endl;
	}
	return;
}

void performNextStepDijikastra() {
	if (Queue.empty()) {
		cout << "Queue is empty. No step to take" << endl;
		return;
	}
	else {
		point p = Queue.front();
		//cout << "goal : " << "( " << p.getx() << " , " << p.gety() << " ) -> "<< Dijikastra[0][0] << endl;
		Queue.pop();
		updatePoint(p.getx()-1, p.gety(), Dijikastra[p.getx()][p.gety()].val);
		updatePoint(p.getx(), p.gety()-1, Dijikastra[p.getx()][p.gety()].val);
		updatePoint(p.getx()+1, p.gety(), Dijikastra[p.getx()][p.gety()].val);
		updatePoint(p.getx(), p.gety()+1, Dijikastra[p.getx()][p.gety()].val);
		return;
	}
}

void buildDijikastra() {
	while (!Queue.empty()) {
		performNextStepDijikastra();
	}
}

//RVO Part of the code

void rvoSetupScenario(RVO::RVOSimulator* sim) {
	
	AddProbe(0, 0);
	AddProbe(300, 300);
	AddProbe(50, 20);
	AddProbe(10, 60);
	AddProbe(370, 480);
	AddProbe(500, 250);
	AddProbe(9, 0);
	AddProbe(300, 320);
	AddProbe(500, 210);
	AddProbe(90, 0);
	AddProbe(3, 390);
	AddProbe(0, 230);

	sim->setTimeStep(5.0f);

	sim->setAgentDefaults(15.0f, 10, 5.0f, 10.0f, 1.0f, 3.0f);

	int i = 0;
	int l = probe_list.size();
	while (i < l) {
		sim->addAgent(RVO::Vector2((float)probe_list[i].getx(), (float)probe_list[i].gety()));
		i++;
	}

	return;

}

void rvoSetPreferredVelocities(RVO::RVOSimulator* sim) {
	// Set the preferred velocity for each agent.
	size_t i = 0;
	for (; i < sim->getNumAgents(); i++) {
		int x = (int)sim->getAgentPosition(i).x();
		int y = (int)sim->getAgentPosition(i).y();

		if (x <= 0) {
			x = 0;
			if (y <= 0) {
				y = 0;
			}
			else if(y>=SCREEN_HEIGHT){
				y = SCREEN_HEIGHT;
			}
			//continue;
		}
		else if ( x >= SCREEN_WIDTH) {
			x = SCREEN_WIDTH;
			if (y <= 0) {
				y = 0;
			}
			else if (y>=SCREEN_HEIGHT) {
				y = SCREEN_HEIGHT;
			}
			//continue;
		}

		int cell_height = SCREEN_HEIGHT / V_CELLS;
		int cell_width = SCREEN_WIDTH / H_CELLS;

		switch (Dijikastra[x/cell_width][y/cell_height].dir) {
			case 0:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(-1.0f, 0.0f))); break;
			case 1:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(-1.0f, -1.0f))); break;
			case 2:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(0.0f, -1.0f))); break;
			case 3:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(1.0f, -1.0f))); break;
			case 4:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(1.0f, 0.0f)));  break;
			case 5:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(1.0f, 1.0f))); break;
			case 6:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(0.0f, 1.0f))); break;
			case 7:	sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(-1.0f, 1.0f))); break;
			default: sim->setAgentPrefVelocity(i, 5*normalize(RVO::Vector2(0.0f, 0.0f))); break;
		}
		//sim->setAgentMaxSpeed(i, 1.1f);
	}

	return;
}

bool reachedGoal(RVO::RVOSimulator* sim) {

	bool success = true;

	for (size_t i = 0;i<sim->getNumAgents();i++) {
		int x = (int)sim->getAgentPosition(i).x();
		int y = (int)sim->getAgentPosition(i).y();
		if (absSq(RVO::Vector2((float)goal_prev_x, (float)goal_prev_y) - sim->getAgentPosition(i)) > sim->getAgentRadius(i) * sim->getAgentRadius(i)) {
			success = false;
			break;
		}
	}

	return success;
}

void rvoUpdateVisualization(RVO::RVOSimulator* sim) {

	size_t i = 0;
	size_t l = sim->getNumAgents();
	for (; i < l ; i++) {
		RVO::Vector2 vec = sim->getAgentPosition(i);
		//cout << vec << endl;
		//cout << "probe_list_size is: " << probe_list.size() << endl;
		probe_list[i].setx((int)vec.x());
		probe_list[i].sety((int)vec.y());
	}
	
}

//End of Algorithmic Part

int main(int argc, char* args[]) {

	if (!init()) {
		cout << "Failed to initialize" << endl;
	}
	else {
		if (!loadMedia()) {
			cout << "Load Media Failed" << endl;
		}
		else {
			//Initial part for adding probes and all that...

			
			SDL_Event e;
			rvoSetupScenario(sim);

			bool quit = false;
			//int loop = 0;
			while (!quit) {
				//cout << loop++ << endl;
				//loop++;


				while (SDL_PollEvent(&e)) {

					switch (e.type) {

					case SDL_QUIT:
						quit = true;
						break;
					case SDL_MOUSEBUTTONDOWN:
						mouseclick(e.button);
						break;

					}

				}
				//End of Event Handling

				resetDijikastra();
				if (goal_prev_x >= 0 && goal_prev_y >= 0) {
					buildDijikastra();
				}

				updateDirection();


				//End of logic
				SDL_RenderClear(gBackground);

				SDL_SetRenderDrawColor(gBackground, 0x00, 0x00, 0x00, 0x00);
				//buildGrid(SCREEN_HEIGHT, SCREEN_WIDTH, H_CELLS, V_CELLS);

				size_t i = 0;
				while (i < obstacle_list.size()) {
					gObstacle.render(obstacle_list[i].getx(), obstacle_list[i].gety());
					i++;
				}

				//renderDijikastra();
				//renderArrows();

				if (!reachedGoal(sim)) {
					//cout << "not reached" << endl;
					rvoSetPreferredVelocities(sim);
					sim->doStep();
				}
				rvoUpdateVisualization(sim);

				size_t u = 0;
				while (u < probe_list.size()) {
					gProbe.render(probe_list[u].getx(), probe_list[u].gety());
					u++;
				}

				if (goal_prev_x>=0&&goal_prev_y>=0) {
					gGoal.render(goal_prev_x, goal_prev_y);
				}

				SDL_SetRenderDrawColor(gBackground, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderPresent(gBackground);
			}
		}
	}


	close();
	return 0;
}