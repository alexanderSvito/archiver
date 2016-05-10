#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;

typedef struct bfile
{
    FILE *file;
    unsigned char status;
    int r;
    int P_COUNTer;
}
BFILE;

#define P_COUNT 2047
#define SHEET_SIZE

/*int l =64;
char sheet[SHT_L][64];
char temp[64];
int pos,i,m,n;*/

BFILE *InputIBFile( char *name )
{
   BFILE *bfile;

   bfile = (BFILE *) calloc( 1, sizeof( BFILE ) );
   bfile->file = fopen( name, "rb" );
   bfile->r = 0;
   bfile->status = 0x80;
   bfile->pcount = 0;
   return bfile;
}

BFILE *InputOBFile ( char * name )
{
   BFILE *bfile;

   bfile = (BFILE *) calloc( 1, sizeof( BFILE ) );
   bfile->file = fopen( name, "wb" );
   bfile->r = 0;
   bfile->status = 0x80;
   bfile->pcount = 0;
   return bfile;
}

void CloseOBFile ( BFILE *bfile )
{
   if ( bfile->status != 0x80 )
      putc( bfile->r, bfile->file );
   fclose ( bfile->file );
   free ( (char *) bfile );
}

void CloseIBFile ( BFILE *bfile )
{
    fclose ( bfile->file );
    free ( (char *) bfile );
}

void WriteBit ( BFILE *bfile, int bit )
{
   if ( bit )
      bfile->r |= bfile->status;
   bfile->status >>= 1;
   if ( bfile->status == 0 )
   {
      putc( bfile->r, bfile->file );
      if ( ( bfile->pcount++ & PACIFIER_COUNT ) == 0 )
         putc( '.', stdout );
      bfile->r = 0;
      bfile->status = 0x80;
   }
}

void WriteBits( BFILE *bfile, unsigned long code, int count )
{
   unsigned long status;

   status = 1L << ( count - 1 );
   while ( status != 0)
   {
      if ( status & code )
  bfile->r |= bfile->status;
      bfile->status >>= 1;
      if ( bfile->status == 0 )
      {
  putc( bfile->r, bfile->file );
  if ( ( bfile->pcount++ & PACIFIER_COUNT ) == 0 )
            putc( '.', stdout );
  bfile->r = 0;
  bfile->status = 0x80;
      }
      status >>= 1;
   }
}


/*int ReadBit( BFILE *bfile )
{
   int value;

   if ( bfile->status == 0x80 )
   {
      bfile->r = getc( bfile->file );
      if ( bfile->r == EOF )
         printf( "Error in function ReadBit!\n" );
      if ( ( bfile->pcount++ & PACIFIER_COUNT ) == 0 )
         putc( '.', stdout );
   }

   value = bfile->r & bfile->status;
   bfile->status >>= 1;
   if ( bfile->status == 0 )
      bfile->status = 0x80;
   return ( value ? 1 : 0 );
}*/

unsigned long ReadBits ( BFILE *bfile, int bit_count )
{
   unsigned long status;
   unsigned long return_value;

   status = 1L << ( bit_count - 1 );
   return_value = 0;
   while ( status != 0 )
   {
      if ( bfile->status == 0x80 )
      {
  bfile->r = getc( bfile->file );
  if ( bfile->r == EOF )
            printf( "Error in function ReadBits!\n" );
  if ( ( bfile->pcount++ & PACIFIER_COUNT ) == 0 )
            putc( '.', stdout );
      }
      if ( bfile->r & bfile->status )
         return_value |= status;
      status >>= 1;
      bfile->status >>= 1;
      if ( bfile->status == 0 )
  bfile->status = 0x80;
   }

   return return_value;
}

long file_size ( char *name )
{
   long eof_ftell;
   FILE *file;

   file = fopen( name, "r" );
   if ( file == NULL )
      return( 0L );
   fseek( file, 0L, SEEK_END );
   eof_ftell = ftell( file );
   fclose( file );
   return eof_ftell;
}

struct dictionary
{
   int code_value;
   int prefix_code;
   char ch;
}
dict[SHEET_SIZE];

/*char decode_stack[SHEET_SIZE];

unsigned int decode_string ( unsigned int count, unsignedint code )
{
   while ( code > 255 ) 
   {
      decode_stack[count++] = dict[code].ch;
      code = dict[code].prefix_code;
   }
   decode_stack[count++] = (char) code;
   return count;
}*/

unsigned int search ( int prefix_code, int ch )
{
   int index;
   int offset;

   index = ( ch << ( BITS - 8 ) ) ^ prefix_code;
   if ( index == 0 )
      offset = 1;
   else
      offset = TABLE_SIZE - index;
   for ( ; ; )
   {
      if ( dict[index].code_value == UNUSED )
         return index;
      if ( dict[index].prefix_code == prefix_code &&
           dict[index].ch == (char) ch )
         return index;
      index -= offset;
      if ( index < 0 )
         index += TABLE_SIZE;
   }
}


unsigned int decode_string ( unsigned int count, unsigned int code )
{
   while ( code > 255 ) /* Пока не встретится код символа */
   {
      decode_stack[count++] = dict[code].ch;
      code = dict[code].prefix_code;
   }
   decode_stack[count++] = (char) code;
   return count;
}

void CompressFile ( FILE *input, BFILE *output )
{
   int next_code, ch, string_code;
   unsigned int index, i;
   next_code = FIRST_CODE;
   for ( i = 0 ; i < SHEET_SIZE ; i++ )
       dict[i].code_value = UNUSED;
   if ( ( string_code = getc( input ) ) == EOF )
       string_code = END_OF_STREAM;
   while ( ( ch = getc( input ) ) != EOF )
   {
      index = search ( string_code, ch );
      if ( dict[index].code_value != -1 )
         string_code = dict[index].code_value;
      else
      {
  if ( next_code <= MAX_CODE )
         {
            dict[index].code_value = next_code++;
            dict[index].prefix_code = string_code;
            dict[index].ch = (char) ch;
         }
         WriteBits( output, (unsigned long) string_code, BITS );
         string_code = ch;
      }
   }
   WriteBits( output, (unsigned long) string_code, BITS );
   WriteBits( output, (unsigned long) END_OF_STREAM, BITS );
}

void ExpandFile ( BFILE *input, FILE *output )
{
   unsigned int next_code, new_code, old_code;
   int ch;
   unsigned int count;

   next_code = FIRST_CODE;
   old_code = (unsigned int) ReadBits( input, BITS );
   if ( old_code == END_OF_STREAM )
      return;
   ch = old_code;

   putc ( old_code, output );

   while ( ( new_code = (unsigned int) ReadBits( input, BITS ) )
             != END_OF_STREAM )
   {
      if ( new_code >= next_code )
      {
         decode_stack[ 0 ] = (char) ch;
         count = decode_string( 1, old_code );
      }
      else
         count = decode_string( 0, new_code );

      ch = decode_stack[ count - 1 ];
      while ( count > 0 )
         putc( decode_stack[--count], output );
      if ( next_code <= MAX_CODE )
      {
         dict[next_code].prefix_code = old_code;
         dict[next_code].ch = (char) ch;
         next_code++;
      }
      old_code = new_code;
   }
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
