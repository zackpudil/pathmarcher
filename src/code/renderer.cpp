#include <main.hpp>
#include <renderer.hpp>
#include <progressbar.hpp>
#include <opencv2/opencv.hpp>

Renderer::Renderer(int f, int p, float t, int w, int h) :
  frames(f), pass(p), timeStep(t), width(w), height(h) { 
    currentTime = 0.0f;
    currentFrame = 0;
    reel = (float **)malloc(sizeof(float*)*frames);
}

void Renderer::prerender(Pixel* pixel) {
  std::vector<std::thread> threads;

  for(int k = 0; k < pixel->deviceCount; k++) {
    threads.push_back(std::thread([&, k]() {
      
      int start = k*(frames/pixel->deviceCount);
      int end = (k + 1)*(frames/pixel->deviceCount);
      
      float time = timeStep*k*(frames/pixel->deviceCount);
      ProgressBar *progress = new ProgressBar((frames/pixel->deviceCount)*pass);
      
      for(int i = start; i < end; i++) {
        reel[i] = (float *)malloc(sizeof(float)*width*height*4);
        for(int j = 0; j < pass; j++) {
          pixel->computeImage(k, j, time, reel[i], reel[i], progress);
        }
        time += timeStep;
      }
    }));
  }

  for(auto& th : threads) th.join();

  for(int i = 0; i < pixel->deviceCount; i++) {
    std::cout << std::endl;
  }
}

void Renderer::save() {
  
  ProgressBar progress(frames);
  cv::Size sz(width, height);
  cv::VideoWriter vw("test.mov", cv::VideoWriter::fourcc('F', 'F', 'V', '1'), 30.0, sz, true);
  
  for(int k = 0; k < frames; k++) {
    
    auto uframe = (uchar *)malloc(sizeof(uchar*)*width*height*4);
    for(int j = 0; j < width*height*4; j++) {
      uframe[j] = (uchar)std::floor((reel[k][j]/pass)*255.0f);
    }
    
    cv::Mat mat(sz, CV_8UC4, uframe);
    
    cv::Mat img;
    cv::cvtColor(mat, img, cv::COLOR_RGBA2BGR);
    cv::flip(img, img, 0);
    
    vw << img;
    progress.incrementProgress(0);
  }
  std::cout << std::endl;
  vw.release();
}
