//
//  Geometry.h
//  surface-calculate
//
//  Created by wong jimsan on 23/03/2017.
//  Copyright © 2017 wong jimsan. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h

#include <vector>
#include <math.h>

#pragma once

#define INFINITY2 1e10
#define ESP 1e-5

template<typename T>
struct Point {
    T x;
    T y;
    Point() {
        
    };
    
    Point(T x,T y):x(x),y(y){
        
    }
};

template<typename T>
struct Point3D {
    T x;
    T y;
    T z;
    Point3D() {
    
    };
    
    Point3D(Point<T> * point, T z):z(z){
        x = point->x;
        y = point->y;
        
    }
    
    Point3D(T x, T y, T z):x(x),y(y),z(z) {
        
    }
    
    
};

template<typename T>
double multiply(Point<T> * p1,Point<T> * p2, Point<T> * p0) {
    return (p1->x - p0->x)*(p2->y - p0->y) - (p2->x - p0->x)*(p1->y - p0->y);
}


template<typename T>
struct Line {
    Point<T> * start;
    Point<T> * end;
    Line(Point<T> * start,Point<T> * end):start(start),end(end) {
        
    };
    
    bool contains(Point<T> * point) {
        bool flag = (point->x - start->x) * (point->x - end->x) <= 0;
        if(flag) {
            flag &= (point->y - start->y) * (point->y - end->y) <= 0;
            if(flag) {
                flag &= fabs(multiply(start, end, point))<ESP;
                return flag;
            }
        }
        return false;
    };
    
    bool intersect(Line<T> * line) {
        bool flag = MAX(start->x ,end->x) >= MIN(line->start->x,line->end->x);
        if(flag) {
            flag &= MAX(line->start->x,line->end->x) >= MIN(start->x ,end->x);
            if(flag) {
                flag &= MAX(start->y ,end->y) >= MIN(line->start->y,line->end->y);
                if(flag) {
                    flag &= MAX(line->start->y,line->end->y) >= MIN(start->y ,end->y);
                    if(flag) {
                        flag &= multiply(line->start, end, start) * multiply(end, line->end, start) >= 0;
                        if(flag) {
                            flag &= multiply(start, line->end, line->start) * multiply(line->end, end,line->start) >= 0;
                            return flag;
                        }
                    }
                }
            }
        }
        return false;
    };
};

template<typename T>
struct BoundingBox {
    T minx;
    T miny;
    T maxx;
    T maxy;
    BoundingBox(T minx,T miny,T maxx,T maxy)
    :minx(minx),miny(miny),maxx(maxx),maxy(maxy){
        
    };
    
    void unite(BoundingBox<T> * bounds) {
        minx = MIN(minx,bounds->minx);
        miny = MIN(miny,bounds->miny);
        maxx = MAX(maxx,bounds->maxx);
        maxy = MAX(maxy,bounds->maxy);
    }
};

template<typename T>
struct Ring {
    std::vector<Point<T> *> points;
    
    Ring() {
        
    };
    
    void add(Point<T> * point) {
        points.push_back(point);
    };
    
    BoundingBox<T> * getBoundingBox() {
        auto itor = points.cbegin();
        T minx = (*itor)->x;
        T miny = (*itor)->y;
        T maxx = (*itor)->x;
        T maxy = (*itor)->y;
        itor++;
        while(itor != points.cend()) {
            minx = MIN(minx,(*itor)->x);
            miny = MIN(miny,(*itor)->y);
            maxx = MAX(maxx,(*itor)->x);
            maxy = MAX(maxy,(*itor)->y);
            itor++;
        };
        BoundingBox<T> *bounds = new BoundingBox<double>(minx,miny,maxx,maxy);
        return bounds;
    }
};

template<typename T>
struct Polygon {
    Ring<T> * exteriorRing;
    //std::vector<Ring<T> *> interiorRings;
    
    Polygon(Ring<T> * exteriorRing):exteriorRing(exteriorRing) {
        
    };
    
    void add(Ring<T> * interiorRing) {
        //interiorRings.push_back(interiorRing);
    };
    
    BoundingBox<T> * getBoundingBox() {
        BoundingBox<T> * bounds = exteriorRing->getBoundingBox();
        //auto itor = interiorRings.cbegin();
        //while(itor != interiorRings.cend()) {
        //    BoundingBox<T> * ib = (*itor)->getBoundingBox();
        //    bounds->unite(ib);
        //    delete ib;
        //    ib = 0;
        //    itor++;
        //};
        return bounds;
    };
    
    bool contains(Point<T> * point) {
        auto itor = exteriorRing->points.cbegin();
        int count = 0;
        Point<T> * end = new Point<T>(-INFINITY2,point->y);
        Line<T> line(point,end);
        while(itor != exteriorRing->points.cend()-1) {
            Point<T> * p1 = *itor;
            itor++;
            Point<T> * p2 = *itor;
            Line<T> side(p1,p2);
            if(side.contains(point)) {
                return true;
            }
            if(fabs(side.start->y - side.end->y)<ESP) {
                continue;
            }
            if(line.contains(side.start)) {
                if(side.start->y > side.end->y) {
                    count++;
                }
            }
            else if(line.contains(side.end)) {
                if(side.end->y > side.start->y) {
                    count++;
                }
            }
            else if(line.intersect(&side)) {
                count++;
            }
        }
        delete end;
        end = 0;
        return count % 2 == 1;
    }
    
};

#endif /* Geometry_h */
