#include <stdio.h>
#include "func.h"

int main(){
	char clearTheBuffer;
    int mat1[M][M];
    int mat2[M][M];
    char Selector = '0';

	int offset1;
	int offset2;
	char input[10];
	char input1[3];
	char input6[4];
	char input7[4];

    show_menu(menu);

    while(1) {  	   
        printf("Choose a menu clause number: ");
		gets(input);
		Selector = input[0];
        switch(Selector){
            case '1': 
                do{
		            printf("In order to creat Mat1 please insert an offset value between 0-100: ");
		            gets(input1);
		            sscanf_s(input1,"%d",&offset1);
		            if (offset1 >= 0 && offset1 <= 100) break;
	            } while (1);
                FillMatrix(mat1, offset1);
                do{
		            printf("In order to creat Mat2 please insert an offset value between 0-100: ");
		            gets(input1);
		            sscanf_s(input1,"%d",&offset2);
		            if (offset2 >= 0 && offset2 <= 100) break;
	            } while (1);
	            FillMatrix(mat2, offset2);
                break;

            case '2': 
				ComputeSumMat(mat1, mat2);
                break;

            case '3': 
				ComputeMaximalDiagonalElement(mat1, mat2);
                break;

            case '4': 
				ComputeMaxTrace(mat1, mat2);
                break;

            case '5':
				CopyDiag1ToDiag2(mat1, mat2);
                break;

            case '6':
				while (1) {
					printf("Insert the wanted matrix [Mat1 or Mat2]: ");
					gets(input6);
					if (strcmp(input6, "Mat1") == 0) {
						ComputeTranspose(mat1);
						break;
					}
					else if (strcmp(input6, "Mat2") == 0) {
						ComputeTranspose(mat2);
						break;
					}
				}
                break;

            case '7':
                while (1){
                    printf("Insert the wanted matrix [Mat1 or Mat2]: ");
                    gets(input7);
                    if (strcmp(input7, "Mat1")==0){
                        show_matrix(mat1);
                        break;
                    }
                    else if (strcmp(input7, "Mat2")==0){
                        show_matrix(mat2);
                        break;
                    }
                }
                break;

            case '8': 
                show_menu(menu);
                break;

            case '9': 
                return 0;

            default:
                printf("Try agian, You have inserted wrong choise number\n");
                break;
        }
    }	
}
