#include <iostream>
#include <thread>
#include "cpen333/process/subprocess.h"
#include "cpen333/process/condition.h"
#include "cpen333/process/unlinker.h"

int main() {

  // conditions
  cpen333::process::condition safetocross("q6/condition/safetocross");
  cpen333::process::condition safetodrive("q6/condition/safetodrive");

  // make sure to "unlink" in case of crash
  cpen333::process::unlinker<decltype(safetocross)> crossunlinker(safetocross);
  cpen333::process::unlinker<decltype(safetodrive)> driveunlinker(safetodrive);

  using process = cpen333::process::subprocess ;
  process *pedestrians[10]; // 10 pedestrians
  process *cars[10];       // 10 cars

  for(int i = 0; i < 10; i++) {
    pedestrians[i] = new process({"./q6pedestrian", std::to_string(i + 1)}, true, false);
    cars[i]  = new process({"./q6car", std::to_string(i+1)}, true, false);
  }

  // loop while the intersection is not empty
  bool empty = false;
  while(!empty)	{
    safetocross.reset();                                    // stop pedestrians
    std::cout << std::endl <<  "No Walking" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing
    std::cout << "Green Light" << std::endl;
    safetodrive.notify();                                   // allow cars to drive over pedestrian crossing
    std::this_thread::sleep_for(std::chrono::seconds(10));  // wait a suitable time delay between lights changing
    safetodrive.reset();                                    // stop cars
    std::cout << std::endl << "Red Light" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing
    std::cout << "Walk on" << std::endl;
    safetocross.notify();                                   // allow pedestrians to cross
    std::this_thread::sleep_for(std::chrono::seconds(15));  // allow pedestrians to cross for 15 seconds

    // count remaining processes
    empty = true;  // assume intersection empty
    for (int i=0; i<10; ++i) {
      if (!pedestrians[i]->wait_for(std::chrono::milliseconds(0))) {
        empty = false;
        break;
      }
      if (!cars[i]->wait_for(std::chrono::milliseconds(0))) {
        empty = false;
        break;
      }
    }
  }

  // clean up memory
  for (int i=0; i<10; ++i) {
    delete pedestrians[i];
    delete cars[i];
  }

  std::cout << "Done." << std::endl;
}