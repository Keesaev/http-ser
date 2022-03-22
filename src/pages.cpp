#include "pages.h"
#include <boost/filesystem.hpp>
#include <string>

Pages::Pages(const std::string &root_path) : m_root(root_path) {}

bool Pages::contains(boost::string_view path) {
  return true;

  if (path.empty())
    return true; // index.html

  // Corner cases not to end up with paths like "../../some_private_data"
  if (path.find(" ") != std::string::npos ||
      path.find("..") != std::string::npos || path.front() == '/') {
    return false;
  }
  if (boost::filesystem::exists(m_root + std::string(path.data(), path.size())))
    return true;
  else
    return false;
}

// Assuming that Pages::contains(path) -> true
boost::beast::http::file_body::value_type Pages::get(boost::string_view path) {
  std::string full_path = m_root + std::string(path.data(), path.size());
  if (path.empty() || boost::filesystem::is_directory(full_path))
    full_path.append("index.html");
  if (path.find(".html") == std::string::npos)
    full_path.append(".html");

  boost::beast::error_code ec;
  boost::beast::http::file_body::value_type body;
  // body.open(full_path.c_str(), boost::beast::file_mode::scan, ec);
  body.open("/home/keesaev/dev/http-ser/resources/pages/index.html",
            boost::beast::file_mode::scan, ec);

  if (ec == boost::system::errc::no_such_file_or_directory) {
    throw NotFound();
  }

  return body;
}
