//
//  main.cpp
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#include <iostream>
#include "calculator_engine.hpp"

int main(int argc, const char * argv[]) {
    gdal_reader gr;
    gr.open("//Users//wongjimsan//Documents//tif//srtm_01_02.img");
    calculator_engine ce(&gr);
    Point<double> point1(-176.796,52.081); 
    Point<double> point2(-176.388,51.925);
    Point<double> point3(-176.478,51.675);
    Point<double> point4(-176.671,51.653);
    Point<double> point5(-176.918,51.816);
    Ring<double> ring;
    ring.add(&point1);
    ring.add(&point2);
    ring.add(&point3);
    ring.add(&point4);
    ring.add(&point5);
    ring.add(&point1);
    Polygon<double> polygon(&ring);
    double areaGrid = ce.area(&polygon);
    double areaEdge = ce.areaEdge(&polygon);
    gr.interpolateElevation(&point3);
    std::cout<<"GridArea:"<<areaGrid<<"\n";
    std::cout<<"EdgeArea:"<<areaEdge<<"\n";
    std::cout<<"Area3D:"<<areaGrid + areaEdge<<"\n";
    return 0;
}


//Geometry
// -176.661,51.921
// -176.521,51.889
// -176.538,51.798
// -176.579,51.768
// -176.665,51.808
//
