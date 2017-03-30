//
//  calculator_engine.cpp
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#include "calculator_engine.hpp"

calculator_engine::calculator_engine(gdal_reader * gr):gr(gr) {
    
}

calculator_engine::~calculator_engine() {
    
}

double calculator_engine::area(Polygon<double> * polygon) {
    BoundingBox<double> * bounds = polygon->getBoundingBox();
    Point<double> p1(bounds->minx,bounds->miny);
    Point<double> p2(bounds->maxx,bounds->maxy);
    Point<int> * pt1 = this->gr->getCell(&p1);
    Point<int> * pt2 = this->gr->getCell(&p2);
    Point<int> start(MIN(pt1->x,pt2->x)-1,MIN(pt1->y,pt2->y)-1);
    Point<int> end(MAX(pt1->x,pt2->x)+1,MAX(pt1->y,pt2->y)+1);
    delete pt1;
    pt1 = 0;
    delete pt2;
    pt2 = 0;
    delete bounds;
    bounds = 0;
    double gridArea = this->area(polygon, &start, &end);
    return gridArea;
}

double calculator_engine::areaEdge(Polygon<double> * polygon) {
    double edgeArea = 0.0;
    auto itor = polygon->exteriorRing->points.cbegin();
    while (itor != polygon->exteriorRing->points.cend()-1) {
        Point<double> * start = *itor;
        itor++;
        Point<double> * end = *itor;
        edgeArea += this->area(polygon, start, end);
    }
    return edgeArea;
}

inline double calculator_engine::area(Polygon<double> * polygon, Point<double> * start, Point<double> * end) {
    double area = 0.0;
    double width = fabs(start->x - end->x);
    if(width != 0) {
        double x = start->x;
        double y = start->y;
        double height = fabs(start->y - end->y);
        double pw = this->gr->getPixelWidth();
        double ph = this->gr->getPixelHeight();
        int s1 = end->x > start->x ? 1 : -1;
        int s2 = end->y > start->y ? 1 : -1;
        double step = MAX(fabs(height/ph),fabs(width/pw));
        double dx = width/step;
        double dy = height/step;
        int steps = (int)(step + 0.5);
        for(int i=0;i<=steps-1;i++) {
            Point<double> p1(x+i*s1*dx,y+i*s2*dy);
            Point<double> p2(x+(i+1)*s1*dx,y+(i+1)*s2*dy);
            bool pt = polygon->contains(&p1) && polygon->contains(&p2);
            if(!pt) continue;
            Point<int> * pixel1 = this->gr->getRealCell(&p1);
            Point<int> * pixel2 = this->gr->getRealCell(&p2);
            int offsetx = pixel2->x - pixel1->x;
            int offsety = pixel2->y - pixel1->y;
            int minx = MIN(pixel1->x,pixel2->x);
            int miny =MIN(pixel1->y,pixel2->y);
            Point<int> p11(minx,miny);
            Point<double> * point11 = this->gr->getPoint(&p11);
            Point<int> p12(minx,miny+1);
            Point<double> * point12 = this->gr->getPoint(&p12);
            Point<int> p22(minx+1,miny+1);
            Point<double> * point22 = this->gr->getPoint(&p22);
            Point<int> p21(minx+1,miny);
            Point<double> * point21 = this->gr->getPoint(&p21);
            double zp1 = this->gr->interpolateElevation(&p1);
            double zp2 = this->gr->interpolateElevation(&p2);
            if(abs(offsetx) ==1&&abs(offsety)==1){
                Point<int> p13(minx,miny+2);
                Point<double> * point13 = this->gr->getPoint(&p13);
                Point<int> p23(minx+1,miny+2);
                Point<double> * point23 = this->gr->getPoint(&p23);
                Point<int> p31(minx+2,miny);
                Point<double> * point31 = this->gr->getPoint(&p31);
                Point<int> p32(minx+2,miny+1);
                Point<double> * point32 = this->gr->getPoint(&p32);
                if(polygon->contains(point11) && polygon->contains(point21) && polygon->contains(point12)) {
                    //p11 p1 p2 p21
                    double z11 = this->gr->getPixel(minx, miny);
                    double z21 = this->gr->getPixel(minx+1, miny);
                    Point3D<double> p113D(point11,z11);
                    Point3D<double> p13D(&p1,zp1);
                    Point3D<double> p23D(&p2,zp2);
                    Point3D<double> p213D(point21,z21);
                    area += this->area(&p113D,&p13D,&p23D,&p213D);
                } else if(polygon->contains(point22) && polygon->contains(point13) && polygon->contains(point23)) {
                    //p1 p13 p23 p2
                    double z13 = this->gr->getPixel(minx, miny+2);
                    double z23 = this->gr->getPixel(minx+1, miny+2);
                    Point3D<double> p13D(&p1,zp1);
                    Point3D<double> p133D(point13,z13);
                    Point3D<double> p233D(point23,z23);
                    Point3D<double> p23D(&p2,zp2);
                    area += this->area(&p13D,&p133D,&p233D,&p23D);
                } else if(polygon->contains(point11) && polygon->contains(point12) && polygon->contains(point22)) {
                    //p11 p1 p2 p12
                    double z11 = this->gr->getPixel(minx, miny);
                    double z12 = this->gr->getPixel(minx, miny+1);
                    Point3D<double> p113D(point11,z11);
                    Point3D<double> p13D(&p1,zp1);
                    Point3D<double> p23D(&p2,zp2);
                    Point3D<double> p123D(point12,z12);
                    area += this->area(&p113D,&p13D,&p23D,&p123D);
                } else if(polygon->contains(point21) && polygon->contains(point31) && polygon->contains(point32)) {
                    //p1 p31 p32 p2
                    double z31 = this->gr->getPixel(minx+2, miny);
                    double z32 = this->gr->getPixel(minx+2, miny+1);
                    Point3D<double> p13D(&p1,zp1);
                    Point3D<double> p313D(point31,z31);
                    Point3D<double> p323D(point32,z32);
                    Point3D<double> p23D(&p2,zp2);
                    area += this->area(&p13D,&p313D,&p323D,&p23D);
                }
            } else {
                if(offsetx == 0) {
                    if(polygon->contains(point11)&&polygon->contains(point12)) {
                        //p11 p1 p2 p12
                        double z11 = this->gr->getPixel(minx, miny);
                        double z12 = this->gr->getPixel(minx, miny+1);
                        Point3D<double> p113D(point11,z11);
                        Point3D<double> p13D(&p1,zp1);
                        Point3D<double> p23D(&p2,zp2);
                        Point3D<double> p123D(point12,z12);
                        area += this->area(&p113D,&p13D,&p23D,&p123D);
                    } else if(polygon->contains(point21)&&polygon->contains(point22)) {
                        //p1 p21 p22 p2
                        double z21 = this->gr->getPixel(minx+1, miny);
                        double z22 = this->gr->getPixel(minx+1, miny+1);
                        Point3D<double> p13D(&p1,zp1);
                        Point3D<double> p213D(point21,z21);
                        Point3D<double> p223D(point22,z22);
                        Point3D<double> p23D(&p2,zp2);
                        area += this->area(&p13D,&p213D,&p223D,&p23D);
                    }
                } else if (offsety == 0) {
                    if(polygon->contains(point11)&&polygon->contains(point21)) {
                        //p11 p1 p2 p21
                        double z11 = this->gr->getPixel(minx, miny);
                        double z21 = this->gr->getPixel(minx+1, miny);
                        Point3D<double> p113D(point11,z11);
                        Point3D<double> p13D(&p1,zp1);
                        Point3D<double> p23D(&p2,zp2);
                        Point3D<double> p213D(point21,z21);
                        area += this->area(&p113D,&p13D,&p23D,&p213D);
                        
                    } else if(polygon->contains(point12)&&polygon->contains(point22)) {
                        //p1 p12 p22 p2
                        double z12 = this->gr->getPixel(minx, miny+1);
                        double z22 = this->gr->getPixel(minx+1, miny+1);
                        Point3D<double> p13D(&p1,zp1);
                        Point3D<double> p123D(point12,z12);
                        Point3D<double> p223D(point22,z22);
                        Point3D<double> p23D(&p2,zp2);
                        area += this->area(&p13D,&p123D,&p223D,&p23D);
                    }
                }
            }
        }
    } else {
        double height = fabs(start->y - end->y);
        double ph = this->gr->getPixelHeight();
        int steps = int(fabs(height/ph)+0.5);
        int s1 = end->y > start->y ? 1 : -1;
        
        for ( int i=0;i<=steps-1;i++) {
            Point<double> p1(start->x,start->y + i*s1*ph);
            Point<double> p2(start->x,start->y + (i+1)*s1*ph);
            Point<int> * pixel1 = this->gr->getRealCell(&p1);
            Point<int> p11(pixel1->x,pixel1->y);
            Point<double> * point11 = this->gr->getPoint(&p11);
            Point<int> p12(pixel1->x,pixel1->y+1);
            Point<double> * point12 = this->gr->getPoint(&p12);
            Point<int> p22(pixel1->x+1,pixel1->y+1);
            Point<double> * point22 = this->gr->getPoint(&p22);
            Point<int> p21(pixel1->x+1,pixel1->y);
            Point<double> * point21 = this->gr->getPoint(&p21);
            double zp1 = this->gr->interpolateElevation(&p1);
            double zp2 = this->gr->interpolateElevation(&p2);
            if(polygon->contains(point11) && polygon->contains(point12)) {
                //p11 p12 p2 p1
                double z11 = this->gr->getPixel(pixel1->x,pixel1->y);
                double z12 = this->gr->getPixel(pixel1->x,pixel1->y+1);
                Point3D<double> p113D(point11,z11);
                Point3D<double> p123D(point12,z12);
                Point3D<double> p23D(&p2,zp2);
                Point3D<double> p13D(&p1,zp1);
                area += this->area(&p113D,&p123D,&p23D,&p13D);
            } else if(polygon->contains(point21)&& polygon->contains(point22)) {
                //p1 p22 p21 p2
                double z21 = this->gr->getPixel(pixel1->x+1,pixel1->y);
                double z22 = this->gr->getPixel(pixel1->x+1,pixel1->y+1);
                Point3D<double> p13D(&p1,zp1);
                Point3D<double> p223D(point22,z22);
                Point3D<double> p213D(point21,z21);
                Point3D<double> p23D(&p2,zp2);
                area += this->area(&p13D,&p223D,&p213D,&p23D);
            }
        }
    }
    return area;
}

inline double calculator_engine::area(Point3D<double> * a,Point3D<double> * b,Point3D<double> * c,Point3D<double> *d){
    double area = 0.0;
    double pw = this->gr->getPixelWidth();
    double ph = this->gr->getPixelHeight();
    double width = this->gr->getWidth();
    double height = this->gr->getHeight();
    double ab = sqrt(pow((a->x-b->x)/pw*width, 2)+pow((a->y-b->y)/ph*height,2)+pow(a->z-b->z,2));
    double bc = sqrt(pow((b->x-c->x)/pw*width, 2)+pow((b->y-c->y)/ph*height,2)+pow(b->z-c->z,2));
    double cd = sqrt(pow((c->x-d->x)/pw*width, 2)+pow((c->y-d->y)/ph*height,2)+pow(c->z-d->z,2));
    double da = sqrt(pow((d->x-a->x)/pw*width, 2)+pow((d->y-a->y)/ph*height,2)+pow(d->z-a->z,2));
    double ac = sqrt(pow((a->x-c->x)/pw*width, 2)+pow((a->y-c->y)/ph*height,2)+pow(a->z-c->z,2));
    area += this->area(ab,bc,ac);
    area += this->area(cd,da,ac);
    return area;
}

inline double calculator_engine::area(Polygon<double> * polygon, Point<int> * start, Point<int> * end) {
    Point<double> * p11 = 0;
    Point<double> * p12 = 0;
    Point<double> * p22 = 0;
    Point<double> * p21 = 0;
    double area = 0.0;
    double width = this->gr->getWidth();
    double height = this->gr->getHeight();
    for(int i=start->x;i<=end->x-1;i++) {
        for(int j=start->y;j<=end->y-1;j++) {
            Point<int> pixel11(i,j);
            p11 = this->gr->getPoint(&pixel11);
            if(polygon->contains(p11)) {
                Point<int> pixel12(i,j+1);
                p12 = this->gr->getPoint(&pixel12);
                if(polygon->contains(p12)) {
                    Point<int> pixel22(i+1,j+1);
                    p22 = this->gr->getPoint(&pixel22);
                    if(polygon->contains(p22)) {
                        Point<int> pixel21(i+1,j);
                        p21 = this->gr->getPoint(&pixel21);
                        if(polygon->contains(p21)) {
                            double z11 = this->gr->getPixel(i,j);
                            double z12 = this->gr->getPixel(i,j+1);
                            double z22 = this->gr->getPixel(i+1,j+1);
                            double z21 = this->gr->getPixel(i+1,j);
                            area += this->area(z11, z12, z22, z21, width, height);
                        }
                    }
                }
            }
        }
    }
    if(p11 != NULL) {delete p11;p11=0;}
    if(p12 != NULL) {delete p12;p12=0;}
    if(p22 != NULL) {delete p22;p22=0;}
    if(p21 != NULL) {delete p21;p21=0;}
    return area;
}

inline double calculator_engine::area(double a, double b, double c, double d,
                                      double width, double height) {
    double p = (a+b+c+d)/4;
    double ab = sqrt(pow(height, 2)+ pow(a-b, 2));
    double bc = sqrt(pow(width,2) + pow(b-c, 2));
    double cd = sqrt(pow(height,2)+pow(c-d, 2));
    double da = sqrt(pow(width,2)+pow(d-a,2));
    double bd = sqrt(pow(height, 2) + pow(width, 2))/2;
    double ap = sqrt(pow(bd, 2) + pow(a-p, 2));
    double bp = sqrt(pow(bd, 2) + pow(b-p, 2));
    double cp = sqrt(pow(bd, 2) + pow(c-p, 2));
    double dp = sqrt(pow(bd, 2) + pow(d-p, 2));
    double area = this->area(ab,ap,bp) + this->area(bc,bp,cp) +
    this->area(cd,cp,dp)+this->area(da,dp,ap);
    return area;
}

inline double calculator_engine::area(double a, double b, double c) {
    double length = (a+b+c)/2;
    return sqrt(length*(length-a)*(length-b)*(length-c));
}







