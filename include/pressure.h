#ifndef PRESSURE_H
#define PRESSURE_H

bool setupPressure(void);

void printPressure(void);

class Pressure
{
private:
    float pressure[1000];
    float temp[1000];
    float height[1000];
public:
    float updatePressure(int start, int stop);
    void addToPressure(int ind);
};

#endif