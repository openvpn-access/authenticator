#include "main.hpp"

/**
 * Exit with the specified code after waiting for the time control thread to finish.
 * @param code Code to exit with.
 */
void do_exit(int code)
{
    if (time_ctrl_thread.joinable())
    {
        time_ctrl_thread.join();
    }

    std::exit(code);
}

/**
 * Thread function to ensure random runtimes for the application to help prevent time attacks.
 * Program will run for at least a random number between 1-TIME_ATTACK_DEFENSE_TIMER_MS ms.
 */
void time_ctrl()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,TIME_ATTACK_DEFENSE_TIMER_MS);

    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
}
