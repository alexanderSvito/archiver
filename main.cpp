#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define P_COUNT 2047
#define BITS 12
#define MAX_CODE ((1 << BITS)-1)
#define SHEET_SIZE 65536
#define END_OF_STREAM 256
#define FIRST_CODE 257
#define UNUSED -1

using namespace std;

typedef struct bfile
{
    FILE *file;
    unsigned char status;
    int r;
    int pcount;
} BFile;

struct dictionary
{
   int code_value;
   int prefix_code;
   char ch;
} dict[SHEET_SIZE];

char decode_stack[SHEET_SIZE];

BFile *InputIBFile(char *name){
   BFile *bfile;

   bfile = (BFile *) calloc( 1, sizeof( BFile ) );
   bfile->file = fopen( name, "rb" );
   bfile->r = 0;
   bfile->status = 0x80;
   bfile->pcount = 0;
   return bfile;
}

BFile *InputOBFile (char *name){
   BFile *bfile;

   bfile = (BFile *) calloc( 1, sizeof( BFile ) );
   bfile->file = fopen( name, "wb" );
   bfile->r = 0;
   bfile->status = 0x80;
   bfile->pcount = 0;
   return bfile;
}

void CloseOBFile (BFile *bfile){
    if ( bfile->status != 0x80 )
        putc( bfile->r, bfile->file );
    fclose ( bfile->file );
    free ( (char *) bfile );
}

void CloseIBFile (BFile *bfile){
    fclose ( bfile->file );
    free ( (char *) bfile );
}

void WriteBit ( BFile *bfile, int bit )
{
    if ( bit )
        bfile->r |= bfile->status;
    bfile->status >>= 1;
    if ( bfile->status == 0 ) {
        putc( bfile->r, bfile->file );
        bfile->r = 0;
        bfile->status = 0x80;
    }
}

void WriteBits(BFile *bfile, unsigned long code, int count){
    unsigned long status;

    status = 1L << ( count - 1 );
   	while ( status != 0) {
	    if ( status & code )
	  		bfile->r |= bfile->status;
	    bfile->status >>= 1;
	    if ( bfile->status == 0 ) {
	  		putc( bfile->r, bfile->file );
	  		bfile->r = 0;
	  		bfile->status = 0x80;
	    }
	    status >>= 1;
    }
}


int ReadBit(BFile *bfile){
   int value;

    if ( bfile->status == 0x80 ) {
        bfile->r = getc( bfile->file );
        if (bfile->r == EOF)
            printf( "Error while reading bit from file!\n" );
    }

    value = bfile->r & bfile->status;
    bfile->status >>= 1;
    if ( bfile->status == 0 )
        bfile->status = 0x80;
   	return ( value ? 1 : 0 );
}

unsigned long ReadBits (BFile *bfile, int bit_count )
{
    unsigned long status;
    unsigned long return_value;

    status = 1L << ( bit_count - 1 );
    return_value = 0;
    while (status != 0 ) {
        if (bfile->status == 0x80 ) {
  			bfile->r = getc( bfile->file );
			if (bfile->r == EOF )
			    printf( "Error while reading bits from file!\n" );
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

long fileSize (char *name)
{
    long eof_ftell;
    FILE *file;

    file = fopen( name, "r" );
    if (file == NULL)
        return(0L);
    fseek(file, 0L, SEEK_END);
    eof_ftell = ftell(file);
    fclose(file);
    return eof_ftell;
}

unsigned int decodeString ( unsigned int count, unsigned int code )
{
    while ( code > 255 ) {
        decode_stack[count++] = dict[code].ch;
        code = dict[code].prefix_code;
    }
    decode_stack[count++] = (char)code;
    return count;
}

unsigned int search ( int prefix_code, int ch )
{
    int index;
    int offset;

    index = ( ch << ( BITS - 8 ) ) ^ prefix_code;
    if ( index == 0 )
        offset = 1;
    else
        offset = SHEET_SIZE - index;
    for ( ; ; ) {
        if ( dict[index].code_value == UNUSED )
            return index;
        if ( dict[index].prefix_code == prefix_code &&
            dict[index].ch == (char) ch )
            return index;
        index -= offset;
        if (index < 0)
            index += SHEET_SIZE;
    }
}


void CompressFile ( FILE *input, BFile *output )
{
    int next_code, character, string_code;
    unsigned int index, i;
    next_code = FIRST_CODE;
    for (i = 0; i < SHEET_SIZE; i++ ) {
        dict[i].code_value = UNUSED;
    }
    if ((string_code = getc(input)) == EOF )
        string_code = END_OF_STREAM;
    while ((character = getc(input)) != EOF ) {
        index = search ( string_code, character);
        if ( dict[index].code_value != -1 )
            string_code = dict[index].code_value;
        else {
            if ( next_code <= MAX_CODE ) {
	            dict[index].code_value = next_code++;
	            dict[index].prefix_code = string_code;
	            dict[index].ch = (char)character;
            }
            WriteBits(output,(unsigned long)string_code, BITS );
            string_code = character;
        }
    }
    WriteBits(output, (unsigned long) string_code, BITS);
    WriteBits(output, (unsigned long) END_OF_STREAM, BITS);
}

void ExpandFile ( BFile *input, FILE *output )
{
    unsigned int next_code, new_code, old_code;
    int ch;
    unsigned int count;

    next_code = FIRST_CODE;
    old_code = (unsigned int) ReadBits( input, BITS );
    if (old_code == END_OF_STREAM )
        return;
    ch = old_code;
    putc (old_code, output);
    while ((new_code = (unsigned int)ReadBits(input, BITS))!= END_OF_STREAM ) {
        if ( new_code >= next_code ) {
            decode_stack[ 0 ] = (char) ch;
            count = decodeString( 1, old_code );
        } else
            count = decodeString( 0, new_code );
        ch = decode_stack[ count - 1 ];
        while ( count > 0 )
            putc( decode_stack[--count], output );
        if ( next_code <= MAX_CODE ) {
            dict[next_code].prefix_code = old_code;
            dict[next_code].ch = (char) ch;
            next_code++;
        }
        old_code = new_code;
    }
}

void printStats(char *input, char *output)
{
    long input_size;
    long output_size;
    int a;

    input_size = fileSize(input);
    if (input_size == 0)
       input_size = 1;
    output_size = fileSize( output );
    a = 100-(int)(output_size * 100/input_size);
    printf( "\nInput size:        %ld bytes\n", input_size );
    printf( "Output size:       %ld bytes\n", output_size );
    if (output_size == 0)
       output_size = 1;
    printf("Efficiency: %d%%\n", a);
}

int main(int argc, char* argv[]) {
	setbuf( stdout, NULL );

	if (argc < 4 ||(argv[1][1]!= 'r' && argv [1][1] != 'R' && argv[1][1]!= 'e' && argv [1][1] != 'E' && argv[1][1] != 'd' && argv[1][1] != 'D')){
	    printf ("Using: svt -e/-d/-r infile outfile\n");
	    printf ("-e   encoding (compression)\n");
	    printf ("-d   decoding (decompression)\n");
	    printf ("-r   display summary when done\n");
	    printf ("All other directives will be ignored\n");
	    exit(0);
	}	 
	if (argc == 5) {
		char *tmp;
		tmp = argv[2];
		argv[2] = argv[3];
		argv[3] = argv[4];
		argv[4] = tmp;
	}
	if (argv[1][1]== 'r' || argv [1][1] == 'R') {
		char *tp;
		tp = argv[4];
		argv[4] = argv[1];
		argv[1] = tp;
	}
	if (argv[1][1]== 'e' || argv [1][1] == 'E') {
	    BFile *output;
	    FILE *input;

	    input = fopen( argv[2], "rb" );
	    if (input == NULL) {
	  		printf("Error while opening %s file\n", argv[2]);
			exit(0);
		}

	    output = InputOBFile(argv[3]);
	    if (output == NULL) {
	        printf("Error while opening %s file\n", argv[3]);
	        exit(0);
	    }
	    printf("Convertation process is taking place, please wait\n");
	  
	  	CompressFile(input, output);
	  	CloseOBFile(output);
	  	fclose(input);

	  	printf( "Compression complete.\n" );
	  	if (argv[4][1] == 'r' || argv[4][1] == 'R') {
	  		printStats(argv[2], argv[3]);
	  	}
	} else if (argv[1][1] == 'd' || argv[1][1] == 'D') {
	   	BFile *input;
	   	FILE *output;

	   	input = InputIBFile(argv[2]);
	   	if (input == NULL ) {
	   		printf("Error while opening %s file\n", argv[2]);
	   		exit(0);
	   	}
	  
	  	output = fopen(argv[3],"wb");
	  	if (output == NULL) {
	  		printf("Error while opening %s file\n", argv[3]);
	  		exit(0);
	  	}
	  	printf( "Decompression process  is taking place\n");
	  
	  	ExpandFile(input, output);
	    CloseIBFile(input);
	    fclose(output);

	  	printf( "\nDecompression complete." );
	  	if (argv[4][1] == 'r' || argv[4][1] == 'R') 
	  		printStats(argv[2], argv[3]);
	}
    return 0;
}