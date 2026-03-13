#ifndef TOOLS_H
#define TOOLS_H

constexpr double epsil_zero = 0.125;

struct Point
{
    double x;
    double y;
};

struct Circle
{
    Point center;
    double radius;
};

struct Square
{
    Point center;
    double half_size;
};

#endif

