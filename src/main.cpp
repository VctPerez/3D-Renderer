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

struct Triangle{
    Vec2i vertex1;
    Vec2i vertex2;
    Vec2i vertex3;
};


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

void drawTriangle(Triangle t, TGAImage& image, float intensity){
    TGAColor color(intensity * 255, intensity * 255, intensity * 255, 255);
    if (t.vertex1.y==t.vertex2.y && t.vertex1.y==t.vertex3.y) return; // i dont care about degenerate triangles
    if (t.vertex1.y>t.vertex2.y) std::swap(t.vertex1, t.vertex2);
    if (t.vertex1.y>t.vertex3.y) std::swap(t.vertex1, t.vertex3);
    if (t.vertex2.y>t.vertex3.y) std::swap(t.vertex2, t.vertex3);

    int total_height = t.vertex3.y-t.vertex1.y;

    for (int i=0; i<total_height; i++) {

        bool second_half = i>t.vertex2.y-t.vertex1.y || t.vertex2.y==t.vertex1.y;

        int segment_height = second_half ? t.vertex3.y-t.vertex2.y : t.vertex2.y-t.vertex1.y;
        
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t.vertex2.y-t.vertex1.y : 0))/segment_height; // careful: with above conditions no division by zero here
        
        Vec2i A = t.vertex1 + (t.vertex3-t.vertex1)*alpha;
        Vec2i B = second_half ? t.vertex2 + (t.vertex3-t.vertex2)*beta : t.vertex1 + (t.vertex2-t.vertex1)*beta;
        
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t.vertex1.y+i, color); // attention, due to int casts t0.y+i != A.y
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

void flatShadingRenderModel(TGAImage &image){
    Vec3f lightDir(0, 0, -1);
    for(int i=0; i < model->nfaces(); i++){
        std::vector<int> face = model->face(i); 
        Vec2i screen_coords[3]; 
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) { 
            Vec3f v = model->vert(face[j]); 
            screen_coords[j] = Vec2i((v.x+1.) * WIDTH/2., (v.y+1.) * HEIGHT/2.); 
            world_coords[j] = v;
        } 
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]); 
        n.normalize(); 
        float intensity = n * lightDir; 
        if(intensity > 0){
            Triangle t;
            t.vertex1 = screen_coords[0];
            t.vertex2 = screen_coords[1];
            t.vertex3 = screen_coords[2];
            drawTriangle(t, image, intensity);
        }
        
    }
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

    // RENDERIZADO DE MODELO
    if(argc == 2){
        model = new Model(argv[1]);
    }else{
        model = new Model("../assets/african_head.obj");
    }

    TGAImage tgaImage(WIDTH, HEIGHT, TGAImage::RGB);
    // loadModel(tgaImage);

    // TGAImage tgaImage(200, 200, TGAImage::RGB);
    // Triangle t;
    // t.vertex1 = Vec2i(30,40);
    // t.vertex2 = Vec2i(120,150);
    // t.vertex3 = Vec2i(90,70);

    // //drawTriangle(t, tgaImage);
    flatShadingRenderModel(tgaImage);

    tgaImage.flip_vertically();
    tgaImage.write_tga_file("../images/african_head/outputFlat.tga");

    return 0;
}