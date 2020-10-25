#include "emscripten_v1/html5.hpp"

#include "data_v1/array.hpp"
#include "data_v1/strided.hpp"

#include <algorithm>
#include <cstdio>
#include <deque>
#include <tuple>

namespace emscripten_v1 {

using namespace data_v1;

static std::deque<mouse_callback_t> s_mouse_callbacks;

static EM_BOOL
mouse_callback(int event_type, const EmscriptenMouseEvent *event, void *data) {
  return (*static_cast<mouse_callback_t *>(data))(
      mouse_event_type_t(event_type), *event);
}

static const std::tuple<EMSCRIPTEN_RESULT, const char *> s_results[] = {
    {EMSCRIPTEN_RESULT_SUCCESS, "SUCCESS"},
    {EMSCRIPTEN_RESULT_DEFERRED, "DEFERRED"},
    {EMSCRIPTEN_RESULT_NOT_SUPPORTED, "NOT_SUPPORTED"},
    {EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED, "FAILED_NOT_DEFERRED"},
    {EMSCRIPTEN_RESULT_INVALID_TARGET, "INVALID_TARGET"},
    {EMSCRIPTEN_RESULT_UNKNOWN_TARGET, "UNKNOWN_TARGET"},
    {EMSCRIPTEN_RESULT_INVALID_PARAM, "INVALID_PARAM"},
    {EMSCRIPTEN_RESULT_FAILED, "FAILED"},
    {EMSCRIPTEN_RESULT_NO_DATA, "NO_DATA"},
};

static void check_success(const char *function, EMSCRIPTEN_RESULT result) {
  if (EMSCRIPTEN_RESULT_SUCCESS != result) {
    auto it =
        std::find_if(begin(s_results), end(s_results), [=](const auto &t) {
          return std::get<0>(t) == result;
        });

    if (it != end(s_results))
      printf("%s: %s\n", function, std::get<1>(*it));
    else
      printf("%s: %d\n", function, result);

    exit(1);
  }
}

using mouse_event_setter_t = EMSCRIPTEN_RESULT (*)(
    const char *, void *, EM_BOOL, em_mouse_callback_func, pthread_t);

static const std::tuple<mouse_event_type_t, mouse_event_setter_t, const char *>
    s_events[] = {
        {CLICK,
         emscripten_set_click_callback_on_thread,
         "emscripten_set_click_callback"},
        {MOUSEDOWN,
         emscripten_set_mousedown_callback_on_thread,
         "emscripten_set_mousedown_callback"},
        {MOUSEUP,
         emscripten_set_mouseup_callback_on_thread,
         "emscripten_set_mouseup_callback"},
        {DBLCLICK,
         emscripten_set_dblclick_callback_on_thread,
         "emscripten_set_dblclick_callback"},
        {MOUSEMOVE,
         emscripten_set_mousemove_callback_on_thread,
         "emscripten_set_mousemove_callback"},
        {MOUSEENTER,
         emscripten_set_mouseenter_callback_on_thread,
         "emscripten_set_mouseenter_callback"},
        {MOUSELEAVE,
         emscripten_set_mouseleave_callback_on_thread,
         "emscripten_set_mouseleave_callback"},
};

} // namespace emscripten_v1

void emscripten_v1::set_mouse_callback(
    const char *target,
    const contiguous_t<const mouse_event_type_t> &event_types,
    const mouse_callback_t &callback,
    capture_t capture) {
  if (event_types.empty())
    return;

  s_mouse_callbacks.push_back(callback);

  for (auto event_type : event_types) {
    for (auto &event : s_events) {
      if (std::get<0>(event) == event_type) {
        check_success(
            std::get<2>(event),
            std::get<1>(event)(target,
                               &s_mouse_callbacks.back(),
                               capture,
                               mouse_callback,
                               EM_CALLBACK_THREAD_CONTEXT_CALLING_THREAD));

        break;
      }
    }
  }
}

static EM_BOOL animation_frame_loop_callback(double time, void *data) {
  auto *fn = static_cast<std::function<bool(double)> *>(data);
  bool result = (*fn)(time);
  if (!result)
    delete fn;
  return result;
}

void emscripten_v1::request_animation_frame_loop(
    const std::function<bool(double)> &callback) {
  auto data = new std::function<bool(double)>(callback);

  emscripten_request_animation_frame_loop(animation_frame_loop_callback, data);
}
