#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class Robot {
    bool waiting_for_left_step_ = false;
    bool waiting_for_right_step _= false;
    std::condition_variable next_step_cv_;
    std::mutex mutex_;
public:
    void StepLeft() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (waiting_for_left_step_) {
            next_step_cv_.wait(lock);
        }
        waiting_for_left_step_ = true;
        waiting_for_right_step_ = false;
        std::cout << "left" << std::endl;
        next_step_cv_.notify_one();
    }

    void StepRight() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (waiting_for_right_step_) {
            next_step_cv_.wait(lock);
        }
        waiting_for_right_step_ = true;
        waiting_for_left_step_ = false;
        std::cout << "right" << std::endl;
        next_step_cv_.notify_one();
    }
};