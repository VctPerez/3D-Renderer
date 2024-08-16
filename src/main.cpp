#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255, 0,   0,   255);
const TGAColor GREEN = TGAColor(0,  255,    0, 255);
const TGAColor BLUE = TGAColor(0,  0,    255, 255);

Model * model = NULL;
const int WIDTH = 800;
const int HEIGHT = 800;


void checkTransposition(int &x0, int &y0, int &x1, int &y1, bool &transposed){
    if(std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0,y0);
        std::swap(x1,y1);
        transposed = true;
    }
}

void checkOrientation(int &x0, int &y0, int &x1, int &y1){
    if(x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
}

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    bool transposed = false;

    checkTransposition(x0,y0,x1,y1, transposed);
    checkOrientation(x0,y0, x1, y1);

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

void loadModel(TGAImage image){
    for(int i=0; i < model->nfaces(); i++){
        std::vector<int> face = model->face(i);
        for(int j=0; j < 3; j++){
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]); 
            int x0 = (v0.x+1.)*WIDTH/2.; 
            int y0 = (v0.y+1.)*HEIGHT/2.; 
            int x1 = (v1.x+1.)*WIDTH/2.; 
            int y1 = (v1.y+1.)*HEIGHT/2.; 
            drawLine(x0, y0, x1, y1, image, WHITE); 
        }
    }

    image.flip_vertically();
    image.write_tga_file("../images/african_head/output.tga");
    delete model;
}

int main(int argc, char ** argv){

    /* RENDERIZADO DE LINEAS
    TGAImage tgaImage(100,100, TGAImage::RGB);
    drawLine(0,0,50,35, tgaImage, WHITE);
    drawLine(10,50, 20, 80, tgaImage, RED);
    drawLine(30,70, 5,60, tgaImage, BLUE);
    tgaImage.flip_vertically(); // origin at the left bottom corner of the image
    tgaImage.write_tga_file("../images/lineas3.tga");*/

    if(argc == 2){
        model = new Model(argv[1]);
    }else{
        model = new Model("../assets/african_head.obj");
    }

    TGAImage tgaImage(WIDTH, HEIGHT, TGAImage::RGB);
    loadModel(tgaImage);
    return 0;
}