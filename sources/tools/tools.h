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


#ifndef TOOLS_H
#define TOOLS_H

constexpr double epsil_zero = 0.125;

class Point
{
public:
    Point();
    Point(double x_, double y_);

    double getX() const;
    double getY() const;

private:
    double x;
    double y;
};

class Circle
{
public:
    Circle();
    Circle(Point center_, double radius_);

    Point getCenter() const;
    double getRadius() const;

private:
    Point center;
    double radius;
};

class Square
{
public:
    Square();
    Square(Point center_, double half_size_);

    Point getCenter() const;
    double getHalfSize() const;

private:
    Point center;
    double half_size;
};

#endif