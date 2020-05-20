#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fileOutputCount10k;

    fileOutputCount10k = fopen("Count10k.txt", "w");

    if(fileOutputCount10k == NULL)
    {
        printf("(I) ERROR : fopen()\n");
        exit(1);
    }

    for(int i = 1; i < 10001; i++)
    {
        fprintf(fileOutputCount10k, "%6d", i);
        if((i % 10) == 0) fputs("\n", fileOutputCount10k);
    }

    fclose(fileOutputCount10k);

    return 0;
}
