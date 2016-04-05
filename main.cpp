#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

int kek=0;
char input[64];
int l =64;
char sheet[1025][64];
char temp[64];
int pos,i,m,n;

int search(int quant,int i)
{
    if (quant==2)
    {
        pos = (int)input[i];
    }
    int k=0;
    for (int j=i; j<i+quant; k++,j++)
    {
        temp[k]=input[j];
    }
    m=quant;
    for (l=257; l<1025; l++)
    {
        if (strcmp(sheet[l],temp)==0)
        {
            pos=l;
            search(quant+1,i);
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
    for (i=0;i<256; i++)
    {
        sheet[i][0]=(char)i;
    };
    ifstream got("test.md");
    ofstream output("compressed.md");
    output<<"256/";
    n=258;
    while (!(got.getline(input,64).eof()))
    {
        got>>input;
        for (i=0; i<64;i=i+m-1)
        {
            m=2;
            output<<search(m,i)<<"/";
        }
        memset(input,'\0',64);
    }
    output<<"257";
    got.close();
    output.close();
    return 0;
}
