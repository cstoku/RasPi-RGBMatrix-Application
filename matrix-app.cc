
#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <chrono>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "led-matrix.h"
#include "graphics.h"


using namespace rgb_matrix;

int main(int argc, char *argv[]){

    if(argc != 2){
        std::cout << "To little or many argments" << std::endl;
        return 0;
    }

    std::string path_prefix = "/usr/local/movieplayer/";

    std::string file_prefix = argv[1]; 

    std::string audio_file = path_prefix + "audio/" + file_prefix + ".flac";
    std::string movie_file = path_prefix + "movie/" + file_prefix + ".mp4"; 

    //Setting
    GPIO io;
    int chain = 3;
    int parallel = 3;
    int rows = 16;

    if(!io.Init())
        return 1;

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Music *music=Mix_LoadMUS(audio_file.c_str());
    if(!music)return 1;

    RGBMatrix *matrix = new RGBMatrix(&io, rows, chain, parallel);
    matrix->set_luminance_correct(true);
    matrix->SetBrightness(100);
    matrix->SetPWMBits(11);
    FrameCanvas *canvas = matrix->CreateFrameCanvas();
    const int width = canvas->width();
    const int height = canvas->height();

    //Loading
    
    cv::Mat src_img;
    cv::Mat dst_img(height, width, src_img.type());
    cv::VideoCapture cap(movie_file.c_str());

    auto fps = cap.get(CV_CAP_PROP_FPS);
    long fps_ustime = 1000000.0/(double)fps+0.5;
    
    const auto hoseicount = (int)fps*10;
    auto hcnt=0;
    //Drawing
    

    if(Mix_PlayMusic(music, 1)==-1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
        return 1;
    }
    auto pre_dc=1ll;

    auto frame_cnt = cap.get(CV_CAP_PROP_FRAME_COUNT) - 1; // Magic!!!
    auto start = std::chrono::system_clock::now();
    while(true){

        auto fpos = cap.get(CV_CAP_PROP_POS_FRAMES);
        if(!(fpos < frame_cnt)){
            std::cout << "end." << std::endl;
            break;
        }

        cap >> src_img;

        auto pos = cap.get(CV_CAP_PROP_POS_MSEC);
        cv::resize(src_img, dst_img, dst_img.size(), cv::INTER_AREA);
        for(int y=0; y<height; y++){
            for(int x=0; x<width; x++){
                cv::Vec3b bgr = dst_img.at<cv::Vec3b>(y, x);
                canvas->SetPixel(x, y, bgr[2], bgr[1], bgr[0]);
            }
        }
        matrix->SwapOnVSync(canvas);
        auto end = std::chrono::system_clock::now();
        
        auto delaytime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        auto delaycount = pos * 1000 - delaytime;
        if(delaycount > 0) {
            usleep(delaycount);
        }
    }
    matrix->Clear();
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
    
    delete matrix;

    std::cout << "See You!" << std::endl;
    return 0;
}

