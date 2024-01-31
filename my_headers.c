#include "stm32f4xx.h"
#include "my_headers.h"


// Function to initialise SPI communication protocol and the 4 LEDs
void Initialise_SPI_LEDs(SPI_Params_Init spi_params) {
   // Declare a handle structure for Port A pins
   GPIO_InitTypeDef GPIOA_Params; 


   // Declare a handle structure for Port B pins
   GPIO_InitTypeDef GPIOE_Params; 


   // Code to initialise the SPI
   RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable the clock for SPI1 which is found in the register APB2.


   // (*spi_params.SPI_Params) accesses the member named ‘SPI_Params’ within the structure pointed to by ‘spi_params’, and the ‘*’ sign is used to dereference and access the value of it
   (*spi_params.SPI_Params).Instance = SPI1; // Set SPI instance to SPI1
   (*spi_params.SPI_Params).Init.Mode = SPI_MODE_MASTER; // Set SPI mode to master
   (*spi_params.SPI_Params).Init.NSS = SPI_NSS_SOFT; // Set SPI NSS (Slave Select) mode to software control
   (*spi_params.SPI_Params).Init.Direction = SPI_DIRECTION_2LINES; // Set SPI data transfer direction to full-duplex
   (*spi_params.SPI_Params).Init.DataSize = SPI_DATASIZE_8BIT; // Set SPI data size to 8 bits
   (*spi_params.SPI_Params).Init.CLKPolarity = SPI_POLARITY_HIGH; // Set SPI clock polarity to high
   (*spi_params.SPI_Params).Init.CLKPhase = SPI_PHASE_2EDGE; // Data is captured on the second edge of the clock signal
   (*spi_params.SPI_Params).Init.FirstBit = SPI_FIRSTBIT_MSB; // Set SPI data frame format to MSB first
   (*spi_params.SPI_Params).Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Set SPI Baud Rate Prescaler to 32
   HAL_SPI_Init(spi_params.SPI_Params); // Initialize SPI using the specified parameters


   // Code to initialise pins 5-7 of GPIOA
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock for GPIOA in the AHB1 peripheral clock register
   GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Configure GPIOA pins 5, 6, and 7 for SPI communication
   GPIOA_Params.Alternate = GPIO_AF5_SPI1; // Set the alternate function for GPIOA pins to SPI1
   GPIOA_Params.Mode = GPIO_MODE_AF_PP; // Set GPIOA pins mode to alternate function push-pull
   GPIOA_Params.Speed = GPIO_SPEED_FAST; // Set GPIOA pins speed to fast
   GPIOA_Params.Pull = GPIO_NOPULL; // Set GPIOA pins pull configuration to no pull-up/pull-down
   HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Initialize GPIOA pins with the specified parameters


   // Code to initialise pin 3 of GPIOE
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable clock for GPIOE in the AHB1 peripheral clock register


   // Configure GPIOE pin 3 for general-purpose output in push-pull mode
   GPIOE_Params.Pin = GPIO_PIN_3;
   GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP;
   GPIOE_Params.Speed = GPIO_SPEED_FAST;


   GPIOE_Params.Pull = GPIO_PULLUP; // Set GPIOE pin 3 pull configuration to pull-up
   HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Initialize GPIOE pin 3 with the specified parameters
   GPIOE->BSRR = GPIO_PIN_3; // Set GPIOE pin 3 to high (BSRR: Bit Set Reset Register)
   __HAL_SPI_ENABLE(spi_params.SPI_Params); // Enable SPI communication on the specified SPI instance


   // Initialize GPIO Port for LEDs
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable clock for GPIOD in the AHB1 peripheral clock register


   // Configure GPIOD pin 12, 13, 14, and 15 for general-purpose output
   GPIOD->MODER |= GPIO_MODER_MODER14_0;
   GPIOD->MODER |= GPIO_MODER_MODER12_0;
   GPIOD->MODER |= GPIO_MODER_MODER15_0;
   GPIOD->MODER |= GPIO_MODER_MODER13_0;


   *spi_params.address = 0x20; // Address for control register 4 on LIS3DSH 
   GPIOE->BSRR = GPIO_PIN_3 << 16; // Initiate communication by setting the SPI communication enable line to low


   // Send the address of the register to be read on the LIS3DSH
   HAL_SPI_Transmit(spi_params.SPI_Params,
                    spi_params.address,
                    spi_params.data_size,
                    spi_params.data_size);


   *spi_params.address = 0x13; // Write a new value to control register 4 of the LIS3DSH by setting register value to give a sample rate of 3.125Hz, continuous update and enable x-axis and y-axis only


   // Send the new register value to the LIS3DSH through the SPI channel
   HAL_SPI_Transmit(spi_params.SPI_Params,
                    spi_params.address,
                    spi_params.data_size,
                    spi_params.data_size);


   GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the  communication process
}




// Function to initialize GPIO for push-button
void Initialise_button(void){
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock for GPIOA in the AHB1 peripheral clock register  
}


// Definition for the function to blink the LED
void Blink_LED(uint8_t LED_state, uint8_t LED_colour){
   // Checks to see if the request is to turn the LED on or off
   if(LED_state == 1){ 
     GPIOD->BSRR = 1<<LED_colour; // Turn on the LED
   }
   else{   
     GPIOD->BSRR = 1<<(LED_colour+16); // Turn off the LED
   }       
}


// Function to initialize I2C and I2S peripherals
void Initialise_I2C_I2S(I2C_I2S_Params_Init i2c_i2s_params) {
    // Initialize GPIO parameters for B, C and D ports
    GPIO_InitTypeDef GPIOD_Params;
    GPIO_InitTypeDef GPIOB_Params;
    GPIO_InitTypeDef GPIOC_Params;


    uint8_t C[2]; // Initialize an array for I2C data


    // Configure GPIOD for reset pin on CS43L22
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable the clock for GPIOD
    GPIOD_Params.Pin = GPIO_PIN_4; // Select pin 4 in port D
    GPIOD_Params.Mode = GPIO_MODE_OUTPUT_PP; // Selects normal output push-pull mode
    GPIOD_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed
    GPIOD_Params.Pull = GPIO_NOPULL; // Selects pull-down activation
    HAL_GPIO_Init(GPIOD, &GPIOD_Params); // Sets GPIOD into the modes specified in GPIOE_Params.


    // Configure GPIOB for SCL (PB6) and SDA (PB9) on I2C1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable the clock for GPIOB
    GPIOB_Params.Pin = GPIO_PIN_6 | GPIO_PIN_9; // Selects pins 6 and 9
    GPIOB_Params.Alternate = GPIO_AF4_I2C1; // Selects alternate function I2C1
    GPIOB_Params.Mode = GPIO_MODE_AF_OD; // Selects alternate function open drain mode
    GPIOB_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed
    GPIOB_Params.Pull = GPIO_NOPULL; // Selects no pull-up or pull-down activation
    HAL_GPIO_Init(GPIOB, &GPIOB_Params); // Sets GPIOB into the modes specified in GPIOA_Params


  // Configure I2C peripheral
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN  ; // Enables the clock for I2C1 
   // (*i2c_i2s_params.I2C_Params) accesses the member named I2C_Params within the structure pointed to by i2c_i2s_params, and the ‘*’ sign is used to dereference and access the value of it
  (*i2c_i2s_params.I2C_Params).Instance = I2C1; // Set I2C instance to I2C1
  (*i2c_i2s_params.I2C_Params).Init.ClockSpeed = 400000; // Set clock speed to 400 kHz
  (*i2c_i2s_params.I2C_Params).Init.DutyCycle = I2C_DUTYCYCLE_2; // Set the I2C duty cycle to 2, where '2' indicates a 50% duty cycle. In this context, '2' is used to represent a 1:1 ratio or 50% duty cycle
  (*i2c_i2s_params.I2C_Params).Init.OwnAddress1 = 0x33; // Set the own address for I2C communication to 0x33
  (*i2c_i2s_params.I2C_Params).Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // Set the addressing mode to 7-bit
  (*i2c_i2s_params.I2C_Params).Init.DualAddressMode = I2C_DUALADDRESS_DISABLED; // Disable dual address mode
  (*i2c_i2s_params.I2C_Params).Init.OwnAddress2 = 0; // Set the second own address to 0 (not used in this case)
  (*i2c_i2s_params.I2C_Params).Init.GeneralCallMode = I2C_GENERALCALL_DISABLED; // Disable general call mode
  (*i2c_i2s_params.I2C_Params).Init.NoStretchMode = I2C_NOSTRETCH_DISABLED; // Disable clock stretching during data transfer
  HAL_I2C_Init(i2c_i2s_params.I2C_Params); // Initialize the I2C peripheral with the specified parameters
   
  // Configure GPIOC for 7(MCLK), 10(SCLK) and 12(SDIN) on I2S:
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable the clock for GPIOC.
  GPIOC_Params.Pin = GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_12; // Selects pins 7(MCLK), 10(SCLK) and 12(SDIN).
  GPIOC_Params.Alternate = GPIO_AF6_SPI3; // Selects alternate function (I2S).
  GPIOC_Params.Mode = GPIO_MODE_AF_PP; // Selects alternate function open drain mode.
  GPIOC_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed.
  GPIOC_Params.Pull = GPIO_NOPULL; // Selects no pull-up or pull-down activation.
  HAL_GPIO_Init(GPIOC, &GPIOC_Params); // Sets GPIOB into the modes specified in GPIOA_Params.


// Configure I2S peripheral
RCC->APB1ENR |= RCC_APB1ENR_SPI3EN  ; // Enables the clock for SPI3 (I2S)  (*i2c_i2s_params.I2S_Params).Instance = SPI3; // Set the I2S instance to SPI3
(*i2c_i2s_params.I2S_Params).Init.Mode = I2S_MODE_MASTER_TX; // Configure I2S mode as Master Transmit
(*i2c_i2s_params.I2S_Params).Init.Standard = I2S_STANDARD_PHILIPS; // Set the I2S standard to Philips format
(*i2c_i2s_params.I2S_Params).Init.DataFormat = I2S_DATAFORMAT_16B; // Configure data format as 16-bit
(*i2c_i2s_params.I2S_Params).Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE; // Enable the Master Clock (MCLK) output
(*i2c_i2s_params.I2S_Params).Init.AudioFreq = I2S_AUDIOFREQ_44K; // Set the audio frequency to 44 kHz
(*i2c_i2s_params.I2S_Params).Init.CPOL = I2S_CPOL_LOW; // Set the clock polarity to low
(*i2c_i2s_params.I2S_Params).Init.ClockSource = I2S_CLOCK_PLL; // Use PLL as the clock source
(*i2c_i2s_params.I2S_Params).Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE; // Disable full duplex mode
HAL_I2S_Init(i2c_i2s_params.I2S_Params); // Initialize I2S with the configured parameters


GPIOD->BSRR = GPIO_PIN_4; // Sets the reset pin of CS43L22 high


// Required Initialization Settings from CS43L22 datasheet      
C[0] = 0x00;  
C[1] = 0x99; // Write 0x99 to register 0x00
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x47;  
C[1] = 0x80; // Write 0x80 to register 0x47 
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x32;  
C[1] = 0x80; // Write ‘1’b to bit 7 in register 0x32 
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x47; 
C[1] = 0x00; // Write ‘0’b to bit 7 in register 0x47
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x00;  
C[1] = 0x00;  // Write 0x00 to register 0x00
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x1E; // Beep & tone configuration register
C[1] = 0xC0; // Write 0xC0 (continuous Beep Occurrence) to register 0x1E 
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);


C[0] = 0x02;  
C[1] = 0x9E; // Set the “Power Ctl 1” register (0x02) to 0x9F
HAL_I2C_Master_Transmit(i2c_i2s_params.I2C_Params, 0x94, C, 2, 50);
}
