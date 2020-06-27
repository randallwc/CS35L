#include <string.h>
#include <stdlib.h>
#include <stdio.h>// I/O

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

// helper functions


int frobcmp(char const * a, char const * b);

char unfrob(char a){
	return (char)(a ^ 42);
}

void is_null_exit(void * a){
	if (a == NULL){
		fprintf(stderr, "memory allocation failure\n");
		exit(1);
	}
}

int compare(void const * a, void const * b) {
	int out = frobcmp(*(char **)a, *(char **)b);
	return out;
}

void prnt(char ** in_arr,unsigned int r_word) {
        int error = 0;
        char ch;
        for(unsigned int i = 0; i < r_word; i++){
                unsigned int j = 0;
                while((ch = in_arr[i][j]) != ' '){
                        error = fprintf(stdout,"%c",ch);
                        if(error < 0){
                                fprintf(stderr, "error while printing to stdout. code:%d\n", error);
                                exit(1);
                        }
                        j++;
                }
                error = 0;
                error = fprintf(stdout, " ");
                if(error < 0){
                        fprintf(stderr, "error while printing to stdout. code:%d\n", error);
                        exit(1);
                }
                free(in_arr[i]);//free each word
        }
	free(in_arr);//free final ptr
}
// frobcmp function

//return an integer greater than, equal to, or less than 0, 
//if the object pointed to by s1 is greater than, equal to, 
//or less than the object pointed to by s2, respectively.

int frobcmp(char const * a, char const * b){
	int i = 0;

	while(a[i] != ' ' && b[i] != ' '){
		char a_c = unfrob(a[i]); 
		char b_c = unfrob(b[i]);
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

// input function

char ** input(unsigned int * row,unsigned int * column){
	//*row = 0;
	//*column = 0;
	
	int len = 0;

	char ** in = (char **) malloc(sizeof(char*));
	char * word = (char *) malloc(sizeof(char));

	is_null_exit(in);
	is_null_exit(word);

	unsigned int word_l = 0; 
	unsigned int row_l = 0;

	int c = 0;	
	while((c = getchar()) != EOF){
		int e_num = ferror(stdin);
		if(e_num != 0){
			fprintf(stderr, "input failure. code:%d\n", e_num);
			exit(1);
		}

		//add chars
		len = (word_l + 1) * sizeof(char);
		word = (char *) realloc(word, len);

		is_null_exit(word);

		word[word_l] = c;
		
		//add words when ' '
		if(c == ' '){
			//update the 2d array column number
			*column = ((word_l+1) > *column) ? (word_l+1) : *column;//maximum 

			len = (row_l + 1) * sizeof(char *);
			in = (char **) realloc(in, len);

			is_null_exit(in);

			in[row_l] = word;
			word_l = 0;
			word = (char *) malloc(sizeof(char));
			row_l++;
			continue;
		}
		word_l++;
	}

	//means that the last char in the last word was not a ' '
	if(word_l != 0){
		*column = ((word_l+1) > *column) ? (word_l+1) : *column;//maximum

		len = (word_l + 1) * sizeof(char);
                word = (char *) realloc(word, len);

		len = (row_l + 1) * sizeof(char *);
		in = (char **) realloc(in, len);

		is_null_exit(word);
		is_null_exit(in);

		word[word_l] = ' ';
		in[row_l] = word;

		row_l++;
	}

	//update function parameters
	if((*row = row_l) == 0 || word_l == 0){
		free(word);
	}
	return in;
}

// main
int main() {
	unsigned int c_char = 0;
	unsigned int r_word = 0;

	char ** in_arr = input(& r_word, & c_char);

	qsort(in_arr, r_word, sizeof(char *), compare);

	prnt(in_arr, r_word);
	exit(0);
}
