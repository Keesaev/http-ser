#include "listener.h"
#include <iostream>
#include <thread>

// ASAN env
#ifndef _WIN32
extern "C" const char *__asan_default_options() {
  return "verify_asan_link_order=0";
}
#endif

int main(int argc, char *argv[]) {

  std::string root_path{"/home/keesaev/dev/http-ser/resources/pages/"};
  if (argc > 1) {
    root_path = argv[1];
  }
  Pages pages(root_path);

  std::cout << argc << std::endl;
  for (int i = 0; i < argc; i++)
    std::cout << argv[i] << std::endl;

  boost::asio::io_context context;
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::make_address_v4("0.0.0.0"), 8077);

  Listener ls(context, endpoint, std::move(pages));

  std::thread runner{[&context, &ls]() {
    ls.start_accept();
    context.run();
  }};

  while (true) {
    std::string str;
    std::cin >> str;

    if (str == "q") {
      break;
    }
  }

  context.stop();
  runner.join();
  return 0;
}
