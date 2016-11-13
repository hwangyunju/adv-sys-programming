#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int readaline_and_out(char* f_in, char* f_out, int* cursor, int* cursor_out);
//void reverse(char* str, int** cursor_out, int count);
int main(int argc, char *argv[])
{
    int file1, file2, fout;
    int cursor1=0, cursor2=0;
    int cursor_out=0;

    int i;
    struct stat sb1,sb2; 
    char *addr1, *addr2, *addr3;  

    int eof1 = 0, eof2 = 0;
    long line1 = 0, line2 = 0, lineout = 0;
    struct timeval before, after;
    int duration;
    int ret = 1;
    
    if (argc != 4) {
        fprintf(stderr, "usage: %s file1 file2 fout\n", argv[0]);
        goto leave0;
    }
    if ((file1 = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        goto leave0;
    }
    if (fstat(file1, &sb1) < 0)
    {
	perror("fstat1 error");
	goto leave0;
    }
    if ((file2 = open(argv[2], O_RDONLY)) == -1) {
        perror(argv[2]);
        goto leave1;
    }
    if (fstat(file2, &sb1) < 0)
    {
	perror("fstat2 error");
	goto leave0;
    }
    if ((fout = open(argv[3], O_RDWR | O_CREAT)) == -1) {
        perror(argv[3]);
	goto leave2;
    }
    if (fstat(fout, &sb2) < 0)
    {
	perror("fstat3 error");
	goto leave0;
    }

    addr1 = mmap(0,sb1.st_size,PROT_READ,MAP_SHARED,file1,(off_t)0);
    if( addr1 == MAP_FAILED){
	perror("mmap1 error");
	goto leave0;
    }
    close(file1);
  
    addr2 = mmap(0,sb1.st_size,PROT_READ,MAP_SHARED,file2,(off_t)0);
    if( addr2 == MAP_FAILED ){
	perror("mmap2 error");
	goto leave0;
    }
    close(file2);
    
    addr3 = mmap(0,sb2.st_size,PROT_WRITE,MAP_SHARED,fout,(off_t)0);
    if( addr3 == MAP_FAILED ){
	perror("mmap3 error");
	goto leave0;
    }
    for(i=0;i<sb2.st_size;i++){
	addr3[i] = '\0';
    }
 
    gettimeofday(&before, NULL);

    do {
	if (!eof1) {
	    if (!readaline_and_out(addr1, addr3, &cursor1, &cursor_out)){
		line1++; lineout++;
	    } else
		eof1 = 1;
	}
	if (!eof2) {
	    if (!readaline_and_out(addr2, addr3, &cursor2, &cursor_out)){
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
    close(fout);
leave2:
    close(file2);
leave1:
    close(file1);
leave0:
    return ret;

}


int readaline_and_out(char* f_in, char* f_out, int* cursor, int* cursor_out)
{
    int ch, temp;
    int count = 0;
    int count2 = 0;
    int i = 0;
 
    if(f_in[*cursor] == '\0'){
        return 1;
    }
 
    while((ch=f_in[*cursor+count]) != '\n'){
        f_out[*cursor_out+count] = ch;
        count++;
    }
    f_out[*cursor_out+count] = '\n';
//    reverse(f_out, &cursor_out, count);
/*    for(i=0;i<=(count/2);i++){
        temp = f_out[*cursor_out+i];
	f_out[*cursor_out+i] = f_out[*cursor_out+count-1-i];
	f_out[*cursor+count-1-i] =temp;
    }
    f_out[*cursor_out+count] = '\n';
*/ 
    *cursor += count + 1;
    *cursor_out += count + 1;

    return 0;
}

void reverse(char* str, int** cursor_out, int count)
{
    char temp;
    int i;
    int start = **cursor_out;
    int end = **(cursor_out+count);
     
    for(i=start; i<(end/2);i++){
	printf("reverse step: %d\n",i);
	temp = str[i];
	str[i] = str[end-1-i];
	str[end-1-i]=temp;
    }
}
