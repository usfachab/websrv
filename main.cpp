#include "./server/Server.hpp"

int main() {
  Server srv;

  srv.CheckReadableSockets();

  return 0;
}
