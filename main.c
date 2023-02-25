#include "stdio.h"
#include "stdlib.h"
#include "windows.h"

#define SCREEN_X 48
#define SCREEN_Y 32

char *initBuffer();
void clearBuffer(char *buffer);
int cross2D(int *v1, int *v2);
int dotProd(int *v1, int *v2, int dimensions);
void drawTriangle(char *buffer, int *v1, int *v2, int *v3);
void drawBuffer(char *buffer);

char *initBuffer(){
	char *buffer=malloc(sizeof(char)*SCREEN_Y*SCREEN_X);
	clearBuffer(buffer);
	return buffer;
}

void clearBuffer(char *buffer){
	for (int i=0; i<SCREEN_Y; i++){
		for (int j=0; j<SCREEN_X; j++){
			buffer[i*SCREEN_X+j]=' ';
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

int *line2D(int *v1, int *v2){
	int *out=malloc(sizeof(int)*3);
	out[0]=v2[0]-v1[0];
	out[1]=v2[1]-v1[1];
	return out;
}

void drawTriangle(char *buffer, int *v1, int *v2, int *v3){
	char shades[7]={'.', ',', ';', 'x', 'X', '@', '#'};
	int *e1=line2D(v1, v2);
	int *e2=line2D(v2, v3);
	int *e3=line2D(v3, v1);
	int area=cross2D(e1, e2);
	if (cross2D(e1, e2) > 0 && cross2D(e2 ,e3) > 0 && cross2D(e3, e1) > 0){
		int *start;
		int *end;
		int *fixup;
		if (v1[0] < v2[0] && v1[0] < v3[0]){
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
		else if (v2[0] < v1[0] && v2[0] < v3[0]){
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
		else{
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
			for (int i=1; i<=ydist; i++){
				for (int j=((ysign*i*slope)>>10)+1+start[0]; j<=fixup[0]; j++){
					int y=ysign*i+start[1]-(pass*ysign);
					int p[2]={j, y};
					int *p1=line2D(v1, p);
					int *p2=line2D(v2, p);
					int *p3=line2D(v3, p);
					int cross1=cross2D(e1, p1);
					int cross2=cross2D(e2, p2);
					int cross3=cross2D(e3, p3);
					free(p1);
					free(p2);
					free(p3);
					if ((cross1 > 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0)){
						int shade=(cross1*7)/area;
						buffer[y*SCREEN_X+j]=shades[shade];
					}
					else{
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
	if (v[0] > 6 && *flag == 0){
		v[0]--;
	}
	else if (v[0] < 28 && *flag == 1){
		v[0]++;
	}
	if (*flag == 0 && v[0] == 6){
		*flag=1;
	}
	else if (*flag == 1 && v[0] == 28){
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
	char *buffer=initBuffer();
	int v1[2]={1, 5};
	int v2[2]={20, 1};
	int v3[2]={28, 18};

	int v21[2]={20, 1};
	int v22[2]={25, 7};
	int v23[2]={28, 18};

	char flag1=0;
	char flag2=0;

	while (1){
		clearBuffer(buffer);
		drawTriangle(buffer, v1, v2, v3);
		drawTriangle(buffer, v21, v22, v23);
		drawBuffer(buffer);
		routine(v3, &flag1);
		routine(v23, &flag2);
		wait(50);
	}
	free(buffer);
	return 0;
}
