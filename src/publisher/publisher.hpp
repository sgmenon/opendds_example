
#ifndef __DDSEXAMPLE_PUBLISHER_
#define __DDSEXAMPLE_PUBLISHER_

#include "StockQuoterC.h"
#include "ExchangeEventOutPort.hpp"
#include "QuoteOutPort.hpp"

namespace SlMdl {
    class Publisher {
        // public data and function members
    public:
        // model initialize function
        void initialize();

        // model step function
        void step();

        // model terminate function
        void terminate();

        // Constructor
        Publisher();

    private:
        StockQuoter::ExchangeEventOutPort ex_port;
		StockQuoter::QuoteOutPort q_port;

        //Model Specific params        
        long int i;                         //Step ID

    };
}
#endif /*__DDSEXAMPLE_PUBLISHER_*/
