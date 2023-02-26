#include "stdio.h"
#include "stdlib.h"
#include "windows.h"

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
Triangle createTriangle(Vertex *v1, Vertex *v2, Vertex *v3);
Vertex createVertex(int x, int y, int z, int shade);
char *initScreenBuffer();
void clearScreenBuffer(char *buffer);
int *initDepthBuffer();
void clearDepthBuffer(int *buffer);
int cross2D(int *v1, int *v2);
int dotProd(int *v1, int *v2, int dimensions);
int *subtract(int *v1, int *v2, int dimensions);
void drawTriangle(char *buffer, int *depthBuffer, Triangle t);
void drawBuffer(char *buffer);

Model createModel(int size){
	Model out;
	out.geometry=malloc(sizeof(Triangle)*size);
	out.size=size;
	out.index=0;
	return out;
}

void pushModel(Model *model, Triangle tri){
	if (model->index < model->size){
		model->geometry[model->index]=tri;
		model->index++;
	}
}

void drawModel(char *buffer, int *depthBuffer, Model *model){
	for (int i=0; i<model->index; i++){
		drawTriangle(buffer, depthBuffer, model->geometry[i]);
	}
}

Triangle createTriangle(Vertex *v1, Vertex *v2, Vertex *v3){
	Triangle out;
	out.v1=v1;
	out.v2=v2;
	out.v3=v3;
	return out;
}

Vertex createVertex(int x, int y, int z, int shade){
	Vertex out;
	int *v=malloc(sizeof(int)*3);
	v[0]=x;
	v[1]=y;
	v[2]=z;
	out.vertex=v;
	out.shade=shade;
	return out;
}

char *initScreenBuffer(){
	char *buffer=malloc(sizeof(char)*SCREEN_Y*SCREEN_X);
	clearScreenBuffer(buffer);
	return buffer;
}

void clearScreenBuffer(char *buffer){
	for (int i=0; i<SCREEN_Y; i++){
		for (int j=0; j<SCREEN_X; j++){
			buffer[i*SCREEN_X+j]=' ';
		}
	}
}

int *initDepthBuffer(){
	int *buffer=calloc(SCREEN_Y*SCREEN_X, sizeof(int));
	return buffer;
}

void clearDepthBuffer(int *buffer){
	for (int i=0; i<SCREEN_Y; i++){
		for (int j=0; j<SCREEN_X; j++){
			buffer[i*SCREEN_X+j]=0;
		}
	}
}

int cross2D(int *v1, int *v2){
	int out=v1[0]*v2[1]-v1[1]*v2[0];
	return out;
}

int dotProd(int *v1, int *v2, int dimensions){
	int out=0;
	for (int i=0; i<dimensions; i++){
		out+=v1[i]*v2[i];
	}
	return out;
}

int *subtract(int *v1, int *v2, int dimensions){
	int *out=malloc(sizeof(int)*dimensions);
	for (int i=0; i<dimensions; i++){
		out[i]=v2[i]-v1[i];
	}
	return out;
}

void drawTriangle(char *buffer, int *depthBuffer, Triangle t){
	int *v1=t.v1->vertex;
	int *v2=t.v2->vertex;
	int *v3=t.v3->vertex;
	char shades[6]={'.', ',', ';', 'x', 'X', '@'};
	int *e1=subtract(v1, v2, 2);
	int *e2=subtract(v2, v3, 2);
	int *e3=subtract(v3, v1, 2);
	int area=cross2D(e1, e2);
	if (cross2D(e1, e2) > 0 && cross2D(e2 ,e3) > 0 && cross2D(e3, e1) > 0){
		int *start;
		int *end;
		int *fixup;
		if (v1[0] <= v2[0] && v1[0] <= v3[0]){
			start=v1;
			if (v2[0] < v3[0]){
				end=v2;
				fixup=v3;
			}
			else{
				end=v3;
				fixup=v2;
			}
		}
		else if (v2[0] <= v1[0] && v2[0] <= v3[0]){
			start=v2;
			if (v1[0] < v3[0]){
				end=v1;
				fixup=v3;
			}
			else{
				end=v3;
				fixup=v1;
			}
		}
		else if (v3[0] <= v1[0] && v3[0] <= v2[0]){
			start=v3;
			if (v1[0] < v2[0]){
				end=v1;
				fixup=v2;
			}
			else{
				end=v2;
				fixup=v1;
			}
		}
		char done=0;
		char pass=0;
		while (done == 0){
			done=pass;
			int dx=end[0]-start[0];
			int dy=end[1]-start[1];
			int ydist=abs(end[1]-start[1]);
			unsigned int size=1<<(sizeof(int)*8-1);
			int ysign=1-(((unsigned int)((dy)&size))>>(sizeof(int)*8-2));
			//int ysign=abs(dy)/dy;
			int slope=(dx<<10)/dy;
			for (int i=0; i<=ydist; i++){
				char draw=0;
				for (int j=((ysign*i*slope)>>10)+start[0]; j<=fixup[0]; j++){
					int y=ysign*i+start[1];
					int p[3]={j, y, 0};
					int *p1=subtract(v1, p, 3);
					int *p2=subtract(v2, p, 3);
					int *p3=subtract(v3, p, 3);
					int cross1=cross2D(e1, p1);
					int cross2=cross2D(e2, p2);
					int cross3=cross2D(e3, p3);
					free(p1);
					free(p2);
					free(p3);
					p[2]=(v1[2]*(cross1<<10)/area + v2[2]*(cross2<<10)/area + v3[2]*(cross3<<10)/area)>>10;
					int shade=((t.v3->shade*cross1) + (t.v1->shade*cross2) + (t.v2->shade*cross3))/area;
					int index=y*SCREEN_X+j;
					if ((cross1 > 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0)){
						if (depthBuffer[index] > p[2] || depthBuffer[index] == 0){
							buffer[index]=shades[shade];
							depthBuffer[index]=p[2];
						}
						draw=1;
					}
					else if (draw == 1){
						break;
					}
					
				}
			}
			if (pass == 0 && (fixup[1] < start[1] || fixup[1] > end[1])){
				end=fixup;
				pass=1;
			}
			else{
				done=1;
			}
		}
	}
	free(e1);
	free(e2);
	free(e3);
}

void drawBuffer(char *buffer){
	char draw[SCREEN_Y*SCREEN_X+SCREEN_Y+1];
	int index=0;
	for (int i=0; i<SCREEN_Y; i++){
		for (int j=0; j<SCREEN_X; j++){
			draw[index]=buffer[(SCREEN_Y-i-1)*SCREEN_X+j];
			index++;
		}
		draw[index]='\n';
		index++;
	}
	draw[index]='\0';
	index++;

	printf("%s", draw);
}

void routine(int *v, char *flag){
	if (v[0] > 16 && *flag == 0){
		v[0]--;
	}
	else if (v[0] < 31 && *flag == 1){
		v[0]++;
	}
	if (*flag == 0 && v[0] == 16){
		*flag=1;
	}
	else if (*flag == 1 && v[0] == 31){
		*flag=0;
	}
}

void wait(int sec){
	#ifdef _WIN32
		Sleep(sec);
	#else
		sleep(sec);
	#endif
}

int main(){
	char *buffer=initScreenBuffer();
	int *dbuffer=initDepthBuffer();

	Vertex v1=createVertex(6, 8, 12, 1);
	Vertex v2=createVertex(25, 4, 2, 2);
	Vertex v3=createVertex(29, 20, 6, 6);
	Vertex v4=createVertex(40, 7, 4, 1);
	Vertex v5=createVertex(24, 0, 6, 0);

	Triangle t1=createTriangle(&v1, &v2, &v3);
	Triangle t2=createTriangle(&v3, &v2, &v4);
	Triangle t3=createTriangle(&v1, &v5, &v2);
	Triangle t4=createTriangle(&v2, &v5, &v4);

	Model model=createModel(4);
	pushModel(&model, t1);
	pushModel(&model, t2);
	pushModel(&model, t3);
	pushModel(&model, t4);

	char flag1=0;
	char flag2=1;

	while (1){
		clearScreenBuffer(buffer);
		clearDepthBuffer(dbuffer);
		drawModel(buffer, dbuffer, &model);
		drawBuffer(buffer);
		routine(v3.vertex, &flag1);
		routine(v5.vertex, &flag2);
		wait(50);
	}
	free(buffer);
	free(dbuffer);
	return 0;
}
