#include "osmc.h"


void OSMC::attach(byte enable, byte ali, byte bli, byte ahi, byte bhi)
{
    // Full H - HIP4081APIZ - OSMC (http://www.robotpower.com/products/osmc_info.html)
    // Enable *MUST* be used otherwise damage can occur,
    // see https://www.renesas.com/us/en/document/oth/tb321-hip4080-and-hip4081-high-frequency-h-bridge-drivers
    enablePin = enable;
    ahiPin = ahi;
    aliPin = ali;
    bhiPin = bhi;
    bliPin = bli;
    init();
}

void TBB6612FNG::attach(byte enable, byte in1, byte in2, byte pwmin){
    // H with internal control logic - TBB6612FNG - Sparkfun Dual Motor Driver (https://www.sparkfun.com/products/14451)
    enablePin = enable;
    ahiPin = in1;
    bhiPin = in2;
    aliPin = pwmin;
    bliPin = pwmin;
    init();
}

void TBB6612FNG::attach(byte in1, byte in2, byte pwmin){
    // Same as above without enable/disable
    enablePin = 0xFF;
    ahiPin = in1;
    bhiPin = in2;
    aliPin = pwmin;
    bliPin = pwmin;
    init();
}

void OSMC::init(){
    pwm=0;
    reverseDirection=0;
    brake=0;
    outputEnabled=0;

    // If enable/disable is configured, start with output disabled
    if (enablePin != 0xFF){
        digitalWrite(enablePin, 0);
        pinMode(enablePin, OUTPUT);
    }
    // Set all pins to output low, *order is important*
    digitalWrite(ahiPin, 0);
    digitalWrite(aliPin, 0);
    digitalWrite(bhiPin, 0);
    digitalWrite(bliPin, 0);

    pinMode(ahiPin, OUTPUT);
    pinMode(aliPin, OUTPUT);
    pinMode(bhiPin, OUTPUT);
    pinMode(bliPin, OUTPUT);
}

bool OSMC::attached(){
    return (ahiPin != 0xFF) &&
           (aliPin != 0xFF) &&
           (bhiPin != 0xFF) &&
           (bliPin != 0xFF);
}

void OSMC::detach(){
    if(!attached())
        return;
    // Set all pins to high Z
    digitalWrite(ahiPin, 0);
    digitalWrite(aliPin, 0);
    digitalWrite(bhiPin, 0);
    digitalWrite(bliPin, 0);

    pinMode(ahiPin, INPUT);
    pinMode(aliPin, INPUT);
    pinMode(bhiPin, INPUT);
    pinMode(bliPin, INPUT);

    // Reset all pin member variables
    enablePin=0xFF;
    aliPin=0xFF;
    bliPin=0xFF;
    ahiPin=0xFF;
    bhiPin=0xFF;
}

void OSMC::setBrake(byte enabled)
{
    if (brake != enabled)
    {
        brake = enabled;
        driveOutput();
    }
}

void OSMC::setEnabled(byte enabled)
{
    if (enabled != outputEnabled)
    {
        outputEnabled = enabled;
        driveOutput();
    }
}

void OSMC::setPower(int power)
{
    reverseDirection = (power < 0);
    pwm = constrain(abs(power), -255, 255);
    driveOutput();
}

void OSMC::setPower(byte power, bool reverse){
    reverseDirection = reverse;
    pwm = power;
    driveOutput();
}

// Hack to avoid virtual functions...
void OSMC::driveOutput(void){
    if(!driveOutputCommon())
        return;
    if(outputEnabled)
        driveOutputHardware();
}

bool OSMC::driveOutputCommon(void)
{
    // Don't change any hardware state if not attached
    if(!attached())
        return false;

    if(enablePin != 0xFF){
        digitalWrite(enablePin, outputEnabled);

    }
    if(!outputEnabled){
        // Turn all outputs off unconditionally
        digitalWrite(ahiPin, LOW);
        digitalWrite(aliPin, LOW);
        digitalWrite(bhiPin, LOW);
        digitalWrite(bliPin, LOW);
    }
    return true;
}

void OSMC::driveOutputHardware(void){
    if (pwm == 0)
    {
        // High side of H OFF (this is the slower operation)
        digitalWrite(ahiPin, LOW);
        digitalWrite(bhiPin, LOW);
        if (brake)
        {
            // Apply braking by turning on both low side FETs
            // 1uS anti-shootthrough delays
            delayMicroseconds(1);
            digitalWrite(aliPin, HIGH);
            digitalWrite(bliPin, HIGH);
        }
        else
        {
            // Just turn all legs of the H off
            digitalWrite(aliPin, LOW);
            digitalWrite(bliPin, LOW);
        }
    }
    else if (reverseDirection)
    {
        // Current flows B to A side
        digitalWrite(ahiPin, LOW);
        digitalWrite(bliPin, LOW);
        delayMicroseconds(1);
        digitalWrite(bhiPin, HIGH);
        analogWrite(aliPin, pwm);
    }
    else
    {
        // Current flows A to B, this is the "positive" direction
        digitalWrite(bhiPin, LOW);
        digitalWrite(aliPin, LOW);
        delayMicroseconds(1);
        digitalWrite(ahiPin, HIGH);
        analogWrite(bliPin, pwm);
    }
}

void TBB6612FNG::driveOutputHardware(void){
    if ((pwm == 0) && brake)
    {
        digitalWrite(ahiPin, HIGH);
        digitalWrite(bhiPin, HIGH);
        digitalWrite(aliPin, HIGH);
        
    }
    else if(pwm == 0){
        digitalWrite(ahiPin, LOW);
        digitalWrite(bhiPin, LOW);
        digitalWrite(aliPin, HIGH);
    }
    else {
        if (reverseDirection) {
            // Current flows 2 to 1 side
            // All delays are internal to the motor driver IC
            digitalWrite(ahiPin, LOW);
            digitalWrite(bhiPin, HIGH);
        }
        else {
            // Current flows 1 to 2, this is the "positive" direction
            digitalWrite(bhiPin, LOW);
            digitalWrite(ahiPin, HIGH);
        }
        analogWrite(aliPin, pwm);
    }
}