#include "http_session.h"
#include "page.h"

#include <iostream>

#include <boost/beast/version.hpp>

namespace http = boost::beast::http;

HttpSession::HttpSession(boost::asio::ip::tcp::socket &&socket,
                         std::string const &root_path)
    : m_socket(std::move(socket)), m_root_path(root_path) {}

void HttpSession::run() {
  // Capturing shared_from_this to extend lifetime
  http::async_read(m_socket, m_buffer, m_request,
                   [self = shared_from_this()](boost::system::error_code ec,
                                               std::size_t bytes) {
                     self->handle_read(ec, bytes);
                   });
}

void HttpSession::handle_read(boost::system::error_code ec, std::size_t bytes) {
  // This means they closed the connection
  if (ec == http::error::end_of_stream) {
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    return;
  }

  // Handle the error, if any
  if (ec) {
    std::cerr << ec << ec.message() << std::endl;
  }

  handle_request();
}

void HttpSession::handle_write(boost::system::error_code ec, std::size_t bytes,
                               bool close) {
  // Handle the error, if any
  if (ec)
    std::cerr << ec.message() << std::endl;

  if (close) {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    return;
  }

  // Clear contents of the request message,
  // otherwise the read behavior is undefined.
  m_request = {};

  // Read another request
  http::async_read(m_socket, m_buffer, m_request,
                   [self = shared_from_this()](boost::system::error_code ec,
                                               std::size_t bytes) {
                     self->handle_read(ec, bytes);
                   });
}

template <class Body>
void HttpSession::send_response(http::response<Body> &&response) {
  using response_type = typename std::decay<decltype(response)>::type;
  auto sp = std::make_shared<response_type>(
      std::forward<http::response<Body>>(response));

  // Write the response
  auto self = shared_from_this();
  http::async_write(
      m_socket, *sp,
      [self, sp](boost::system::error_code ec, std::size_t bytes) {
        self->handle_write(ec, bytes, sp->need_eof());
      });
}

void HttpSession::handle_request() {
  std::cout << "Request\t" << m_request.method_string() << " target:\t'"
            << m_request.target() << "'" << std::endl;

  // Returns a bad request response
  auto const bad_request = [req = m_request](boost::beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = why.to_string();
    res.prepare_payload();
    std::cout << "Response:\tBad request: " << why << std::endl;
    return res;
  };

  Page page(m_root_path, m_request.target());

  if (!page.is_valid())
    send_response(bad_request("Not found"));
  // HEAD / GET
  try {
    auto body = page.get();
    auto const size = body.size();

    // Respond to HEAD request
    if (m_request.method() == http::verb::head) {
      http::response<http::empty_body> res{http::status::ok,
                                           m_request.version()};
      res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(http::field::content_type, "text/html");
      res.content_length(size);
      res.keep_alive(m_request.keep_alive());
      send_response(std::move(res));
    } else {
      // Respond to GET request
      http::response<http::file_body> res{

          std::piecewise_construct, std::make_tuple(std::move(body)),
          std::make_tuple(http::status::ok, m_request.version())};
      res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(http::field::content_type, page.mime_type());
      res.content_length(size);
      res.keep_alive(m_request.keep_alive());
      send_response(std::move(res));
    }
  } catch (NotFound &ex) {
    send_response(bad_request("Not found"));
  }
}
