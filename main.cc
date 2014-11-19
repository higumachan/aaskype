#include <cstdio>
#include <cstdlib>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

int CHAR_FRAME_WIDTH;
int CHAR_FRAME_HEIGHT;

void resize(){
	struct winsize ws;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
	CHAR_FRAME_WIDTH=ws.ws_col;CHAR_FRAME_HEIGHT=ws.ws_row;
}

void cursesInit()
{
	if(!initscr()) {
		printf("Error initializing screen.\n");
		exit(EXIT_FAILURE);
	}
	if(!has_colors()) {
		printf("This terminal does not support colours.\n");
		exit(EXIT_FAILURE);
	}
	leaveok(stdscr, FALSE);
	use_default_colors();
	start_color();
	nonl();

	for (int i = 0; i <= 255; i++){
		init_pair(i, -1, i);
	}
}

char charTable[256][256];
void genCharTable(const char*file){
	FILE*fp=fopen(file,"r");
	if(!fp){
		fprintf(stderr,"charInfoFile :%s not found\n",file);
		exit(EXIT_FAILURE);
	}
	char infoChar[256];
	double info[256][2];
	int infoLength=0;
	char line[256];
	while(fgets(line,sizeof(line),fp)){
		if(line[0]&&line[1]==' '){
			int hexup,hexdown;
			int cnt=sscanf(line+2,"%x %x",&hexup,&hexdown);
			if(cnt!=2)continue;
			info[infoLength][0]=hexup;
			info[infoLength][1]=hexdown;
			infoChar[infoLength]=line[0];
			infoLength++;
		}
	}
	fclose(fp);
	double min=0xff;
	for(int i = 0; i < infoLength; i++){
		double av=(info[i][0] + info[i][1]) / 2.0;
		if(av < min) min = av;
	}
	for(int i = 0;i < infoLength; i++){
		for(int j = 0;j < 2; j++){
			info[i][j] = 0xff * (info[i][j] - min) / (0xff - min);
		}
	}
	for(int i = 0; i < 256; i++){
		for(int j = 0;j < 256;j++){
			double costMin=-1;
			int indexMin=0;
			for(int k=0;k<infoLength;k++){
				double difUp = i - info[k][0], difDown = j - info[k][1], difAv = difUp + difDown;
				double cost = difUp * difUp + difDown * difDown + difAv * difAv * difAv * difAv / 1024;
				if(costMin < 0 || cost < costMin){
					costMin=cost;
					indexMin=k;
				}
			}
			charTable[i][j] = infoChar[indexMin];
		}
	}
}



void render(const Mat& img)
{
	int width = img.cols;
	int height = img.rows;
	double aspect = width / height;
	int cfw = CHAR_FRAME_WIDTH;
	int cfh = CHAR_FRAME_HEIGHT;
	Mat gray;

	cvtColor(img, gray, CV_RGB2GRAY);

	/*
	// 横の方が大きい間
	while (cfw / cfh * 2 > aspect){
	cfw--;
	}
	// 縦の方が大きい間
	while (cfw / cfh * 2 < aspect){
	cfh--;
	}
	*/
	Mat resized_img;
	width = width / cfw * cfw;
	height = height / cfh * cfh;
	resize(gray, resized_img, Size(width, height));

	int w_offset = width / cfw;
	int h_offset = height / cfh;
	for (int y = 0; y < cfh; y++){
		move(y, 0);
		for (int x = 0; x < cfw; x++){
			int uy = y * h_offset + (h_offset * 0.25);
			int dy = y * h_offset + (h_offset * 0.75);
			int mx = x * w_offset + (w_offset * 0.5);

			//printf("(%d %d) (%d %d)\n", mx, uy, mx, dy);
			unsigned char a = resized_img.at<unsigned char>(uy, mx);
			unsigned char b = resized_img.at<unsigned char>(dy, mx);
			putc(charTable[a][b], stdout);
			//attron(COLOR_PAIR(5| A_NORMAL));
			//addch(charTable[a][b]);
		}
		//puts("");
	}
	//imshow("test", resized_img);
}

int main(int argc, char* argv[])
{
	Mat frame = imread(argv[1]);

	cursesInit();
	genCharTable("charinfo/Monaco.charinfo");

	resize();
    render(frame);
}
