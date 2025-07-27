#include "Arduino.h"
int32_t adc_to_bar( uint16_t adc_value )
{
    if( adc_value <= 255 )
    {
        return 0;  // 0.0000 Bar
    }
    else
    {
        return ( 1554 * adc_value - 39740 ) / 10000;  // Bar * 10000
    }
}
// ADC --> cmH2O, egész szám (centiméter, két tizedessel)
int32_t adc_to_cmH2O( uint16_t adc_value )
{
    if( adc_value <= 255 )
    {
        return 0;  // 0 cm
    }
    else
    {
        return ( 159 * adc_value - 40587 ) / 100;
    }
}