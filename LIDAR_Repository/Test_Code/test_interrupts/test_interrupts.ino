/* This code tests using interrupts with the ultrasonics */

#define MICROSECONDS_PER_CM 373.38f
#define CM_PER_MICROSECONDS (1/MICROSECONDS_PER_CM)

int ultraPinRight = 2; // interrupt input for the right ultrasonic -> int.0
int LEDPinRight = 10; // PW output for the right LED
int ultraPinLeft = 3; // interrupt input for the left ultrasonic -> int.1
int LEDPinLeft = 9; // PW output for the left LED

typedef struct _ultra_state {
  volatile uint32_t startTime; // Start time of pulse
  volatile uint32_t dt; // Pulse duration in microseconds
  volatile boolean pulseStarted; // Flag to ensure correct initial conditions
  volatile boolean lastState; // Keep track of the last logical state of the pin
} ultra_state_t;

ultra_state_t rightUltra;
ultra_state_t leftUltra;


void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  attachInterrupt(0, rightInterrupt, CHANGE);
  attachInterrupt(1, leftInterrupt, CHANGE);
}

void loop() {
}

/* When the right ultrasonic starts/finishes a pulse, this method will be called */
void rightInterrupt() {

}

/* When the left ultrasonic starts/finishes a pulse, this method will be called */
void leftInterrupt() {

}
