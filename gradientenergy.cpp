#include "gradientenergy.h"
#include <cmath>

GradientEnergy::GradientEnergy(QImage& I):image(I)
{
    greyTones();
}
GradientEnergy::~GradientEnergy(){
    free(grey);
}

int GradientEnergy::calculateEnergy(int x, int y){
    int g1 = qRed(Gx.pixel(x,y));
    int g2 = qRed(Gy.pixel(x,y));

    return abs(g1-128)+abs(g2-128);
}

void GradientEnergy::update(){
    calculateGradients();
}

QImage GradientEnergy::getGX(){
    return Gx;
}
QImage GradientEnergy::getGY(){
    return Gy;
}
QImage GradientEnergy::getEnergyPlot(){
    //QImage plot = QImage(image.width(),image.height(),QImage::Format_Mono);
    unsigned char* pixs = (unsigned char*) malloc(sizeof(unsigned char)*image.height()*image.width());
    int temp = 0;
    for (int y = 0; y < image.height(); y++){
        for (int x = 0; x < image.width(); x++){
            temp = calculateEnergy(x,y) / 510;
            pixs[y*image.width()+x] = (unsigned char) temp;
        }
    }
    return QImage(pixs,image.width(),image.height(),QImage::Format_Mono);
}

void GradientEnergy::greyTones(){
    grey = (unsigned char*)malloc(sizeof(unsigned char)*image.width()*image.height());
    const unsigned int* src=(unsigned int*)image.bits();
    for(int y=0;y<image.height();y++)
    {
        for(int x=0;x<image.width();x++)
        {
            unsigned int c=src[x+y*image.width()];
            unsigned int r=qRed(c);
            unsigned int g=qGreen(c);
            unsigned int b=qBlue(c);
            unsigned int gs=(0.299f*r+0.587f*g+0.114f*b);
            grey[x+y*image.width()] = (unsigned char) gs;
        }
    }
}

QImage GradientEnergy::conv(double* mat){
    unsigned char* src = grey;
    unsigned char* dst = (unsigned char*)malloc(sizeof(unsigned char)*image.width()*image.height());
    int width = image.width();
    int height = image.height();
    for(int y=0;y<image.height();y++)
    {
        for(int x=0;x<image.width();x++)
        {
            int color=0;
            //Convolute repair 180 degree
            for(int cy=-1;cy<=1;cy++)
            {
                for(int cx=-1;cx<=1;cx++)
                {
                    int c=0;
                    if((cx+x>=0 && cy+y>=0) && (cx+x<width && cy+y<height))
                    {
                        c = round(src[(x+cx)+(y+cy)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                    }
                    else if(cx+x<0)
                    {
                        if(cy+y<0)
                        {
                            c = round(src[(0)+(0)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else if(cy+y>=height)
                        {
                            c = round(src[(0)+(height-1)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else
                        {
                            c = round(src[(0)+(y+cy)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                    }
                    else if(cx+x>=width)
                    {
                        if(cy+y<0)
                        {
                            c = round(src[(width-1)+(0)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else if(cy+y>=height)
                        {
                            c = round(src[(width-1)+(height-1)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else
                        {
                            c = round(src[(width-1)+(y+cy)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }

                    }
                    else if(cy+y<0)
                    {
                        if(cx+x<0)
                        {
                            c = round(src[(0)+(0)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else if(cx+x>=width)
                        {
                            c = round(src[(width-1)+(0)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else
                        {
                            c = round(src[(cx+x)+(0)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                    }
                    else if(cy+y>height)
                    {
                        if(cx+x<0)
                        {
                            c = round(src[(0)+(height-1)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else if(cx+x>=width)
                        {
                            c = round(src[(width-1)+(height-1)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                        else
                        {
                            c = round(src[(width)+(height-1)*image.width()]*mat[(2-(cx+1))+((2-(cy+1))*3)]);
                        }
                    }
                    color+=c;
                }
            }
            color+=128;
            if(color>255)
            {
                color=255;
            }
            else if(color<0)
            {
                color = 0;
            }
            dst[x+y*image.width()]= color;
        }
    }

    QImage conv_img((unsigned char*)dst,width,height,QImage::Format_Mono);
    return conv_img;
}
void GradientEnergy::calculateGradients(){
    double dx[] =  {-1, 0,1,
                    -1, 0,1,
                    -1, 0,1};

    double dy[] =  {-1,-1,-1,
                     0, 0, 0,
                     1, 1, 1};

    //Gx = QImage(image.width(),image.height(),QImage::Format_ARGB32);
    //Gy = QImage(image.width(),image.height(),QImage::Format_ARGB32);
    Gx = conv(dx);
    Gy = conv(dy);
}
