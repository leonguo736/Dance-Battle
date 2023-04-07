.include    "address_map_arm.s"
.include    "defines.s"     
.extern     KEY_DIR         /* externally defined variable */
/***************************************************************************************
 * Pushbutton KEY - Interrupt Service Routine
 *
 * This routine toggles the KEY_DIR variable from 0 <-> 1
 ****************************************************************************************/
.global     KEY_ISR         
KEY_ISR:
        LDR     R0, =KEY_BASE   // base address of pushbutton KEY parallel port
/* KEY[1] is the only key configured for interrupts, so just clear it. */
        LDR     R1, [R0, #0xC]  // read edge capture register
        STR     R1, [R0, #0xC]  // clear the interrupt

        LDR     R1, =KEY_DIR    // set up a pointer to the shift direction variable
        LDR     R2, [R1]        // load value of shift direction variable
        EOR     R2, R2, #1      // toggle the shift direction
        STR     R2, [R1]        

END_KEY_ISR:
        BX      LR              
.end                        

