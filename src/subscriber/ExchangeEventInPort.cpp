#include "ExchangeEventInPort.h"
#include "ExchangeEventDataReaderListenerImpl.h"

#include "StockQuoterTypeSupportC.h"
#include "StockQuoterTypeSupportImpl.h"
#include "ddsContext.hpp"

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include <ace/streams.h>

#include "subscriber.hpp"

namespace {
    const char* QUOTER_EXCHANGE_EVENT_TYPE = "Exchange Event Type";
    const char* QUOTER_EXCHANGE_EVENT_TOPIC = "Stock Exchange Events";
}

namespace StockQuoter {
    ExchangeEventInPort::ExchangeEventInPort()
        : ctx{ MWOpenDDS::context::getInstance() }
    {
    }

    ExchangeEventInPort::~ExchangeEventInPort()
    {
    }

    void ExchangeEventInPort::init(const SlMdl::Subscriber& mdl)
    {
        callback = std::bind(&SlMdl::Subscriber::on_exchange_event_recv, mdl, std::placeholders::_1);
        // We need to do this as we want the qos_listener (this class) to be managed by the Simulink model.
        try {
            auto &participant = ctx->participant;

            // Create a subscriber for the two topics
            // (SUBSCRIBER_QOS_DEFAULT is defined in Marked_Default_Qos.h)
            auto& sub = ctx->sub;

            // Register the ExchangeEvent type
            // (same as publisher)
            StockQuoter::ExchangeEventTypeSupport_var exchange_evt_servant
                = new StockQuoter::ExchangeEventTypeSupportImpl();

            if (DDS::RETCODE_OK != exchange_evt_servant->register_type(participant.in(),
                QUOTER_EXCHANGE_EVENT_TYPE)) {
                std::cerr << "register_type for " << QUOTER_EXCHANGE_EVENT_TYPE
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // Create a topic for the Exchange Event type
            // (same as publisher)
            topic = participant->create_topic(QUOTER_EXCHANGE_EVENT_TOPIC,
                    QUOTER_EXCHANGE_EVENT_TYPE,
                    TOPIC_QOS_DEFAULT,
                    DDS::TopicListener::_nil(),
                    ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
            if (CORBA::is_nil(topic.in())) {
                std::cerr << "create_topic for " << QUOTER_EXCHANGE_EVENT_TOPIC << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // Create DataReaders and DataReaderListeners
            qos_listener = new ExchangeEventDataReaderListenerImpl(*this);

            if (CORBA::is_nil(qos_listener.in())) {
                std::cerr << "ExchangeEvent listener is nil." << std::endl;
                ACE_OS::exit(1);
            }

            // Create the ExchangeEvent DataReaders
            datareader = sub->create_datareader(topic.in(),
                DATAREADER_QOS_DEFAULT,
                qos_listener.in(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in main.cpp:" << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }

    }

    void ExchangeEventInPort::term()
    {
        // Cleanup
        try {
            ctx->sub->delete_datareader(datareader);
            ctx->participant->delete_topic(topic);
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in ExchangeEventInPort cleanup."
                << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
        MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::CLEAR);
    }

    void ExchangeEventInPort::messageReceive(ExchangeEvent& in) {
        if (callback) {
            callback(in);
        }
    }


}
