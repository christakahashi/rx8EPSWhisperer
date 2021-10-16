# WARNING
Operating a modified production car can be dangerous or life threatening.  Don't do it.  This project is only for testing your powersteering components in either an otherwise inoperable car or under SLOW SAFE conditions and NEVER on the road or in public. 

# rx8EPSWhisperer
This is a hack to recover the power steering on an RX8 power steering rack thats not longer connected to the ECU.

It reads a potentiometer connected to ADC7 to determine the power steering assist level.

# Hardware
The hardware was designed to be multipurpose as either a CAN sensor interface or to enable the EPS on an RX8.  All components were choosen to be AEC-Q100/AEC-Q200 qualified or have variants that are qualified.  It shoudn't matter for bench test use, but if that matters to you then make sure to buy the right variants.

# Hookup
Connect 12v, ground, CAN+, CAN-, to their respective connections.  If you don't have external termination please close the termination jumper on the PCB.  
To wire it you can either cut the wires on your connector short and run them directly into the PCB making it directly hang off the EPS computer, or you can run longer wires from the 4 pin connector.  Either way 12v and ground need to be connected to the cars accessory power.  

# NOTES
- This isn't a product if anything here doesn't make sense or makes you nervous don't use this hardware/software.
- The can library came modified from atmel, now microchip.  The license referred to in the source for that library can no longer be found, but if its like similar libraries from atmel its free to use as long as its only used on atmel chips.  The licene text file included with this repository only applies to the non-library files.  If you represent atmel/microchip and have clarification or want me to remove the library please contact me.
