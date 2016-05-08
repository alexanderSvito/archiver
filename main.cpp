#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;

typedef struct bfile
{
    FILE *file;
}
BFILE;

#define P_COUNT 2047
#define SHEET_SIZE

/*int l =64;
char sheet[SHT_L][64];
char temp[64];
int pos,i,m,n;*/

BFILE *InputIBFile(char *name)
{
   BFILE *bfile;
   bfile = (BFILE *) calloc(1, sizeof(BFILE));
   if (bfile == NULL){
      pritnf("Error while opening file\n");
      exit(0);
   }
   bfile->file = fopen(name, "rb");
   return bfile;
}

BFILE *InputOBFile(char * name)
{
   BFILE *bfile;
   bfile = (BFILE *) calloc(1, sizeof(BFILE));
   if (bfile == NULL){
      pritnf("Error while opening file\n");
      exit(0);
   }
   bfile->file = fopen(name, "wb");
   return bfile;
}

void CloseOBFile(BFILE *bfile)
{
   fclose( bfile->file);
   free((char *) bfile);
}

void CloseIBFile(BFILE *bfile)
{
    fclose( bfile->file);
    free((char *) bfile);
}

long file_size(char *name)
{
   long eof_ftell;
   FILE *file;
   file = fopen(name, "r");
   if ( file == NULL )
      return(0L);
   fseek(file, 0L, SEEK_END);
   eof_ftell = ftell(file);
   fclose(file);
   return eof_ftell;
}

struct dictionary
{
   int code_value;
   int prefix_code;
   char character;
}
dict[SHEET_SIZE];

char decode_stack[TABLE_SIZE];

unsigned int decode_string ( unsigned int count, unsignedint code )
{
   while ( code > 255 ) /* Пока не встретится код символа */
   {
      decode_stack[count++] = dict[code].character;
      code = dict[code].prefix_code;
   }
   decode_stack[count++] = (char) code;
   return count;
}

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
	char input[128], tem[8], ch[1];
	int count=1, sym=0, g=0;
    for (i=0;i<256; i++)
    {
        sheet[i][0]=(char)i;
    };
    int output[300];
    FILE *fp, *of;
    output[0]=256;
    n=258;
    int o=1;
    fp = fopen("test.txt", "r");
    of = fopen("compressed.txt", "w");
    while (count!=0) {
    	//printf("%d\n",count);
    	fgets(input, 128, fp);
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
	        g=0;
	        while (tem[g]!='\0'){
	        	ch[0]=tem[g++];
	        	printf("%c\n",ch[0]);
	        	//printf("%c\n",ch);
	        	fwrite(ch, 1, 1, of);
	    	}
	    }	
    }
    printf("I've finished!\n");
    fclose(fp);
    fclose(of);
    return 0;
}
