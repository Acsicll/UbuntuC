#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <future>

class ThreadPool_t {
    using funcPtr = std::packaged_task<void()>;
public:
    ThreadPool_t(int n = 1) : _threadNum(n),_atomic_(0){
        start();
    }
    ~ThreadPool_t(){
        stop();
    }
    template<typename F,typename... Args>
    auto exec(F&& func,Args&&... args)->std::future<decltype(func(args...))>;
    void stop();
private:
    void start();
    void run();
    std::atomic<int> _atomic_;
    bool _bterminate;
    int _threadNum;
    std::queue<funcPtr>_tasks;
    std::vector<std::thread>_threads;
    std::mutex _mtx;
    std::condition_variable _cv;
};

template<typename F, typename... Args>
auto ThreadPool_t::exec(F&& func,Args&&... args)->std::future<decltype(func(args...))>{
    using ReturnType = decltype(func(args...));
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(func),std::forward<Args>(args)...));
    std::future<ReturnType>ret = task->get_future();
    {
        std::unique_lock<std::mutex>lock(_mtx);
        _tasks.emplace([task] { (*task)(); });
    }
    _cv.notify_one();
    return ret;
}

void ThreadPool_t::start(){
    for(size_t i = 0;i<_threadNum;i++){
        _threads.emplace_back(std::thread(&ThreadPool_t::run,this));
    }
}

void ThreadPool_t::stop(){
    {
        std::unique_lock<std::mutex>lock(_mtx);
        _bterminate = true;
    }
    _cv.notify_all();

    for(std::thread &thread : _threads){
        if(thread.joinable()){
            thread.join();
        }
    }
}

void ThreadPool_t::run(){
    while(!_bterminate){
        funcPtr task;
        {
            std::unique_lock<std::mutex>lock(_mtx);
            _cv.wait(lock,[this](){return _bterminate || !_tasks.empty();});
            if(_bterminate){
                return;
            }
            if(!_tasks.empty()){
                task = std::move(_tasks.front());
                _tasks.pop();
            }
        }
        if(&task){
            _atomic_++;
            (task)();
            _atomic_--;
        } 
        if(_atomic_ == 0 && !_tasks.empty()){
           _cv.notify_all(); 
        }
    }
}

void testThreadPool() {
    ThreadPool_t pool(4); // Create a thread pool with 4 threads
    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) {
        results.emplace_back(pool.exec([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
            return i * 2;
        }));
    }

    for (auto &result : results) {
        std::cout << "Result: " << result.get() << std::endl; // Get results
    }
}

int main() {
    testThreadPool();
    return 0;
}