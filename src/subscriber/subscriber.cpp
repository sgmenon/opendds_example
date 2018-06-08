// -*- C++ -*-
// *******************************************************************
//
// (c) Copyright 2006, Object Computing, Inc.
// All Rights Reserved.
//
// *******************************************************************

#include "StockQuoterC.h"
#include "subscriber.hpp"
#include <assert.h>
#include <mutex>
#include <iostream>

namespace {
    bool is_exchange_closed_received_ = false;
    std::mutex _lock;
}

namespace SlMdl {

    Subscriber::Subscriber()
    {}

    void Subscriber::initialize()
    {
        q_port.init(*this);
        ex_port.init(*this);
    }

    void Subscriber::step()
    {
        //empty
    }

    void Subscriber::terminate() {
        q_port.term();
        ex_port.term();
    }

    void Subscriber::on_exchange_event_recv(StockQuoter::ExchangeEvent& exchange_evt) {
        std::cout << "ExchangeEvent: exchange  = " << exchange_evt.exchange.in() << std::endl;

        switch (exchange_evt.event) {
        case StockQuoter::TRADING_OPENED:
            std::cout << "               event     = TRADING_OPENED" << std::endl;
            break;
        case StockQuoter::TRADING_CLOSED: {
            std::lock_guard<std::mutex> guard(_lock);
            std::cout << "               event     = TRADING_CLOSED" << std::endl;
            is_exchange_closed_received_ = true;
            break;
        }
        case StockQuoter::TRADING_SUSPENDED:
            std::cout << "               event     = TRADING_SUSPENDED" << std::endl;
            break;
        case StockQuoter::TRADING_RESUMED:
            std::cout << "               event     = TRADING_RESUMED" << std::endl;
            break;
        default:
            std::cerr << "ERROR: reader received unknown ExchangeEvent: " << exchange_evt.event << std::endl;
        }

        std::cout << "               timestamp = " << exchange_evt.timestamp << std::endl;
    }
    void Subscriber::on_quote_event_recv(StockQuoter::Quote& quote) {
        std::cout << "Quote: ticker    = " << quote.ticker.in() << std::endl
            << "       exchange  = " << quote.exchange.in() << std::endl
            << "       full name = " << quote.full_name.in() << std::endl
            << "       value     = " << quote.value << std::endl
            << "       timestamp = " << quote.timestamp << std::endl;
    }
    bool is_exchange_closed() {
        std::lock_guard<std::mutex> guard(_lock);
        return is_exchange_closed_received_;
    }
}