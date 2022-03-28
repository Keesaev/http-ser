#pragma once

#include <boost/beast/http/file_body.hpp>
#include <boost/utility/string_view.hpp>
#include <unordered_map>

// TODO PageException(message) -> send(bad_request(message))
class NotFound : public std::exception {
public:
  NotFound() noexcept {}
  virtual const char *what() const noexcept final { return "Not found"; }
};

class Page {
public:
  Page(const std::string &root_path, boost::string_view path);
  bool is_valid();
  boost::beast::http::file_body::value_type get();
  std::string mime_type();

private:
  const std::string m_root;
  std::string m_full_path;
  static std::unordered_map<std::string, std::string> m_mimes;
};
