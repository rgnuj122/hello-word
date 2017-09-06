#include <stdio.h>


int lower(int a) 
{
    if ((a >= 'A') && (a <= 'Z'))
        a |= 0x20;
    return a;  
}

void get_lower_case (char *str)
{
    while (*str) {
        *str = lower(*str);
        str++;
    }
}

void search_str_in_file(char *fname, char* sub)
{
	get_lower_case(sub);
	printf ("%s\n", sub);
}

int main (void)
{
   
    search_str_in_file((char*)"test.txt", (char*)"FindTag");

    
    return (0);
}
