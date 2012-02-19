/*
 * Spdylay - SPDY Library
 *
 * Copyright (c) 2012 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "spdylay_stream.h"

#include <assert.h>

void spdylay_stream_init(spdylay_stream *stream, int32_t stream_id,
                         uint8_t flags, uint8_t pri,
                         spdylay_stream_state initial_state,
                         void *stream_user_data)
{
  stream->stream_id = stream_id;
  stream->flags = flags;
  stream->pri = pri;
  stream->state = initial_state;
  stream->shut_flags = SPDYLAY_SHUT_NONE;
  stream->pushed_streams = NULL;
  stream->pushed_streams_length = 0;
  stream->pushed_streams_capacity = 0;
  stream->stream_user_data = stream_user_data;
  stream->deferred_data = NULL;
}

void spdylay_stream_free(spdylay_stream *stream)
{
  free(stream->pushed_streams);
  spdylay_outbound_item_free(stream->deferred_data);
  free(stream->deferred_data);
}

void spdylay_stream_shutdown(spdylay_stream *stream, spdylay_shut_flag flag)
{
  stream->shut_flags |= flag;
}

int spdylay_stream_add_pushed_stream(spdylay_stream *stream, int32_t stream_id)
{
  if(stream->pushed_streams_capacity == stream->pushed_streams_length) {
    int32_t *streams;
    size_t capacity = stream->pushed_streams_capacity == 0 ?
      5 : stream->pushed_streams_capacity*2;
    streams = realloc(stream->pushed_streams, capacity*sizeof(uint32_t));
    if(streams == NULL) {
      return SPDYLAY_ERR_NOMEM;
    }
    stream->pushed_streams = streams;
    stream->pushed_streams_capacity = capacity;
  }
  stream->pushed_streams[stream->pushed_streams_length++] = stream_id;
  return 0;
}

void spdylay_stream_defer_data(spdylay_stream *stream,
                               spdylay_outbound_item *data)
{
  assert(stream->deferred_data == NULL);
  stream->deferred_data = data;
}

void spdylay_stream_detach_deferred_data(spdylay_stream *stream)
{
  stream->deferred_data = NULL;
}
