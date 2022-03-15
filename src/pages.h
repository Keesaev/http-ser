#include <boost/beast/http/file_body.hpp>
#include <boost/utility/string_view.hpp>

// TODO PageException(message) -> send(bad_request(message))
class NotFound : public std::exception {
public:
  NotFound() noexcept {}
  virtual const char *what() const noexcept final { return "Not found"; }
};

class Pages {
public:
  Pages() = delete;
  static bool contains(boost::string_view path);
  static boost::beast::http::file_body::value_type get(boost::string_view path);

private:
  const static std::string m_root;
};
