#include "cmsis_os.h"                            // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_headers.h"
#include "stdio.h"


uint8_t LED_on = 1; // Defines parameter for LED on
uint8_t LED_off = 0; // Defines parameter for LED off


uint8_t green_LED = 12; // Defines parameter for yellow LED (GPIOD pin 12)
uint8_t orange_LED = 13; // Defines parameter for orange LED (GPIOD pin 13)
uint8_t red_LED = 14; // Defines parameter for red LED (GPIOD pin 14)
uint8_t blue_LED = 15; // Defines parameter for blue LED (GPIOD pin 15)


int flag = 0; // Define a flag to make the push-button work as a latch switch

        /* Thread(1): SPI thread declaration and initialisation */
void SPI_Thread(void const *argument); // Declaration for the main thread function for SPI
osThreadId tid_SPI_Thread; // Declares an ID for SPI thread
osThreadDef (SPI_Thread, osPriorityNormal, 1, 0); // Define SPI_Thread with normal priority and a stack size of 1 word


        // Initialisation function for the SPI thread
int Init_SPI_Thread(const SPI_Params_Init *spi_params) {
  // Create an SPI thread that takes a pointer of 'SPI_Params_Init' type as parameter that we created in ‘my_defines.h’, which contain the suitable variables and parameters for initialising SPI communication
  tid_SPI_Thread = osThreadCreate(osThread(SPI_Thread), (void *)spi_params); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared
  if (!tid_SPI_Thread) return (-1); // Checks to make sure the thread has been created
   return (0);
}

        /* Thread(2): Blink thread declaration and initialisation */
void Blink_Thread (void const *argument); // Declaration for the main thread function for blinking LED
osThreadId tid_Blink_Thread; // Declares an ID for Blink thread
osThreadDef (Blink_Thread, osPriorityNormal, 1, 0); // Define Blink_Thread with normal priority and a stack size of 1 word


        // Initialisation function for the Blink thread
int Init_Blink_Thread (void) {
 tid_Blink_Thread = osThreadCreate (osThread(Blink_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared
 if(!tid_Blink_Thread) return(-1); // Checks to make sure the thread has been created
  return(0);
}

       /* Thread(3): Button thread declaration and initialisation */
void Button_Thread (void const *argument); // Declaration for the main thread function for the button
osThreadId tid_Button_Thread; // Declares an ID for Button thread
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); // Define Button_Thread with normal priority and a stack size of 1 word


int Init_Button_Thread (void) {
 tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared
 if(!tid_Button_Thread) return(-1); // Checks to make sure the thread has been created
  return(0);
}

       /* Thread(4): I2C and I2S thread declaration and initialisation */
void I2C_I2S_Thread (void const *argument); // Declaration for the main thread function for the I2C and I2S
osThreadId tid_I2C_I2S_Thread; // Declares an ID for I2C and I2S thread
osThreadDef (I2C_I2S_Thread, osPriorityNormal, 1, 0); // Define I2C_I2S_Thread with normal priority and a stack size of 1 word


int Init_I2C_I2S_Thread (const I2C_I2S_Params_Init *i2c_i2s_params) {
 // Create an I2C and I2S thread that takes a pointer of ‘I2C_I2S_Params_Init’ type as parameter that we created in ‘my_defines.h’, which contain the suitable variables and parameters for initialising I2C and I2S communication
 tid_I2C_I2S_Thread = osThreadCreate (osThread(I2C_I2S_Thread), (void *)i2c_i2s_params); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared
 if(!tid_I2C_I2S_Thread) return(-1); // Checks to make sure the thread has been created
  return(0);
}

        /* Thread(1): SPI main thread function */
void SPI_Thread(void const *argument) {
 SPI_Params_Init *spi_params = (SPI_Params_Init *)argument; // To access the parameters from the main file using the struct that we have created


 uint8_t XL, XH, YL, YH; // Variables to store the register values for XL, XH, YL and YH of LIS3DSH
 int16_t X, Y; // Variables to combine XL and XH into single variable, and the same done for Y
 
 osSignalSet(tid_SPI_Thread, 0x01); // Set the 0x01 flag of the SPI thread to enable its execution when we upload the program
 while(1) {
   osSignalWait(0x01, osWaitForever); // Waits until flag 0x01 of this thread is set (like if we click on the push button)
   osSignalSet(tid_SPI_Thread, 0x01); // Set flag 0x01 of the SPI thread so that it resumes next time wait is called
  
   *spi_params->address = 0x80 | 0x29; // Address for the x-axis (H) data register on the LIS3DSH
   GPIOE->BSRR = GPIO_PIN_3 << 16; // Set the SPI communication enable line low to initiate communication
   HAL_SPI_Transmit(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Send the address of the register to be read on the LIS3DSH
   *spi_params->address = 0x00; // Set a blank address because we are waiting to receive data
   HAL_SPI_Receive(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Get the data from the LIS3DSH through the SPI channel
   GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
  
   XH = *spi_params->address; // Read the data from the SPI data array into our internal variable
  
   *spi_params->address = 0x80 | 0x28; // Address for the x-axis (L) data register on the LIS3DSH
   GPIOE->BSRR = GPIO_PIN_3 << 16; // Set the SPI communication enable line low to initiate communication
   HAL_SPI_Transmit(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Send the address of the register to be read on the LIS3DSH
   *spi_params->address = 0x00; // Set a blank address because we are waiting to receive data
   HAL_SPI_Receive(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Get the data from the LIS3DSH through the SPI channel
   GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
  
   XL = *spi_params->address; // Read the data from the SPI data array into our internal variable
   X = ((XH<<8) | XL); // Combine XL and XH into a single variable by left-shifting XH by 8 units and adding XL to the first 8 bits
  
   *spi_params->address = 0x80 | 0x2B; // Address for the MSB y-axis (H) data register on the LIS3DSH
   GPIOE->BSRR = GPIO_PIN_3 << 16; // Set the SPI communication enable line low to initiate communication
   HAL_SPI_Transmit(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Send the address of the register to be read on the LIS3DSH
   *spi_params->address = 0x00; // Set a blank address because we are waiting to receive data
   HAL_SPI_Receive(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Get the data from the LIS3DSH through the SPI channel
   GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
  
   YH = *spi_params->address; // Read the data from the SPI data array into our internal variable
  
   *spi_params->address = 0x80 | 0x2A; // Address for the MSB y-axis (L) data register on the LIS3DSH
   GPIOE->BSRR = GPIO_PIN_3 << 16; // Set the SPI communication enable line low to initiate communication
   HAL_SPI_Transmit(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Send the address of the register to be read on the LIS3DSH
   *spi_params->address = 0x00; // Set a blank address because we are waiting to receive data
   HAL_SPI_Receive(spi_params->SPI_Params, spi_params->address, spi_params->data_size, spi_params->data_timeout); // Get the data from the LIS3DSH through the SPI channel
   GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
  
   YL = *spi_params->address; // Read the data from the SPI data array into our internal variable
   Y = ((YH<<8) | YL); // Combine YL and YH into a single variable by left-shifting YH by 8 units and adding YL to the first 8 bits
  
   if (Y > 500) {
   // Y is now a 16-bit signed variable, resulting in an acceleration range typically from -32768 to +32767
   // When the device is in static condition the X and Y values should be close to 0 (zero)
   // When the device is being tilted in a direction corresponding to the positive Y-axis the value of Y will be increasing
  // Here the sensitivity value for positive Y is 500 when the device is being tilted more than this value the orange led will turn on


   Blink_LED(LED_on, orange_LED); // If the receive value is more than 500 turn on the orange LED
   } else {
     Blink_LED(LED_off, orange_LED); // Otherwise turn off the LED
   }
  
   if (Y < -500) {
     // When the device is being tilted in a direction corresponding to the negative Y-axis the value of Y will be decreasing
     // Here the sensitivity value for negative Y is -500 when the device is being tilted less than this value the blue led will turn on
     Blink_LED(LED_on, blue_LED); // If the receive value is less than -500 turn on the blue LED
   } else {
     Blink_LED(LED_off, blue_LED); // Otherwise turn off the LED
   }


   if (X > 500) {
      // When the device is being tilted in a direction corresponding to the positive X-axis the value of X will be increasing
     // Here the sensitivity value for positive X is 500 when the device is being tilted more than this value the red led will turn on
     Blink_LED(LED_on, red_LED); // If the receive value is more than 500 turn on the red LED
   } else {
     Blink_LED(LED_off, red_LED); // Otherwise turn off the LED
   }
   if (X < -500) {
     // When the device is being tilted in a direction corresponding to the negative X-axis the value of X will be decreasing
     // Here the sensitivity value for negative X is -500 when the device is being tilted less than this value the blue led will turn on
     Blink_LED(LED_on, green_LED); // If the received value is positive turn on the green LED
   } else {
     Blink_LED(LED_off, green_LED); // Otherwise turn off the LED
   }
  
   osThreadYield(); // Yield the processor to other ready-to-run threads
 }
}


        /* Thread(2): Blink main thread function */
void Blink_Thread (void const *argument) {
  
 osSignalClear(tid_Blink_Thread, 0x01); // Clear the 0x01 flag of the blink LED thread to prevent it from running when we upload the program
while (1) { 
  osSignalWait(0x01, osWaitForever); // Waits until flag 0x01 of this thread is set (it will be set the first time when we click on the push button)
  osSignalSet(tid_Blink_Thread, 0x01); // Set flag 0x01 of the blink LED thread so that it resumes next time the wait is called


          // Repeatedly blinking on for 0.5 seconds and off for 0.5 seconds
  Blink_LED(LED_on, red_LED);
  osDelay(500);
  Blink_LED(LED_off, red_LED);   
  osDelay(500);
      
  osSignalWait(0x01, osWaitForever); // If we click the push button again, it will pause the blinking. We may click on it during the blinking that’s why we called it again here
  osSignalSet(tid_Blink_Thread, 0x01); // Set flag 0x01 of the blink LED thread so that it resumes next time the wait is called
      
  osThreadYield(); // Yield the processor to other ready-to-run threads
 }
}


        /* Thread(3): Button main thread function */
void Button_Thread (void const *argument) {
 while (1) { 
       if(((GPIOA->IDR & 0x00000001) == 0x00000001) && flag == 0){
        // If the push button has been clicked and the value for the flag variable is zero, then turn on the blinking, I2C and I2S threads and turn off the SPI thread


           osSignalClear(tid_SPI_Thread, 0x01); // Clear the 0x01 flag of the SPI thread when we click on the button
          
           flag = 1; // Set the flag value to 1. This ensures that if we click the button again, it will trigger a different functionality
            
           // All four user LEDs blinking on for 1 second
           Blink_LED(LED_on, red_LED);
           Blink_LED(LED_on, blue_LED);
           Blink_LED(LED_on, green_LED);
           Blink_LED(LED_on, orange_LED);
           osDelay(1000);
           Blink_LED(LED_off, red_LED);
           Blink_LED(LED_off, blue_LED);
           Blink_LED(LED_off, green_LED);
           Blink_LED(LED_off, orange_LED);
                    
           osSignalSet(tid_Blink_Thread, 0x01); // Set flag 0x01 of the blink LED thread so that it can run now
           osSignalSet(tid_I2C_I2S_Thread, 0x01); // Set flag 0x01 of the I2C and I2S thread so that it can run now
       }
       if (((GPIOA->IDR & 0x00000001) == 0x00000001) && flag == 1) {
        // If the push button has been clicked and the value for the flag variable is one, then turn off the blinking, I2C and I2S threads and turn on the SPI thread


           osSignalClear(tid_Blink_Thread, 0x01); // Clear the 0x01 flag of the blinking thread so it can stop working
           osSignalClear(tid_I2C_I2S_Thread, 0x01); // Clear the 0x01 flag of the I2C and I2S thread so it can stop working
        
           osDelay(200); // A delay of 200 milliseconds so that we could remove the debounce effect of the switch


           flag = 0; // Set the flag value to 0. This ensures that if we click the button again, it will trigger a different functionality
          
           osSignalSet(tid_SPI_Thread, 0x01); // Set flag 0x01 of the SPI thread so that it can run now
       }


       osThreadYield(); // Yield the processor to other ready-to-run threads
 }
}


        /* Thread(4): I2C and I2S main thread function */
void I2C_I2S_Thread(void const *argument) {
   I2C_I2S_Params_Init *i2c_i2s_params = (I2C_I2S_Params_Init *)argument; // To access the parameters from the main file using the struct that we have created


   uint16_t S[8] = {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000}; // A square wave array


   while (1) { 
       osSignalWait(0x01, osWaitForever); // Waits until flag 0x01 of this thread is set (it will be set the first time when we click on the push button)
       osSignalSet(tid_I2C_I2S_Thread, 0x01); // Set flag 0x01 of the I2C and I2S thread so that it resumes next time the wait is called

       HAL_I2S_Transmit_IT(i2c_i2s_params->I2S_Params, S, 8); // Transmit the data of S through I2S to generate the sound
       osDelay(1000); // Delay of one second

       osSignalWait(0x01, osWaitForever); // If we click the push button again, it will pause the I2C and I2C thread. We may click on it during the transmission that’s why we called it again here
       osSignalSet(tid_I2C_I2S_Thread, 0x01); // Set flag 0x01 of the blink LED thread so that it resumes next time the wait is called

       osThreadYield(); // Yield the processor to other ready-to-run threads
   }
}
