#include <Arduino.h>

#include <ESPmDNS.h>
#include <WiFi.h>

#include "FlowSensor.h"
#include "def.h"
#include <ArduinoHA.h>
#include <ArduinoOTA.h>
#include "filter.h"

HardwareSerial distanceSerial( 2 );

uint8_t distRaw[4];

int distances[20];
int distancesSorted[20];
uint8_t distanceCounter = 0;
uint32_t distanceSum;
uint16_t dist_mm;
filter distanceFilter;
void distanceBegin() { distanceSerial.begin( 9600 ); }


void getDistance()
{
    // Serial.print( "check dist sens \r\n" );
    dist_mm = 0;
    distanceSerial.write( 0x01 );
}
uint32_t readDistance()
{
    if( distanceSerial.available() > 3 )
    {
        // Serial.print( "Read value \r\n" );
        if( distanceSerial.read() == 0xFF )
        {
            //  Serial.print( "FF found \r\n" );
            distRaw[1] = distanceSerial.read();
            distRaw[2] = distanceSerial.read();
            distRaw[3] = distanceSerial.read();
            // Serial.printf( "0x%02x 0x%02x 0x%02x\r\n", distRaw[1], distRaw[2], distRaw[3] );
            if( ( ( distRaw[1] + distRaw[2] ) & 0xff ) == ( distRaw[3] + 1 ) )
            {
                //   Serial.printf( "New value:%d\r\n", ( distRaw[1] << 8 ) + distRaw[2] );
                dist_mm = distanceFilter.doFilter( ( distRaw[1] << 8 ) + distRaw[2] );
                Serial.printf( "\r\ndist:%d mm\r\n", dist_mm );
            }
        }
    }
    return dist_mm;
}