#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
  HttpSession(boost::asio::ip::tcp::socket &&socket);
  ~HttpSession();
  void run();

private:
  boost::asio::ip::tcp::socket m_socket;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::string_body> m_request;

  void handle_read(boost::system::error_code er, std::size_t bytes);
  void handle_write(boost::system::error_code er, std::size_t bytes,
                    bool close);
  void handle_request();

  template <class Body>
  void send_response(boost::beast::http::response<Body> &&response);

  const std::string m_pages_root{"resources/pages/"};
};
