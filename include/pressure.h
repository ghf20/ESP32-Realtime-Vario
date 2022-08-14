#ifndef PRESSURE_H
#define PRESSURE_H

bool setupPressure(void);

class Pressure
{
private:
    float pressure;
public:
    
    float getPressure(void);
};

#endif