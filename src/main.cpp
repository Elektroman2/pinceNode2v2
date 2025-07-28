#include <Arduino.h>
#include "Open_Sans_Regular_11.h"
#include <ESPmDNS.h>
#include <WiFi.h>
#include <ezButton.h>
#include "FlowSensor.h"
#include "def.h"
#include <ArduinoHA.h>
#include <ArduinoOTA.h>
#include "distance.h"
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "filter.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier

#include "SSD1306Wire.h"
#include "WaterMeter.h"
#include "utils.h"


#define WELL_WATERMETER_IN_PIN 4
#define WELL_PUMP_OUT_PIN 18
#define COMPRESSOR_OUT_PIN 19
#define IRRIGATION_PUMP_OUT_PIN 21
#define WELL_VALVE_OUT_PIN 22
#define IRRIGATION_WATERMETER_IN_PIN 23
#define RESERVED_IN_PIN 13
#define BTN1_IN_PIN 26
#define BTN2_IN_PIN 25
#define BTN3_IN_PIN 33

#define COMPRESSOR_PRESSURE_AIN_PIN A0
#define WELL_PRESSURE_AIN_PIN A3

SSD1306Wire display( 0x3c, 14, 27 );
WiFiClient client;
HADevice device;
HAMqtt mqtt( client, device, 25 );


unsigned long AnalogLastUpdateAt   = 0;
unsigned long AnalogUpdateInterval = 150;

unsigned long ControlLastUpdateAt   = 0;
unsigned long ControlUpdateInterval = 450;

unsigned long flowLastUpdateAt   = 0;
unsigned long flowUpdateInterval = 2000;

unsigned long distanceLastUpdateAt = 0;

unsigned long distanceInterval = 500;

HASwitch led( "Led1" );
bool compressorState = false;
HASwitch compressor( "wellCompressor1" );

uint32_t airPressureRawValue;
int32_t airPressureValue;
int32_t airPressureValueUnfilterd  = 0;
int32_t airPressureTargetValue     = 0;
int32_t airPressureHysteresisValue = 0;
filter airPressureFilter;

HASensorNumber airPressureRaw{ "airPressureRaw", HASensorNumber::PrecisionP3 };
HASensorNumber airPressureActual{ "airPressureActual", HASensorNumber::PrecisionP3 };
HANumber airPressureTarget{ "airPressureTarget", HASensorNumber::PrecisionP0 };
HANumber airPressureHystersis{ "airPressureHysteresis", HASensorNumber::PrecisionP0 };

uint32_t wellLevelRawValue;
uint32_t wellLevelValue;
filter wellLevelFilter;
HASensorNumber wellLevelRaw{ "wellLevelRaw1", HASensorNumber::PrecisionP0 };
HASensorNumber wellLevel{ "wellLevel1", HASensorNumber::PrecisionP3 };


bool wellPumpState = false;
HASwitch wellPump( "wellPump" );

WaterMeter wellWaterMeter( WELL_WATERMETER_IN_PIN );
HASensorNumber wellVolume{ "wellVolume", HASensorNumber::PrecisionP3 };
uint32_t wellVolumeSumValue = 0;
HANumber wellVolumeSum{ "wellVolumeSum", HASensorNumber::PrecisionP0 };
filter TankDistanceFilter;
uint32_t TankDistanceValue = 0;
HASensorNumber TankDistance{ "TankDistance", HASensorNumber::PrecisionP0 };


bool irrigationPumpState = false;
HASwitch irrigationPump{ "wateringPump" };


HASwitch valve{ "backpumpingValve" };
HASwitch backPump{ "wateringPump" };
DualPurposeWaterMeter irrigationBackpumpMeter( IRRIGATION_WATERMETER_IN_PIN, WELL_VALVE_OUT_PIN );
HASensorNumber backVolume{ "backpumpVolume", HASensorNumber::PrecisionP3 };
HASensorNumber irrigationVolume{ "irrigationVolume", HASensorNumber::PrecisionP3 };

ezButton button1( BTN1_IN_PIN );
ezButton button2( BTN2_IN_PIN );
ezButton button3( BTN3_IN_PIN );


#define stringify( name ) #name


void onMqttConnected() { Serial.println( "Connected to the broker!" ); }
void onMqttMessage( const char *topic, const uint8_t *payload, uint16_t length )
{
    char payloadBuf[50];
    uint8_t len = ( length < 50 ? length : 50 );
    snprintf( payloadBuf, len, "%s", payload );
    Serial.printf( "Message received: topic: %s, payload:%s\r\n", topic, payloadBuf );
}


void ledSwitchCommand( bool state, HASwitch *sender )
{
    digitalWrite( LED_PIN, ( state ? HIGH : LOW ) );
    Serial.printf( "Led command arrived: %d\r\n", state );
    sender->setState( state );  // report state back to the Home Assistant
}

void irrigationSwitchCommand( bool state, HASwitch *sender )
{
    digitalWrite( IRRIGATION_PUMP_OUT_PIN, ( state ? HIGH : LOW ) );
    Serial.println( "Irrigation command arrived" );
    sender->setState( state );  // report state back to the Home Assistant
}


void wellpumpForceCommand( bool state, HASwitch *sender )
{
    digitalWrite( WELL_PUMP_OUT_PIN, ( state ? HIGH : LOW ) );
    Serial.println( "Wellpump force command arrived" );
    sender->setState( state );  // report state back to the Home Assistant
}


void wellVolumeOnCommand( HANumeric number, HANumber *sender )
{
    if( !number.isSet() )
    {
        // the reset command was send by Home Assistant
    }
    else
    {
        uint32_t numberUInt32 = number.toUInt32();
        Serial.printf( "Well volume sum change command arrived: %d\r\n", numberUInt32 );
        wellVolumeSumValue = numberUInt32;
    }

    sender->setState( number );  // report the selected option back to the HA panel
}
void airPressureTargetOnCommand( HANumeric number, HANumber *sender )
{
    int32_t value = number.toInt32();
    if( value < 0 )
    {
        Serial.println( "Air pressure target command arrived: reset" );
        airPressureTargetValue = 0;
    }
    else
    {
        Serial.printf( "Air pressure target command arrived: %d\r\n", value );
        airPressureTargetValue = value;
    }
    sender->setState( value );  // report the selected option back to the HA panel
}
void airPressureHysteresisOnCommand( HANumeric number, HANumber *sender )
{
    int32_t value = number.toInt32();
    if( value < 0 )
    {
        Serial.println( "Air pressure hysteresis command arrived: reset" );
        airPressureHysteresisValue = 0;
    }
    else
    {
        Serial.printf( "Air pressure hysteresis command arrived: %d\r\n", value );
        airPressureHysteresisValue = value;
    }
    sender->setState( value );  // report the selected option back to the HA panel
}

const uint8_t pageCount     = 3;
uint8_t pageCntr            = 0;
uint32_t pageSwitchLastTick = 0;
uint32_t pageSwitchDelay    = 2000;
uint32_t displayLastTick    = 0;
uint32_t displayDelay       = 100;

void displayPrint()
{
    // Serial.printf( "Display print:%d ,(%d)\r\n", pageCntr, millis() );
    char buf1[50];
    char buf2[50];
    char buf3[50];
    char buf4[50];

    if( millis() > ( pageSwitchLastTick + pageSwitchDelay ) )
    {
        // if( ( ++pageCntr ) == pageCount )
        {
            pageCntr = 1;
        }
        Serial.printf( "Display switching to page %d\r\n", pageCntr );
        pageSwitchLastTick = millis();
    }
    display.clear();

    switch( pageCntr )
    {
        case 0:
            /* code */
            display.drawString( 0, 0, "Kút vízszint:" );
            display.drawString( 0, 16, String( wellLevelValue ) + " cm" );
            display.drawString( 0, 32, "Kút vízmennyiség: " );
            display.drawString( 0, 48, String( wellVolumeSumValue ) + " l" );
            break;

        case 1:
            display.drawString( 0, 0, "Tartály vízszint: " );
            display.drawString( 0, 16, String( TankDistanceValue ) + " mm" );
            display.drawString( 0, 32, "Kivett vízmennyiség:" );
            display.drawString( 0, 48, String( irrigationVolume.getCurrentValue().toUInt32() ) + " l" );

            break;
        case 2:
            display.drawString( 0, 0, "Levegőnyomás: " );
            display.drawString( 0, 16, String( (float)airPressureValue / ( 1000.0 ) ) + " bar" );
            display.drawString( 0, 32, "Gyors levegőnyomás: " );
            display.drawString( 0, 48, String( (float)wellLevelValue ) + " cm" );
            break;

        default:
            break;
    }
    display.display();
}
void setup()
{
    Serial.begin( 115200 );

    Serial.println( "Starting..." );

    display.init();

    display.flipScreenVertically();
    display.setFont( ArialMT_Plain_16 );
    display.clear();


    pinMode( LED_PIN, OUTPUT );

    pinMode( IRRIGATION_PUMP_OUT_PIN, OUTPUT );
    pinMode( WELL_WATERMETER_IN_PIN, INPUT_PULLUP );
    pinMode( WELL_PUMP_OUT_PIN, OUTPUT );
    pinMode( COMPRESSOR_OUT_PIN, OUTPUT );
    pinMode( IRRIGATION_PUMP_OUT_PIN, OUTPUT );
    pinMode( WELL_VALVE_OUT_PIN, OUTPUT );
    pinMode( IRRIGATION_WATERMETER_IN_PIN, INPUT );
    pinMode( RESERVED_IN_PIN, INPUT );
    pinMode( BTN1_IN_PIN, INPUT_PULLUP );
    pinMode( BTN2_IN_PIN, INPUT_PULLUP );
    pinMode( BTN3_IN_PIN, INPUT_PULLUP );

    pinMode( A4, ANALOG );
    pinMode( A7, ANALOG );
    pinMode( A6, ANALOG );
    pinMode( A3, ANALOG );
    pinMode( A0, ANALOG );


    byte mac[6];
    WiFi.macAddress( mac );
    WiFi.setHostname( "Kutvezerlo_ESP32" );
    WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
    while( WiFi.status() != WL_CONNECTED )
    {
        Serial.print( "." );
        delay( 500 );  // waiting for the connection
    }
    device.setUniqueId( mac, sizeof( mac ) );
    device.setName( "Kútvezérlő v2" );
    device.setSoftwareVersion( "2.0.7" );


    Serial.println();
    Serial.println( "Connected to the network" );
    Serial.println( "IP address: " );
    Serial.println( WiFi.localIP() );

    led.setName( "Panel LED" );
    led.onCommand( ledSwitchCommand );
    led.setIcon( "mdi:lightbulb" );
    delay( 10 );

    wellWaterMeter.begin();
    irrigationBackpumpMeter.begin();

    compressor.setName( "wellCompressor" );
    compressor.setIcon( "mdi:piston" );

    airPressureValue = 0;
    airPressureRaw.setName( "Air pressure raw" );
    airPressureRaw.setIcon( "mdi:gauge" );

    airPressureActual.setName( "Air pressure Actual" );
    airPressureActual.setIcon( "mdi:gauge" );
    airPressureActual.setUnitOfMeasurement( "bar" );

    airPressureTarget.setName( "Air pressure Target" );
    airPressureTarget.setIcon( "mdi:gauge" );
    airPressureTarget.setUnitOfMeasurement( "bar" );
    airPressureTarget.setMin( 0 );
    airPressureTarget.setMax( 100000 );
    airPressureTarget.onCommand( airPressureTargetOnCommand );
    airPressureTarget.setRetain( true );

    airPressureHystersis.setName( "Air pressure Hystersis" );
    airPressureHystersis.setIcon( "mdi:gauge" );
    airPressureHystersis.setUnitOfMeasurement( "bar" );
    airPressureHystersis.setMin( 0 );
    airPressureHystersis.setMax( 100000 );
    airPressureHystersis.onCommand( airPressureHysteresisOnCommand );
    airPressureHystersis.setRetain( true );

    wellLevelRaw.setName( "Well level raw" );
    wellLevelRaw.setIcon( "mdi:ArrowExpandVertical" );
    wellLevel.setName( "Well level" );
    wellLevel.setUnitOfMeasurement( "cm" );
    wellLevel.setIcon( "mdi:ArrowExpandVertical" );

    wellPump.setName( "wellPump" );
    wellPump.setIcon( "mdi:water-pump" );

    wellVolume.setName( "Well volume" );
    wellVolume.setUnitOfMeasurement( "l" );

    wellVolumeSum.setName( "Well volume sum" );
    wellVolumeSum.setUnitOfMeasurement( "l" );
    wellVolumeSum.setIcon( "mdi:water" );
    wellVolumeSum.setRetain( true );
    wellVolumeSum.onCommand( wellVolumeOnCommand );

    TankDistance.setName( "Tank distance" );
    TankDistance.setUnitOfMeasurement( "mm" );

    irrigationPump.setName( "wateringPump" );
    irrigationPump.onCommand( irrigationSwitchCommand );

    irrigationVolume.setName( "watering volume" );
    irrigationVolume.setUnitOfMeasurement( "l" );

    backVolume.setName( "Back volume" );
    backVolume.setUnitOfMeasurement( "l" );

    backPump.setName( "wateringBackpump" );
    valve.setName( "backpumpingValve" );
    valve.setIcon( "mdi:valve" );


    mqtt.onConnected( onMqttConnected );
    mqtt.onMessage( onMqttMessage );

    delay( 10 );
    mqtt.begin( BROKER_ADDR, "HA", "homeassistant" );
    delay( 10 );
    Serial.print( "mqtt begin initiated...\r\n" );
    delay( 10 );

    distanceBegin();
    Serial.println( "Sensors added" );
    ArduinoOTA.setHostname( "Kutvezerlo_ESP32" );
    {
        ArduinoOTA
            .onStart( []() {
                String type;
                if( ArduinoOTA.getCommand() == U_FLASH )
                {
                    type = "sketch";
                }
                else
                {  // U_SPIFFS
                    type = "filesystem";
                }

                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
                // using SPIFFS.end()
                Serial.println( "Start updating " + type );
            } )
            .onEnd( []() { Serial.println( "\nEnd" ); } )
            .onProgress( []( unsigned int progress, unsigned int total ) {
                Serial.printf( "Progress: %u%%\r", ( progress / ( total / 100 ) ) );
            } )
            .onError( []( ota_error_t error ) {
                Serial.printf( "Error[%u]: ", error );
                if( error == OTA_AUTH_ERROR )
                {
                    Serial.println( "Auth Failed" );
                }
                else if( error == OTA_BEGIN_ERROR )
                {
                    Serial.println( "Begin Failed" );
                }
                else if( error == OTA_CONNECT_ERROR )
                {
                    Serial.println( "Connect Failed" );
                }
                else if( error == OTA_RECEIVE_ERROR )
                {
                    Serial.println( "Receive Failed" );
                }
                else if( error == OTA_END_ERROR )
                {
                    Serial.println( "End Failed" );
                }
            } );
    }
    Serial.println( "OTA registered" );
    ArduinoOTA.begin();
    led.setState( 0 );

    adc_power_acquire();

    analogSetClockDiv( 200 );

    analogReadResolution( 12 );
    analogSetAttenuation( ADC_11db );
    button1.setDebounceTime( 50 );  // set debounce time to 50 milliseconds
    button2.setDebounceTime( 50 );  // set debounce time to 50 milliseconds
    button3.setDebounceTime( 50 );  // set debounce time to 50 milliseconds

    wellWaterMeter.begin();
    irrigationBackpumpMeter.begin();
    Serial.println( "Setup done" );
}
bool distanceWait = false;
// The loop function is called in an endless loop
void loop()
{
    button1.loop();
    button2.loop();
    button3.loop();

    if( button1.isPressed() )
    {
        irrigationPumpState = !irrigationPumpState;
        digitalWrite( IRRIGATION_PUMP_OUT_PIN, ( irrigationPumpState ? HIGH : LOW ) );
        irrigationPump.setState( irrigationPumpState );
        Serial.printf( "Irrigation pump state changed: %d\r\n", irrigationPumpState );
    }
    if( ( millis() > ( distanceLastUpdateAt + distanceInterval ) ) && !distanceWait )
    {
        Serial.println( "Get distance" );

        getDistance();
        distanceWait = true;
    }


    if( millis() > ( distanceLastUpdateAt + 25 + distanceInterval ) )
    {
        TankDistanceValue = readDistance();
        TankDistance.setValue( TankDistanceValue );
        distanceLastUpdateAt = millis();
        distanceWait         = 0;
        Serial.println( "Tank distance: " + String( TankDistanceValue ) + " mm" );
    }

    if( millis() > ( flowLastUpdateAt + flowUpdateInterval ) )
    {
        flowLastUpdateAt = millis();
        // Serial.println( "Flow update" );

        wellVolume.setValue( wellWaterMeter.readAndReset() );
        irrigationVolume.setValue( irrigationBackpumpMeter.readAndResetIrrigation() );
        backVolume.setValue( irrigationBackpumpMeter.readAndResetBackpump() );
    }


    if( millis() > ( AnalogLastUpdateAt + AnalogUpdateInterval ) )
    {
        // Serial.println( "Analog update" );
        AnalogLastUpdateAt = millis();

        airPressureRawValue       = airPressureFilter.doFilter( analogRead( COMPRESSOR_PRESSURE_AIN_PIN ) );
        airPressureValue          = adc_to_bar( airPressureRawValue );
        airPressureValueUnfilterd = adc_to_bar( wellLevelFilter.doFilter( analogRead( WELL_PRESSURE_AIN_PIN ) ) );
        airPressureActual.setValue( (float)( airPressureValue / 1000.0 ) );
        airPressureRaw.setValue( airPressureRawValue );
        // Serial.printf( "Air pressure raw: %d, value: %f\r\n", airPressureRawValue, (float)airPressureValue );

        wellLevelRawValue = wellLevelFilter.doFilter( analogRead( WELL_PRESSURE_AIN_PIN ) );
        wellLevelValue    = adc_to_cmH2O( wellLevelRawValue );
        wellLevelRaw.setValue( wellLevelRawValue );
        wellLevel.setValue( wellLevelValue );

        // Serial.printf( "Well level raw: %d, value: %d cm\r\n", wellLevelRawValue, wellLevelValue );
        //  compressor.setState( digitalRead( WELL_COMPRESSOR_IN_PIN ) );
        //
    }
    if( millis() > ( ControlLastUpdateAt + ControlUpdateInterval ) )
    {
        ControlLastUpdateAt = millis();
        // Serial.println( "Control update Target: " + String( airPressureTargetValue ) +
        //          ", Hysteresis: " + String( airPressureHysteresisValue ) );
        if( airPressureValue < ( airPressureTargetValue - airPressureHysteresisValue ) )
        {
            digitalWrite( COMPRESSOR_OUT_PIN, HIGH );
            compressorState = true;
            compressor.setState( true );
            //   Serial.println( "Compressor ON" );
        }
        else if( airPressureValue > ( airPressureTargetValue + airPressureHysteresisValue ) )
        {
            digitalWrite( COMPRESSOR_OUT_PIN, LOW );
            compressorState = false;
            compressor.setState( false );
            // Serial.println( "Compressor OFF" );
        }
    }

    if( millis() > ( displayLastTick + displayDelay ) )
    {
        displayLastTick = millis();
        displayPrint();
    }

    mqtt.loop();
    ArduinoOTA.handle();
}
