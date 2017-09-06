#define _GNU_SOURCE         /* See feature_test_macros(7) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int lower(int a) 
{
    if ((a >= 'A') && (a <= 'Z'))
        a |= 0x20; 
    return a;  
}

void get_lower_case(char** str)
{
    int str_len = strlen(*str);
    char *temp = (char *)malloc(str_len);


    for(char *t = temp, *s = *str; *s; ++s, ++t){//*str++ meant *(str++), not (*str)++
		*t =lower(*s);
	}
    strncpy( *str, temp, str_len);
    free(temp);
}

/*
void get_lower_case(char* str)
{
	char *temp = (char *)malloc(strlen(str));
	int str_len = strlen(str);
	while (*str)
    {
        *temp =lower(*str);
        temp++;
        str++;
    }
    strncpy( str, temp, str_len);
	free(temp);
}
*/

void fw_print_buffer(char *buff, int len)
{
	int i,j;
	char *buf = (char*)buff;
	
	for (i=0;i<len;i+=16)
	{
		printf("%04X  ",i);
		for (j=0;j<16;j++)
		{
			if ((i+j)>=len) break;
			printf("%.2c",buf[i+j]);
			printf((j==7)?"-":" ");
		}
		printf("\n");
		
	}
}

int string_finder(char* str, char* sub)
{
	char *p1, *p2, *p3;
  int i=0,j=0,flag=0;
  bool ret = 0;
  get_lower_case(&sub);
  
  p1 = str;
  p2 = sub;

  for(i = 0; i<strlen(str); i++)
  {
    if(*p1 == *p2)
      {
          p3 = p1;
          for(j = 0;j<strlen(sub);j++)
          {
            if(*p3 == *p2)
            {
              p3++;p2++;
            } 
            else
              break;
          }
          p2 = sub;
          if(j == strlen(sub))
          {
             flag = 1;
            printf("\nSubstring found at index : %d\n",i);
            ret = 1;
          }
      }
    p1++;
	usleep(100000); 
  }
  if(flag==0)
  {
       printf("Substring NOT found\n");
  }
  return ret;
}

void open_file_search(char *fname, char *sub)
{
	int check, line_num = 1;
	char line_storage[512], buffer[512];
	//FILE *input = fopen("OldFW.bin", "rb");
	FILE *input = fopen(fname, "r");
	//char sub[] = "SsdBintag";
	
	get_lower_case(&sub);
	while( fgets(line_storage, sizeof(line_storage), input) != NULL )  
	{
		printf(">>>>>>>>>>>>>>>>>>>>line %d \n", line_num);
	    check = 0;
	    
	    sscanf(line_storage,"%s",buffer);
	    fw_print_buffer(line_storage, sizeof(line_storage));
	    if(strcmp(buffer,"SsdBintag") == 0)  check = 1;
	    //if(string_finder(buffer,sub) == 1)  check = 1;
	    /*while(strstr(buffer,sub)!=NULL) 
		{
			check = 1;
			break;
	    }*/
		/*int i =0;
		while(i++<512)
		{
			if (memcmp(buffer++, sub, 9) == 0)check == 1;
			break;
		}*/
		if(check == 1){ printf("Word found on line %d ", line_num);break;}
	    line_num++;
	}	
	/*char str[] = "String1 subString1 Strinstrnd subStr ing1subString";
	char sub[] = "subString";
	string_finder(str, sub);
	string_finder((char*)"test reiojd jiosjioqw test ", (char*)"test");*/
	
}
static void parse_config_file(const char *p)
{
	const char *q, *r;

	while ((p = strstr(p, "SsdBintag"))) {
		printf("found!!!!!!!!!!\n\n\n\n");
	}
}

void open_fd(const char *filename)
{
	int fd;
	struct stat st;
	char *map;
	fd = open(filename, O_RDONLY);
	if (fstat(fd, &st) < 0) {
		fprintf(stderr, "fixdep: error fstat'ing config file: ");
		perror(filename);
		exit(2);
	}
	if (st.st_size == 0) {
		close(fd);
		return;
	}
	map = (char*)malloc(sizeof(char*)*st.st_size + 1);
	if (!map) {
		perror("fixdep: malloc");
		close(fd);
		return;
	}
	if (read(fd, map, st.st_size) != st.st_size) { //here dead
		perror("fixdep: read");
		close(fd);
		return;
	}
	map[st.st_size] = '\0';
	close(fd);
	fw_print_buffer(map ,st.st_size);
	parse_config_file(map);

	free(map);
}

const void *memmem(
  const void *haystack, size_t haystacklen, 
  const void *needle,   size_t needlelen )
{
  // Sanity check
  if (needlelen > haystacklen) return NULL;

  // Void is useless -- we must treat our data as bytes (== unsigned chars)
  typedef const unsigned char* p;

  // We'll stop searching at the last possible position for a match, 
  // which is haystack[ haystacklen - needlelen + 1 ]
  haystacklen -= needlelen - 1;

  while (haystacklen)
  {
    // Find the first byte in a potential match
    p z = (p)memchr( (p)haystack, *(p)needle, haystacklen );
    if (!z) return NULL;

    // Is there enough space for there to actually be a match?
    ptrdiff_t delta = z - (p)haystack;
    ptrdiff_t remaining = (ptrdiff_t)haystacklen - delta;
    if (remaining < 1) return NULL;

    // Advance our pointer and update the amount of haystack remaining
    haystacklen -= delta;
    haystack = z;

    // Did we find a match?
    if (!memcmp( haystack, needle, needlelen )) return haystack;
    
    // Ready for next loop
    haystack = (p)haystack + 1;
    haystacklen -= 1;
  }
  return NULL;
}

void open_file_fread(const char *fname)
{
	char *tmp;
	FILE *file;
	struct stat st;

	file = fopen(fname, "r");
	if (file == NULL)
		goto open_fail;

	if (fstat(fileno(file), &st) < 0)
		goto close_write;

	if (st.st_size == 0)
		goto close_write;

	tmp = (char*)malloc(sizeof(char*)*st.st_size + 1);
	if (fread(tmp, 1, st.st_size, file) != st.st_size)
		goto free_write;

	//if (memcmp(tmp, b->p, b->pos) != 0)
	//	goto free_write;
	

	free(tmp);
	fclose(file);
	return;

 free_write:
	free(tmp);
 close_write:
	fclose(file);
 open_fail:
 	printf("open fail\n");
	fclose(file);
	
}
int Search_in_File(char *fname, char *str) {
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[65535];
	char* pos;
	
	get_lower_case(&str);
	//gcc users
	if((fp = fopen(fname, "r")) == NULL) {
		return(-1);
	}

	//Visual Studio users
	//if((fopen_s(&fp, fname, "r")) != NULL) {
	//	return(-1);
	//}

	while(fgets(temp, 65535, fp) != NULL) {
		if((pos = strstr(temp, str)) != NULL) {
			printf("A match found on line: %d\n", line_num);
			printf("\n%s\n", temp);
			printf("%c%c%c\n",pos[0],pos[1],pos[2]);
			find_result++;
			break;
		}
		line_num++;
		printf("line num: %d\n", line_num);
	}

	if(find_result == 0) {
		printf("\nSorry, couldn't find a match.\n");
	}
	
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
   	return(0);
}
int Search_in_File_memmem(char *fname, char *str) {
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[1024];
	char* pos;
	int i;
	//gcc users
	if((fp = fopen(fname, "r")) == NULL) {
		return(-1);
	}

	//Visual Studio users
	//if((fopen_s(&fp, fname, "r")) != NULL) {
	//	return(-1);
	//}

	while(fgets(temp, 1024, fp) != NULL) {
		/*for (i=0;i<65535;i++)
		{
			if(memcmp(temp, str, 9)==0)
			{
				printf("A match found on line: %d\n", line_num);
				find_result++;
				break;
			}
		}*/
		
		pos = (char*)memmem(temp,  strlen(temp), str, strlen(str)-1);
		if(pos != NULL) {
			printf("A match found on line: %d\n", line_num);
			printf("\n%s\n", temp);
			printf("%c%c%c\n",pos[0],pos[1],pos[2]);
			find_result++;
			break;
		}
		line_num++;
		if(find_result>0)break;
		printf("line num: %d\n", line_num);
	}

	if(find_result == 0) {
		printf("\nSorry, couldn't find a match.\n");
	}
	
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
   	return(0);
}

int search_in_binary(char cFileInName[], char cFileOutName[])
{
	 // used to store the user input which represent
  //  the input and output file name and location
  //char cFileInName[]= "OldFW.bin";
  //char cFileOutName[]= "output.txt";
  // pointers to input and output files
  FILE *FileIn, *FileOut;
 
  char outStringData[256]; // Character variable to hold data
  // char outFilename[256];   // Another character variable to hold data*/
 
  long int position = 0;   // position counter
  int currentFile = 1;     // file counter
 
  // hex pattern to search for
  //long pattern = 0x53736442696E746167; 
  //char pattern[9] = {'S','s','d','B','i','n','t','a','g'};
  char pattern[9] = {0x53,0x73,0x64,0x42,0x69,0x6e,0x74,0x61,0x67};
  char *patternArray = (char *)&pattern; // Create an array called pattern to hold the bytes read in from the input file
 
  long int tempInteger = 0; // file size accumulator
  char *tempBuffer = (char *)&tempInteger;
 
  //printf("Please enter the name and path of the binary file: ");                  
  //gets(cFileInName); // Get the user input and store it in the character array

 
  if ( (FileIn = fopen(cFileInName, "rb") ) == NULL)
  { // error reading the file
    printf("The file could not be opened, please check your file and location before trying again.\n");
    return 0;
  }
 
  //printf("Enter the name and path for the output file: ");
  //gets(cFileOutName);

 
  // create a file and open it in append mode ready for output
  FileOut = fopen(cFileOutName, "a");
 
  // while the end of file has not been reached
  while (!feof(FileIn)) 
  {
    position++;   // increase the character counter
    // Check the next byte in the file to see if it matches the
    //  file header, fgetc takes the file pointer and returns
    //  the byte that was read from the file.
    if (fgetc(FileIn) == patternArray[0])
        {
 
      // When it finds the first byte in patternArray. Set the first
      //  byte in tempBuffer to the first byte in patternArray.
      tempBuffer[0] = patternArray[0];
 
      // read the next 3 bytes into the rest of tempBuffer
      fread(tempBuffer + 1, sizeof(char), 9, FileIn);
 /*
      // Check if the 4 bytes read from the file are the pattern.
            if(tempInteger != pattern)
            { printf("Hello Mofo\n");
        // Skip back three bytes so that it doesnt miss any headers
        //  due to a flase positive being read in
        fseek(FileIn, -3, SEEK_CUR);                                     
        continue;  // Go back to the top of the loop
            }
 */
            //fseek(FileIn, 7, SEEK_CUR); // Seek ahead 7 bytes and then extract 4
            fread(tempBuffer, sizeof(char), 4, FileIn);
 
      // create the string so we can write to the output file, store it in outStringData
            sprintf(outStringData, "&#37;-5i\tOffset: %-12ld\tSize: %ld\n", currentFile++, position - 1, tempInteger);
      // write the string to the output file
            fwrite(outStringData, sizeof(char), strlen(outStringData), FileOut);   
        } 
    }
 
  // close the files and at the same time flush the buffers
  fclose(FileIn);
  fclose(FileOut);
 
  printf("\n Done!\n Press enter to exit\n");
  getchar();
 
}
int fw_search_by_byte(char *fname, char* searchChars, char* lastBytes, int lastBytes_len)
{
	
	int pos = 0; 
	int i = 0;
	int curSearch = 0;
	FILE *pfile;
	char curChar;
	size_t read_result;
	if ( (pfile = fopen(fname, "rb") ) == NULL)
	{ // error reading the file
		printf("The file could not be opened, please check your file and location before trying again.\n");
		return 0;
	}
	//char* sc = searchChars;
	get_lower_case(&searchChars);
	printf ("%s\n", searchChars);
//	memcpy(searchChars, sc, strlen(searchChars));
	
	while(!feof(pfile)) {
	    curChar = getc(pfile); pos++;            /*readone char*/
	
	    if(curChar == searchChars[curSearch]) { /* found a match */
	        curSearch++;                        /* search for next char */
	        if(curSearch > 3)printf("find %c in pos:%d\n", curChar, pos);
	        if(curSearch > 8) {                 /* found the whole string! */
	        printf("find %s in pos:%d\n", searchChars, pos);
	            curSearch = 0;                  /* start searching again */
	            read_result = fread(lastBytes,1,lastBytes_len,pfile); /* read 10 bytes */
	            fw_print_buffer(lastBytes, 32);
				//printf("get str:%s\n",lastBytes);
	            pos += 5;                      /* advance position by how much we read */
	        }
	    } else { /* didn't find a match */
	        curSearch = 0;                     /* go back to searching for first char */
	    }
 	}
 
}
int main()
{
	//open_file_search((char*)"OldFW.bin", (char*)"SsdBintag");
	//open_fd("test.txt");
	//open_file_fread("test.txt");
	//
	//fw_print_buffer((char*)"SsdBinTag", 10);
	//Search_in_File_memmem((char*)"OldFW.bin", (char*)"SsdBinTag");
	//Search_in_File((char*)"test.txt", (char*)"SsdBinTag");
	
	//search_in_binary((char*)"test.txt", (char*)"output.txt");
	char lastBytes[32];
	//fw_search_by_byte((char*)"test.txt", (char*)"SsdBintag", lastBytes, 32);
	char fname[] = "OldFW.bin";
	char searchchars[] = "SsdBintag";
	printf("by getc:\n");
	fw_search_by_byte(fname, searchchars, lastBytes, 32);
	printf("by a*==b*:\n");
	open_file_search(fname, searchchars);
	printf("by fread\n");
	open_file_fread(fname);
	printf("by memmem\n");
	Search_in_File_memmem(fname, searchchars);
	printf("by strstr\n");
	Search_in_File(fname, searchchars);
	return (0);

}
