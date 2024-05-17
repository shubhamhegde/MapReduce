#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// compile with gcc reader.c -o reader.o -lm
// (note the need to link the math library with the -lm command)

#define NUMFILES 1          // number of text files reader will attempt to open
#define WORDBOXCOUNT 5      // number of words the wordbox can hold
#define MAXWORDCHARLEN 31   // one greater than the maximum number of characters that can be stored in a wordbox word element before truncation (one element needed for null terminator)
#define DEBUG 0             // switches some additional functionality for debug on and off

FILE **fpArray;             // declaring array of file pointer handles that will be initialized to point to the text files we will open
static const char INPUTPATH[] = "../inputs/"; // file path stem from src directory to directory with text files

typedef struct wordbox {
    char word[WORDBOXCOUNT][MAXWORDCHARLEN];
} wordbox;


typedef struct rdrelement {
  wordbox key;
  struct rdrelement* nxt;
} rdrelement;

// FIXME - this is already in map_red_q.h so if you include it you don't need to do that again here
typedef struct Queue {
  rdrelement* head;
  rdrelement* tail;
  int len;
} Queue;


void init(){
     fpArray = malloc(sizeof(FILE)*NUMFILES); // create array of file pointer handles
     return;
}

// inits queue. Identical function to that in map_red_q.h
Queue* InitReaderQ() {
  Queue* q = (Queue*) malloc(1*sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  q->len = 0;
  return q;
}

int Add2ReaderQ(wordbox wordList, Queue* q) {
  rdrelement* e = (rdrelement *)malloc(sizeof(rdrelement));
  if(e == NULL) {
    return 0;
  }
  e->key = wordList;
  e->nxt = NULL;
  if( q->len == 0 )
    {
      q->tail = e;
      q->head = e;
    }
  else
    {
      q->tail->nxt = e;
      q->tail = e;
    }
  q->len +=1;
  return 1;
}

// initialize q
void PrintReaderQ(Queue* q) {
    if(q->len == 0){
        printf("Queue Empty!\n");
    } else {
        rdrelement* p = q->head;
        while(p != NULL){ // iterate through entire q
            for(int ct=0; ct<WORDBOXCOUNT; ct++){
                printf("wordList.word[%d]=%s ", ct, p->key.word[ct]);
            }
            printf("\n\n");            
            p = p->nxt;
        }
    printf("\n");
    }
} 

// concatenates two strings
char* concat(const char* str1, const char* str2){
    char *concatenated = malloc(sizeof(str1)+sizeof(str2)+1);
    strcpy(concatenated, str1);
    strcat(concatenated, str2);
    return(concatenated);
}

// converts and integer into a string
char* int2Str(int x){
    // calculate the size needed for the string and malloc space for it
    int strSize;
    if(x==0){
        strSize = 1;
    } else {
        strSize = (int)((ceil(log10(x))+1)*sizeof(char));
    }
    //printf("size of string = %d\n", strSize);
    char *str = malloc(strSize); // credit https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
    sprintf(str, "%d", x);
    return(str);
}

// initialize file pointer array
void initFpArray(){
   
    for(int ct=0; ct < NUMFILES; ct++){
         // create path to file name
        char *fileStub = concat(INPUTPATH, int2Str(ct+1));
        char *filePath = concat(fileStub, ".txt");
        //printf("filePath = %s\n", filePath);
        // populate this file handle
        fpArray[ct] = fopen(filePath, "r");
        if (fpArray[ct] == NULL){
            printf("Error: couldn't open file at path %s\n", filePath);
            printf("Terminating...\n");
            exit(0); // terminate program
        }
    }
    return;
}   

void printFile(){
    // declarations
    char charachter; 

    for(int ct=0; ct<NUMFILES; ct++){ // keep running through all the files until you're done
        // run through file character by character
        while((charachter = getc(fpArray[ct])) != EOF){ // Credit: https://www.freecodecamp.org/news/file-handling-in-c-how-to-open-close-and-write-to-files/
        printf("%c",charachter);
        }
        // close file handle
        fclose(fpArray[ct]);
    }
    
    return;
}

// copies a word into a position in the wordlist, truncating if too long
void insertWord(wordbox *wordList, int position, char* str){
    if(DEBUG && strlen(str)>MAXWORDCHARLEN-1){ // DEBUG print warning when we will truncate a word
        printf("Warning: word %s will be truncated to %d characters\n", str, MAXWORDCHARLEN-1);
    }
    memset((*wordList).word[position], '\0', sizeof((*wordList).word[position])); // wipe the space for the word
    strncpy((*wordList).word[position],str,MAXWORDCHARLEN-1); // copy characters in up until size of word
    return;
}

void wipeString(char *string, int length){
    memset(string, '\0', length*sizeof(char)); // wipe the length of the string
    return;
}

void test(){
    wordbox wordList1;
    //strcpy(wordList1.word[0],"hello");
    insertWord(&wordList1, 0, "hello");
    insertWord(&wordList1, 1, "test");
    insertWord(&wordList1, 2, "supercalifragilistacaspialidocious");
    insertWord(&wordList1, 3, "hey");
    insertWord(&wordList1, 4, "belowme");
    printf("word 1 %s 2 %s 3 %s 4 %s 5 %s\n", wordList1.word[0],wordList1.word[1],wordList1.word[2],wordList1.word[3],wordList1.word[4]);

    Queue* q = InitReaderQ();
    PrintReaderQ(q);
    Add2ReaderQ(wordList1, q);
    PrintReaderQ(q);
}

void book2Wordbox2Queue(Queue *q){ // will loop through books, add words to wordbox and add wordboxes to queue
    // declarations
    char character; 
    char word[MAXWORDCHARLEN];
    wipeString(word, MAXWORDCHARLEN);
    int wordCharCounter = 0;
    wordbox wordList1;
    int wordBoxWordCounter = 0; // number of words currently in wordbox, used to insert new words in correct position
  
    for(int ct=0; ct<NUMFILES; ct++){ // keep running through all the files until you're done
        // run through file character by character
        while((character = getc(fpArray[ct])) != EOF){ // Credit: https://www.freecodecamp.org/news/file-handling-in-c-how-to-open-close-and-write-to-files/
            // create a string
            if(character == ' ' || character == '\n' || character == '\r'){ // create new word at spaces, newlines or carriage returns
                // WE FOUND A WORD
                if(wordCharCounter > 0){ // ignore empty new lines or multiple spaces
                    //printf("word = %s\n", word);
                    // ADD TO WORDBOX AND OR QUEUE
                    if(wordBoxWordCounter < WORDBOXCOUNT){ // put word in wordbox up to max wordbox size
                        insertWord(&wordList1, wordBoxWordCounter, word);
                        wordBoxWordCounter++;
                    } else {
                        Add2ReaderQ(wordList1, q); // add wordbox to queue
                        wordBoxWordCounter = 0; // reset wordbox 
                        insertWord(&wordList1, wordBoxWordCounter, word); // add current word to the new wordbox
                        wordBoxWordCounter++;
                    }
                    

                    wipeString(word, MAXWORDCHARLEN);
                    wordCharCounter = 0;
                }
            // KEEP ADDING CHARACTERS TO WORD UNTIL (MAXWORDCHARLEN) OR (' ', '\n', '\r')
            } else {
                if(wordCharCounter<(MAXWORDCHARLEN-1)){ // copy characters into word up to max word size, allowing space for termination character
                    strncat(word, &character,1);
                    wordCharCounter++;
                }

            }    
        }
                   
        // close file handle
        fclose(fpArray[ct]);
    }

    // handle partially filled wordboxes
    if(wordBoxWordCounter>0){
        for(int ct = wordBoxWordCounter; ct < WORDBOXCOUNT; ct++){
            insertWord(&wordList1, ct, ""); // wipe the rest of the wordbox
        }
        Add2ReaderQ(wordList1, q); // add wordbox to queue
        wordBoxWordCounter = 0; // reset wordbox (shouldn't matter but good practice)
    }

    
    
    return;
}

int main(){
    init();
    initFpArray();
    //printFile();
    //test();
    Queue* q = InitReaderQ();
    book2Wordbox2Queue(q);
    PrintReaderQ(q);
}