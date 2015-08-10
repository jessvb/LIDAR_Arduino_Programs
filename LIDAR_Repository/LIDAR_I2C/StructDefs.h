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

#endif
