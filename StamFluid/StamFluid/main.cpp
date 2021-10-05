#include <iostream>
#include <SDL.h>
#include "solver.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/* macros */

#define IX(i,j) ((i)+(N+2)*(j))


int N, h;
float *u, *v, *u0, *v0;
float *dens, *dens0;
float dt, diff, visc;
float force, source;
SDL_Window *window;
SDL_Renderer *renderer;
int mouseX, mouseY;
int oldMouseX = -1;
int oldMouseY;
bool quit = false;
bool addDensity = false;
bool addForce = false;
bool densityMode = true;
bool stop = true;

void input(float *d, float *paramU, float *paramV) {
	SDL_Event e;
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);
	int i, j, size = (N + 2) * (N + 2);
	i = (int)(mouseX / h);
	j = (int)(mouseY / h);
	for (int i = 0; i < size; i++) {
		paramU[i] = paramV[i] = d[i] = 0.0f;
	}
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			quit = true;
		}
		SDL_GetMouseState(&mouseX, &mouseY);
		if (keystates[SDL_SCANCODE_C]) {
			for (int i = 0; i < size; i++) {
				u[i] = v[i] = u0[i] = v0[i] = dens[i] = 0.0f;
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
		}
		if (keystates[SDL_SCANCODE_D]) {
			densityMode = true;
		}
		if (keystates[SDL_SCANCODE_V]) {
			densityMode = false;
		}
		if (keystates[SDL_SCANCODE_SPACE]) {
			stop = true;
		}
		if (keystates[SDL_SCANCODE_S]) {
			stop = false;
		}
		if (!stop) {
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					addDensity = true;
				}
				if (e.button.button == SDL_BUTTON_RIGHT && oldMouseX == -1) {
					oldMouseX = mouseX;
					oldMouseY = mouseY;
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					addDensity = false;
				}
				if (e.button.button == SDL_BUTTON_RIGHT) {
					addForce = true;
				}
			}
			if (addDensity) {
				d[IX(i, j)] = source;
				for (int p = 0; p < 6; p++) {
					if (i + p > N || i - p < 1) continue;
					for (int p2 = 0; p2 < 6; p2++) {
						if (j + p2 > N || j - p2 < 1) continue;
						int d1, d2, d3, d4;
						d1 = IX(i - p, j + p2);
						d2 = IX(i + p, j + p2);
						d3 = IX(i - p, j - p2);
						d4 = IX(i + p, j - p2);
						d[d1] = d[d2] = d[d3] = d[d4] = source;
					}
				}
			}
			if (addForce) {
				int i = (int)(oldMouseX / h);
				int j = (int)(oldMouseY / h);
				paramU[IX(i, j)] = force * (mouseX - oldMouseX)/h;
				paramV[IX(i, j)] = force * (mouseY - oldMouseY)/h;
				addForce = false;
				oldMouseX = -1;
			}
		}
	}
}

void drawDensity() {
	float x, y, d;
	for (int i = 0; i <= N; i++) {
		x = i * h;
		for (int j = 0; j <= N; j++) {
			y = j * h;
			d = dens[IX(i, j)];
			if (d > 255)
				d = 255;
			SDL_Rect fillRect = { x,y,h,h };
			SDL_SetRenderDrawColor(renderer,d,d,d,255);
			SDL_RenderFillRect(renderer, &fillRect);
		}
	}
}

void drawVelocity() {
	float x, y, forceX, forceY;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	for (int i = 0; i <= N; i++) {
		x = i * h;
		for (int j = 0; j <= N; j++) {
			y = j * h;
			forceX = u[IX(i, j)];
			forceY = v[IX(i, j)];
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawLine(renderer, x, y, x + forceX*h, y + forceY*h);
		}
	}
}

void init() {
	//read config file
	std::ifstream file("../../fluid config/config.csv");
	// Make sure the file is open
	if (!file.is_open()) throw std::runtime_error("Could not open file");
	std::string line, val;
	// Get meta info
	if (file.good())
	{
		std::getline(file, line);
		std::stringstream ss(line);
		std::vector<std::string> res;
		while (std::getline(ss, val, ',')) {
			res.push_back(val);
		}
		N = stoi(res[0]), h = stoi(res[1]), dt = stof(res[2]);
		diff = stof(res[3]), visc = stof(res[4]), force = stof(res[5]), source = stof(res[6]);
	}
	int size = (N + 2) * (N + 2);
	u = new float[size]();
	v = new float[size]();
	u0 = new float[size]();
	v0 = new float[size]();
	dens = new float[size]();
	dens0 = new float[size]();
	float temp[3];
	int i = 0;
	float *ptr = u;
	while (std::getline(file, line)) {
		int count = stoi(line);
		for (count; count > 0; count--) {
			std::getline(file, line);
			std::stringstream ss(line);
			for (int i = 0; i < 3; i++) {
				std::getline(ss, val, ',');
				temp[i] = stof(val);
			}
			ptr[IX((int)temp[0], (int)temp[1])] = temp[2];
		}
		i++;
		if (i == 1) { //x component
			ptr = v;
		}
		else{ //density
			ptr = dens;
		}
	}
	file.close();
	SDL_CreateWindowAndRenderer((N + 2) * h, (N + 2) * h, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

int main(int argc, char *args[]) {
	init();
	while (!quit) {
		input(dens0, u0, v0);
		if (!stop) {
			velocStep(N, h, u, v, u0, v0, visc, dt);
			densStep(N, dens, dens0, u, v, diff, dt);
		}
		if (densityMode) {
			drawDensity();
		}
		else {
			drawVelocity();
		}
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderPresent(renderer);
	}

	return 0;
}