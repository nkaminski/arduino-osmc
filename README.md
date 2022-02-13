# OSMC - A library to drive H-bridge motor controllers

## Compatible motor controllers
* Robot Power OSMC
* Toshiba TBB6612FNG

## API
```
    // (OSMC) Associates an OSMC with an instance of this class
        void attach(byte enable, byte ali, byte bli, byte ahi, byte bhi)
    
    // (TBB6612FNG) Associates one half of a Toshiba TBB6612FNG with an instance of this class
    // Enable/standby pin usage is optional
        void attach(byte in1, byte in2, byte pwmin);
        void attach(byte enable, byte in1, byte in2, byte pwmin);

    // (all) Sets whether or not the low side FETs are turned on when the power is set to zero to provide braking force.
        void setBrake(byte enabled)

    // (all) Enables output to the motor. If this has not been set to nonzero, the motor will unconditionally be free-spinning.
    // MUST not be enabled before OSMCs are powered up unless damage may occur.
        void setEnabled(byte enabled)

    // (all) Sets the motor output power. Valid range is -255 to 255.
        void setPower(int power)
    
    // (all) Returns nonzero if this class instance is associated with a motor (has output pins assigned)
        byte attached()

    // (all) Disassociates this class instance from the motor, putting all outputs into high-Z state
        void detach();
```