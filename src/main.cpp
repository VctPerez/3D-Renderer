#include <iostream>
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    for(float t=0; t<1; t+=0.01){
        int x = x0 + (x1 - x0)*t;
        int y = y0 + (y1 - y0)*t;
        image.set(x, y, color);
    }
}

int main(){
    TGAImage tgaImage(100,100, TGAImage::RGB);
    drawLine(0,0,50,35, tgaImage, white);
    tgaImage.flip_vertically(); // origin at the left bottom corner of the image
    tgaImage.write_tga_file("../images/linea1.tga");
    return 0;
}