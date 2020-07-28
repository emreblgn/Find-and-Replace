#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAX 50

// Creating a struck for bad match table.
typedef struct badMatch{
	
	char letter;
	int index;
}badMatch;

void boyerMooreHorspool(char*,int, char*, char*, int);	// Boyer Moore Horspool function, parameters: array of string, size of the file, string to be changed, new string, case sensitive or not.
void createBadMatchTable(badMatch*, char*, int, int);	// Bad match table function, parameters:  bad match table structure, new string, new string length, case sensitive or not.
int tablePos(badMatch*, char, int);						// Return index of selected char in bad match table, parameters: bad match table structure, selected char, size of bad match table.
int indexOfCharInTable(badMatch*, char, int);			// Return index of selected char in bad match table depends on case sensitive variable.

static int changeCount = 0;								// Variable that holds number of changes.

int main(){
	
	FILE *fp;
	int size;
	char *strArray;
	char file1[MAX];
	int cs;
	char temp;
	
	printf("Write file name with extension: ");
	scanf("%s", file1);

	char strOld[MAX];
	printf("Write the string to be changed: ");
	scanf("%c",&temp);
	scanf("%[^\n]",strOld);
	
	char strNew[MAX];
	printf("Write the new string that replaced to %s: ", strOld);
	scanf("%c",&temp);
	scanf("%[^\n]",strNew);
	
	printf("Select case sensitive or not: \n 0 - Not case sensitive \n 1 - Case sensitive \n ");
	scanf("%d", &cs);
	
	if((fp = fopen(file1, "r")) == NULL){
		printf("%s not found.", file1);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);					// Get size of the file
	fseek(fp, 0, SEEK_SET);
	strArray = (char*) malloc(sizeof(char) * size);
	
	int i = 0;
    char tempC;
    
    for(i=0; i<size; i++){
    	tempC = fgetc(fp);
    	strArray[i] = tempC;
	}

    fclose(fp);
	
	struct timeval start, stop;			// Create a timeval structure to calculate execution time.
	double secs = 0;
	gettimeofday(&start, NULL);
	
	boyerMooreHorspool(strArray, size, strOld, strNew, cs);
	
	gettimeofday(&stop, NULL);
	secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

	if((fp = fopen(file1, "w")) == NULL){
		printf("%s not found.", file1);
		return -1;
	}
	
	i = 0;
	while(strArray[i] != '\0'){
		tempC = strArray[i];
		fputc(tempC, fp);
		i++;
	}
	
	fclose(fp);
	
	printf("New string replaced to file: %s, number of strings changed: %d, Execution time: %f seconds\n", file1, changeCount, secs);

	return 0;
}

void boyerMooreHorspool(char* strArray, int strFileLen, char* strOld, char* strNew, int cs){
	
	
	int strOldLen = 0;
	int strNewLen = 0;
	int i = 0;
	
	while(strOld[i] != '\0'){
		strOldLen++;
		i++;
	}
	i = 0;
	while(strNew[i] != '\0'){
		strNewLen++;
		i++;
	}
	
	struct badMatch badMatchTable[MAX];
	createBadMatchTable(badMatchTable, strOld, strOldLen, cs);
	
	i = strOldLen - 1;		
	int push;
	printf("Looking for word: %s and replace word: %s \n", strOld, strNew);
	
	while(i < strFileLen){
		
		push = badMatchTable[indexOfCharInTable(badMatchTable, strArray[i], cs)].index;		// variable that holds what to add index variable if the word did not found this loop.
		int j = strOldLen - 1;																// variable that holds old words control index from end to beginning.
		int iold = i;																		// variable that holds old i value for changing i after word equality control.
		
		if(cs == 1){
			
			// If case sensitive is on program will just controls character equality and j value for if j value arrived the beginning of the word or not.
			while(strArray[i] == strOld[j] && j>= 0){
				i--;
				j--;
			}
		}else{   
			
			// If case sensitive is off program will control every possible equality situation.
			while(j >= 0 && ( (strArray[i] < 'A' || strArray[i] > 'Z' && strArray[i] < 'a' || strArray[i] >'z') && (strArray[i] == strOld[j]) || (strArray[i] >= 'a' && strArray[i] <= 'z') && (strArray[i]  == strOld[j] || strArray[i] - 32 == strOld[j]) || (strArray[i] >= 'A' && strArray[i] <= 'Z') && (strArray[i]  == strOld[j] || strArray[i] + 32 == strOld[j]))){
				i--;
				j--;
			}
		}
		
		if(j<0){								// j<0 means the word is found.
			
			changeCount++;								
			int foundIndex = i + 1;				// variable that holds start index of the word to be changed.
			
			if(strOldLen >= strNewLen){
				
				// If old word longer than new word then rest of the string will be shifted left by old word length - new word length and new word will be replace on old word.
				int k = foundIndex;
				int l = 0;
				while(k < foundIndex + strNewLen){
					strArray[k] = strNew[l];
					k++;
					l++;
				}
				
				k = foundIndex + strOldLen;
				l = foundIndex + strNewLen;
				
				while(k<strFileLen){
					strArray[l] = strArray[k];
					
					l++;
					k++;
				}
				
				strFileLen = strFileLen - (strOldLen - strNewLen); 
				strArray = (char*) realloc(strArray, sizeof(char) * strFileLen);
				strArray[strFileLen] = '\0';
				i = iold + strNewLen;
				
			}else{
				
				// If old word shorter than new word then rest of the string will be shifted right by new word length - old word length and new word will be replace on old word.
				int k = strFileLen - 1;
				strFileLen = strFileLen + (strNewLen - strOldLen);
				strArray = (char*) realloc(strArray, sizeof(char) * strFileLen);
				strArray[strFileLen] = '\0';
				int l = strFileLen - 1;
				
				while(k >=  foundIndex + strOldLen){
					strArray[l] = strArray[k];
					k--;
					l--;
				}
				
				k = foundIndex;
				l = 0;
				
				while(k < foundIndex + strNewLen){
					strArray[k] = strNew[l];
					k++;
					l++;
				}
				i = iold + strNewLen;   
			}
		}
		else{								// j>=0 means the word is not foud.
			i = iold + push;
		}
	}
}

void createBadMatchTable(badMatch* badMatchTable, char* strNew, int strNewLen, int cs){
	
	int i;
	int j = 0;	
	
	if (cs == 1){
		
		// If case sensitive is on every character has its own place in bad match table.
		for(i = 0; i<strNewLen; i++){
			if(i == strNewLen - 1){
				
				// If it is the last index then its bad match table value will be set as length of the word.
				if(tablePos(badMatchTable, strNew[i], j) == -1){
					badMatchTable[j].index = strNewLen;
					badMatchTable[j].letter = strNew[i];
					j++;
				}
			}else{
				// If it is not the last index then check if it was added before or not. If it was added before update the value else create new value.
				if(tablePos(badMatchTable, strNew[i], j) == -1){
					badMatchTable[j].index = strNewLen - i - 1;
					badMatchTable[j].letter = strNew[i];
					j++;
				}
				else{
					badMatchTable[tablePos(badMatchTable, strNew[i], j)].index = strNewLen - i - 1;
				}
			}
		}
	}else{
		
		// If case senditive is off every upper letters will be saved as lover letters.
		for(i = 0; i < strNewLen; i++){
			if(strNew[i] >= 'A' && strNew[i] <= 'Z'){
					if(i == strNewLen - 1){
						badMatchTable[j].index = strNewLen;
						badMatchTable[j].letter = strNew[i] + 32;
						j++;
					}else{
						if(tablePos(badMatchTable, strNew[i] + 32, j) == -1){
							badMatchTable[j].index = strNewLen - i - 1;
							badMatchTable[j].letter = strNew[i] + 32;
							j++;
						}else{
							badMatchTable[tablePos(badMatchTable, strNew[i] + 32, j)].index = strNewLen - i - 1;
						}
					}
				}else{
					if(i == strNewLen - 1){
						badMatchTable[j].index = strNewLen;
						badMatchTable[j].letter = strNew[i];
						j++;
					}else{
						if(tablePos(badMatchTable, strNew[i], j) == -1){
							badMatchTable[j].index = strNewLen - i - 1;
							badMatchTable[j].letter = strNew[i];
							j++;
						}else{
							badMatchTable[tablePos(badMatchTable, strNew[i], j)].index = strNewLen - i - 1;
						}
					}
				}	
			}
		}
		
	// The last element of the bad match table will be set as length of the word for non-added characters.
	badMatchTable[j].index = strNewLen;
	badMatchTable[j].letter = '\0';
}

int tablePos(badMatch* badMatchTable, char c, int size){
	
	// Check if bad match table contains letter c, if so return its index otherwise return -1.
	int i = 0;
	while(i<size){
		if(c == badMatchTable[i].letter){
			return i;
		}
		i++;
	}
	return -1;
}

int indexOfCharInTable(badMatch* badMatchTable, char c, int casesensitive){
	
	// Return character c's bad match table value depends on case sensitive option.
	int i = 0;
	if(casesensitive == 1){
		while(badMatchTable[i].letter != '\0'){
		
		if(badMatchTable[i].letter == c){
			return i;
		}
		i++;
		}
	}else{
		while(badMatchTable[i].letter != '\0'){
			if(c >= 'A' && c <= 'Z'){
				if(badMatchTable[i].letter == c || badMatchTable[i].letter == c + 32){
					return i;
				}
			}else if(c >= 'a' && c <= 'z'){
				if(badMatchTable[i].letter == c || badMatchTable[i].letter == c - 32){
					return i;
				}
			}
			i++;
		}
	}
	return i;	
}

