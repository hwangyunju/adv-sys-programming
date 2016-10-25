#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

int readaline_and_out(FILE *fin, FILE *fout);
void reverse(char str[],int size);

int
main(int argc, char *argv[])
{
    FILE *file1, *file2, *fout;
    int eof1 = 0, eof2 = 0;
    long line1 = 0, line2 = 0, lineout = 0;
    struct timeval before, after;
    int duration;
    int ret = 1;

    if (argc != 4) {
        fprintf(stderr, "usage: %s file1 file2 fout\n", argv[0]);
        goto leave0;
    }
    if ((file1 = fopen(argv[1], "rt")) == NULL) {
        perror(argv[1]);
        goto leave0;
    }
    if ((file2 = fopen(argv[2], "rt")) == NULL) {
        perror(argv[2]);
        goto leave1;
    }
    if ((fout = fopen(argv[3], "wt")) == NULL) {
        perror(argv[3]);
        goto leave2;
    }
    
    gettimeofday(&before, NULL);
    do {
        if (!eof1) {
            if (!readaline_and_out(file1, fout)) {
                line1++; lineout++;
            } else
                eof1 = 1;
        }
        if (!eof2) {
            if (!readaline_and_out(file2, fout)) {
                line2++; lineout++;
            } else
                eof2 = 1;
        }
    } while (!eof1 || !eof2);
    gettimeofday(&after, NULL);
    
    duration = (after.tv_sec - before.tv_sec) * 1000000 + (after.tv_usec - before.tv_usec);
    printf("Processing time = %d.%06d sec\n", duration / 1000000, duration % 1000000);
    printf("File1 = %ld, File2= %ld, Total = %ld Lines\n", line1, line2, lineout);
    ret = 0;
    
leave3:
    fclose(fout);
leave2:
    fclose(file2);
leave1:
    fclose(file1);
leave0:
    return ret; 
}

/* Read a line from fin and write it to fout */
/* return 1 if fin meets end of file */
int
readaline_and_out(FILE *fin, FILE *fout)
{    
    char ch,temp;
    char arr[200];
    int i=0;
    int count=0;

    do {
        if ((ch = fgetc(fin)) == EOF) {
            if (!count)
                return 1;
            else {
                fputc(0x0a, fout);
                break;
            }
        }
	arr[count]=ch;
        count++;
    } while (ch != 0x0a);
    arr[count]='\0';
    int n = count;
    for(n;n>=0;n--){
	temp = arr[n];
	fputc(temp,fout);
    }



//    reverse(arr,count);
//    fputs(arr, fout);
    return 0;
}

void reverse(char str[],int size)
{
    int i;
    char temp;
    for(i=0; i<size/2; i++){
	temp = str[i];
	str[i] = str[size-1-i];
	str[size-1-i]=temp;
    }
}

