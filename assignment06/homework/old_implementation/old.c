#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/syscall.h>

#include <ctype.h>//toupper

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//frobcmp
//two arguments a and b as input // type char const *  // each points to an array of non-space bytes that is followed by space byte
//
//both arrays are frobnicated via memfrob
//should return the equivalent of running memcmp on the corresponding unfrobnicated arrays
//The space bytes are not considered to be part of either array, so they do not participate in the comparison.
//null bytes '\0' are allowed in the byte arrays and do contribute to the comparison.
//
//returns an int result that is negative, zero, or positive depending on whether a is less than, equal to, or greater than b
//If one unfrobnicated array is a prefix of the other, then consider the shorter to be less than the longer.
//it should look only at frobnicated bytes one at a time, and unfrobnicate them "by hand", so to speak
//
//should work on empty files and growing files
//
//errors
//input, 
//output or 
//memory allocation failures
//report the error to stderr
//exit with status 1
//otherwise exit with status 0
//
//sfrob that reads frobnicated text lines from standard input
//writes a sorted version to standard output in frobnicated form
//Frobnicated text lines consist of a series of non-space bytes followed by a single space
//
//If standard input ends in a partial record that does not have a trailing space
//your program should behave as if a space were appended to the input

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//uses system calls rather than <stdio.h> to read standard input and write standard output
//use a small number of read system calls
//by determining the input file's size when possible and allocating a buffer that is slightly larger than that size
//If the input's size cannot easily be determined
//ex because the input is a pipe
//start with a small buffer size of 8 KiB
//repeatedly append data to the buffer by reading the data
//(passing the largest count to read that cannot overrun the buffer)
//until read reports an error or end-of-file
//if the buffer fills up
//reallocate the buffer to be twice as large as it was before and then resume reading
//Assuming enough memory exists
//this approach should work even if the input file grows while your program is reading it

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// helper functions

int frobcmp(char const * a, char const * b, int f_flag);

char unfrob(char a){
	return (char)(a ^ 42);
}

void is_null_exit(void * a){
	if (a == NULL){
		write(2, "memory allocation failure\n", 26);
		exit(1);
	}
}

void read_err(int a){
	if (a < 0){
		write(2, "read error\n", 11);
		exit(1);
	}
}

void write_err(int a){
	if (a < 0){
		write(2, "write error\n", 12);
		exit(1);
	}
}

int compare(void const * a, void const * b) {
	int out = frobcmp(*(char **)a, *(char **)b, 0);
	return out;
}

int compare_f(void const * a, void const * b) {
        int out = frobcmp(*(char **)a, *(char **)b, 1);
        return out;
}

void prnt(char ** in_arr,unsigned int word_cnt) {
        int error = 0;
        char ch;
        for(unsigned int i = 0; i < word_cnt; i++){
                unsigned int j = 0;
                while((ch = in_arr[i][j]) != ' '){
                        error = write(1, &in_arr[i][j], 1);
                        write_err(error);
                        j++;
                }
                error = write(1," ",1);
                write_err(error);
        }
}
// frobcmp function

//return an integer greater than, equal to, or less than 0, 
//if the object pointed to by s1 is greater than, equal to, 
//or less than the object pointed to by s2, respectively.

int frobcmp(char const * a, char const * b, int f_flag){
	int i = 0;

	while(a[i] != ' ' && b[i] != ' '){
		char a_c = unfrob(a[i]); 
		char b_c = unfrob(b[i]);
		if(f_flag == 1){
			a_c = toupper(a_c);
			b_c = toupper(b_c);
		}
		if (a_c != b_c){
			return ((a_c - b_c) > 0) ? 1 : -1;
		}	
		i++;
	}
	
	//they are equal
	if(a[i] == ' '  && b[i] == ' '){
		return 0;
	}
	//a is a prefix
	else if(a[i] != ' ' && b[i] == ' '){
		return 1;
	}
	//b is a prefix
	else{
		return -1;
	}
}

int main(int argc, char *argv[]) {
	//check for -f flag
	unsigned int f_flag = 0;
	if(argc == 2 && 
	argv[1][0] == '-' &&
	argv[1][1] == 'f' &&
	argv[1][2] == '\0'){
		f_flag = 1;
	}
	else if(argc > 1){
		write(2,"incorrect function parameters\n",30);
		exit(1);	
	}

	struct stat fileData;
	if (fstat(0, &fileData) < 0)
	{
		write(2, "fstat error\n", 12);
		exit(1);
	}

	//create buffers
	char * file_b = NULL;	//buffer for input file
	char ** input_b = NULL;	//buffer for 2d arr
	char * word_b = NULL;	//buffer for each word in 2d arr

	//create indexes
	unsigned int input_i = 0;	//index for each word in 2d arr
	unsigned int word_l = 0;	//index for each char in each word in 2d arr
	unsigned int word_cnt = 0;	//count of words in input
	unsigned int grow_i = -1;	//index for growing input

	if(S_ISREG(fileData.st_mode)){
		//check if file is empty
		if(fileData.st_size == 0)
		{
			exit(0);
		}

		int len = fileData.st_size * sizeof(char);
		file_b = (char *) malloc(len);
		is_null_exit(file_b);

		int err = read(0, file_b, fileData.st_size);
		read_err(err);

		//find the ammount of words in the file
		unsigned int i = 0;
		while(i < fileData.st_size){
			if(file_b[i] == ' '){
				word_cnt++;
			}
			i++;
		}

		// increase ammount of words 
		//if there is not a space at the end of the file buffer
		unsigned int eob = fileData.st_size - 1;
		if(file_b[eob] != ' '){
			word_cnt++;
		}

		// allocate input buffer 2d array
		// length = amount of words
		len = word_cnt * sizeof(char *);
		input_b = (char **) malloc(len);
		is_null_exit(input_b);

		//create 2d array
		unsigned int word_i = 0;
		for(i = 0; i < fileData.st_size; i++){
			//growing file
			if(input_i == word_cnt - 1){
				len = (word_l + 1) * sizeof(char);
				word_b = (char *) realloc(word_b, len);
				is_null_exit(word_b);
				word_b[word_l] = file_b[i];

				//word is sliced
				if(file_b[i] != ' '){
					word_l++;
					continue;
				}

				input_b[input_i] = file_b + word_i;

				//reset length of word and free word buffer
				word_l = 0;
				free(word_b);
				input_i++;
			}
			//at space in file
			else if(file_b[i] == ' '){
				input_b[input_i] = file_b + word_i;
				input_i++;
				word_i = i + 1;
			}
		}
	}
	//input from stdin
	else{
		//allocate one by one char* for input buffer
		input_b = (char **) malloc(sizeof(char *));
		is_null_exit(input_b);
	}

	//growing input read char by char with read
	char c[1];
	while(read(0, c, 1)){
		read_err((int) *c);

		unsigned int len = (word_l + 1) * sizeof(char);
		word_b = (char *) realloc(word_b, len);
		is_null_exit(word_b);

		char ch = c[0];
		word_b[word_l] = ch;

		//if not space continue
		if(ch != ' '){
			word_l++;
			continue;
		}

		len = (input_i + 1) * sizeof(char *);
		input_b = (char **) realloc(input_b, len);
		is_null_exit(input_b);

		input_b[input_i] = word_b;

		if(grow_i == -1){
			//store where the input is growing from
			grow_i = input_i;
		}

		//reset word length and increase the input index
		word_l = 0;
		input_i++;

		//allocate new word
		word_b = (char *) malloc(sizeof(char));
		is_null_exit(word_b);
	}

	//fix the input to have a ' ' at the end
	if(word_l != 0){
		unsigned int len = (word_l + 1) * sizeof(char);
		word_b = (char *) realloc(word_b, len);
		is_null_exit(word_b);

		len = (input_i + 1) * sizeof(char *);
		input_b = (char **) realloc(input_b, len);
		is_null_exit(input_b);

		word_b[word_l] = ' ';
		input_b[input_i] = word_b;

		//last word added is not from file check if grow_i has changed
		if(grow_i == -1){
			grow_i = input_i;
		}

		input_i++;
	}
	else{
		free(word_b);
	}


	//update word count to show grown input
	word_cnt = input_i;

	//copy input buffer so we can later free the dynamically allocated parts
	unsigned int len = word_cnt * sizeof(char *);
	char ** input_b_copy = (char **) malloc(len);
	is_null_exit(input_b_copy);

	for(int i = 0; i < word_cnt; i++){
		input_b_copy[i] = input_b[i];
	}

	f_flag ? 
	qsort(input_b_copy, word_cnt, sizeof(char *), compare_f) :
	qsort(input_b_copy, word_cnt, sizeof(char *), compare);

	prnt(input_b_copy, word_cnt);

	//free growing file allocations
	if(grow_i != -1){
		for(int i = grow_i; i < word_cnt; i++){
			free(input_b[i]);
		}
	}

	//free file buffer
	if(S_ISREG(fileData.st_mode)){
		free(file_b);
	}

	//free input buffers
	free(input_b_copy);
	free(input_b);
	exit(0);
}