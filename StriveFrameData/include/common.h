#pragma once
#include <memory>

#define PIMPL                  \
  struct Data;                 \
  std::unique_ptr<Data> data;  \
  Data &d() { return *data; }; \
  Data const &d() const { return *data; };