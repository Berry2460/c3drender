#ifndef RENDERER_H

#define RENDERER_H
#define SCREEN_X 48
#define SCREEN_Y 32

typedef struct Vertex{
	int *vertex;
	char shade;
}Vertex;

typedef struct Triangle{
	Vertex *v1;
	Vertex *v2;
	Vertex *v3;
}Triangle;

typedef struct Model{
	Triangle *geometry;
	int size;
	int index;
}Model;

Model createModel(int size);
void pushModel(Model *model, Triangle tri);
void drawModel(char *buffer, int *depthBuffer, Model *model);
Triangle createTriangle(Vertex *v1, Vertex *v2, Vertex *v3);
Vertex createVertex(int x, int y, int z, int shade);
char *initScreenBuffer();
void clearScreenBuffer(char *buffer);
int *initDepthBuffer();
void clearDepthBuffer(int *buffer);
void drawTriangle(char *buffer, int *depthBuffer, Triangle t);
void drawBuffer(char *buffer);
#endif
