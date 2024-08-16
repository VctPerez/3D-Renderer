#include <iostream>
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,  255,    0, 255);
const TGAColor blue = TGAColor(0,  0,    255, 255);


void checkTransposition(int &x0, int &x1, int &y0, int &y1, bool &transposed){
    if(std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0,y0);
        std::swap(x1,y1);
        transposed = true;
    }
}

void checkOrientation(int &x0, int &x1, int &y0, int &y1){
    if(x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
}

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    bool transposed = false;
    //checkTransposition(x0,x1,y0,y1, transposed);
    //checkOrientation(x0,y0, x1, y1);
    if(std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0,y0);
        std::swap(x1,y1);
        transposed = true;
    }

    if(x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float dError = std::abs(dy/(float)dx);
    float error = 0;

    for(int x = x0; x<=x1; x++){
        float t = (x - x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1 * t;
        if(transposed){
            image.set(y, x, color);
        }else{
            image.set(x, y, color);
        }
        error += dError;
        if(error > .5){
            y += y1 > y0 ? 1 : -1;
            error -= 1.;
        }

    }
}

int main(){
    TGAImage tgaImage(100,100, TGAImage::RGB);
    drawLine(0,0,50,35, tgaImage, white);
    drawLine(10,50, 20, 80, tgaImage, red);
    drawLine(30,70, 5,60, tgaImage, blue);
    tgaImage.flip_vertically(); // origin at the left bottom corner of the image
    tgaImage.write_tga_file("../images/lineas3.tga");
    return 0;
}