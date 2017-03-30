//
//  gdal_reader.cpp
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#include "gdal_reader.hpp"

gdal_reader::gdal_reader() {
    GDALAllRegister();
}

void gdal_reader::open(const char * filePath) {
    this->dataset = (GDALDataset *)GDALOpen(filePath, GA_ReadOnly);
    double * trans = new double[6];
    
    this->dataset->GetGeoTransform(trans);
    this->transform = new Transform(*trans,*(trans+3),*(trans+1),*(trans+5));
    this->bandCount = this->dataset->GetRasterCount();
    this->rasterXSize = this->dataset->GetRasterXSize();
    this->rasterYSize = this->dataset->GetRasterYSize();
    this->nodata = (short)this->dataset->GetRasterBand(1)->GetNoDataValue();
    delete [] trans;
    trans = 0;
}

double gdal_reader::getPixel(int x, int y) {
    if(x < 0 || y < 0 || x > this->rasterXSize || y > this->rasterYSize) {
        return 0;
    }
    GDALRasterBand * band = this->dataset->GetRasterBand(1);
    double * pData = new double[1];
    band->RasterIO(GF_Read, x, y, 1, 1,pData, 1, 1, GDT_Float64, 0, 0);
    double pixel = *pData;
    delete [] pData;
    pData = 0;
    return pixel != nodata ? pixel : 0;
}

double gdal_reader::interpolateElevation(Point<double> * point) {
    double elevation = -1.0;
    double x = (point->x - this->transform->left)/this->transform->pixelWidth;
    double y = (point->y- this->transform->top)/this->transform->pixelHeight;
    int ix = (int)x;
    int iy = (int)y;
    double px = this->transform->left + this->transform->pixelWidth * ix;
    double py = this->transform->top + this->transform->pixelHeight * iy;
    double dx = point->x - px;
    double dy = point->y - py;
    double xNormalized = dx/this->transform->pixelWidth;
    double yNormalized = dy/this->transform->pixelHeight;
    double z11 = this->getPixel(ix, iy);
    double z21 = this->getPixel(ix + 1, iy);
    double z22 = this->getPixel(ix + 1, iy + 1);
    double z12 = this->getPixel(ix, iy + 1);
    elevation = z21 + xNormalized*(z22 - z21) + yNormalized*(z11-z21) + xNormalized*yNormalized*(z21-z11 + z12 - z22);
    return elevation;
}

Point<double> * gdal_reader::getPoint(Point<int> * point) {
    Point<double> * p = new Point<double>();
    p->x = this->transform->left + this->transform->pixelWidth * point->x;
    p->y = this->transform->top + this->transform->pixelHeight * point->y;
    return p;
}

Point<int> * gdal_reader::getCell(Point<double> * point) {
    Point<int> * p = new Point<int>();
    double x = (point->x - this->transform->left)/this->transform->pixelWidth;
    double y = (point->y - this->transform->top)/this->transform->pixelHeight;
    p->x = int(x+0.5);
    p->y = int(y + 0.5);
    return p;
}

Point<int> * gdal_reader::getRealCell(Point<double> * point) {
    Point<int> * p = new Point<int>();
    double x = (point->x - this->transform->left)/this->transform->pixelWidth;
    double y = (point->y - this->transform->top)/this->transform->pixelHeight;
    p->x = int(x);
    p->y = int(y);
    return p;
}

double gdal_reader::getPixelWidth() {
    return this->transform->pixelWidth;
}

double gdal_reader::getPixelHeight() {
    return this->transform->pixelHeight;
}

double gdal_reader::getWidth() {
    return 30.0;
}

double gdal_reader::getHeight() {
    return 30.0;
}

gdal_reader::~gdal_reader() {
    GDALClose(this->dataset);
    //delete this->dataset;
    this->dataset = 0;
    delete this->transform;
    this->transform = 0;
}
