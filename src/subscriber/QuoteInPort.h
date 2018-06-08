
#ifndef __QUOTE_IN_PORT
#define __QUOTE_IN_PORT

#include <dds/DdsDcpsSubscriptionC.h>

#include "InPortInterface.hpp"
#include "ddsContext.hpp"
#include <functional>

namespace SlMdl {
	class Subscriber;
}

namespace StockQuoter {
    struct Quote;

    class QuoteInPort
        : public PortInterfaces::InPortInterface<Quote>
    {
    public:
        QuoteInPort();

        virtual ~QuoteInPort();

        void init(const SlMdl::Subscriber&);
        void term();

        virtual void messageReceive(Quote&) override; 

    private:
        std::shared_ptr<MWOpenDDS::context> ctx;
        DDS::Topic_var topic;
        DDS::DataReader_var datareader;
        DDS::DataReaderListener_var qos_listener;
        std::function<void(Quote&)> callback;
    };
}

#endif /* __QUOTE_IN_PORT  */
