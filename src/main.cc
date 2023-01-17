#include "Application.hh"

int main() {
  auto app = new Application();
  app->run();
  delete app;
}
