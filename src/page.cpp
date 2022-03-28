#include "page.h"
#include <boost/filesystem.hpp>
#include <memory>
#include <string>

#include <stdio.h>

std::unordered_map<std::string, std::string> Page::m_mimes{
    {".html", "text/html"}, {".css", "text/css"}};

Page::Page(const std::string &root_path, boost::string_view path)
    : m_root(root_path) {
  // Fill m_full_path; If empty, address is invalid
  if (path.empty()) {
    m_full_path = m_root + "index.html";
    return;
  }
  // Corner cases not to end up with paths like "../../some_private_data"
  if (path.find(" ") != std::string::npos ||
      path.find("..") != std::string::npos) {
    return;
  }
  std::string f_path{m_root + std::string(path.data(), path.size())};
  if (boost::filesystem::exists(f_path)) {
    if (boost::filesystem::is_directory(f_path)) {
      if (f_path.back() != '/')
        f_path.append("/");
      f_path.append("index.html");
    }
    m_full_path = f_path;
  }
}

bool Page::is_valid() { return !m_full_path.empty(); }

// If content of m_full_path is empty, address is not found
boost::beast::http::file_body::value_type Page::get() {
  if (m_full_path.empty()) {
    throw NotFound();
  }
  boost::beast::error_code ec;
  boost::beast::http::file_body::value_type body;
  body.open(m_full_path.c_str(), boost::beast::file_mode::scan, ec);

  if (ec == boost::system::errc::no_such_file_or_directory) {
    throw NotFound();
  }
  return body;
}

std::string Page::mime_type() {
  auto entry{m_mimes.find(boost::filesystem::extension(m_full_path))};
  if (entry == m_mimes.end())
    return "text/html";
  else
    return entry->second;
}
