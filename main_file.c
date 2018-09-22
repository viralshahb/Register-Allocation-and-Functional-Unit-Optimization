/* Module for Register and FU Allocation and Binding */
/******************* IIT Guwahati *******************/
/************* Developed By ************************/
/******* Viral Shah [174101054], Parth Sorthiya[174101004], Priya Badchariya[174101047] ********/
/******   *********/

/* Header Files */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/* Constants */
#define BUFFER_SIZE 1024			// To store an individual token while reading from a file
#define BUFFER 250				// Max size of variable name
#define MAX_SIZE 1000				// MAx-size for number of variables
#define MODE 2					// To store mode of operation (Read = 0, Write = 1)

/* Structure to read and store instructions as per input file */
struct tok {
	int time;				// Time-stamp
	char var[BUFFER];			// Variable Name
	int len;				// Length of Variable
	int num;				// Unique number assigned to each variable
	char block[BUFFER];			// Block Number
	char port[BUFFER];			// Port Number which is used for reading
	char op[BUFFER];			// Operation which is performed
	char mode[MODE];			// Mode of operation ( Read / Write )
};

/* Structure for writing data back to file */
struct out_tok {
	int time;				// Time-stamp
	char var[BUFFER][BUFFER];		// Variable Name
	int len[BUFFER];			// Length of Variable
	char block[BUFFER];			// Block Number
	char port[BUFFER];			// Port Number which is used for reading
	char mode[BUFFER];			// Mode of operation ( Read / Write )
	char op[BUFFER];			// Operation which is performed
	int num[BUFFER];		

};

/* Structure for reading register specifications from a file */
struct reg {
	char name[BUFFER];			// Name of Register
	int width;				// Width of Register
	bool used;
};

/* Structure for reading functional-unit specifications from a file */
struct Fu {
	char name[BUFFER];			// Name of functional-unit
	char op[BUFFER][BUFFER];		// Operations supported by functional-unit
	int inWidth[MODE];			// input width of two inputs
	int outWidth;				// output width 
	int num;				// // Unique number assigned to each functional-unit
	bool used;

};

struct reg regData[MAX_SIZE];			// Structure to store Register Data
struct tok table[MAX_SIZE]; 			// Structure to store input file data
struct out_tok out_table[MAX_SIZE];		// Structure to write data into an output file
struct Fu fuData[MAX_SIZE];			// Structure to store Functional-Unit Data

int k;						// Number of unique variables
int t;						// Number of lines in a input-text file 

int largest;					// Total timestamp
int input[BUFFER][MAX_SIZE];			// Input matrix ( Timestamp * Variables )
int final[BUFFER][MAX_SIZE];			// Final Matrix ( Timestamp * (No. of Reg + Var) )
char basicBlock[BUFFER][BUFFER];		// Names of all Basic Blocks
int basicBlockcount;				// Total no. of Basic Blocks
int varCount=1;					// Total no. of Variables in a program
int regCount = 0;				// Total no. of Registers available 
int width[MAX_SIZE] = {0};				// Width used to allocate optimize reg to a variable
int temporary[MAX_SIZE] = {0};			// Array to store temporarily width of variables

int all_space(const char *str) {
    while (*str) {
        if (!isspace(*str++)) {
            return 0;
        }
    }
    return 1;
}

int findWidth(int var,int time){
	int maxW = 1;
	//bool flag = 0;
	for(int i = 0; i < k; i++){
		if(var == table[i].num && time==table[i].time){
			if(table[i].len > maxW)
				maxW = table[i].len;
			//flag = 1;
		}
	}
	return maxW;
}


/*  Storing data into structure by tokenizing the line of file  */
void tok_to_table(char** tokens){
    char *rName=(char *)malloc(sizeof(char)*BUFFER);
    char *buff= (char *)malloc(sizeof(char)*BUFFER);
    strcat(rName,"R");
	if(strcmp(tokens[2],"R") == 0 || strcmp(tokens[2],"O") == 0){			// If instruction is read
		strcpy(table[k].block,tokens[0]);	
		strcpy(table[k].var,tokens[4]);
		table[k].time = atoi(tokens[1]);
		strcpy(table[k].port,tokens[3]);
		table[k].len = atoi(tokens[5]);
		sprintf(buff, "%d", regCount);  
		strcpy(regData[regCount].name,rName);
		width[regCount] = 0;
		strcat(regData[regCount].name,buff);
		strcpy(table[k].mode,"R");
		k++;
		regCount++;
 	}
	else if(strcmp(tokens[2],"A") == 0){						// If instruction is assignment
		strcpy(table[k].block,tokens[0]);
	        strcpy(table[k].var,tokens[3]);
		table[k].time = atoi(tokens[1])+1;
		table[k].len = atoi(tokens[4]);
		sprintf(buff, "%d", regCount);  
		strcpy(regData[regCount].name,rName);
		width[regCount] = 0;
		strcat(regData[regCount].name,buff);
		strcpy(table[k].mode,"W");
		k++;
		regCount++;
		
		strcpy(table[k].block,tokens[0]);
		strcpy(table[k].var,tokens[5]);
		table[k].time = atoi(tokens[1]);
		table[k].len = atoi(tokens[6]);
		sprintf(buff, "%d", regCount);  
		strcpy(regData[regCount].name,rName);
		width[regCount] = 0;
		strcat(regData[regCount].name,buff);
		strcpy(table[k].mode,"R");
		k++;
		regCount++;
		
		if(tokens[9] != NULL) {
			strcpy(table[k].block,tokens[0]);
			strcpy(table[k].var,tokens[8]);
			table[k].time = atoi(tokens[1]);
			table[k].len = atoi(tokens[9]);
			sprintf(buff, "%d", regCount);  
			strcpy(regData[regCount].name,rName);
			width[regCount] = 0;
			strcat(regData[regCount].name,buff);
			strcpy(table[k].mode,"R");
			k++;
			regCount++;
		}
		
	}
}

/* This function returns unique number assigned to each variable */
int findNum(char* variable){
	int temp;
	for(int i = 0; i < k; i++){
		if(strcmp(table[i].var,variable) == 0){
			temp = table[i].num;
		}
	}
	return temp;
}

/* This function converts data ready for printing in output file */
void token_to_table(char** tokens){
	if(strcmp(tokens[2],"R") == 0 || strcmp(tokens[2],"O") == 0){	// If instruction is read
		strcpy(out_table[t].block,tokens[0]);
		strcpy(out_table[t].var[0],tokens[4]);
		out_table[t].time = atoi(tokens[1]);
		strcpy(out_table[t].port,tokens[3]);
		out_table[t].len[0] = atoi(tokens[5]);
		strcpy(out_table[t].mode,tokens[2]);
		out_table[t].num[0] = findNum(out_table[t].var[0]);
		t++;
 	}
	else if(strcmp(tokens[2],"A") == 0){				// If instruction is assignment
		strcpy(out_table[t].block,tokens[0]);
	        strcpy(out_table[t].var[0],tokens[3]);
		out_table[t].time = atoi(tokens[1]);
		out_table[t].num[0] = findNum(tokens[3]);
		out_table[t].len[0] = atoi(tokens[4]);
		strcpy(out_table[t].var[1],tokens[5]);
		out_table[t].num[1] = findNum(out_table[t].var[1]);
		out_table[t].len[1] = atoi(tokens[6]);
	
		if(!strcmp(tokens[7],"=") == 0){
			strcpy(out_table[t].var[2],tokens[8]);
			out_table[t].len[2] = atoi(tokens[9]);
			out_table[t].num[2] = findNum(out_table[t].var[2]);
		}
		strcpy(out_table[t].mode,tokens[2]);
		strcpy(out_table[t].op,tokens[7]);
		t++;
	}
}

int f;			// Total number of functional-unit
int addCounter;		// Total number of Adders
int mulCounter;		// Total number of Multipliers
int compCounter;	// Total number of Comparators
int divCounter;     	// Total number of Divisor
int shiftCounter;       // Total number of Shift

/* Storing functional-unit specifications into structure */
void token_to_FU(char** tokens,int argN){
	if(strncmp(tokens[0],"ADDER",5) == 0){			// If FU is ADDER				
		strcpy(fuData[f].name,tokens[0]);
		for(int i=0;i<argN-3;i++)
		strcpy(fuData[f].op[i],tokens[1+i]);
		fuData[f].inWidth[0]=atoi(tokens[argN-1]);
		fuData[f].inWidth[1]=atoi(tokens[argN]);
		fuData[f].outWidth=atoi(tokens[argN-2]);
		fuData[f].num=addCounter;
		addCounter++;
		f++;	
 	}
	else if(strncmp(tokens[0],"MULT",4) == 0){		// If FU is MULTIPLIER		
		strcpy(fuData[f].name,tokens[0]);
		for(int i=0;i<argN-3;i++)
		strcpy(fuData[f].op[i],tokens[1+i]);
		fuData[f].inWidth[0]=atoi(tokens[argN-1]);
		fuData[f].inWidth[1]=atoi(tokens[argN]);
		fuData[f].outWidth=atoi(tokens[argN-2]);
		fuData[f].num=mulCounter;
		mulCounter++;
		f++;
	}
	else if(strncmp(tokens[0],"COMP",4) == 0){		// If FU is COMPARATOR			
		strcpy(fuData[f].name,tokens[0]);
		for(int i=0;i<argN-3;i++)
		strcpy(fuData[f].op[i],tokens[1+i]);
		fuData[f].inWidth[0]=atoi(tokens[argN-1]);
		fuData[f].inWidth[1]=atoi(tokens[argN]);
		fuData[f].outWidth=atoi(tokens[argN-2]);
		fuData[f].num=compCounter;
		compCounter++;
		f++;
	}
	else if(strncmp(tokens[0],"DIVISOR",6) == 0){		// If FU is DIVISOR			
		strcpy(fuData[f].name,tokens[0]);
		for(int i=0;i<argN-3;i++)
		strcpy(fuData[f].op[i],tokens[1+i]);
		fuData[f].inWidth[0]=atoi(tokens[argN-1]);
		fuData[f].inWidth[1]=atoi(tokens[argN]);
		fuData[f].outWidth=atoi(tokens[argN-2]);
		fuData[f].num=divCounter;
		divCounter++;
		f++;
	}
	else if(strncmp(tokens[0],"SHIFT",6) == 0){		// If FU is SHIFT		
		strcpy(fuData[f].name,tokens[0]);
		for(int i=0;i<argN-3;i++)
		strcpy(fuData[f].op[i],tokens[1+i]);
		fuData[f].inWidth[0]=atoi(tokens[argN-1]);
		fuData[f].inWidth[1]=atoi(tokens[argN]);
		fuData[f].outWidth=atoi(tokens[argN-2]);
		fuData[f].num=shiftCounter;
		shiftCounter++;
		f++;
	}
	
}

/* This function calculates which variable will be assigned to which register in an optimized fashion */
void optimizeReg(){
	
	for(int i = 0; i < largest; i++){		// Initializing Reg. Columns to zero
		for(int j = 0; j < regCount; j++){
			final[i][j] = 0;
		}
	}
	for(int i = 0; i < largest; i++){		// Initializing rest of the matrix as per input matrix
		for(int j = regCount; j < varCount+regCount; j++){
			final[i][j-1] = input[i][j-regCount];
		}
	}

	int flag = 0, count = 0;
	int index;
	for(int i = 0; i < largest; i++){		// Doing operation for each timestamp
		flag = 0;
		for(int j = 0; j < varCount + regCount; j++){
			if(final[i][j] == 0){		// Storing first element which we get zero to find out the proper index of register
				if(!flag){
					flag = 1;
					index = j;
				}
			}else{
				if(! (final[i-1][j] == final[i][j]) ){
					if(flag){
						int temp = 1, i1 = i, j1 = j, sum = 0, count = 1;
						while(temp){
							/*  Counting number of timestamp of variable used continuosly */ 
							if(final[i1+1][j1] == final[i1][j1] ){ 
								i1++; 
								count++;
							}else{
								temp = 0;
							}
						}
						for(int y = index; y < j; y++){
							//if(width[y] >= width[j]){	// If Reg has enough width to hold variable 
								int sum = 0, xInd = i;
								for(int t = 0; t < count; t++){
									sum += final[xInd][y] + final[xInd][j];
									xInd++;
								}
								/* If enough space is available to hold all variable with all timestamp */
								if(sum == count*final[i][j]){ 
									int x = i;
									for(int a = 0; a < count; a++){
										final[x][y] = final[x][j];
										final[x][j] = 0;
										x++;
									}
									break;
								}	
							//}
						}
					}
				}
			}
		} 
	}
	for(int i = 0; i < regCount; i++){
		int maxWidth = 0;
		//bool firstOccurence = 0;
		for(int j = 0; j < largest; j++){
			//printf("%d %d\n", final[j][i], findWidth(final[j][i]));
			if(final[j][i] != 0){
				if(maxWidth < findWidth(final[j][i], j+1))
					maxWidth = findWidth(final[j][i], j+1);
			}
		}
		if(width[i] < maxWidth){
			width[i] = maxWidth;
			regData[i].width = width[i];
		}
		//printf("Width[i] %d\n", width[i]);
		//printf("%d  Max %d\n", i, maxWidth);
	}
}



/* This function finds number of variables in a program and highest timestamp value */
void assignNumToVar(){
	int i,j;
	for(i = 0; i < k; i++){		
		for (j = 0; j < i; j++){
			if(strcmp(table[i].var,table[j].var) == 0 /*&& ( table[i].len == table[j].len )*/){
				table[i].num= table[j].num;
				break;
			}
		}
		if (i == j){		
			table[i].num=varCount;		// Assigning unique number to each variable for its identification
			varCount++;
			temporary[varCount-2] = table[i].len;
		}
	}
	largest = table[0].time;
	for (i = 1; i < k; i++){
		if (largest < table[i].time)
			largest = table[i].time;
	}
}

/* This function finds total number of basic blocks in our program */
void findBasicBlock(){
	int i,j;
	for(i = 0; i < k; i++){
		for (j = 0; j < i; j++){
			if (strcmp(table[i].block,table[j].block) == 0){
				break;
			}
		}
		if (i == j){
			strcpy(basicBlock[basicBlockcount],table[i].block);
			basicBlockcount++;   		// Counting number of basic blocks in a program  
		}
	}
}

/* This function returns an index of basic blocks */
int findBasicBlockIndex(char *block){
	for(int i=0;i<basicBlockcount;i++){
		if(strcmp(basicBlock[i],block)==0)
		return i;
	}
}

/* This function creates a matrix of Timestamp * Variables */
void matrix(char *Block){
	int temp=0;
	bool flag=0;
	int ind = 0;
	//printf("\n\nBasic Block:%s",Block);
	for(int i = 0; i < k; i++){
		if(strcmp(table[i].block,Block)==0){
			
			flag=0;
			for(int j = table[i].time-2; j >= 0; j--){
				if(input[j][table[i].num] == table[i].num){
					ind = j;
					flag = 1;
				}
			}
			/* If variable is used for reading in succeding timestamp, then we need to store that variable upto that timestamp*/
			if(flag && strcmp(table[i].mode, "R")==0){		
				for(int j = table[i].time-1; j >= ind; j--){
				    //printf("jjjj:%d",j);			  
					input[j][table[i].num]=table[i].num;		
				}
			}
			input[table[i].time-1][table[i].num] = table[i].num;
		}
	}
}


/*  This function reads the file and creates token*/
void readFileInput(){
	char **tokens ;
	k = 0;
	const char delimiter_characters[] = ",() ";
	const char *filename = "proj3_output.txt";
	FILE *input_file = fopen( filename, "r" );
	const char *filename1 = "proj4_output.txt";
	FILE *output_file = fopen( filename1, "w" );
	fputs("<Specify RT Operations>\n", output_file);
	char buffer[ BUFFER_SIZE ];
	char *last_token;
	int  i = 0;
	tokens = malloc(BUFFER_SIZE * sizeof(char*));
	if( input_file == NULL ){
		fprintf( stderr, "Unable to open file %s\n", filename );
	}else{
		while(fgets(buffer, BUFFER_SIZE, input_file) != NULL ){		// Read each line into the buffer
			if(all_space(buffer))
				continue;
			i=0;
			last_token = strtok( buffer, delimiter_characters );	// Gets each token as a string
			while( last_token != NULL ){
				tokens[i++] = last_token;
				last_token = strtok( NULL, delimiter_characters );
			}
			
			tok_to_table(tokens);		// Reading input from text and storiing it into structure
		}
		if( ferror(input_file) ){
			perror( "The error occurred" );
		}
		fclose( input_file );
		fclose(output_file);
	}
}


void readFile(){
	char **tokens ;
	t = 0;
	const char delimiter_characters[] = " ,()";
	const char *filename2 = "proj3_output.txt";
	FILE *input_file2 = fopen( filename2, "r" );
	
	char buffer[ BUFFER_SIZE ];
	char *last_token;
	int  i = 0;
	tokens = malloc(BUFFER_SIZE * sizeof(char*));
	if( input_file2 == NULL ){
		fprintf( stderr, "Unable to open file %s\n", filename2 );
	}else{
		while(fgets(buffer, BUFFER_SIZE, input_file2) != NULL ){		// Read each line into the buffer
			if(all_space(buffer))
				continue;
			i=0;
			last_token = strtok( buffer, delimiter_characters );		// Gets each token as a string
			while( last_token != NULL ){
				tokens[i++] = last_token;
				last_token = strtok( NULL, delimiter_characters );
			}
			token_to_table(tokens);		// Reading input from text and making appopriate ouput structure
		}
		if( ferror(input_file2) ){
			perror( "The error occurred" );
		}
		fclose( input_file2 );
	}
}

/* This function reads register and its width from the file */
void readFU(){
	char **tokens ;
	f = 0;
	const char delimiter_characters[] = ",() ";
	const char *filename2 = "resource.txt";
	const char *filename1 = "proj4_output.txt";
	
	FILE *input_file2 = fopen( filename2, "r" );
	//FILE *output_file = fopen( filename1, "a" );	
	
	char buffer[ BUFFER_SIZE ];
	char *last_token;
	int  i = 0;
	tokens = malloc(BUFFER_SIZE * sizeof(char*));
	if( input_file2 == NULL ){
		fprintf( stderr, "Unable to open file %s\n", filename2 );
	}else{
		// Read each line into the buffer
		while(fgets(buffer, BUFFER_SIZE, input_file2) != NULL ){
			if(all_space(buffer))
				continue;
			//fputs(buffer,output_file);
			i=0;
			// Gets each token as a string and prints it
			last_token = strtok( buffer, delimiter_characters );
			while( last_token != NULL ){
				tokens[i++] = last_token;
				last_token = strtok( NULL, delimiter_characters );
			}
			//printf("Argv = %d",i);
			token_to_FU(tokens,i-2);		//pass number of arguments
		}
		if( ferror(input_file2) ){
			perror( "The error occurred" );
		}
		fclose( input_file2 );
		//fclose( output_file );
	}

}

void readfun(char* tempName, FILE *ptr){
	char **tokens ;
	f = 0;
	const char delimiter_characters[] = ",() ";
	const char *filename2 = "resource.txt";
	const char *filename1 = "proj4_output.txt";
	
	FILE *input_file2 = fopen( filename2, "r" );
	FILE *output_file = ptr;	
	
	char buffer[ BUFFER_SIZE ];
	char buffer1[BUFFER_SIZE];
	char *last_token;
	int  i = 0;
	tokens = malloc(BUFFER_SIZE * sizeof(char*));
	if( input_file2 == NULL ){
		fprintf( stderr, "Unable to open file %s\n", filename2 );
	}else{
		// Read each line into the buffer
		while(fgets(buffer, BUFFER_SIZE, input_file2) != NULL ){
			if(all_space(buffer))
				continue;
			i=0;
			strcpy(buffer1, buffer);
			// Gets each token as a string and prints it
			last_token = strtok( buffer, delimiter_characters );
			while( last_token != NULL ){
				tokens[i++] = last_token;
				last_token = strtok( NULL, delimiter_characters );
			}
			if(strcmp(tokens[0], tempName) == 0)
				fputs(buffer1,output_file);
			
		}
		if( ferror(input_file2) ){
			perror( "The error occurred" );
		}
		fclose( input_file2 );
		//fclose( output_file );
	}

}

/* This function returns register name allocated to particular variable */
char* regFind(int n,int m){
	char *str;
	for(int i=0;i<regCount+varCount-1;i++){
		if(final[n-1][i]==m){	// Checking which register has been allocated to varibale whose unique num is m in timestamp n-1
			regData[i].used = true;
			return regData[i].name;
		}
	}
}

/* This function returns line number on which particular functional-unit is stored */
int findFu(char *str,int k){
	for(int i = 0; i < f; i++){
		if(strncmp(str,fuData[i].name,4) == 0 && fuData[i].num == k){	
			return i;
		}
	}
}

int addArray[BUFFER][BUFFER][BUFFER];   //To store all the ADDERs
int mulArray[BUFFER][BUFFER][BUFFER];   //To store all the MULTs
int compArray[BUFFER][BUFFER][BUFFER];  //To store all the COMPARATORs
int divArray[BUFFER][BUFFER][BUFFER];   //To store all the DIVISORs
int shiftArray[BUFFER][BUFFER][BUFFER]; //To store all the SHIFTs

/* This function allocates optimal functional-unit to the instruction of all the available functional-units */
char *optimizeFu(char *block,char *op,int time,int q){

	char *msg=(char *)malloc(sizeof(char)*BUFFER);	// Functional-unit and operation carried out by functional-unit
	bool flag = 0;					// Flag will be set if there is one fu available which will meet our requirements
	int funIndex;					// Index of functional unit which is allocated as of now
	int finf;					// Index of functional unit which is allocated finally

	if(strcmp(op,"+") == 0){			// If '+' is encountered
		bool tempFlag = 0;	
		if(addCounter == 0){
			printf("No FU for '+' is available\n");
			exit(0);
		}
		for(int i = 0; i < addCounter; i++){
			int findf = findFu("ADDER",i);
			int findb = findBasicBlockIndex(block);
			
			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(addArray[findb][time][i] == 0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
			    fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){	
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("ADDER",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("ADDER",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("ADDER",funIndex)].outWidth){
						addArray[findb][time][i]=1;	
						addArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}	
		}
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"ADD");
			return msg;
		}  	
	}
	else if(strcmp(op,"-")==0){		// If '-' is encountered
		bool tempFlag = 0;
		if(addCounter == 0){
			printf("No FU for '-' is available\n");
			exit(0);
		}
		for(int i = 0; i < addCounter; i++){
			int findf=findFu("ADDER",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(addArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){
	
				tempFlag = 1;
				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("ADDER",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("ADDER",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("ADDER",funIndex)].outWidth){
						addArray[findb][time][i]=1;	
						addArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
	
			}
		}
		if(tempFlag){
			/* msg has (FU-Name,Operation) */
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"SUB");
			return msg;
		}  	
	}
	else if(strcmp(op,"*")==0){		// If '*' is encountered
		bool tempFlag  = 0;
		if(mulCounter == 0){
			printf("No FU for '*' is available\n");
			exit(0);
		}
		for(int i = 0; i < mulCounter; i++){
			int findf=findFu("MULT",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(mulArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("MULT",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("MULT",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("MULT",funIndex)].outWidth){
						mulArray[findb][time][i]=1;	
						mulArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
			     
		}   
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"MULT");
			return msg;
		}  
	}
	else if(strcmp(op,"/")==0){		// If '/' is encountered
		bool tempFlag = 0;
		if(divCounter == 0){
			printf("No FU for '/' is available\n");
			exit(0);
		}
		for(int i = 0; i < divCounter; i++){
			int findf=findFu("DIVISOR",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(divArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("DIVISOR",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("DIVISOR",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("DIVISOR",funIndex)].outWidth){
						divArray[findb][time][i]=1;	
						divArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
			     
		}   
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"DIV");
			return msg;
		}  
	}
	else if(strcmp(op,"<")==0){		// If '<' is encountered
		bool tempFlag = 0;
		if(compCounter == 0){
			printf("No FU for '<' is available\n");
			exit(0);
		}
		for(int i=0;i<compCounter;i++){
			int findf=findFu("COMP",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(compArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("COMP",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("COMP",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("COMP",funIndex)].outWidth){
						compArray[findb][time][i]=1;	
						compArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}     
		}  
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"LT");
			return msg;
		}  
	}
	else if(strcmp(op,">")==0){		// If '>' is encountered
		bool tempFlag = 0;
		if(compCounter == 0){
			printf("No FU for '>' is available\n");
			exit(0);
		}
		for(int i=0;i<compCounter;i++){
			int findf=findFu("COMP",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(compArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){ 

				tempFlag = 1;
				
				// If any other FU had been allocatde previously
				if(flag){				
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("COMP",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("COMP",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("COMP",funIndex)].outWidth){
						compArray[findb][time][i]=1;	
						compArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}   
		}   
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"GT");
			return msg;
		}  
	}
	else if(strcmp(op,">=")==0){		// If '>=' is encountered
		bool tempFlag = 0;
		if(compCounter == 0){
			printf("No FU for '>=' is available\n");
			exit(0);
		}
		for(int i=0;i<compCounter;i++){
			int findf=findFu("COMP",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(compArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("COMP",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("COMP",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("COMP",funIndex)].outWidth){
						compArray[findb][time][i]=1;	
						compArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
			     
		}  
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"GTE");
			return msg;
		}  
	}
	else if(strcmp(op,"<=")==0){		// If '<=' is encountered
		bool tempFlag = 0;
		if(compCounter == 0){
			printf("No FU for '<=' is available\n");
			exit(0);
		}
		for(int i=0;i<compCounter;i++){
			int findf=findFu("COMP",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(compArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("COMP",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("COMP",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("COMP",funIndex)].outWidth){
						compArray[findb][time][i]=1;	
						compArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
		}
		if(tempFlag){
			/* msg has (FU-Name,Operation) */
			fuData[finf].used = 1; 
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"LTE");
			return msg;
		}  
	}
	else if(strcmp(op,"==")==0){		// If '==' is encountered
		bool tempFlag = 0;
		if(compCounter == 0){
			printf("No FU for '==' is available\n");
			exit(0);
		}
		for(int i=0;i<compCounter;i++){
			int findf=findFu("COMP",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(compArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("COMP",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("COMP",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("COMP",funIndex)].outWidth){
						compArray[findb][time][i]=1;	
						compArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}     
		}  
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"E");
			return msg;
		}  
	}
	
	else if(strcmp(op,"<<")==0){		// If '<<' is encountered
		bool tempFlag = 0;
		if(shiftCounter == 0){
			printf("No FU for '<<' is available\n");
			exit(0);
		}
		for(int i=0;i<shiftCounter;i++){
			int findf=findFu("SHIFT",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(shiftArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){

				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("SHIFT",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("SHIFT",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("SHIFT",funIndex)].outWidth){
						shiftArray[findb][time][i]=1;	
						shiftArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
		}
		if(tempFlag){
			/* msg has (FU-Name,Operation) */ 
			fuData[finf].used = 1;
			strcpy(msg,fuData[finf].name);
			strcat(msg,",");
			strcat(msg,"LS");
			return msg;
		}  
	}
	
	else if(strcmp(op,">>")==0){		// If '>>' is encountered
		bool tempFlag = 0;
		if(shiftCounter == 0){
			printf("No FU for '>>' is available\n");
			exit(0);
		}
		for(int i=0;i<shiftCounter;i++){
			int findf=findFu("SHIFT",i);
			int findb=findBasicBlockIndex(block);

			/*  if we are getting a functional unit which can be allocated to the instruction */ 
			if(shiftArray[findb][time][i]==0 && fuData[findf].inWidth[0] >= out_table[q].len[1] &&
				fuData[findf].inWidth[1] >= out_table[q].len[2] && fuData[findf].outWidth >= out_table[q].len[0]){
			
				tempFlag = 1;

				// If any other FU had been allocatde previously
				if(flag){
					/* If we are getting optimal FU than previous one */
					if(fuData[findf].inWidth[0] <= fuData[findFu("SHIFT",funIndex)].inWidth[0] &&
					fuData[findf].inWidth[1] <= fuData[findFu("SHIFT",funIndex)].inWidth[1] &&
					fuData[findf].outWidth <= fuData[findFu("SHIFT",funIndex)].outWidth){
						shiftArray[findb][time][i]=1;	
						shiftArray[findb][time][funIndex]=0;	
					}
				}
				flag = 1;
				funIndex = i;
				finf=findf;
			}
		}
		/* msg has (FU-Name,Operation) */
		fuData[finf].used = 1;   
		strcpy(msg,fuData[finf].name);
		strcat(msg,",");
		strcat(msg,"RS");
		return msg;
	}
	
	printf("Error:No such FUs are available ");
	return "";
} 



/* This function prints data in output file */
void generateOutput(char *Block){

	char str[BUFFER];
	const char *filename1 = "proj4_output.txt";
	FILE *output_file = fopen( filename1, "a" );

	for(int i = 0; i < t; i++){
		if(strcmp(out_table[i].block,Block)==0){
			if(strcmp(out_table[i].mode,"R")==0 || strcmp(out_table[i].mode,"O")==0) {	// If mode is Read or Write	
				fputs(out_table[i].block,output_file);
				sprintf(str, ", %d, ", out_table[i].time);
				fputs(str,output_file);

				fputs(out_table[i].mode,output_file);
				fputs(", (",output_file);
				fputs(out_table[i].port,output_file);
				fputs(", ",output_file);

				fputs(regFind(out_table[i].time,out_table[i].num[0]),output_file);
				sprintf(str, ", %d)", out_table[i].len[0]);
				fputs(str,output_file);	
				fputs("\n",output_file);
			}
			else if(strcmp(out_table[i].mode,"A")==0){					// If mode is Assignment
				if(!strcmp(out_table[i].op,"=")==0){
				fputs(out_table[i].block,output_file);
				sprintf(str, ", %d, ", out_table[i].time);
				fputs(str,output_file);

				fputs(out_table[i].mode,output_file);
				fputs(", (",output_file);

				fputs(regFind(out_table[i].time+1,out_table[i].num[0]),output_file);
				sprintf(str, ", %d), (", out_table[i].len[0]);
				fputs(str,output_file);

				fputs(regFind(out_table[i].time,out_table[i].num[1]),output_file);
				sprintf(str, ", %d), ", out_table[i].len[1]);
				fputs(str,output_file);
					
				char *opt;
				opt=optimizeFu(out_table[i].block,out_table[i].op,out_table[i].time,i);
				
				fputs("(",output_file);
				fputs(opt,output_file);
				fputs(")",output_file);
				
				
				fputs(", (",output_file);
				fputs(regFind(out_table[i].time,out_table[i].num[2]),output_file);
				sprintf(str, ", %d)", out_table[i].len[2]);
				fputs(str,output_file);
								
				fputs("\n",output_file);
				}
				else{
		if(strcmp(regFind(out_table[i].time,out_table[i].num[1]), regFind(out_table[i].time+1,out_table[i].num[0])) != 0){
						fputs(out_table[i].block,output_file);
						sprintf(str, ", %d, ", out_table[i].time);
						fputs(str,output_file);

						fputs("R",output_file);
						fputs(", (",output_file);
						fputs(regFind(out_table[i].time,out_table[i].num[1]),output_file);
						fputs(", ",output_file);

						fputs(regFind(out_table[i].time+1,out_table[i].num[0]),output_file);
						sprintf(str, ", %d)", out_table[i].len[0]);
						fputs(str,output_file);
								
						fputs("\n",output_file);
					}
				}		
				
			}
		}
	}
	fclose(output_file);
}

int main( int argc, char** argv ){
	readFileInput();	// Function for reading input from a file
	assignNumToVar();	// Assigning unique number to each variable for its identification
	readFile();		// Function for making data ready for output file
	readFU();		// Function for reading FU Specifications from a file
	findBasicBlock();	// Function for finding total number of Basic-Blocks in a file
	
		
	//printf("   TimeStamp\t  Variable\tWidthLength\tCount\tBasicBlock");
	
	for(int j = 0; j < k; j++){
		//printf("\n\t%d\t\t%s\t\t%d\t%d\t%s", table[j].time,table[j].var,table[j].len,table[j].num,table[j].block);
	}
	
	for(int q = 0; q < basicBlockcount; q++){	// Performing operation for each basic block
		for(int i = 0; i < BUFFER; i++){
			for(int j=1;j<MAX_SIZE;j++)
				input[i][j]=0;
		}
		matrix(basicBlock[q]);			// Creating a input matrix for Basic-Block[q] for optimize Reg
	
		
		//printf("\n\n%s:\n",basicBlock[q]);
		//printf("Input");
		for(int i = 0; i < largest; i++){
			//printf("\n");
			for(int j = 1; j < varCount; j++){
				//printf("%d  ",input[i][j]);
			}
		}
		
	
		optimizeReg();				// Allocating optimal Register to each variable
		/*for(int h = 0; h < regCount; h++){
			regData[h].width = width[h];
		
		}*/
		//printf("\n\n\n\n\n\n");
		//printf("Final\n");
		for(int i = 0; i < largest; i++){
			for(int j = 0; j < varCount+regCount; j++){
				//printf("%d ", final[i][j]);
		}//printf("\n");
		}
		
	 	generateOutput(basicBlock[q]);		// Generating Output File
	}
	const char *filename1 = "proj4_output.txt";
	FILE *output_file = fopen( filename1, "a" );
	fputs("<Specify Registers>\n", output_file);
	for(int i = 0; i < regCount; i++){
		if(regData[i].used){
			char str[100];
			fputs(regData[i].name, output_file);
			fputs(",", output_file);
			sprintf(str,  "%d\n", regData[i].width);
			fputs(str, output_file);
		}
	}
	
	fputs("<Specify FUs>\n", output_file);
	for(int i = 0; i < MAX_SIZE; i++){
		if(fuData[i].used){
			readfun(fuData[i].name, output_file);

		}
	}
	fputs("\n",output_file);
	fclose(output_file);
	
	return 0;
	
	

}