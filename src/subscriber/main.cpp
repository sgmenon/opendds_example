#include "subscriber.hpp"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "ddsContext.hpp"

int main(int argc, char *argv[])
{
    MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::SET, argc, argv);
    SlMdl::Subscriber obj;
    obj.initialize();

    //no need for step
    try {
        // Wait for events from the Publisher; shut down when "close" received
        std::cout << "Subscriber: waiting for events" << std::endl;
        while (!SlMdl::is_exchange_closed()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Received CLOSED event from publisher; exiting..." << std::endl;

    }
    catch (CORBA::Exception& e) {
        std::cerr << "Exception caught in main.cpp:" << std::endl
            << e << std::endl;
        ACE_OS::exit(1);
    }

    obj.terminate();

    return 0;
}