#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
/**
 * code that compare between two files and return 2 if equals, else return 1.
 * parts of code takes from : www.geeksforgeeks.org/c-program-compare-two-files-report-mismatches/
 * @return 2 (if equals) , else 1
 * @author Eli Haimov - ID : 308019306
 */

int cmpFiles(FILE *fp1,FILE *fp2){
    char ch1 = getc(fp1);
    char ch2 = getc(fp2);
    int countErr =0;

        while(ch1 !=EOF && ch2 !=EOF)     // EOF == End Of File
        {
            if(ch1 != ch2){
                countErr++;
            }
            ch1=getc(fp1);
            ch2=getc(fp2);

        }
    return countErr;
}


int main(){
    FILE *fp1 = fopen("A1.txt", "r");  // opening both file in read only mode
    FILE *fp2 = fopen("A3.txt", "r");
    fseek(fp1, 0L, SEEK_END);      // For checking the size of the files
    fseek(fp2, 0L, SEEK_END);
    int ch1_size = ftell(fp1);
    int ch2_size = ftell(fp2);

    if(fp1==NULL || fp2==NULL)
    {
        printf("Error : Files not open", stderr);
        exit(0);
    }
    else if (ch1_size == ch2_size)       // If size of the files are Equals
    {
        int counter = cmpFiles(fp1,fp2);      // Compare function
        if (counter==0){   // If files are equals
            printf("are Equals");
            exit(EXIT_SUCCESS);
        }
        else{
            printf("are not Equals");
            exit(EXIT_SUCCESS);
        }
    }
    else{
        printf("are not Equals");
        exit(EXIT_SUCCESS);
    }

    fclose(fp1);
    fclose(fp2);
    return 0;

}