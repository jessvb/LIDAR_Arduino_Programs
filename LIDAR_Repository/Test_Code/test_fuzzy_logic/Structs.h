#ifndef Structs_h
#define Structs_h

typedef const struct _fuzzy_values {
  float minVal; // The point where lo 1
  float loVal; // The point where lo 0.5, med 0.5
  float medVal; // The midpoint (1 med)
  float hiVal; // The point where med 0.5, hi 0.5
  float maxVal; // The point where hi 1

  //const int LO = 0;
  //const int MED = 1;
  //const int HI = 2;
} fuzzy_values_t;

#endif
