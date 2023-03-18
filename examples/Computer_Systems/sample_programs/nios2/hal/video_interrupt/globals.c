#include "globals.h"

/* global variables */
volatile int timeout; // used to synchronize with the timer

struct alt_up_dev up_dev; // struct to hold pointers to open devices
