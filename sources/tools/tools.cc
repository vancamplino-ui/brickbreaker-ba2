#include "tools.h"


#include "tools.h"

Point::Point()
    : x(0.0), y(0.0)
{
}

Point::Point(double x_, double y_)
    : x(x_), y(y_)
{
}

double Point::getX() const
{
    return x;
}

double Point::getY() const
{
    return y;
}

Circle::Circle()
    : center(), radius(0.0)
{
}

Circle::Circle(Point center_, double radius_)
    : center(center_), radius(radius_)
{
}

Point Circle::getCenter() const
{
    return center;
}

double Circle::getRadius() const
{
    return radius;
}

Square::Square()
    : center(), half_size(0.0)
{
}

Square::Square(Point center_, double half_size_)
    : center(center_), half_size(half_size_)
{
}

Point Square::getCenter() const
{
    return center;
}

double Square::getHalfSize() const
{
    return half_size;
}