#include "stm32f4xx.h"


// Structure to hold SPI initialization parameters
typedef struct {
 SPI_HandleTypeDef* SPI_Params;
 uint8_t* address;
 uint16_t data_size;
 uint32_t data_timeout;
} SPI_Params_Init;


// Structure to hold I2C and I2S initialization parameters
typedef struct {
 I2C_HandleTypeDef* I2C_Params;
 I2S_HandleTypeDef* I2S_Params;
} I2C_I2S_Params_Init;


// Function declaration to initialize SPI and LEDs
void Initialise_SPI_LEDs(SPI_Params_Init spi_params);


// Function declaration to initialize I2C and I2S
void Initialise_I2C_I2S(I2C_I2S_Params_Init i2c_i2s_params);


// Function declarations for controlling LED and button initialization
void Initialise_button(void);
void Blink_LED(uint8_t state, uint8_t color);
