#pragma once

#if defined(WIN32)
  #define OVERLAY_EXPORT __declspec(dllexport)
#else
  #define OVERLAY_EXPORT __attribute__((visibility("default")))
#endif
