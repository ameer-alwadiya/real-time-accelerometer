#include "cmsis_os.h"                           
#include "stm32f4xx.h"


extern int Init_SPI_Thread(const SPI_Params_Init *spi_params); // Initializing SPI thread
void SPI_Thread(void const *argument); // The main thread function for the SPI


extern int Init_Button_Thread (void); // Initializing button thread
void Button_Thread (void const *argument); // The main thread function for the button


extern int Init_Blink_Thread (void); // Initializing blink thread
void Blink_Thread (void const *argument); // The main thread function for the blink


extern int Init_I2C_I2S_Thread(const I2C_I2S_Params_Init *i2c_i2s_params); // Initializing I2C and I2S thread
void I2C_I2S_Thread(void const *argument);  // The main thread function for I2C and I2S
