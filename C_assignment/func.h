#define M 10
#define CEIL 100 

extern char** menu[];

extern void show_menu(char** strlst);
extern void show_matrix(unsigned int Mat[M][M]);
extern void ComputeSumMat(unsigned int Mat1[M][M], unsigned int Mat2[M][M]);
extern void ComputeMaximalDiagonalElement(unsigned int Mat1[M][M], unsigned int Mat2[M][M]);
extern void ComputeMaxTrace(unsigned int Mat1[M][M], unsigned int Mat2[M][M]);
extern void CopyDiag1ToDiag2(unsigned int Mat1[M][M], unsigned int Mat2[M][M]);
extern void ComputeTranspose(unsigned int Mat[M][M]);
