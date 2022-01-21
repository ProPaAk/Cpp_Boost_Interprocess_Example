#include "boost/interprocess/managed_shared_memory.hpp"
#include <thread>
#include <mutex>
#include <string>
#include <set>
#include <algorithm>
#include <iostream>
#include <condition_variable>

std::string inputProcessor(std::string& inStr, const std::set<char>& digitSet){
    //Сhecking if string consists of number charecters only
    if(inStr.size() > 0 && inStr.size() <= 64){
        for(auto& it : inStr){
            if(digitSet.find(it) == digitSet.end()){
                std::cout << "Input string contain letters" << std::endl;
                return "";
            }            
        }
    }
    else{
        std::cout << "Input string has incorrect size" << std::endl;
        return "";
    }
    
    //Sorting the string in descending order 
    std::sort(inStr.begin(), inStr.end(), 
        [](const auto& a, const auto& b){ return (a > b);});

    //Replacing even numbers to "KB" 
    std::string resultStr;
    for(auto& it : inStr){
        if(it % 2 == 0){            
            resultStr.append("KB");
        }
        else{
            resultStr.push_back(it);
        }
    }

    return resultStr;    
}

int main(){    
    std::condition_variable cvProc;
    std::condition_variable cvIn;
    std::string buffer;
    bool varProc = false;
    bool varIn = true;
    std::mutex mutex;
    std::set<char> digitSet{ '0','1','2','3','4','5','6','7','8','9' };

    std::thread inputThread([&](){
        std::string inStr;
        while(true){
            std::unique_lock<std::mutex> ul(mutex);
            cvIn.wait(ul, [&varIn](){return varIn;});

            //Getting a string from user
            std::cout << "Enter the string: "; 
            std::getline(std::cin, inStr, '\n');
            buffer = inputProcessor(inStr, digitSet);

            varProc = true;
            varIn = false;
            cvProc.notify_one();
        }
    });

    //Remove shared memory on construction and destruction
    struct shm_remove {
        shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
        ~shm_remove(){ boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
    } remover;

    std::thread processThread([&](){
        int sum;
        while(true){
            std::unique_lock<std::mutex> ul(mutex);
            cvProc.wait(ul, [&varProc](){return varProc;});

            //Calculating the sum
            sum = 0;
            for(auto& it : buffer){
                if(it != 'K' && it !='B'){
                    sum += (it - '0');
                }
            }

            std::cout << buffer << std::endl;
            buffer.clear(); 

            //Writing to shared memory
            boost::interprocess::managed_shared_memory segment(
                boost::interprocess::open_or_create, "MySharedMemory", 65536);
            if(segment.find<int> ("SharedSum").first){
                segment.destroy<int>("SharedSum");
            }
            segment.construct<int>("SharedSum")(sum);

            varProc = false;
            varIn = true;
            cvIn.notify_one();
        }
    });

    inputThread.join();
    processThread.join();
    return 0;
}