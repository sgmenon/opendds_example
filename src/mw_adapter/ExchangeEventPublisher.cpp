#include "ExchangeEventOutPort.hpp"

#include <dds/DdsDcpsInfrastructureC.h>

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include "StockQuoterTypeSupportImpl.h"

namespace {
    const char* STOCK_EXCHANGE_NAME = "Sid's Stock Exchange";
    const char* QUOTER_EXCHANGE_EVENT_TYPE = "Exchange Event Type";
    const char* QUOTER_EXCHANGE_EVENT_TOPIC = "Stock Exchange Events";
}

namespace StockQuoter {

    void ExchangeEventOutPort::init()
    {
        try {
            auto &participant = ctx->participant;
            auto & pub = ctx->pub;

            // Register the ExchangeEvent type
            ExchangeEventTypeSupport_var exchange_evt_servant
                = new ExchangeEventTypeSupportImpl();

            if (DDS::RETCODE_OK !=
                exchange_evt_servant->register_type(
                    participant.in(),
                    QUOTER_EXCHANGE_EVENT_TYPE))
            {
                std::cerr << "register_type for "
                    << QUOTER_EXCHANGE_EVENT_TYPE
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // .. add a topic for the Exchange Event type
            topic = participant->create_topic(QUOTER_EXCHANGE_EVENT_TOPIC,
                    QUOTER_EXCHANGE_EVENT_TYPE,
                    TOPIC_QOS_DEFAULT,
                    DDS::TopicListener::_nil(),
                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

            if (CORBA::is_nil(topic.in()))
            {
                std::cerr << "create_topic for "
                    << QUOTER_EXCHANGE_EVENT_TOPIC
                    << " failed."
                    << std::endl;
                ACE_OS::exit(1);
            }

            // Create a DataWriter for the Exchange Event topic
            DDS::DataWriter_var exchange_evt_base_dw =
                pub->create_datawriter(topic.in(),
                    DATAWRITER_QOS_DEFAULT,
                    DDS::DataWriterListener::_nil(),
                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

            if (CORBA::is_nil(exchange_evt_base_dw.in()))
            {
                std::cerr << "create_datawriter for "
                    << QUOTER_EXCHANGE_EVENT_TOPIC
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            datawriter = ExchangeEventDataWriter::_narrow(
                exchange_evt_base_dw.in());

            if (CORBA::is_nil(datawriter.in()))
            {
                std::cerr << "ExchangeEventDataWriter could not "
                    << "be narrowed" << std::endl;
                ACE_OS::exit(1);
            }

            // We have one ExchangeEvent instance, for the "Test Stock Exchange".
            ExchangeEvent ex_evt;
            ex_evt.exchange = STOCK_EXCHANGE_NAME;
            auto exchange_handle = datawriter->register_instance(ex_evt);
            handles[STOCK_EXCHANGE_NAME] = exchange_handle;
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in main.cpp:" << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
    }

    void ExchangeEventOutPort::term() {
        // Cleanup
        try {
            ctx->pub->delete_datawriter(datawriter);
            ctx->participant->delete_topic(topic.in());
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in ExchangeEventOutPort cleanup."
                << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
        MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::CLEAR);
    }

    void ExchangeEventOutPort::send(const ExchangeEvent& data)
    {
        try {
            auto inst = handles.find(data.exchange);
            if (inst == handles.end()) {
                auto newhandle = datawriter->register_instance(data);
                inst = handles.insert(handles.end(), std::make_pair(data.exchange, newhandle));
            }
            DDS::ReturnCode_t ret =
                datawriter->write(data, inst->second);

            if (ret != DDS::RETCODE_OK)
            {
                ACE_ERROR((
                    LM_ERROR,
                    ACE_TEXT("(%P|%t)ERROR: send returned %d.\n"),
                    ret));
            }
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught while sending message:" << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
    }
}
