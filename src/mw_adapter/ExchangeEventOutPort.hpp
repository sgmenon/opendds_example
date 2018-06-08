#ifndef __DDSEXAMPLE_EXCHANGE_EVENT_PORTINTERFACE_
#define __DDSEXAMPLE_EXCHANGE_EVENT_PORTINTERFACE_

#include "ddsContext.hpp"
#include "StockQuoterTypeSupportImpl.h"

#include <functional>
#include <iostream>
#include <map>
namespace StockQuoter {
    class ExchangeEventOutPort {
    public:
        ExchangeEventOutPort()
            :ctx{ MWOpenDDS::context::getInstance() } {
        }
        ~ExchangeEventOutPort() {}
        void init();
        void term();
        void send(const ExchangeEvent& data);
    private:
        ExchangeEventOutPort(const ExchangeEventOutPort&) = delete;
        void operator=(const ExchangeEventOutPort&) = delete;
        DDS::Topic_var topic;
        ExchangeEventDataWriter_var datawriter;
        std::map<::TAO::String_Manager, DDS::InstanceHandle_t> handles;
        std::shared_ptr<MWOpenDDS::context> ctx;
    };
}
#endif /*__DDSEXAMPLE_EXCHANGE_EVENT_PORTINTERFACE_*/