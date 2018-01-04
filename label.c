#include "label.h"

/**
  This file contains all the functionality for the labels used in goto and if.
  @file label.c
  @author David Lovato
*/

void initMap( LabelMap *labelMap )
{
  labelMap->cap = INITIAL_CAPACITY;
  labelMap->len = 0;
  labelMap->labels = (char **)malloc(labelMap->cap * sizeof(char *));
  labelMap->label_numbers = (int *)malloc(labelMap->cap * sizeof(int));
}

void addLabel( LabelMap *labelMap, char *name, int loc )
{
  if (findLabel(labelMap, name) != -1) {
    printf("Duplicate label: %s\n", name);
    exit(1);
  }
  if (labelMap->len >= labelMap->cap) {
    labelMap->cap *= GROWTH_RATE;
    labelMap->labels = (char **)realloc(labelMap->labels, labelMap->cap * sizeof(char *));
    labelMap->label_numbers = (int *) realloc(labelMap->label_numbers, labelMap->cap * sizeof(int));
  }
  labelMap->labels[labelMap->len] = (char *) malloc(MAX_TOKEN + 1);
  strcpy(labelMap->labels[labelMap->len], name);
  labelMap->label_numbers[labelMap->len] = loc;
  labelMap->len = labelMap->len + 1;
}

void freeMap(LabelMap *labelMap)
{
  for (int i = 0; i < labelMap->len; i++) {
    free(labelMap->labels[i]);
  }
  free(labelMap->labels);
  free(labelMap->label_numbers);
}

int findLabel(LabelMap *labelMap, char *name)
{
  for (int i = 0; i < labelMap->len; i++) {
    if (strcmp(labelMap->labels[i], name) == 0) {
      return labelMap->label_numbers[i];
    }
  }
  return -1;
}
