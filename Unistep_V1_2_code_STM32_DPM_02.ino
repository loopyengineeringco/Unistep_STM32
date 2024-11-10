/*
This an STM32F030F4 based Unipolar stepper driver - taking step/dir/enable and controlling 4 coils via mosfets/darlington array at 1/2 steps.
Hardware is minimal: bare STM32F030F4, (no external osc needed) and a ULN2003, or something more powerful if needed.

Arduino IDE, official STM32 core https://github.com/stm32duino.

Code adapted to STM32 from the brilliant ATTiny13A based project https://github.com/ChronicMechatronic/UniStep-V1.0 
by Chronic Mechatronic https://www.youtube.com/@ChronicMechatronic.

Differences to the UniStep project:
- STM32F030F4 has more IO and processing headroom, potential for extra functionality in the future.
- Higher maximum stepping rate (theoretically, not tested back to back)
- STM32F030F4 is a 3v3 chip, so step/dir/enable need to be at 3v3 or voltage-divided down to 3v3.
- dir/enable are broken out to individual IOs instead of being combined & needing ADC evaluation.

Steps for preparing a bare STM32F030F4 chip:
- BOOT0 (Pin1) put high, SWDIO and SWDCLK (pins 19 & 20 respectivelly) connected to STLINK V2, as well as ground to ground (Pin 15) and 3v3 to pins 4, 5, 16 (NRST, VDDA and VDD).
- Using STM32CubeProgrammer, connect to the MCU and disable the read-protection, then full erase.
- No need for bootloader, Arduino will now be able to upload using SWD without any need to set serial ports etc.
- BOOT0 needs to be high for programming and low to run the code.

Upload settings:
- Board: Generic STM32F0 series
- Debug symbols and core logs: "None"
- Optimize: "Smallest (-Os default)"
- Board part number: "STM32F030F4 Demo board (HSI internal RC oscillator)"
- C Runtime Library: "Newlib Nano (default)"
- Upload method: "STM32CubeProgrammer (SWD)"
- USB support (if available): "None"
- U(S)ART support: "Disabled (No Serial support)"
*/

// Define input pins
#define StepIn_Pin PB1
#define Enable_Pin PA4
#define Dir_Pin PA5

// Define output pins
#define Out1_Pin PA0
#define Out2_Pin PA1
#define Out3_Pin PA2
#define Out4_Pin PA3

volatile bool direction = false; // CW or CCW rotation
volatile uint8_t StepCount = 1; // Step counter variable

void setup() {
  // Configure input pins using pinMode() and digitalRead()
  pinMode(StepIn_Pin, INPUT);
  pinMode(Enable_Pin, INPUT);
  pinMode(Dir_Pin, INPUT);

  // Configure output pins using direct port manipulation
  // Set PA0-PA3 as outputs (using MODER register for PA0-PA3)
  GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0);

  // Attach an interrupt to the Step input pin
  attachInterrupt(digitalPinToInterrupt(StepIn_Pin), stepISR, RISING);
}

void loop() {
  // Check Enable pin state 
  if ((GPIOA->IDR & (1 << 4)) == 0) {  // Check if PA4 (Enable) is LOW
    executeStepNew(StepCount); // Execute the step sequence based on StepCount
  } else { // Disable is high, stop motor
    // Turn off all motor outputs using direct port manipulation
    GPIOA->ODR &= (1 << 0); // Clear PA0
    GPIOA->ODR &= (1 << 1); // Clear PA1
    GPIOA->ODR &= (1 << 2); // Clear PA2
    GPIOA->ODR &= (1 << 3); // Clear PA3
  }
}

// Step ISR - called when a rising edge is detected on StepIn_Pin
void stepISR() {
  // Determine direction from the Dir pin state
  direction = (GPIOA->IDR & (1 << 5)) != 0;  // Check if PA5 (Dir_Pin) is HIGH or LOW

  // Adjust StepCount based on direction
  if (direction) {
    StepCount++; // Clockwise
  } else {
    StepCount--; // Counterclockwise
  }

  // Keep StepCount within bounds of the 1-8 sequence
  if (StepCount > 8) StepCount = 1;
  if (StepCount < 1) StepCount = 8;
}

// Updated executeStepNew to manipulate the pins directly on GPIOA
void executeStepNew(uint8_t step) {
  // Step 1: Clear all output pins (turn off all coils)
  GPIOA->ODR &= (1 << 0); // Clear PA0
  GPIOA->ODR &= (1 << 1); // Clear PA1
  GPIOA->ODR &= (1 << 2); // Clear PA2
  GPIOA->ODR &= (1 << 3); // Clear PA3

  // Step 2: Set the appropriate coil high based on the step (full-step sequence)
  switch (step) {
    case 1:
      GPIOA->ODR |= (1 << 0);  // Set PA0 high (coil 1)
      break;
    case 2:
      GPIOA->ODR |= (1 << 0);  // Set PA0 high (coil 1)
      GPIOA->ODR |= (1 << 1);  // Set PA1 high (coil 2)
      break;
    case 3:
      GPIOA->ODR |= (1 << 1);  // Set PA1 high (coil 2)
      break;
    case 4:
      GPIOA->ODR |= (1 << 1);  // Set PA1 high (coil 2)
      GPIOA->ODR |= (1 << 2);  // Set PA2 high (coil 3)
      break;
    case 5:
      GPIOA->ODR |= (1 << 2);  // Set PA2 high (coil 3)
      break;
    case 6:
      GPIOA->ODR |= (1 << 2);  // Set PA2 high (coil 3)
      GPIOA->ODR |= (1 << 3);  // Set PA3 high (coil 4)
      break;
    case 7:
      GPIOA->ODR |= (1 << 3);  // Set PA3 high (coil 4)
      break;
    case 8:
      GPIOA->ODR |= (1 << 3);  // Set PA3 high (coil 4)
      GPIOA->ODR |= (1 << 0);  // Set PA0 high (coil 1)
      break;
    default:
      GPIOA->ODR &= (1 << 0); // Clear PA0
      GPIOA->ODR &= (1 << 1); // Clear PA1
      GPIOA->ODR &= (1 << 2); // Clear PA2
      GPIOA->ODR &= (1 << 3); // Clear PA3
      break;
  }
}
