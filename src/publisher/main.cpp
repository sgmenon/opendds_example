#include "publisher.hpp"
#include <thread>        
#include <chrono>        
#include <mutex>   
#include <condition_variable>
#include <atomic>
#include <vector>
#include <deque>
#include "ddsContext.hpp"


class Scheduler {
public:
    Scheduler(SlMdl::Publisher& aObj) :fObj(aObj), fBaseRate(250), fModelRunning(false) { 
        addRate(1); //Base Rate
    }
    void addRate(uint32_t rateMultiplier) {
        fRates.emplace_back(rateMultiplier);
    }
    void start() {
        if (fModelRunning) {
            return;
        }
        fModelRunning = true;
        for (size_t ii = 0;ii < fRates.size();++ii) {
            fWorkers.emplace_back(&Scheduler::rateFcn, this, ii);
        }
        fWorkers.emplace_back(&Scheduler::worker_thread, this);
    }
    void stop() {
        fModelRunning = false;
        for (auto&it : fWorkers) {
            it.join();
        }
    }
private:
    void worker_thread()
    {
        size_t count = 0;
        while (fModelRunning) {
            std::this_thread::sleep_for(fBaseRate);
            ++count;
            for (auto&it : fRates) {
                ///\todo:potential to use meta-programming to prevent the runtime check
                if (count%it.baseRateMultiplier == 0) {
                    it.cond.notify_all();
                }
            }
        }
    }
    void rateFcn(uint32_t idx)
    {
        std::unique_lock<std::mutex> lk(fRates[idx].mtx);
        while (fModelRunning) {
            fRates[idx].cond.wait(lk);
            fObj.step();
        }
    }
	SlMdl::Publisher& fObj;
    std::chrono::milliseconds fBaseRate;
    std::atomic<bool> fModelRunning;
    struct RateInfo {
        RateInfo(uint32_t mult) :baseRateMultiplier(mult) {}
        uint32_t baseRateMultiplier;
        std::mutex mtx;
        std::condition_variable cond;
    };
    std::deque<RateInfo> fRates;
    std::vector<std::thread> fWorkers;
};

int main(int argc, char *argv[])
{
    MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::SET, argc, argv);
	SlMdl::Publisher obj;
    obj.initialize();
    Scheduler s(obj);
    s.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    s.stop();
    obj.terminate();
    return 0;
}
