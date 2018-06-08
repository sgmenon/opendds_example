#ifndef __DDSEXAMPLE_QUOTE_PORTINTERFACE_
#define __DDSEXAMPLE_QUOTE_PORTINTERFACE_

#include "ddsContext.hpp"
#include "StockQuoterTypeSupportImpl.h"

#include <functional>
#include <iostream>
#include <map>
namespace StockQuoter {
    class QuoteOutPort {
    public:
        QuoteOutPort()
            :ctx{ MWOpenDDS::context::getInstance() } {
        }
        ~QuoteOutPort() {}
        void init();
        void term();
        void send(const Quote& data);
    private:
        QuoteOutPort(const Quote&) = delete;
        void operator=(const Quote&) = delete;
        DDS::Topic_var topic;
        QuoteDataWriter_var datawriter;
        std::map<::TAO::String_Manager, DDS::InstanceHandle_t> handles;
        std::shared_ptr<MWOpenDDS::context> ctx;
    };
}
#endif /*__DDSEXAMPLE_QUOTE_PORTINTERFACE_*/