#ifndef FILTER_H
#define FILTER_H
#include <Arduino.h>


class filter
{
  public:
     filter();
    int doFilter( int newVal );

  private:
    uint8_t valueCounter = 0;
    int values[20];
    int valuesSorted[20];
};


#endif