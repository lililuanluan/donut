#pragma once
#ifndef MYTIMER_H
#define MYTIMER_H
#include <iostream>
#include <memory>
#include <chrono>
#include <string_view>

using std::cout, std::string, std::endl, std::string_view;

class Timer { 
public:
    Timer() {
        m_StartTimepoint = std::chrono::high_resolution_clock::now(); 
    }
    Timer(string_view func_name) {
        m_Name = func_name;
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }
    void Stop() {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count(); 
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        auto duration = end - start;
        double ms = duration * 0.001; 
        cout << "Timer: <" << m_Name << "> " << duration << " us (" << ms << "ms)\n";

    }
    ~Timer() {
        Stop();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    std::string m_Name {};
};

#endif