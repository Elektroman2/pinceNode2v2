#include "filter.h"
int comp( const void *a, const void *b ) { return ( *(int *)a - *(int *)b ); }

filter::filter()
{
    memset( values, 0, sizeof( values ) );
    valueCounter = 0;
}
int filter::doFilter( int newValue )
{
    int filteredValue;
    int Summa = 0;

    values[valueCounter++] = ( newValue );
  //  Serial.printf( "NewValue: %d, counter: %d", newValue, valueCounter );
    if( valueCounter == 20 )
        valueCounter = 0;
    memcpy( valuesSorted, values, sizeof( valuesSorted ) );
    qsort( valuesSorted, ( sizeof( valuesSorted ) / sizeof( valuesSorted[0] ) ), sizeof( valuesSorted[0] ), comp );

  //  Serial.printf( "\r\nvalues:\r\n" );
  //  for( uint8_t i = 0; i < 20; i++ )
  //  {
  //      Serial.printf( "%d ", values[i] );
  //  }
  //  Serial.printf( "\r\nsorted:\r\n" );
    for( uint8_t i = 0; i < 20; i++ )
    {
        //Serial.printf( "%d ", valuesSorted[i] );
        if( ( i > 1 ) && ( i < 18 ) )
            Summa += valuesSorted[i];
    }
    //Serial.println();
    filteredValue = Summa / 16;
    return filteredValue;
}