#include "windows.h"

#include "renderer.h"
#include "stdio.h"
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
void routiney(int *v, char *flag){
	if (v[1] > 4 && *flag == 0){
		v[1]--;
	}
	else if (v[1] < 10 && *flag == 1){
		v[1]++;
	}
	if (*flag == 0 && v[1] == 4){
		*flag=1;
	}
	else if (*flag == 1 && v[1] == 10){
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
	char flag3=0;
	char flag4=0;

	while (1){
		clearScreenBuffer(buffer);
		clearDepthBuffer(dbuffer);
		drawModel(buffer, dbuffer, &model);
		drawBuffer(buffer);
		routine(v3.vertex, &flag1);
		routine(v5.vertex, &flag2);
		routiney(v2.vertex, &flag3);
		routine(v2.vertex, &flag4);
		wait(80);
	}
	free(buffer);
	free(dbuffer);
	return 0;
}
