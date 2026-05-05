#include <iostream>
#include <functional>
#include <map>
#include <thread>
#include "monocap.hpp"

int constr_test(int argc, char* argv[])
{
  monocap::Camera cam(0,720,480,60,false);
  cam.start_stream();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  return 0;  
}

int photoTest(int argc, char* argv[])
{
  monocap::Camera cam(0,720,480,60,false);
  cam.start_stream();
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  cam.save_image("picture.png");
  return 0;  
}

std::map<std::string,std::function<int(int argc,char* argv[])>> handler =
{
  {"--constrTest", &constr_test},
  {"--photoTest", &photoTest}
};

void printUsage(std::string bin_name)
{
  std::cout << " " << std::endl;
  std::cout << "usage: "<< bin_name << " [option]" << std::endl;
  std::cout << "Available options: [";
  for(auto v_pair : handler)
  {
    std::cout << "  " << v_pair.first;
  }
  std::cout << "]" << std::endl;
}


int main(int argc, char* argv[])
{
  if ((argc-1) < 1)
  {
    std::cout << "Invalid usage." << std::endl;
    printUsage("monocap-test");
    return 1;
  }
  
  if(handler.count(argv[1])!=1)
  {
    std::cout << "Invalid option: " << argv[1] << std::endl;
    printUsage("monocap-test");
    return 1;
  }
  
  return handler[argv[1]](argc,argv);
}




