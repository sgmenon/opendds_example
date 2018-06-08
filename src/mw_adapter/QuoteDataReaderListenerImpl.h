// -*- C++ -*-
// *******************************************************************
//
// (c) Copyright 2006, Object Computing, Inc.
// All Rights Reserved.
//
// *******************************************************************

#ifndef QUOTE_DATAREADER_LISTENER_IMPL
#define QUOTE_DATAREADER_LISTENER_IMPL

#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/LocalObject.h>

#include "InPortInterface.hpp"

namespace StockQuoter {
    struct Quote;

    class QuoteDataReaderListenerImpl
        : public OpenDDS::DCPS::LocalObject<DDS::DataReaderListener>
    {
    public:
        using Port = PortInterfaces::InPortInterface<StockQuoter::Quote>;
        QuoteDataReaderListenerImpl(Port&);

        virtual ~QuoteDataReaderListenerImpl(void);

        void on_requested_deadline_missed(
            DDS::DataReader_ptr reader,
            const DDS::RequestedDeadlineMissedStatus & status) override;

        void on_requested_incompatible_qos(
            DDS::DataReader_ptr reader,
            const DDS::RequestedIncompatibleQosStatus & status) override;

        void on_liveliness_changed(
            DDS::DataReader_ptr reader,
            const DDS::LivelinessChangedStatus & status) override;

        void on_subscription_matched(
            DDS::DataReader_ptr reader,
            const DDS::SubscriptionMatchedStatus & status) override;

        void on_sample_rejected(
            DDS::DataReader_ptr reader,
            const DDS::SampleRejectedStatus& status) override;

        void on_data_available(
            DDS::DataReader_ptr reader) override;

        void on_sample_lost(
            DDS::DataReader_ptr reader,
            const DDS::SampleLostStatus& status) override;

    private:
        Port& _inst;
    };
}

#endif /* QUOTE_DATAREADER_LISTENER_IMPL  */
