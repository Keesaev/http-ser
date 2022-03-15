#include <boost/beast/http/file_body.hpp>

class NotFound : public std::exception {
public:
  NotFound() noexcept {}
  virtual const char *what() const noexcept final { return "Not found"; }
};

class Pages {
public:
  Pages() = delete;
  static bool contains(std::string const &path);
  static boost::beast::http::file_body::value_type get(std::string const &path);

private:
  const static std::string m_root;
};
