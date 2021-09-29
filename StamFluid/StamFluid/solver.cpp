#include "solver.h"
#include <iostream>

#define IX(i,j,n) ((i) + (n+2)*(j))
#define SWAP(x, x0) {float *tmp = x; x = x0; x0 = tmp;}

void add_source(int n, float *dens, float *s, float dt) {
	for (int i = 0; i < (n + 2) * (n + 2); i++) dens[i] += dt * s[i];
}

void set_bnd(int n, int b, float *x, short *boundary) {
	int i;
	for (i = 1; i <= n; i++) {
		x[IX(0, i, n)] = b == 1 ? -x[IX(1, i, n)] : x[IX(1, i, n)];
		x[IX(n + 1, i, n)] = b == 1 ? -x[IX(n, i, n)] : x[IX(n, i, n)];
		x[IX(i, 0 , n)] = b == 2 ? -x[IX(i, 1, n)] : x[IX(i, 1, n)];
		x[IX(i, n + 1, n)] = b == 2 ? -x[IX(i, n, n)] : x[IX(i, n, n)];
	}
	if (boundary != nullptr) {
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= n; j++) {
				if (b == 0 && boundary[IX(i, j)] != 0) {
					x[IX(i, j, n)] = 0;
				}
				else {
					switch (boundary[IX(i, j)]) {
					case 1:
						x[IX(i, j, n)] = b == 1 ? -x[IX(i + 1, j, n)] : -x[IX(i, j - 1, n)];
						break;
					case 2:
						x[IX(i, j, n)] = b == 1 ? -x[IX(i - 1, j, n)] : -x[IX(i, j - 1, n)];
						break;
					case 3:
						x[IX(i, j, n)] = b == 1 ? -x[IX(i - 1, j, n)] : -x[IX(i, j + 1, n)];
						break;
					case 4:
						x[IX(i, j, n)] = b == 1 ? -x[IX(i + 1, j, n)] : -x[IX(i, j + 1, n)];
						break;
					}
				}
			}

			}
		}
	x[IX(0, 0, n)] = 0.5 * (x[IX(1, 0, n)] + x[IX(0, 1, n)]);
	x[IX(0, n + 1, n)] = 0.5 * (x[IX(1, n + 1, n)] + x[IX(0, n, n)]);
	x[IX(n + 1, 0, n)] = 0.5 * (x[IX(n, 0, n)] + x[IX(n + 1, 1, n)]);
	x[IX(n + 1,n + 1, n)] = 0.5 * (x[IX(n, n + 1, n)] + x[IX(n + 1, n)]);
}

void diffuse(int n, int b, float *dens, float *dens0, float rate, float dt, short *boundary) {
	float amt = dt * rate * n * n;
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= n; j++) {
				dens[IX(i, j, n)] = (dens0[IX(i, j, n)] + amt * (dens[IX(i - 1, j, n)] + dens[IX(i + 1, j, n)] + dens[IX(i, j - 1, n)] + dens[IX(i, j + 1, n)])) / (1 + 4 * amt);
			}
		}
	}
	set_bnd(n, b, dens, boundary);
}

void advect(int n, int b, float *dens, float *dens0, float *u, float *v, float dt, short *boundary) {
	float dt0 = dt * n;
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			float x0 = i - dt0 * u[IX(i, j, n)]; float y0 = j - dt0 * v[IX(i, j, n)];
			if (x0 < 0) x0 = 0; if (x0 > n + 1) x0 = n + 1;
			if (y0 < 0) y0 = 0; if (y0 > n + 1) y0 = n + 1;
			int i0 = (int)x0; int i1 = i0 + 1; int j0 = (int)y0; int j1 = j0 + 1;
			float s1 = x0 - i0; float s0 = 1 - s1; float t1 = y0 - j0; float t0 = 1 - t1;
			dens[IX(i, j, n)] = s0 * (t0 * dens0[IX(i0, j0, n)] + t1 * dens0[IX(i0, j1, n)]) +
				s1 * (t0 * dens0[IX(i1, j0, n)] + t1 * dens0[IX(i1, j1, n)]);
		}
	}
	set_bnd(n, b, dens, boundary);
}

void project(int n, int h, float *u, float *v, float *p, float *div, short *boundary) {
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			div[IX(i, j, n)] = -0.5 * h * (u[IX(i + 1, j, n)] - u[IX(i - 1, j, n)] + v[IX(i, j + 1, n)] - v[IX(i, j - 1, n)]);
			p[IX(i, j, n)] = 0;
		}
	}
	set_bnd(n, 0, div, boundary); set_bnd(n, 0, p, boundary);
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= n; j++) {
				p[IX(i, j, n)] = (div[IX(i, j, n)] + p[IX(i - 1, j, n)] + p[IX(i + 1, j, n)] +
					p[IX(i, j - 1, n)] + p[IX(i, j + 1, n)]) / 4;
			}
		}
		set_bnd(n, 0, p, boundary);
	}
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			u[IX(i, j, n)] -= 0.5 * (p[IX(i + 1, j, n)] - p[IX(i - 1, j, n)]) / h;
			v[IX(i, j, n)] -= 0.5 * (p[IX(i, j + 1, n)] - p[IX(i, j - 1, n)]) / h;
		}
	}
	set_bnd(n, 1, u, boundary); set_bnd(n, 2, v, boundary);
}

void velocStep(int n, int h, float *u, float *v, float *u0, float *v0, float visc, float dt, short *boundary) {
	add_source(n, u, u0, dt); add_source(n, v, v0, dt);
	SWAP(u, u0); diffuse(n, 1, u, u0, visc, dt, boundary);
	SWAP(v, v0); diffuse(n, 2, v, v0, visc, dt, boundary);
	project(n, h, u, v, u0, v0, boundary);
	SWAP(u, u0); SWAP(v, v0);
	advect(n, 1, u, u0, u0, v0, dt, boundary); advect(n, 2, v, v0, u0, v0, dt, boundary);
	project(n, h, u, v, u0, v0, boundary);
}

void densStep(int n, float *dens, float *dens0, float *u, float *v, float diffRate, float dt, short *boundary) {
	add_source(n, dens, dens0, dt);
	SWAP(dens, dens0);
	diffuse(n, 0, dens, dens0, diffRate, dt, boundary);
	SWAP(dens, dens0);
	advect(n, 0, dens, dens0, u, v, dt, boundary);
}