//
//  gdal_reader.hpp
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#ifndef gdal_reader_hpp
#define gdal_reader_hpp

#pragma once

#include <stdio.h>
#include <gdal_priv.h>
#include "geometry.h"

struct Transform {
    double left;
    double top;
    double pixelWidth;
    double pixelHeight;
    Transform(double left,double top,double pixelWidth,double pixelHeight)
    :left(left),top(top),pixelWidth(pixelWidth),pixelHeight(pixelHeight) {
    
    }
};

class gdal_reader {

public:
    gdal_reader();
    ~gdal_reader();
public:
    void open(const char * filePath);
    double getPixel(int x, int y);
    double interpolateElevation(Point<double> * point);
    Point<double> * getPoint(Point<int> * point);
    Point<int> * getCell(Point<double> * point);
    Point<int> * getRealCell(Point<double> * point);
    double getPixelWidth();
    double getPixelHeight();
    double getWidth();
    double getHeight();
private:

private:
    GDALDataset * dataset;
    Transform * transform;
    int bandCount;
    int rasterXSize;
    int rasterYSize;
    short nodata;
};

#endif /* gdal_reader_hpp */
