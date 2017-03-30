//
//  calculator_engine.hpp
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#ifndef calculator_engine_hpp
#define calculator_engine_hpp

#pragma once

#include <stdio.h>
#include "gdal_reader.hpp"

class calculator_engine {
public:
    calculator_engine(gdal_reader * gr);
    ~calculator_engine();
public:
    double area(Polygon<double> * polygon);
    double areaEdge(Polygon<double> *polygon);
private:
    double area(Polygon<double> * polygon, Point<int> * start, Point<int> * end);
    double area(double a, double b, double c, double d, double width, double height);
    double area(double a, double b, double c);
    double area(Polygon<double> * polygon, Point<double> * start, Point<double> * end);
    double area(Point3D<double> * a,Point3D<double> * b,Point3D<double> * c,Point3D<double> *d);
    double interpolateElevation(Point<double> * point);
private:
    gdal_reader * gr;
};

#endif /* calculator_engine_hpp */
