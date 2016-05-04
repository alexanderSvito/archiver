#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
#define SHT_L 32768

int l =64;
char sheet[SHT_L][64];
char temp[64];
int pos,i,m,n;

int search(int quant,int i, char input[128])
{
    if (quant==2)
    {
        pos = (int)input[i];
    }
    int k=0;
    if (strlen(input)==0) {
    	exit(0);
    }
    for (int j=i; j<i+quant; k++,j++)
    {
        temp[k]=input[j];
    }
    m=quant;
    for (l=257; l<SHT_L; l++)
    {
        if (strcmp(sheet[l],temp)==0)
        {
            pos=l;
            search(quant+1,i,input);
        }
    }
    for (int f=0; f<65;f++)
    {
        sheet[n][f]=temp[f];
    }
    n++;
    quant=2;
    for (int g=0;g<64;g++)
    {
        temp[g]='\0';
    }
    return pos;
}

int main()
{
	char input[128], tem[8];
	memset(input,0,128);
	int count=1, sym=0, g=0;
    for (i=0;i<256; i++)
    {
        sheet[i][0]=(char)i;
    };
    int output[300], myFile, outFile;
    output[0]=256;
    n=258;
    int o=1;
    myFile = open("test.md", O_RDONLY);
    outFile = open("compressed.md", O_WRONLY);
    while (count!=0) {
    	//printf("%d\n",count);
    	count=read(myFile,input,128);
    	//printf("I've counted\n");
    	if (strlen(input)==0) {
    		exit(0);
    	}
    	for (i=0; i<128;i=i+m-1,o++)
	    {
	    	
	    	memset(tem, '\0',g);
	    	g=0;
	        m=2;
	        //printf("The input is: %s\n",input);
	        sym=search(m,i,input);
	        //printf("%d\n",sym);
	        while (sym>=256) {
	        	tem[g]=(char)255;
	        	sym=sym-256;
	        	g++;
	        }
	        tem[g++]=(char)sym;
	        tem[g]='\0';

	        write(outFile, tem , 8);
	    }
    }
    printf("I've finished!\n");
    close(myFile);
    close(outFile);
    return 0;
}
