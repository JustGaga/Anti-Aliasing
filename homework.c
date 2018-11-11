#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>

int num_threads;
int resize_factor;
image *in_tmp;
image *out_tmp;

void readInput(const char * fileName, image *img) {
	FILE *f = fopen(fileName, "rt");
	fscanf(f, "%s", img->p56);
	fscanf(f, "%d", &img->width);
	fscanf(f, "%d", &img->height);
	fscanf(f, "%d", &img->maxval);

	if(img->p56[1] == '5'){
		//alocare matrice grayscale
		img->grayMat = (unsigned char**)malloc(img->height * sizeof(unsigned char*));
		for(int i = 0; i < img->height; i++){
			img->grayMat[i] = (unsigned char*)malloc(img->width * sizeof(unsigned char));
		}
	}else{
		//alocare matrice rgb
		img->mat = (pixel**)malloc(img->height * sizeof(pixel*));
		for(int i = 0; i < img->height; i++){
			img->mat[i] = (pixel*)malloc(img->width * sizeof(pixel));
		}

	}


	fclose(f);
	//redeschide fisierul in citire binara
	f = fopen(fileName, "rb");
	char c;
	int counter = 0;
	//skip caractere pana la matricea imaginii
	while(counter < 3){
		fread(&c, 1, 1, f);
		if(c == '\n')
			counter++;
	}
	if(img->p56[1] == '5'){
		//citire matrice grayscale
		for(int i = 0; i < img->height; i++){
			for(int j = 0; j < img->width; j++){
				fread(&c, 1, 1, f);
				img->grayMat[i][j] = c;
			}
		}
	}else{
		//citire matrice rgb
		for(int i = 0; i < img->height; i++){
			for(int j = 0; j < img->width; j++){
				fread(&c, 1, 1, f);
				img->mat[i][j].r = c;
				fread(&c, 1, 1, f);
				img->mat[i][j].g = c;
				fread(&c, 1, 1, f);
				img->mat[i][j].b = c;
			}
		}
	}

	fclose(f);
}

void writeData(const char * fileName, image *img) {
	FILE *f = fopen(fileName, "wt");
	fprintf(f, "%s\n", img->p56);
	fprintf(f, "%d ", img->width);
	fprintf(f, "%d\n", img->height);
	fprintf(f, "%d\n", img->maxval);

	fclose(f);
	f = fopen(fileName, "ab");
	if(img->p56[1] == '5'){
		for(int i = 0; i < img->height; i++){
			for(int j = 0; j < img->width; j++){
				fwrite(&img->grayMat[i][j], 1, 1, f);
			}
		}
	}else{
		for(int i = 0; i < img->height; i++){
			for(int j = 0; j < img->width; j++){
				fwrite(&img->mat[i][j].r, 1, 1, f);
				fwrite(&img->mat[i][j].g, 1, 1, f);
				fwrite(&img->mat[i][j].b, 1, 1, f);
			}
		}
	}	

	fclose(f);


}


void* threadFunction(void *var)
{	

	out_tmp->p56[0] = in_tmp->p56[0];
	out_tmp->p56[1] = in_tmp->p56[1];
	out_tmp->p56[2] = in_tmp->p56[2];
	out_tmp->width = in_tmp->width / resize_factor;
	out_tmp->height = in_tmp->height / resize_factor;
	out_tmp->maxval = in_tmp->maxval;

	//black 'n white
	if(out_tmp->p56[1] == '5'){
		//alocare matrice resized
		out_tmp->grayMat = (unsigned char**)malloc(out_tmp->height * sizeof(unsigned char*));
		for(int i = 0; i < out_tmp->height; i++){
			out_tmp->grayMat[i] = (unsigned char*)malloc(out_tmp->width * sizeof(unsigned char));
		}

		if(resize_factor % 2 == 0){
			int p = 0; //indice linie matrice out
			for(int i = 0; i < in_tmp->height - resize_factor + 1;){
				int q = 0; //indice coloana matrice out
				for(int j = 0; j < in_tmp->width - resize_factor + 1;){
					int medie = 0;
					for(int k = i; k < (i+resize_factor); k++){
						for(int t = j; t < (j+resize_factor); t++){
							medie += in_tmp->grayMat[k][t];
						}
					}
					medie = medie / (resize_factor * resize_factor);
					out_tmp->grayMat[p][q] = medie;
					q++;
					j += resize_factor;	
				}
				p++;
				i += resize_factor;
			}

		}else if(resize_factor == 3){
			int p = 0; //indice linie matrice out
			for(int i = 1; i < in_tmp->height - 2;){
				int q = 0; //indice coloana matrice out
				for(int j = 1; j < in_tmp->width - 2;){
					//filtrare gaussiana
					int gauss = (in_tmp->grayMat[i-1][j-1] + in_tmp->grayMat[i-1][j] * 2 + in_tmp->grayMat[i-1][j+1] 
								 + in_tmp->grayMat[i][j-1] * 2 + in_tmp->grayMat[i][j] * 4 + in_tmp->grayMat[i][j+1] * 2 
								 + in_tmp->grayMat[i+1][j-1] + in_tmp->grayMat[i+1][j] * 2 + in_tmp->grayMat[i+1][j+1]) / 16.0;
					out_tmp->grayMat[p][q] = gauss;
					q++;
					j += resize_factor;
				}	

				p++;
				i += resize_factor;
			}
		}
	}else{
		//color case
		out_tmp->mat = (pixel**)malloc(out_tmp->height * sizeof(pixel*));
		for(int i = 0; i < out_tmp->height; i++){
			out_tmp->mat[i] = (pixel*)malloc(out_tmp->width * sizeof(pixel));
		}

		if(resize_factor % 2 == 0){
			int p = 0; //indice linie matrice out
			for(int i = 0; i < in_tmp->height - resize_factor + 1;){
				int q = 0; //indice coloana matrice out
				for(int j = 0; j < in_tmp->width - resize_factor + 1;){
					int medieR = 0;
					int medieG = 0;
					int medieB = 0;
					for(int k = i; k < (i+resize_factor); k++){
						for(int t = j; t < (j+resize_factor); t++){
							medieR += in_tmp->mat[k][t].r;
							medieG += in_tmp->mat[k][t].g;
							medieB += in_tmp->mat[k][t].b;
						}
					}
					medieR = medieR / (resize_factor * resize_factor);
					medieG = medieG / (resize_factor * resize_factor);
					medieB = medieB / (resize_factor * resize_factor);
					out_tmp->mat[p][q].r = medieR;
					out_tmp->mat[p][q].g = medieG;
					out_tmp->mat[p][q].b = medieB;
					q++;
					j += resize_factor;	
				}
				p++;
				i += resize_factor;
			}

		}else if(resize_factor == 3){
			int p = 0; //indice linie matrice out
			for(int i = 1; i < in_tmp->height - 2;){
				int q = 0; //indice coloana matrice out
				for(int j = 1; j < in_tmp->width - 2;){
					//filtrare gaussiana
					int gaussR = (in_tmp->mat[i-1][j-1].r + in_tmp->mat[i-1][j].r * 2 + in_tmp->mat[i-1][j+1].r 
								 + in_tmp->mat[i][j-1].r * 2 + in_tmp->mat[i][j].r * 4 + in_tmp->mat[i][j+1].r * 2 
								 + in_tmp->mat[i+1][j-1].r + in_tmp->mat[i+1][j].r * 2 + in_tmp->mat[i+1][j+1].r) / 16.0;
					out_tmp->mat[p][q].r = gaussR;
					int gaussG = (in_tmp->mat[i-1][j-1].g + in_tmp->mat[i-1][j].g * 2 + in_tmp->mat[i-1][j+1].g 
								 + in_tmp->mat[i][j-1].g * 2 + in_tmp->mat[i][j].g * 4 + in_tmp->mat[i][j+1].g * 2 
								 + in_tmp->mat[i+1][j-1].g + in_tmp->mat[i+1][j].g * 2 + in_tmp->mat[i+1][j+1].g) / 16.0;
					out_tmp->mat[p][q].g = gaussG;
					int gaussB = (in_tmp->mat[i-1][j-1].b + in_tmp->mat[i-1][j].b * 2 + in_tmp->mat[i-1][j+1].b 
								 + in_tmp->mat[i][j-1].b * 2 + in_tmp->mat[i][j].b * 4 + in_tmp->mat[i][j+1].b * 2 
								 + in_tmp->mat[i+1][j-1].b + in_tmp->mat[i+1][j].b * 2 + in_tmp->mat[i+1][j+1].b) / 16.0;
					out_tmp->mat[p][q].b = gaussB;
					q++;
					j += resize_factor;
				}	

				p++;

				i += resize_factor;
			}
		}

	}
	return NULL;
}

void resize(image *in, image *out) { 

	in_tmp = in;
	out_tmp = out;
	pthread_t tid[num_threads];
	int thread_id[num_threads];


	for(int i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}
	for(int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	out = out_tmp;


	

	
}