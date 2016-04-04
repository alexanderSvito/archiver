#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
char* input = "hello my baby, hello my darling, hello my right time gal";
int l =64;
char sheet[1025][64];
char temp[64];
int pos,i,m,n;
int compare(char* s1,char *s2)
{
    int res =0;
    for (int p=0; p<65; p++)
    {
        res=res+(s1[p]-s2[p]);
    }
    return res;
}

int search(int quant,int i)
{
    if (quant==2)
    {
        pos = (int)input[i];
    }
    //printf("%d ",pos);
    int k=0;
    for (int j=i; j<i+quant; k++,j++)
    {
        temp[k]=input[j];
        //printf("%d\n",quant);
        //printf("%c\n",input[j]);
        //printf("%c\n",temp[k]);
    }
    //printf("%s\n",temp);
    //sheet[2014]=;
    for (l=257; l<1025; l++)
    {
        //printf("%s \n %s \n\n",sheet[l],temp);
        if (strcmp(sheet[l],temp)==0)
        {
            pos=l;
            printf("%d\n",pos);
            //printf("%s\n",sheet[1024]);
            //printf("%s\n",temp);
            //printf("Hello! \n");
            /*if (quant==7)
            {
            exit(0);
            }*/
            search(quant+1,i);
        }
    }
    for (int f=0; f<65;f++)
    {
        sheet[n][f]=temp[f];
        //printf("%d\n",n);
    }
    n++;
    //printf("%d\n",n);
    quant=2;
    return pos;
}

int main()
{
    for (i=0;i<256; i++)
    {
        sheet[i][0]=(char)i;
     //   printf("%c ",sheet[i]);
    };
    //printf("%c ",sheet[1024]);
    int output[300];
    output[0]=256;
    n=258;
    int o=1;
    for (i=0; i<56;i=i+1,o++)
    {
        m=2;
        output[o]=search(m,i);
//        printf("%d ",output[o]);
        //printf("I work !");
    }
    output[o]=257;
    for (i=0; i<o+1; i++)
    {
        printf("%d ",output[i]);
    }
    /*for (i=0;i<400;i++)
    {
        printf("%s \n",sheet[i]);
    }*/
    return 0;
}
