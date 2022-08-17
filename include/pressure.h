#ifndef PRESSURE_H
#define PRESSURE_H

bool setupPressure(void);

class Pressure
{
private:
    float pressure;
public:
    
    float getHeight(void);
    float getPressure(void);
};

#endif