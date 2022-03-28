#include "listener.h"

#include <boost/asio/placeholders.hpp> //bind
#include <boost/beast.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "http_session.h"

Listener::Listener(boost::asio::io_context &context,
                   boost::asio::ip::tcp::endpoint endpoint,
                   std::string const &root_path)
    : m_io_context(context), m_acceptor(m_io_context), m_socket(context),
      m_root_path(root_path) {
  boost::beast::error_code ec;
  // TODO: перенести в make-метод и делать throw?
  m_acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
    return;
  }

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
    return;
  }

  m_acceptor.bind(endpoint, ec);
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
    return;
  }

  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
    return;
  }
}

void Listener::start_accept() {
  m_acceptor.async_accept(m_socket,
                          boost::bind(&Listener::handle_accept, this,
                                      boost::asio::placeholders::error));
}

void Listener::handle_accept(boost::beast::error_code ec) {
  std::cout << "Accepting " << m_socket.remote_endpoint().address() << ":"
            << m_socket.remote_endpoint().port() << std::endl;
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
    return;
  } else {
    // HttpSession::run() controls it's lifetime
    std::make_shared<HttpSession>(std::move(m_socket), m_root_path)->run();
  }
  start_accept();
}
