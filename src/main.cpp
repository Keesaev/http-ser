#include "listener.h"
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  boost::asio::io_context context;
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::make_address_v4("0.0.0.0"), 8077);

  Listener ls(context, endpoint);

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
