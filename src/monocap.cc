#include "monocap.hpp"
#include "apputils.hpp"
#include <chrono>
#include <iostream>
#include <thread>

namespace monocap
{
  Camera::Camera()
  {
    #ifdef __DEBUG__
      cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_INFO);
    #endif

    #ifndef __RELEASE__
      cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    #endif
  
    device = cv::VideoCapture();
    this->mirror = false;
    device_index = 0; 
    capture_width = 720;
    capture_height = 480;
    capture_fps = 60;
    cam_backend = (AppUtils::OSNAME() != "Linux") ? cv::CAP_ANY : cv::CAP_V4L2;
  }

  Camera::Camera(int id, int width, int height, int fps, bool mirror = false)
  {
    #ifdef __DEBUG__
      cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_INFO);
    #endif

    #ifndef __RELEASE__
      cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    #endif
  
    device = cv::VideoCapture();
    this->mirror = mirror;
    device_index = id; 
    capture_width = width;
    capture_height = height;
    capture_fps = fps;
    cam_backend = (AppUtils::OSNAME() != "Linux") ? cv::CAP_ANY : cv::CAP_V4L2;
  }

  Camera::~Camera()
  {
    #ifdef __DEBUG__
    std::cout << "[monocap::Camera::~Camera]" << std::endl;
    #endif
    _keep_frame_loop = false;
  }
  
  void Camera::mirrorToggle(){ mirror = !mirror; }
  void Camera::setMirror(bool state){ mirror = state; }


  void Camera::start_stream()
  {
    #ifdef __DEBUG__
    std::cout << "[monocap::Camera::start_stream] " << std::endl;
    #endif

    device.set(cv::CAP_PROP_FRAME_WIDTH,capture_width);
    device.set(cv::CAP_PROP_FRAME_HEIGHT,capture_height);
    device.set(cv::CAP_PROP_FPS,capture_fps);
    device.setExceptionMode(true);
    _keep_frame_loop = true;

    #ifdef __DEBUG__
    std::cout << "[monocap::Camera::start_stream] starting camera thread" << std::endl;
    #endif
    
    std::thread t_cam = std::thread(&Camera::frame_loop,this);
    t_cam.detach();
  }

  void Camera::start_sstream()
  {
    #ifdef __DEBUG__
    std::cout << "[monocap::Camera::start_sstream] " << std::endl;
    #endif
  
    device.set(cv::CAP_PROP_FRAME_WIDTH,capture_width);
    device.set(cv::CAP_PROP_FRAME_HEIGHT,capture_height);
    device.set(cv::CAP_PROP_FPS,capture_fps);
    device.setExceptionMode(true);
    _keep_frame_loop = true;
    frame_loop();
  }

  void Camera::stop_stream()
  {
    _keep_frame_loop = false;
     device.release();
  }

  int Camera::cam_count(){ return 0; }

  bool Camera::save_image(std::string filepath)
  {
    #ifdef __DEBUG__
    std::cout << "[monocap::Camera::save_image] "+filepath << std::endl;
    #endif
    
    bool save_success = false;
    try
    {
      save_success =  cv::imwrite(filepath,_frame2);
    }
    catch(std::exception &e){std::cout << "Saving file error: " << "\n\t" << e.what() << std::endl; }
  
    return save_success;
  }

  bool Camera::cam_available()
  {
     #ifdef __DEBUG__
     std::cout << "[monocap::Camera::frame_cam_available] " << std::endl;
     #endif
     
     cv::VideoCapture temp_camera(this->device_index, this->cam_backend);
     bool res = temp_camera.isOpened();
     temp_camera.release();

     #ifdef __DEBUG__
       std::cout << "[monocap::Camera::frame_cam_available] Debug state [res]:" << res << std::endl;
     #endif
     return res;
  }

  void Camera::frame_loop()
  {
    #ifdef __DEBUG__
      std::cout << "[monocap::Camera::frame_loop] Starting... " << std::endl;
      std::cout << "[monocap::Camera::frame_loop] Debug state [__keep_frame_loop]:" << _keep_frame_loop << std::endl; 
    #endif

    // This loop keeps running until told otherwise ==> awaits device
    while(_keep_frame_loop)
    {
      const std::lock_guard<std::mutex> lock(frame_mutex);
      if(cam_available()==true)
      {
      // Attempts to open camera by device_id 
      try 
      {
        #ifdef __DEBUG__
          std::cout << "Trying to open device..." << std::endl;
        #endif
        device.open(device_index,cam_backend);
      }
      catch(std::exception &e)
      { 
        #ifdef __DEBUG__
          std::cout << "Opening device failed: " << e.what() << std::endl;
        #endif

      }

      // Starts if 
      while(device.isOpened() && _keep_frame_loop)
      { 
        try
        {
          frame_valid = (device.read(_frame) && _frame.empty()==false);
        }
        catch(std::exception &e)
        {
          frame_valid = false;
          device.release();
        }

        if(frame_valid)
        {
          if(mirror)
          {
            cv::flip(_frame,_frame2,1);
          } 
          else
          {
            _frame2 = _frame;
          }
          
          cv::cvtColor(_frame2,frame, cv::COLOR_BGR2RGB); 
        }
     }
     
     device.release();
  
     #ifdef __DEBUG__
     std::cout << "Device closed" << std::endl;
     #endif
  
     }
     
     std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }

  #ifdef __DEBUG__
  std::cout << "[monocap::frame_loop] end " << std::endl;
  #endif
  
  }
}
