#include <iostream>
#include "cpen333/process/shared_memory.h"

//
//	It's important to realise that all processes accessing the same datapool must
//	describe exactly the same datapool or structure template
//
struct 	DataPoolData {  // start of structure template
  int floor;            // floor corresponding to lifts current position
  int direction;        // direction of travel of lift
  int floors[10];       // an array representing the floors and whether requests are set
};                      // end of structure template


int main() {

// 	Attempt to make the datapool 'car1'. This process will not know
//	whether the datapool exists or not yet (we know that it does because we only
//	set the process running after the parent program had created it and stored data
//	in it) but that shouldn't matter, since the program can just attempt to make it. If it already exists
//	then it will simply use it. If it doesn't exist, then it will be created below

  std::cout << "Child attempting to create/use the datapool....." << std::endl;
  cpen333::process::shared_object<DataPoolData> datapool("car1") ;

  std::cout << "Child linked to datapool at address " << datapool.get() << std::endl;

// print out the data in the datapool that was stored there by the parent
  std::cout << "Child Read value for Floor = " << datapool->floor << std::endl;
  std::cout << "Child Read value for Direction = " << datapool->direction << std::endl;

  std::cout << "Child Read values for Floors array = ";
  for(int i=0; i < 10; i ++) {
    std::cout << datapool->floors[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Press Enter to quit child process" << std::endl;
  std::cin.get();  // pause so programmer can view output before child dies

  // we don't 'unlink' the datapool here in case another child thread wishes to read the data
  // If nobody unlinks the datapool, then on POSIX systems, it will persist until the next reboot (or someone manually
  // unlinks it)

  return 0 ;
}