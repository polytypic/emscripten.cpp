#pragma once

#include "emscripten_v1/config.hpp"

#include "data_v1/synopsis.hpp"

#include <emscripten/html5.h>

#include <cstdint>
#include <functional>

namespace emscripten_v1 {

// html5.h =====================================================================

enum capture_t : bool { NO_CAPTURE, USE_CAPTURE };

enum mouse_event_type_t : int {
  CLICK = EMSCRIPTEN_EVENT_CLICK,
  DBLCLICK = EMSCRIPTEN_EVENT_DBLCLICK,
  MOUSEDOWN = EMSCRIPTEN_EVENT_MOUSEDOWN,
  MOUSEENTER = EMSCRIPTEN_EVENT_MOUSEENTER,
  MOUSELEAVE = EMSCRIPTEN_EVENT_MOUSELEAVE,
  MOUSEMOVE = EMSCRIPTEN_EVENT_MOUSEMOVE,
  MOUSEUP = EMSCRIPTEN_EVENT_MOUSEUP,
};

using mouse_callback_t =
    std::function<bool(mouse_event_type_t, const EmscriptenMouseEvent &)>;

void set_mouse_callback(
    const char *target,
    const contiguous_t<const mouse_event_type_t> &event_types,
    const mouse_callback_t &callback,
    capture_t capture = NO_CAPTURE);

void request_animation_frame_loop(const std::function<bool(double)> &callback);

} // namespace emscripten_v1
