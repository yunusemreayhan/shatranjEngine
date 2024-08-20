#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

template<bool debug = false>
class TimeChecker {
   public:
    TimeChecker(std::atomic<bool>&        stopper_flag_inp,
                std::chrono::milliseconds t = std::chrono::milliseconds(0)) :
        stopper_flag(stopper_flag_inp),
        period(t) {}

    inline bool IsTimeUp() {
        if (deadline < std::chrono::system_clock::now())
            return true;
        return false;
    }
    inline void Cancel() {
        if constexpr (debug)
            std::cout << "canceling timer" << std::endl;
        active.store(false);  // for disabling stopper flag setting
        cancel.store(true);
        while (true)
        {
            std::unique_lock<std::mutex> timerlk(timer_mutex);
            if (0 != cv.wait_for(timerlk, std::chrono::seconds(1), [&]() {
                    return clock_is_active.load() == false;
                }))
            {
                if constexpr (debug)
                    std::cout << "breaking from cancel while loop." << std::endl;
                break;
            }
            else
            {
                if constexpr (debug)
                    std::cout << "waiting for timer; for resetting it (1s)." << std::endl;
            }
        }
        if constexpr (debug)
            std::cout << "canceled timer" << std::endl;
    }

    inline void Reset() {
        if (clock_is_active.load())
            Cancel();

        // std::cout << "starting timer" << std::endl;
        deadline = std::chrono::system_clock::now() + period;

        std::thread([&]() {
            std::unique_lock<std::mutex> timerlk(timer_mutex);
            cv.wait(timerlk, [&]() -> bool { return clock_is_active.load() == false; });
            cancel.store(false);
            active.store(true);
            clock_is_active.store(true);
            if constexpr (debug)
            {
                std::cout << "starting timer" << std::endl;
            }
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                if (IsTimeUp())
                {
                    if constexpr (debug)
                        std::cout << "time is up with timer" << std::endl;
                    break;
                }
                if (cancel.load())
                {
                    if constexpr (debug)
                        std::cout << "canceling timer, cancel flag detected" << std::endl;
                    break;
                }
            }
            if (active.load())
            {
                if constexpr (debug)
                {
                    std::cout << "firing stopper flag!" << std::endl;
                }
                stopper_flag.store(true);
            }
            clock_is_active.store(false);
            if constexpr (debug)
            {
                std::cout << "notifying all" << std::endl;
                std::cout << "done with the timer" << std::endl;
            }
            cv.notify_all();
        }).detach();
    }


    ~TimeChecker() {
        active.store(false);
        cancel.store(true);
        while (true)
        {
            if constexpr (debug)
                std::cout << "destructing timer" << std::endl;
            std::unique_lock<std::mutex> timerlk(timer_mutex);
            if (0 != cv.wait_for(timerlk, std::chrono::seconds(1), [&]() {
                    return clock_is_active.load() == false;
                }))
            {
                if constexpr (debug)
                    std::cout << "breaking from ~timechecker while loop." << std::endl;
                break;
            }
            else
            {
                if constexpr (debug)
                    std::cout << "waiting for timer; destructing it (1s)." << std::endl;
            }
        }
        if constexpr (debug)
            std::cout << "destructed timer" << std::endl;
    }

   private:
    std::chrono::time_point<std::chrono::system_clock> deadline;
    std::atomic<bool>&                                 stopper_flag;
    std::atomic<bool>                                  active;
    std::atomic<bool>                                  clock_is_active;
    std::atomic<bool>                                  cancel;
    std::chrono::milliseconds                          period;
    std::mutex                                         timer_mutex;
    std::condition_variable                            cv;
};
