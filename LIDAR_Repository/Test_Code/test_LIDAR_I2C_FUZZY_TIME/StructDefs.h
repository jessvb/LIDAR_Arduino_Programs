#ifndef StructDefs_h
#define StructDefs_h

typedef struct _ultra_state {
  volatile uint32_t startTime; // Start time of pulse
  volatile uint32_t dt; // Pulse duration in microseconds
  volatile boolean pulseStarted; // Flag to ensure correct initial conditions
  volatile boolean newDt; // Flag to tell the main loop there's a new pulse time ready -> calc dist
  volatile boolean lastState; // Keep track of the last logical state of the pin
  volatile float distance; // Distance reading based on the pulse read
  volatile float totalDist = 0; // The last couple distance values combined (used to take an average)
  volatile int counter; // A counter for taking the average distance
} ultra_state_t;

typedef const struct _fuzzy_values {
  float minVal; // The point where lo 1
  float loVal; // The point where lo 0.5, med 0.5
  float medVal; // The midpoint (1 med)
  float hiVal; // The point where med 0.5, hi 0.5
  float maxVal; // The point where hi 1
} fuzzy_values_t;

#endif
