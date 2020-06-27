//tr2u.c
#include <unistd.h>
#include <stdlib.h>

//function prototype
void tr(char* from, char* to); 

//setup buffer
char buf[256] = {0};

int main(int argc, char *argv[]) {
	//check operands
	if(argc != 3){
		write(2,"incorrect amount of arguments\n",30);//cant
		exit(1);
	}

	//use tr helper fxn
	tr(argv[1],argv[2]);

	//read till end of file
	int c[0];
	while(read(0, c, 1)){//cant
		//check for stdin error
		if(c < 0){
			write(2,"stdin error\n",12);//cant
			exit(1);
		}

		//if char in buf was set to something different
		//change out to what it was changed to
		//else keep it the same
		char out;
		if(buf[c[0]] == 0){
			out = c[0];
		}
		else{
			out = buf[c[0]];
		}
		
		//check for output error 
		if(write(1, &out, 1) < 1){
			write(2,"output error\n",13);
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
			write(2,"no duplicate bytes\n",19);
			exit(1);
		}
		buf[f] = to[i];
		i++;
	}

	if(from[i] == 0 && to[i] == 0){
		return;
	}
	else{//to and from were different lengths
		write(2,"inputs cannot be different lengths\n",35);
		exit(1);
	}
}
