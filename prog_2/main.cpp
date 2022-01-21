#include "boost/interprocess/managed_shared_memory.hpp"
#include <thread>
#include <iostream>

void inputProcessor(const int& inpSum){
    if(inpSum > 9 && inpSum % 32 == 0){
        std::cout << "Received data: " << inpSum << std::endl;
    }
    else{
        std::cout << "An error accured!" << std::endl;
    }
}

int main(){
    std::pair<int*, std::size_t> res;
    
    std::thread inputThread([&](){       
        while(true){
            //Getting data from first process
            boost::interprocess::managed_shared_memory segment(
                boost::interprocess::open_or_create, "MySharedMemory", 65536);
            if(segment.find<int> ("SharedSum").first){
                res = segment.find<int> ("SharedSum");
                inputProcessor(*res.first);
                segment.destroy<int>("SharedSum");
            }
        }
    });

    inputThread.join();
    return 0;
}