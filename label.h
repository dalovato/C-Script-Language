/**
  @file label.h
  @author David Lovato, dalovato
  Mapping from label names to lines of code.
*/

#ifndef _LABEL_H_
#define _LABEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Initial capacity for resizable arrays. */
#define INITIAL_CAPACITY 5

/** Growth rate (multiplier) for resizable arrays. */
#define GROWTH_RATE 2

// Maximum length of a token in the source file.
#define MAX_TOKEN 1023

/** Map from label names to locations in the code. */
typedef struct {

  //Corresponding integer array containing the command number for
  //each label at the same index of the label in the char array.
  int *label_numbers;
  
  int cap;
  
  int len;
  
  /** Pointer to pointers to char arrays which contain labels */
  char **labels;

} LabelMap;

/** Initialize the fields of the given labelMap structure.
    @param labelMap Addres of the structure to initialize.
*/
void initMap( LabelMap *labelMap );

/** Add a label to the given labelMap.  Print an error message and
    exit if the label is a duplicate.
    @param labelMap LabelMap to add a label to.
    @param name Name of the label to add.
    @param loc Location of the label in the code.
*/
void addLabel( LabelMap *labelMap, char *name, int loc );

/**
  This function will find the label's command number by searching through
  the struct's label array, returning the command number to jump to.
  @param *labelMap, pointer to LabelMap
  @param name, the label to search for
  @return int, the command to jump to
*/
int findLabel(LabelMap *labelMap, char *name);

/**
  This function will free all the dynamically allocated memory for the
  labelMap.
  @param *labelMap, a pointer to a LabelMap struct
*/
void freeMap(LabelMap *labelMap);

#endif
