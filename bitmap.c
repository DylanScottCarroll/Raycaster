#include <stdio.h>

void Save(unsigned char *pixels, int width, int height, char dir[]){
	
	int file_size = (53 + width * height * 3);

	unsigned char header[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char info_header[40] = {40,0,0,0,  0,0,0,0,  0,0,0,0,  1,0, 24,0};

	header[2] = (unsigned char) (file_size);
	header[3] = (unsigned char) (file_size>>8);
	header[4] = (unsigned char) (file_size>>16);
	header[5] = (unsigned char) (file_size>>24);

	info_header[4] = (unsigned char) (width);
	info_header[5] = (unsigned char) (width>>8);
	info_header[6] = (unsigned char) (width>>16);
	info_header[7] = (unsigned char) (width>>24);

	info_header[8] = (unsigned char) (height);
	info_header[9] = (unsigned char) (height>>8);
	info_header[10] = (unsigned char) (height>>16);
	info_header[11] = (unsigned char) (height>>24);


	FILE *fp = fopen(dir, "wb");
	fwrite(header, 1, 14, fp);
	fwrite(info_header, 1, 40, fp);

	unsigned char pad[3] = {0, 0, 0};

	int h = width * height * 3;
	for(int i = 0; i < height; i++){

		fwrite((pixels + (i*width*3)) , 1,(width * 3), fp);
		fwrite(pad, 1, (4-((width*3)%4))%4 , fp);
	}
	fclose(fp);
}

/*
int WIDTH = 100;
int HEIGHT = 100;

int main(){

	
	unsigned char pixels[100 * 1000 * 3];
	for(int y = 0; y < HEIGHT; y++){
		for(int x = 0; x < WIDTH; x++){
			pixels[(y*3*WIDTH) + (x*3) + 0] = x;
			pixels[(y*3*WIDTH) + (x*3) + 1] = 0;
			pixels[(y*3*WIDTH) + (x*3) + 2] = y;
		}
	}

	Save(pixels, 100, 100);
	
	

	return 0;
}
*/