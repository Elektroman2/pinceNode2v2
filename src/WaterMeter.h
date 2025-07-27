#pragma once

#include <Arduino.h>

class WaterMeter
{
  public:
    WaterMeter( uint8_t pin, uint32_t interval = 200 )
        : pin_( pin ), count_( 0 ), lastCountAt_( 0 ), countInterval_( interval )
    {
        instance_ = this;
    }
    static WaterMeter *instance_;

    static void handleInterruptStatic()
    {
        if( instance_ )
            instance_->handleInterrupt();
    }


    void begin()
    {
        pinMode( pin_, INPUT_PULLUP );
        attachInterrupt( digitalPinToInterrupt( pin_ ), handleInterruptStatic, CHANGE );
    }
    void handleInterrupt()
    {
        if( millis() > lastCountAt_ + countInterval_ )
        {
            if( digitalRead( pin_ ) == LOW )
            {
                count_++;
            }
            lastCountAt_ = millis();
            Serial.println( "Interrupt detected on well water meter, count: " + String( count_ ) );
        }
    }

    uint32_t readAndReset()
    {
        uint32_t temp = count_;
        count_        = 0;
        return temp;
    }

    uint32_t getCount() const { return count_; }

  private:
    uint8_t pin_;
    volatile uint32_t count_;
    volatile uint32_t lastCountAt_;
    uint32_t countInterval_;
};

// For the irrigation/backpumping meter:
class DualPurposeWaterMeter
{
  public:
    DualPurposeWaterMeter( uint8_t meterPin, uint8_t modePin, uint32_t interval = 200 )
        : meterPin_( meterPin ), modePin_( modePin ), irrigationCount_( 0 ), backpumpCount_( 0 ), lastCountAt_( 0 ),
          countInterval_( interval )
    {
        instance_ = this;
    }

    static DualPurposeWaterMeter *instance_;

    static void handleInterruptStatic()
    {
        if( instance_ )
            instance_->handleInterrupt();
    }

    void handleInterrupt()
    {
        if( millis() > lastCountAt_ + countInterval_ )
        {
            if( digitalRead( meterPin_ ) == LOW )
            {
                if( digitalRead( modePin_ ) )
                {
                    backpumpCount_++;
                    Serial.println( "Interrupt detected on backpump meter, count: " + String( backpumpCount_ ) );
                }
                else
                {
                    irrigationCount_++;
                    Serial.println( "Interrupt detected on irrigation meter, count: " + String( irrigationCount_ ) );
                }
            }
            lastCountAt_ = millis();
        }
    }

    void begin()
    {
        attachInterrupt( digitalPinToInterrupt( meterPin_ ), handleInterruptStatic, CHANGE );
        pinMode( meterPin_, INPUT );
        pinMode( modePin_, INPUT );
    }

    uint32_t readAndResetIrrigation()
    {
        uint32_t temp    = irrigationCount_;
        irrigationCount_ = 0;
        return temp;
    }

    uint32_t readAndResetBackpump()
    {
        uint32_t temp  = backpumpCount_;
        backpumpCount_ = 0;
        return temp;
    }

  private:
    uint8_t meterPin_;
    uint8_t modePin_;
    volatile uint32_t irrigationCount_;
    volatile uint32_t backpumpCount_;
    volatile uint32_t lastCountAt_;
    uint32_t countInterval_;
};

inline DualPurposeWaterMeter *DualPurposeWaterMeter::instance_ = nullptr;
inline WaterMeter *WaterMeter::instance_                       = nullptr;