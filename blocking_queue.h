#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

class MyException : public std::exception {
public:
    virtual char const *what() const noexcept {
        return "Something bad happend.";
    }
};

template <class T, class Container = std::deque<T>, class ConditionVariable = std::condition_variable>
class BlockingQueue {
public:
    explicit BlockingQueue(const size_t& capacity) : capacity_(capacity) {
        is_active_ = true;
    }
    void Put(T&& element) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!is_active_) {
            throw MyException();
        }
        while (is_active_ && queue_items_.size() >= capacity_) {
            adding_cv_.wait(lock);
        }
        if (is_active_ && queue_items_.size() < capacity_) {
            queue_items_.push_back(std::move(element));
            extracting_cv_.notify_one();
        }
        else{
            throw MyException();
        }
    }
    bool Get(T& result) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (is_active_ && queue_items_.empty()) {
            extracting_cv_.wait(lock);
        }
        if (is_active_ && !queue_items_.empty()) {
            result = std::move(queue_items_.front());
            queue_items_.pop_front();
            adding_cv_.notify_one();
            return true;
        }
        else {
            return false;
        }
    }
    void Shutdown() {
        std::unique_lock<std::mutex> lock(mutex_);
        is_active_ = false;
        extracting_cv_.notify_all();
        adding_cv_.notify_all();
    }
private:
    const size_t capacity_;
    bool is_active_;
    std::mutex mutex_;
    Container queue_items_;
    ConditionVariable adding_cv_, extracting_cv_;
};
