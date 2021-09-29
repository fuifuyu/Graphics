#pragma once
void densStep(int N, float *x, float *x0, float *u, float *v, float diff, float dt, short *boundary);
void velocStep(int N, int h, float *u, float *v, float *u0, float *v0, float visc, float dt, short *boundary);