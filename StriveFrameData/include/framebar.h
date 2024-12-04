#pragma once

#include "common.h"

class FrameBar {
  PIMPL

public:
  ~FrameBar();
  FrameBar();
  void addFrame();
  void reset();
  void draw();
};