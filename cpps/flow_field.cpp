#include<iostream>
#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include<queue>
#include<string>

using namespace std;

/*
Tasks to-do: 
1. BuildGrid and not use image.
2. 
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
private:
	int x;
	int y;
};

int Dijikastra[V_CELLS][H_CELLS];
queue<point> Queue;


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


SDL_Window* gWindow = NULL;
SDL_Renderer* gBackground = NULL;
TTF_Font* gFont = NULL;

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
myTexture gText;
myTexture probe;

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
			gBackground = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED );
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

	gFont = TTF_OpenFont( font_path.c_str(), FONT_SIZE);
	if (!gFont) {
		cout << "Unable to load font Error : " << TTF_GetError() << endl;
		success = false;
	}
	

	return success;
}

void close() {
	
	//Free loaded images
	gGoal.free();
	gObstacle.free();

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

/*SDL_Surface* pngSurfaceLoader(string path) {

	SDL_Surface* loadPNG = IMG_Load(path.c_str());
	SDL_Surface* optimizedPng = NULL;

	if(!loadPNG){
		cout << "SDL_image load failed: " << path.c_str() << " Error : " << IMG_GetError() << endl;
	}
	else {
		optimizedPng = SDL_ConvertSurface(loadPNG, gBackground->format, NULL);

		if (!optimizedPng) {
			cout << "Unable to Optimize Surface : " << path.c_str() << " Error : " << SDL_GetError() << endl;
		}

		SDL_FreeSurface(loadPNG);
	}

	return optimizedPng;

}

SDL_Texture* pngTextureLoader(string path) {

	SDL_Surface* loadPNG = IMG_Load(path.c_str());
	SDL_Texture* newTexture = NULL;

	if (!loadPNG) {
		cout << "SDL_image load failed: " << path.c_str() << " Error : " << IMG_GetError() << endl;
	}
	else {
		newTexture = SDL_CreateTextureFromSurface(gBackground, loadPNG);

		if (!newTexture) {
			cout << "Unable to Create Texture : " << path.c_str() << " Error : " << SDL_GetError() << endl;
		}

		SDL_FreeSurface(loadPNG);
	}

	return newTexture;

}*/

void AddObstacle(int x, int y) {

	//cout << "x : " << x << " y : " << y << endl;

	int cell_height = SCREEN_HEIGHT / V_CELLS;
	int cell_width = SCREEN_WIDTH / H_CELLS;

	x = (x / cell_width)*cell_width;
	y = (y / cell_height)*cell_height;

	//cout << "x : " << x << " y : " << y << endl;

	point* temp = new point(x, y);

	obstacle_list.push_back(*temp);

	Dijikastra[x / cell_width][y / cell_height] = -2;
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
			string number = to_string(Dijikastra[i][j]);

			SDL_Color textColor = { 0,0,0 };
			gText.textTextureLoader(number, textColor);
			gText.render(i*cell_width+(cell_width/4), j*cell_height);
		}
	}
	return;
}
//End of Renderer Part of the code


void resetDijikastra() {
	//cout << "resetting..." << endl;
	//cout << "goal_prev : " << "( " << goal_prev_x << " , " << goal_prev_y << " )  " << endl;
	int cell_width = CELL_WIDTH;
	int cell_height = CELL_HEIGHT;
	//cout << "ij : " << "( " << (goal_prev_x / cell_width) << " , " << (goal_prev_y / cell_height) << " )" << endl;
	int i,j;
	for (i = 0;i<V_CELLS;i++) {
		for (j = 0;j<H_CELLS;j++) {
			if (i == (goal_prev_x/cell_width) && j == (goal_prev_y/cell_height)) {
				Dijikastra[i][j] = 0;
				//cout << "Dijikastra["<<i<<"]["<<j<<"] = "<< Dijikastra[i][j] << endl;
				while (!Queue.empty()) { Queue.pop(); };//clear the queue
				point* goal_coordinates = new point(i,j);
				Queue.push(*goal_coordinates);   
			}else if (Dijikastra[i][j]!=-2) {
				Dijikastra[i][j] = -1;
			}
		}
	}
	return;
}//Only resets the reachable positions.

void updatePoint(int x, int y, int distance) {
	if (x>=0 && x<=H_CELLS && y>=0 && y<=V_CELLS && Dijikastra[x][y]==-1) {
		Dijikastra[x][y] = distance + 1;
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
		updatePoint(p.getx()-1, p.gety(), Dijikastra[p.getx()][p.gety()]);
		updatePoint(p.getx(), p.gety()-1, Dijikastra[p.getx()][p.gety()]);
		updatePoint(p.getx()+1, p.gety(), Dijikastra[p.getx()][p.gety()]);
		updatePoint(p.getx(), p.gety()+1, Dijikastra[p.getx()][p.gety()]);
		return;
	}
}

void buildDijikastra() {
	while (!Queue.empty()) {
		performNextStepDijikastra();
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


			SDL_Event e;
			bool quit = false;
			//int loop = 0;
			while (!quit) {
				//cout << loop++ << endl;

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
				if (goal_prev_x>=0 && goal_prev_y>=0) {
					buildDijikastra();
				}
				
				//End of logic
				SDL_RenderClear(gBackground);

				SDL_SetRenderDrawColor(gBackground, 0x00, 0x00, 0x00, 0x00);
				buildGrid(SCREEN_HEIGHT, SCREEN_WIDTH, H_CELLS, V_CELLS);

				int i = 0;
				while (i < obstacle_list.size()) {
					gObstacle.render(obstacle_list[i].getx(), obstacle_list[i].gety());
					i++;
				}

				renderDijikastra();

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