//tr2b.c
#include <stdio.h>
#include <stdlib.h>

//function prototype
void tr(char* from, char* to); 

//setup buffer
char buf[256] = {0};

int main(int argc, char *argv[]) {
	//check operands
	if(argc != 3){
		fprintf(stderr,"incorrect amount of arguments\n");
		exit(1);
	}

	//use tr helper fxn
	tr(argv[1],argv[2]);

	//read till end of file
	int c;
	while((c=getchar()) != EOF){
		int err = ferror(stdin);
		
		//check for stdin error
		if(err != 0){
			fprintf(stderr,"stdin error. code: %d\n",err);
			exit(1);
		}

		//if char in buf was set to something different
		//change out to what it was changed to
		//else keep it the same
		char out;
		if(buf[c] == 0){
			out = c;
		}
		else{
			out = buf[c];
		}
		
		int pc = putchar(out);
		//check for putchar being EOF
		if(pc == EOF){
			fprintf(stderr,"output error\n");
			exit(1);
		}
	}
	return 0;
}

//tr function definition
void tr(char* from, char* to){

	unsigned int i = 0;

	while(from[i] != 0 && to[i] != 0){
		int f = from[i];
		
		//check for duplicate bytes
		if(buf[f] != 0){
			fprintf(stderr,"no duplicate bytes\n");
			exit(1);
		}
		buf[f] = to[i];
		i++;
	}

	if(from[i] == 0 && to[i] == 0){
		return;
	}
	else{//to and from were different lengths
		fprintf(stderr,"inputs cannot be different lengths\n");
		exit(1);
	}
}
