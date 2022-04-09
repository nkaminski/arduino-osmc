#ifndef osmc_h
#define osmc_h

#include "Arduino.h"

/* OSMC (or other similar H-bridge) motor controller driver
 *
 * Usage details:
 * OSMC ALI and BLI are the low side driver inputs and must ALWAYS be low/zero when the ready signal is not provided
 * OSMC AHI and BHI are the high side driver inputs.

 * ----------- Vdd
 *   |     |
 *   AHI   BHI
 *   |     |
 *   ---M---
 *   |     |
 *   ALI   BLI
 *   |     |
 *   --------- GND
 */

class OSMC {
    public:
        void attach(byte enable, byte ali, byte bli, byte ahi, byte bhi);
        void setBrake(bool enabled);
        void setEnabled(bool enabled);
        void setPower(byte power, bool reverse);
        void setRampEnabled(bool enable);
        void doRamp(byte units);
        bool attached();
        void detach();
    protected:
        void init();
        virtual void driveOutputHardware();
        bool driveOutputCommon();
        byte pwm, reverseDirection, brake, outputEnabled;
        byte enablePin=0xFF, aliPin=0xFF, bliPin=0xFF, ahiPin=0xFF, bhiPin=0xFF;
    private:
        byte rampTarget=0;
        bool rampEnabled=false;
        void driveOutput();
};

class TBB6612FNG : public OSMC {
    public:
        void attach(byte in1, byte in2, byte pwmin);
        void attach(byte enable, byte in1, byte in2, byte pwmin);
    private:
        void driveOutputHardware();
};
#endif