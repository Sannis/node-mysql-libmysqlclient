/*!
 * Use this header file to conditionally invoke different libev/libeio/libuv functions
 * depending on the node version that the module is being compiled for.
 *
 * Copyright (c) 2012, Oleg Efimov <efimovov@gmail.com>
 * Copyright (c) 2011-2012, Nathan Rajlich <nathan@tootallnate.net>
 * Original gist by Nathan: https://gist.github.com/1368935
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <node.h>
#include <node_version.h>

/* Node IO watching compatibility */
#if NODE_VERSION_AT_LEAST(0, 7, 9)
  #define NODE_ADDON_SHIM_START_IO_WATCH(_data, after, fd, events) \
    uv_poll_t* handle = new uv_poll_t; \
    handle->data = _data; \
    uv_poll_init(uv_default_loop(), handle, fd); \
    uv_poll_start(handle, events, after);
  #define NODE_ADDON_SHIM_STOP_IO_WATCH(on_close) \
    uv_poll_stop(handle); \
    uv_close((uv_handle_t *) handle, on_close);
  #define NODE_ADDON_SHIM_STOP_IO_WATCH_ONCLOSE(on_close) \
  static void on_close(uv_handle_t* handle) { \
      delete handle; \
  }
#else
  #define NODE_ADDON_SHIM_START_IO_WATCH(_data, after, fd, events) \
    ev_io* io_watcher = new ev_io; \
    io_watcher->data = _data; \
    ev_init(io_watcher, after); \
    ev_io_set(io_watcher, fd, events); \
    ev_io_start(EV_DEFAULT_UC_ io_watcher);
  #define NODE_ADDON_SHIM_STOP_IO_WATCH(on_close) \
    ev_io_stop(EV_DEFAULT_UC_ io_watcher); \
    delete io_watcher;
  #define NODE_ADDON_SHIM_STOP_IO_WATCH_ONCLOSE(on_close)
#endif

#if NODE_VERSION_AT_LEAST(0, 7, 9)
    #define NODE_ADDON_SHIM_IO_WATCH_CALLBACK_ARGUMENTS \
      uv_poll_t* handle, int status, int events
#else
    #define NODE_ADDON_SHIM_IO_WATCH_CALLBACK_ARGUMENTS \
      EV_P_ ev_io *io_watcher, int events
#endif
