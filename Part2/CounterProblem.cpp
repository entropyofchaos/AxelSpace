#include <cstdio>
#include <thread>
#include <vector>

std::atomic_int counter;

void increment_counter(size_t numIters)
{
    for (size_t i = 0; i < numIters; ++i)
    {
        ++counter;
    }
}

int main() {
    size_t nb_iter = 1000000000;

    // Creating a vector to hold the active threads so I can create them and
    // monitor them dynamically.
    std::vector<std::thread> runing_threads;

    // Get the number of physical hardware threads
    size_t num_threads = std::thread::hardware_concurrency();

    // Number of iterations per thread
    size_t iters_per_thread = nb_iter / num_threads;
    size_t remaining_iters = nb_iter % num_threads;

    for (size_t i = 0; i < num_threads; ++i)
    {
        // For every thread up to the last, increment the number of iterations
        // per thread. For the last thread, add the remainder iterations that
        // didn't divide in evenly.
        size_t iterations = (i == num_threads - 1) ? iters_per_thread + remaining_iters : iters_per_thread;
        runing_threads.emplace_back(increment_counter, iterations);
    }

    // Keep waiting until all threads have stopped
    for (auto &thread : runing_threads)
    {
        thread.join();
    }

    printf("counter: %ld / %ld\n", static_cast<long>(counter), static_cast<long>(nb_iter));
    return 0;
}