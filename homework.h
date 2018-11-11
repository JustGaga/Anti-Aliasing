#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct{
	unsigned char r, g, b;

}pixel;

typedef struct {
	char p56[3];
	unsigned int width,height;
	int maxval;
	unsigned char **grayMat;
	pixel **mat;
}image;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */