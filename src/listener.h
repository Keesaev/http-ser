#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>

#include "pages.h"

class Listener {
public:
  Listener(boost::asio::io_context &context,
           boost::asio::ip::tcp::endpoint endpoint, Pages &&pages);
  void start_accept();

private:
  void handle_accept(boost::beast::error_code ec);

  boost::asio::io_context &m_io_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
  Pages m_pages;
};

