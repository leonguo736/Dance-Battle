.include    "address_map_arm.s"         
.include    "defines.s"                 
.include    "interrupt_ID.s"            

/*********************************************************************************
 * Initialize the exception vector table
 ********************************************************************************/
.section    .vectors, "ax"              

        B       _start                      // reset vector
        B       SERVICE_UND                 // undefined instruction vector
        B       SERVICE_SVC                 // software interrrupt vector
        B       SERVICE_ABT_INST            // aborted prefetch vector
        B       SERVICE_ABT_DATA            // aborted data vector
.word       0                               // unused vector
        B       SERVICE_IRQ                 // IRQ interrupt vector
        B       SERVICE_FIQ                 // FIQ interrupt vector

/*********************************************************************************
 * This program demonstrates use of interrupts with assembly code. It first starts
 * two timers: an HPS timer, and the Altera interval timer (in the FPGA). The
 * program responds to interrupts from these timers in addition to the pushbutton
 * KEYs in the FPGA.
 *
 * The interrupt service routine for the HPS timer causes the main program to flash
 * the green light connected to the HPS GPIO1 port.
 *
 * The interrupt service routine for the interval timer displays a pattern on
 * the LED lights, and shifts this pattern either left or right. The shifting
 * direction is set in the pushbutton interrupt service routine; it is reversed
 * each time a KEY is pressed
 ********************************************************************************/
.text                                   
.global     _start                      
_start:                                     
/* Set up stack pointers for IRQ and SVC processor modes */
        MOV     R1, #INT_DISABLE | IRQ_MODE 
        MSR     CPSR_c, R1                  // change to IRQ mode
        LDR     SP, =A9_ONCHIP_END - 3      // set IRQ stack to top of A9 onchip
                                            // memory

/* Change to SVC (supervisor) mode with interrupts disabled */
        MOV     R1, #INT_DISABLE | SVC_MODE 
        MSR     CPSR_c, R1                  // change to supervisor mode
        LDR     SP, =DDR_END - 3            // set SVC stack to top of DDR3 memory

        BL      CONFIG_GIC                  // configure the ARM generic interrupt
                                            // controller
        BL      CONFIG_HPS_TIMER            // configure the HPS timer
        BL      CONFIG_INTERVAL_TIMER       // configure the Altera interval timer
        BL      CONFIG_KEYS                 // configure the pushbutton KEYs

/* initialize the GPIO1 port */
        LDR     R0, =HPS_GPIO1_BASE         // GPIO1 base address
        MOV     R4, #0x01000000             // value to turn on the HPS green light
                                            // LEDG
        STR     R4, [R0, #0x4]              // write to the data direction register
                                            // to set
                                            // bit 24 (LEDG) to be an output
/* enable IRQ interrupts in the processor */
        MOV     R1, #INT_ENABLE | SVC_MODE  // IRQ unmasked, MODE = SVC
        MSR     CPSR_c, R1                  
        LDR     R3, =TICK                   // global variable
LOOP:                                       
        LDR     R5, [R3]                    // read tick variable
        CMP     R5, #0                      // HPS timer expired?
        BEQ     LOOP                        
        MOV     R5, #0                      
        STR     R5, [R3]                    // reset tick variable
        STR     R4, [R0]                    // turn on/off LEDG
        EOR     R4, R4, #0x01000000         // toggle bit that controls LEDG
        B       LOOP                        

/* Configure the HPS timer to create interrupts at one-second intervals */
CONFIG_HPS_TIMER:                           
/* initialize the HPS timer */
        LDR     R0, =HPS_TIMER0_BASE        // base address
        MOV     R1, #0                      // used to stop the timer
        STR     R1, [R0, #0x8]              // write to timer control register
        LDR     R1, =100000000              // period = 1/(100 MHz) x (100 x 10^6)
                                            // = 1 sec
        STR     R1, [R0]                    // write to timer load register
        MOV     R1, #0b011                  // int mask = 0, mode = 1, enable = 1
        STR     R1, [R0, #0x8]              // write to timer control register
        BX      LR                          

/* Configure the Altera interval timer to create interrupts at 50-msec intervals */
CONFIG_INTERVAL_TIMER:                      
        LDR     R0, =TIMER_BASE             
/* set the interval timer period for scrolling the LED displays */
        LDR     R1, =5000000                // 1/(100 MHz) x 5x10^6 = 50 msec
        STR     R1, [R0, #0x8]              // store the low half word of counter
                                            // start value
        LSR     R1, R1, #16                 
        STR     R1, [R0, #0xC]              // high half word of counter start value

                                            // start the interval timer, enable its interrupts
        MOV     R1, #0x7                    // START = 1, CONT = 1, ITO = 1
        STR     R1, [R0, #0x4]              
        BX      LR                          

/* Configure the pushbutton KEYS to generate interrupts */
CONFIG_KEYS:                                
                                            // write to the pushbutton port interrupt mask register
        LDR     R0, =KEY_BASE               // pushbutton key base address
        MOV     R1, #0x3                    // set interrupt mask bits
        STR     R1, [R0, #0x8]              // interrupt mask register is (base + 8)
        BX      LR                          


/* Global variables */
.global     TICK                        
TICK:                                       
.word       0x0                         // used by HPS timer
.global     PATTERN                     
PATTERN:                                    
.word       0x0F0F0F0F                  // pattern to show on the LED lights
.global     KEY_DIR                   
KEY_DIR:                                  
.word       0                           
.end                                    
