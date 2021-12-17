#include <pxr/usd/usd/stage.h>

int main(int argc, char **argv) {
  pxr::UsdStageRefPtr p;
  p->Save();
  throw std::runtime_error("wtf?");
  return 1;
}
