
#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <chrono>
#include "led-matrix.h"
#include "graphics.h"


using namespace rgb_matrix;

int main(void){


    //Setting
    GPIO io;
    int chain = 3;
    int parallel = 3;
    int rows = 16;

    if(!io.Init())
        return 1;

    RGBMatrix *matrix = new RGBMatrix(&io, rows, chain, parallel);
    matrix->set_luminance_correct(true);
    matrix->SetBrightness(80);
    matrix->SetPWMBits(11);
    FrameCanvas *canvas = matrix->CreateFrameCanvas();
    const int width = canvas->width();
    const int height = canvas->height();
    
    std::cout << "Setting end" << std::endl;
    //Loading
    
    cv::Mat src_img;
    cv::Mat dst_img(height, width, src_img.type());
    cv::VideoCapture cap("aikatsu.mp4");

    long fps_ustime = 1000000.0/(double)cap.get(CV_CAP_PROP_FPS)+0.5;
    std::cout << "Loading end" << std::endl;
    //Drawing
    
    std::cout << "Drawing..." << std::endl;

    while(true){
        auto start = std::chrono::system_clock::now();

        cap >> src_img;
        if(src_img.empty()){
            std::cout << "end." << std::endl;
            break;
        }
    
        cv::resize(src_img, dst_img, dst_img.size());
        for(int y=0; y<height; y++){
            for(int x=0; x<width; x++){
                cv::Vec3b bgr = dst_img.at<cv::Vec3b>(y, x);
                canvas->SetPixel(x, y, bgr[2], bgr[1], bgr[0]);
            }
        }
        matrix->SwapOnVSync(canvas);

        auto end = std::chrono::system_clock::now();
        auto diff = end-start;
        auto delaytime = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();

        auto delaycount = fps_ustime-delaytime;
        if(delaycount > 0)
            usleep(delaycount);
    }
    matrix->Clear();
    
    std::cout << "Clear." << std::endl;
    delete matrix;

    std::cout << "See You!" << std::endl;
    return 0;
}

