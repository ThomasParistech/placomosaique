/*********************************************************************************************************************
 * File : timer.h                                                                                                    *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <chrono>
#include <iostream>
#include <string>

#ifndef TIMER_H
#define TIMER_H

/// @brief Class representing a timer based on the lifetime of an object on the stack
class Timer
{
public:
    enum TimeUnit
    {
        US,
        MS,
        S,
        MIN
    };

    /// @brief Constructor
    /// @param scope_name Name of the scope that is currently being timed
    /// @param time_unit Unit in which to output the elapsed time
    Timer(const std::string &scope_name, TimeUnit time_unit = TimeUnit::MS) : scope_name_(scope_name), time_unit_(time_unit)
    {
        begin_ = std::chrono::high_resolution_clock::now();
    }

    /// @brief Destructor
    ~Timer()
    {
        const auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Elapsed Time [" << scope_name_ << "]: ";
        switch (time_unit_)
        {
        case US:
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin_).count() << " us.";
            break;
        case MS:
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin_).count() << " ms.";
            break;
        case S:
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - begin_).count() << " s.";
            break;
        case MIN:
            std::cout << std::chrono::duration_cast<std::chrono::minutes>(end - begin_).count() << " min.";
            break;
        default:
            break;
        }
        std::cout << std::endl;
    }

private:
    std::string scope_name_;
    TimeUnit time_unit_;
    std::chrono::high_resolution_clock::time_point begin_;
};

#endif // TIMER_H
