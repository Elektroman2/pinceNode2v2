#ifndef _UTILS_H

#define _UTILS_H
#include <Arduino.h>
int32_t adc_to_bar( uint16_t adc_value );
// ADC --> cmH2O, egész szám (centiméter, két tizedessel)
int32_t adc_to_cmH2O( uint16_t adc_value );


#endif  // _UTILS_H
