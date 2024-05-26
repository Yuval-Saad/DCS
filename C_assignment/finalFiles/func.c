#include <stdio.h>
#include "func.h" 

void show_menu(char** strlst) {
	int i;
	for (i = 0; strlst[i]; i++) 
		printf("%s\n", strlst[i]);
	return;
}

void show_matrix(unsigned int Mat[M][M]){
    int i, j;
	printf("[\n");
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++)
			printf("%d, ", Mat[i][j]);
		printf("\n");
	}
	printf("]\n\n");
	return;
}

void FillMatrix(unsigned int Mat[M][M], int offset) {
	int i, j;
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++) {
			Mat[i][j] = (offset + i * M + j) % CEIL;
		}
	}
}

void ComputeSumMat(unsigned int Mat1[M][M], unsigned int Mat2[M][M]) {
	int sumMat[M][M];
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			sumMat[i][j] = Mat1[i][j] + Mat2[i][j];
		}
	}
	show_matrix(sumMat);
}

void ComputeMaximalDiagonalElement(unsigned int Mat1[M][M], unsigned int Mat2[M][M]) {
	int maxNum = 0;
	int maxInMat = 1;
	for (int i = 0; i < M; i++) {
		if (Mat1[i][i] > maxNum)
			maxNum = Mat1[i][i];
		if (Mat1[i][M - i - 1] > maxNum)
			maxNum = Mat1[i][M - i - 1];
	}
	for (int i = 0; i < M; i++) {
		if (Mat2[i][i] >= maxNum) {
			maxNum = Mat1[i][i];
			maxInMat = 2;
		}
		if (Mat2[i][M - i - 1] >= maxNum) {
			maxNum = Mat2[i][M - i - 1];
			maxInMat = 2;
		}
	}
	if (maxInMat == 1)
		printf("mat1 ");
	else
		printf("mat2 ");
	printf("has the maximal diagonal element value: %d\n", maxNum);
}

void ComputeMaxTrace(unsigned int Mat1[M][M], unsigned int Mat2[M][M]) {
	int trace1 = 0;
	int trace2 = 0;
	for (int i = 0; i < M; i++) {
		trace1 += Mat1[i][i];
		trace2 += Mat2[i][i];
	}
	if (trace1 > trace2)
		printf("mat1 has the maximal Trace value: %d\n", trace1);
	else
		printf("mat2 has the maximal Trace value: %d\n", trace2);
}


void CopyDiag1ToDiag2(unsigned int Mat1[M][M], unsigned int Mat2[M][M]) {
	for (int i = 0; i < M; i++) {
		Mat2[i][i] = Mat1[i][M - i - 1];
		Mat2[i][M - i - 1] = Mat1[i][i];

	}
}

void ComputeTranspose(unsigned int Mat[M][M]) {
	int TransMat[M][M];
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			TransMat[i][j] = Mat[j][i];
			TransMat[j][i] = Mat[i][j];
		}
	}
	show_matrix(TransMat);
}



char** menu[] = {
	" --------------------- Menu --------------------------------",
    " -----------------------------------------------------------",
	"1) Create two 10x10 matrices Mat1,Mat2 using the next expression: ",
	"   for i,j = {0-9}: Mat[i][j] = (offset + i * 10 + j) % 100",
	"2) Compute and show the matrix Mat1+Mat2",
	"3) Compute and show the maximal diagonal element between Mat1 and Mat2",
	"4) Compute and show the the maximal Trace between matrices Mat1 and Mat2",
    "5) Copy mat1 right diagonals to mat2 left diagonals",
	"6) Compute and show the transposed of a given Matrix",
    "7) Show a given Matrix",
    "8) Show menu",
	"9) exit",
	NULL
};
