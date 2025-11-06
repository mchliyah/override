#include <stdio.h>
#include <stdlib.h>


int main(){
    int ref = 5276;
    int input;
    printf("************************\n");
    printf("*    -Level00 -	  *\n");
    printf("************************\n");
    printf("password: ");
    scanf("%d", &input);

    if (ref == input)
        system("/bin/sh");
    else
        printf("invalide password !\n");
    
    return 0;
}