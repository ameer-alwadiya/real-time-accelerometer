#define osObjectsPublic
#include "osObjects.h"
#include "stm32f4xx.h"
#include "my_headers.h"
#include "Thread.h"


SPI_HandleTypeDef SPI_Params; // Declares the structure handle for SPI
uint8_t address; // Declare a variable to hold the address for SPI


I2C_HandleTypeDef I2C_Params; // Declares the structure handle for I2C
I2S_HandleTypeDef I2S_Params; // Declares the structure handle for I2S


SPI_Params_Init spi_params; // Declare a variable of type ‘SPI_Params_Init’ that we have created in ‘my_defines.h’ for SPI
I2C_I2S_Params_Init i2c_i2s_params; // Declare a variable of type ‘I2C_I2S_Params_Init’ that we have created in ‘my_defines.h’ for I2C and I2S


int main(void) {
   SystemCoreClockUpdate();


   spi_params.SPI_Params = &SPI_Params; // Assign the value of ‘SPI_Params’ in the struct ‘spi_params’  with the variable the we declared outside main ‘SPI_Params’
   spi_params.address = &address; // Assign the value of address in the struct ‘spi_params’  with the variable the we declared outside main ‘address’
   spi_params.data_size = 1; // Assign the value of ‘data_size’ in the struct ‘spi_params’  with 1
   spi_params.data_timeout = 1000; // Assign the value of ‘data_timeout’ in the struct ‘spi_params’  with 1000


   i2c_i2s_params.I2C_Params = &I2C_Params; // Assign the value of ‘I2C_Params’ in the struct ‘i2c_i2s_params’  with the variable the we declared outside main ‘I2C_Params’
   i2c_i2s_params.I2S_Params = &I2S_Params; // Assign the value of ‘I2S_Params’ in the struct ‘i2c_i2s_params’  with the variable the we declared outside main ‘I2S_Params’


   osKernelInitialize(); // Initialize CMSIS-RTOS


   /* Initialise any peripherals or system components */
   Initialise_button(); 
   Initialise_SPI_LEDs(spi_params);
   Initialise_I2C_I2S(i2c_i2s_params);
   
   /* Initialise the main threads */
   Init_SPI_Thread(&spi_params);
   Init_Blink_Thread();
   Init_Button_Thread();
   Init_I2C_I2S_Thread(&i2c_i2s_params);


   osKernelStart(); // start thread execution


   while (1) {}; // While loop so the program doesn’t terminate
}