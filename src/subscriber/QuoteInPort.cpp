// -*- C++ -*-
// *******************************************************************
//
// (c) Copyright 2006, Object Computing, Inc.
// All Rights Reserved.
//
// *******************************************************************

#include "QuoteInPort.h"
#include "QuoteDataReaderListenerImpl.h"

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
    const char* QUOTER_QUOTE_TYPE = "Quote Type";
    const char* QUOTER_QUOTE_TOPIC = "Stock Quotes";
}

namespace StockQuoter {
    QuoteInPort::QuoteInPort()
        : ctx{ MWOpenDDS::context::getInstance() }
    {
    }

    QuoteInPort::~QuoteInPort()
    {
    }

    void QuoteInPort::init(const SlMdl::Subscriber& mdl)
    {
        callback = std::bind(&SlMdl::Subscriber::on_quote_event_recv, mdl, std::placeholders::_1);
        // We need to do this as we want the qos_listener (this class) to be managed by the Simulink model.
        try {
            auto &participant = ctx->participant;

            // Create a subscriber for the two topics
            // (SUBSCRIBER_QOS_DEFAULT is defined in Marked_Default_Qos.h)
            auto& sub = ctx->sub;

            // Register the Quote type
            // (same as publisher)
            StockQuoter::QuoteTypeSupport_var quote_servant
                = new StockQuoter::QuoteTypeSupportImpl();

            if (DDS::RETCODE_OK != quote_servant->register_type(participant.in(),
                QUOTER_QUOTE_TYPE)) {
                std::cerr << "register_type for " << QUOTER_QUOTE_TYPE << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // Create a topic for the Quote type...
            // Could also use TOPIC_QOS_DEFAULT instead
            // (same as publisher)
            topic = participant->create_topic(QUOTER_QUOTE_TOPIC,
                    QUOTER_QUOTE_TYPE,
                    TOPIC_QOS_DEFAULT,
                    DDS::TopicListener::_nil(),
                    ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
            if (CORBA::is_nil(topic.in())) {
                std::cerr << "create_topic for " << QUOTER_QUOTE_TOPIC << " failed." << std::endl;
                ACE_OS::exit(1);
            }
            
            // Create DataReaders and DataReaderListeners
            qos_listener = new QuoteDataReaderListenerImpl(*this);

            if (CORBA::is_nil(qos_listener.in())) {
                std::cerr << "Quote listener is nil." << std::endl;
                ACE_OS::exit(1);
            }

            // Create the Quote and ExchangeEvent DataReaders
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

    void QuoteInPort::term()
    {
        // Cleanup
        try {
            ctx->sub->delete_datareader(datareader);
            ctx->participant->delete_topic(topic);
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in ExchangeEventDataReaderListenerImpl cleanup."
                << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
        MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::CLEAR);
    }

    void QuoteInPort::messageReceive(Quote& in) {
        if (callback) {
            callback(in);
        }
    }
}
