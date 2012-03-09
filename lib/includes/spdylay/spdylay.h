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
#ifndef SPDYLAY_H
#define SPDYLAY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include <spdylay/spdylayver.h>

struct spdylay_session;
typedef struct spdylay_session spdylay_session;

/* SPDY protocol version 2 */
#define SPDYLAY_PROTO_SPDY2 2
/* SPDY protocol version 3 */
#define SPDYLAY_PROTO_SPDY3 3

typedef enum {
  /* Invalid argument passed. */
  SPDYLAY_ERR_INVALID_ARGUMENT = -501,
  /* Zlib error. */
  SPDYLAY_ERR_ZLIB = -502,
  /* The specified protocol version is not supported. */
  SPDYLAY_ERR_UNSUPPORTED_VERSION = -503,
  /* Used as a return value from spdylay_send_callback and
     spdylay_recv_callback to indicate that the operation would
     block. */
  SPDYLAY_ERR_WOULDBLOCK = -504,
  /* General protocol error */
  SPDYLAY_ERR_PROTO = -505,
  /* The frame is invalid. */
  SPDYLAY_ERR_INVALID_FRAME = -506,
  /* The peer performed a shutdown on the connection. */
  SPDYLAY_ERR_EOF = -507,
  /* Used as a return value from spdylay_data_source_read_callback to
     indicate that data transfer is postponed. See
     spdylay_data_source_read_callback for details. */
  SPDYLAY_ERR_DEFERRED = -508,
  /* Stream ID has reached maximum value. Therefore no stream ID is
     available. */
  SPDYLAY_ERR_STREAM_ID_NOT_AVAILABLE = -509,
  /* The stream is already closed or it does not exist. */
  SPDYLAY_ERR_STREAM_CLOSED = -510,
  /* RST_STREAM has been queued in outbound queue. The stream is in
     closing state. */
  SPDYLAY_ERR_STREAM_CLOSING = -511,
  /* The transmission is not allowed for this stream (e.g., a frame
     with FIN flag set has already sent) */
  SPDYLAY_ERR_STREAM_SHUT_WR = -512,
  /* The stream ID is invalid. */
  SPDYLAY_ERR_INVALID_STREAM_ID = -513,
  /* The state of the stream is not valid (e.g., SYN_REPLY cannot be
     sent to the stream where SYN_REPLY has been already sent). */
  SPDYLAY_ERR_INVALID_STREAM_STATE = -514,
  /* Another DATA frame has already been deferred. */
  SPDYLAY_ERR_DEFERRED_DATA_EXIST = -515,
  /* SYN_STREAM is not allowed. (e.g., GOAWAY has been sent and/or
     received. */
  SPDYLAY_ERR_SYN_STREAM_NOT_ALLOWED = -516,
  /* GOAWAY has been already sent. */
  SPDYLAY_ERR_GOAWAY_ALREADY_SENT = -517,
  /* The errors < SPDYLAY_ERR_FATAL mean that the library is under
     unexpected condition that it cannot process any further data
     reliably (e.g., out of memory). */
  SPDYLAY_ERR_FATAL = -900,
  /* Out of memory. */
  SPDYLAY_ERR_NOMEM = -901,
  /* The user callback function failed. */
  SPDYLAY_ERR_CALLBACK_FAILURE = -902,
} spdylay_error;

typedef enum {
  SPDYLAY_MSG_MORE
} spdylay_io_flag;

typedef enum {
  SPDYLAY_SYN_STREAM = 1,
  SPDYLAY_SYN_REPLY = 2,
  SPDYLAY_RST_STREAM = 3,
  SPDYLAY_SETTINGS = 4,
  SPDYLAY_NOOP = 5,
  SPDYLAY_PING = 6,
  SPDYLAY_GOAWAY = 7,
  SPDYLAY_HEADERS = 8,
  /* Since SPDY/3 */
  SPDYLAY_WINDOW_UPDATE = 9,
  SPDYLAY_DATA = 100,
} spdylay_frame_type;

typedef enum {
  SPDYLAY_CTRL_FLAG_NONE = 0,
  SPDYLAY_CTRL_FLAG_FIN = 0x1,
  SPDYLAY_CTRL_FLAG_UNIDIRECTIONAL = 0x2
} spdylay_ctrl_flag;

typedef enum {
  SPDYLAY_DATA_FLAG_NONE = 0,
  SPDYLAY_DATA_FLAG_FIN = 0x1
} spdylay_data_flag;

typedef enum {
  SPDYLAY_FLAG_SETTINGS_NONE = 0,
  SPDYLAY_FLAG_SETTINGS_CLEAR_SETTINGS = 1
} spdylay_settings_flag;

typedef enum {
  SPDYLAY_ID_FLAG_SETTINGS_NONE = 0,
  SPDYLAY_ID_FLAG_SETTINGS_PERSIST_VALUE = 1,
  SPDYLAY_ID_FLAG_SETTINGS_PERSISTED = 2
} spdylay_settings_id_flag;

typedef enum {
  SPDYLAY_SETTINGS_UPLOAD_BANDWIDTH = 1,
  SPDYLAY_SETTINGS_DOWNLOAD_BANDWIDTH = 2,
  SPDYLAY_SETTINGS_ROUND_TRIP_TIME = 3,
  SPDYLAY_SETTINGS_MAX_CONCURRENT_STREAMS = 4,
  SPDYLAY_SETTINGS_CURRENT_CWND = 5,
  SPDYLAY_SETTINGS_DOWNLOAD_RETRANS_RATE = 6,
  SPDYLAY_SETTINGS_INITIAL_WINDOW_SIZE = 7,
  /* This first appeared in SPDY/3 */
  SPDYLAY_SETTINGS_CLIENT_CERTIFICATE_VECTOR_SIZE = 8
} spdylay_settings_id;

/* Maximum ID of spdylay_settings_id. */
#define SPDYLAY_SETTINGS_MAX 8

/* Default maximum concurrent streams */
#define SPDYLAY_CONCURRENT_STREAMS_MAX 100

/* Status code for RST_STREAM */
typedef enum {
  /* SPDYLAY_OK is not valid status code for RST_STREAM. It is defined
     just for spdylay library use. */
  SPDYLAY_OK = 0,
  SPDYLAY_PROTOCOL_ERROR = 1,
  SPDYLAY_INVALID_STREAM = 2,
  SPDYLAY_REFUSED_STREAM = 3,
  SPDYLAY_UNSUPPORTED_VERSION = 4,
  SPDYLAY_CANCEL = 5,
  SPDYLAY_INTERNAL_ERROR = 6,
  SPDYLAY_FLOW_CONTROL_ERROR = 7,
  /* Following status codes were introduced in SPDY/3 */
  SPDYLAY_STREAM_IN_USE = 8,
  SPDYLAY_STREAM_ALREADY_CLOSED = 9,
  SPDYLAY_INVALID_CREDENTIALS = 10,
  FRAME_TOO_LARGE = 11
} spdylay_status_code;

/* Status code for GOAWAY, introduced in SPDY/3 */
typedef enum {
  SPDYLAY_GOAWAY_OK = 0,
  SPDYLAY_GOAWAY_PROTOCOL_ERROR = 1,
  SPDYLAY_GOAWAY_INTERNAL_ERROR = 11
} spdylay_goaway_status_code;

#define SPDYLAY_SPDY2_PRI_LOWEST 3
#define SPDYLAY_SPDY3_PRI_LOWEST 7

typedef struct {
  uint16_t version;
  uint16_t type;
  uint8_t flags;
  int32_t length;
} spdylay_ctrl_hd;

typedef struct {
  spdylay_ctrl_hd hd;
  int32_t stream_id;
  int32_t assoc_stream_id;
  /* 0 (Highest) to SPDYLAY_SPDY2_PRI_LOWEST or
     SPDYLAY_SPDY3_PRI_LOWEST (loweset), depending on the protocol
     version. */
  uint8_t pri;
  /* Since SPDY/3 */
  uint8_t slot;
  char **nv;
} spdylay_syn_stream;

typedef struct {
  spdylay_ctrl_hd hd;
  int32_t stream_id;
  char **nv;
} spdylay_syn_reply;

typedef struct {
  spdylay_ctrl_hd hd;
  int32_t stream_id;
  char **nv;
} spdylay_headers;

typedef struct {
  spdylay_ctrl_hd hd;
  int32_t stream_id;
  uint32_t status_code;
} spdylay_rst_stream;

typedef struct {
  int32_t settings_id;
  uint8_t flags;
  uint32_t value;
} spdylay_settings_entry;

typedef struct {
  spdylay_ctrl_hd hd;
  /* Number of entries in |iv| */
  size_t niv;
  spdylay_settings_entry *iv;
} spdylay_settings;

typedef struct {
  spdylay_ctrl_hd hd;
  uint32_t unique_id;
} spdylay_ping;

typedef struct {
  spdylay_ctrl_hd hd;
  int32_t last_good_stream_id;
  /* Since SPDY/3 */
  uint32_t status_code;
} spdylay_goaway;

/* WINDOW_UPDATE is introduced since SPDY/3 */
typedef struct {
  spdylay_ctrl_hd hd;
  int32_t stream_id;
  int32_t delta_window_size;
} spdylay_window_update;

typedef union {
  int fd;
  void *ptr;
} spdylay_data_source;

/*
 * Callback function invoked when the library wants to read data from
 * |source|. The read data is sent in the stream |stream_id|. The
 * implementation of this function must read at most |length| bytes of
 * data from |source| (or possibly other places) and store them in
 * |buf| and return number of data stored in |buf|. If EOF is reached,
 * set |*eof| to 1.  If the application wants to postpone DATA frames,
 * (e.g., asynchronous I/O, or reading data blocks for long time), it
 * is achieved by returning SPDYLAY_ERR_DEFERRED without reading any
 * data in this invocation.  The library removes DATA frame from
 * outgoing queue temporarily.  To move back deferred DATA frame to
 * outgoing queue, call spdylay_session_resume_data().  In case of
 * error, return SPDYLAY_ERR_CALLBACK_FAILURE, which leads to session
 * failure.
 */
typedef ssize_t (*spdylay_data_source_read_callback)
(spdylay_session *session, int32_t stream_id,
 uint8_t *buf, size_t length, int *eof,
 spdylay_data_source *source, void *user_data);

typedef struct {
  spdylay_data_source source;
  spdylay_data_source_read_callback read_callback;
} spdylay_data_provider;

typedef struct {
  int32_t stream_id;
  uint8_t flags;
  /* Initially eof is 0. It becomes 1 if all data are read. */
  uint8_t eof;
  spdylay_data_provider data_prd;
} spdylay_data;

typedef union {
  spdylay_syn_stream syn_stream;
  spdylay_syn_reply syn_reply;
  spdylay_rst_stream rst_stream;
  spdylay_settings settings;
  spdylay_ping ping;
  spdylay_goaway goaway;
  spdylay_headers headers;
  /* Since SPDY/3 */
  spdylay_window_update window_update;
  spdylay_data data;
} spdylay_frame;

/*
 * Callback function invoked when |session| want to send data to
 * remote peer. The implementation of this function must send at most
 * |length| bytes of data stored in |data|. It must return the number
 * of bytes sent if it succeeds.  If it cannot send any single byte
 * without blocking, it must return SPDYLAY_ERR_WOULDBLOCK. For other
 * errors, it must return SPDYLAY_ERR_CALLBACK_FAILURE.
 */
typedef ssize_t (*spdylay_send_callback)
(spdylay_session *session,
 const uint8_t *data, size_t length, int flags, void *user_data);

/*
 * Callback function invoked when |session| want to receive data from
 * remote peer. The implementation of this function must read at most
 * |length| bytes of data and store it in |buf|. It must return the
 * number of bytes written in |buf| if it succeeds. If it cannot read
 * any single byte without blocking, it must return
 * SPDYLAY_ERR_WOULDBLOCK. If it gets EOF before it reads any single
 * byte, it must return SPDYLAY_ERR_EOF. For other errors, it must
 * return SPDYLAY_ERR_CALLBACK_FAILURE.
 */
typedef ssize_t (*spdylay_recv_callback)
(spdylay_session *session,
 uint8_t *buf, size_t length, int flags, void *user_data);

/*
 * Callback function invoked by spdylay_session_recv() when a control
 * frame is arrived.
 */
typedef void (*spdylay_on_ctrl_recv_callback)
(spdylay_session *session, spdylay_frame_type type, spdylay_frame *frame,
 void *user_data);

/*
 * Callback function invoked by spdylay_session_recv() when an invalid
 * control frame is arrived, which typically the case where RST_STREAM
 * will be sent
 */
typedef void (*spdylay_on_invalid_ctrl_recv_callback)
(spdylay_session *session, spdylay_frame_type type, spdylay_frame *frame,
 void *user_data);

/*
 * Callback function invoked when data chunk of DATA frame is
 * received. |stream_id| is the stream ID of this DATA frame belongs
 * to. |flags| is the flags of DATA frame which this data chunk is
 * contained. flags & SPDYLAY_DATA_FLAG_FIN does not necessarily mean
 * this chunk of data is the last one in the stream. You should use
 * spdylay_on_data_recv_callback to know all data frames are received.
 */
typedef void (*spdylay_on_data_chunk_recv_callback)
(spdylay_session *session, uint8_t flags, int32_t stream_id,
 const uint8_t *data, size_t len, void *user_data);

/*
 * Callback function invoked when DATA frame is received. The actual
 * data it contains are received by spdylay_on_data_recv_callback.
 */
typedef void (*spdylay_on_data_recv_callback)
(spdylay_session *session, uint8_t flags, int32_t stream_id, int32_t length,
 void *user_data);

/*
 * Callback function invoked after frame |frame| of type |type| is
 * sent.
 */
typedef void (*spdylay_on_ctrl_send_callback)
(spdylay_session *session, spdylay_frame_type type, spdylay_frame *frame,
 void *user_data);

/*
 * Callback function invoked after the control frame |frame| of type
 * |type| is not sent because of the error. The error is indicated by
 * the |error|, which is one of the values defined in spdylay_error.
 */
typedef void (*spdylay_on_ctrl_not_send_callback)
(spdylay_session *session, spdylay_frame_type type, spdylay_frame *frame,
 int error, void *user_data);

/*
 * Callback function invoked after DATA frame is sent.
 */
typedef void (*spdylay_on_data_send_callback)
(spdylay_session *session, uint8_t flags, int32_t stream_id, int32_t length,
 void *user_data);

/*
 * Callback function invoked before frame |frame| of type |type| is
 * sent. This may be useful, for example, to know the stream ID of
 * SYN_STREAM frame (see also spdylay_session_get_stream_user_data),
 * which is not assigned when it was queued.
 */
typedef void (*spdylay_before_ctrl_send_callback)
(spdylay_session *session, spdylay_frame_type type, spdylay_frame *frame,
 void *user_data);

/*
 * Callback function invoked when stream |stream_id| is closed. The
 * reason of closure is indicated by |status_code|. stream_user_data
 * is still available in this function.
 */
typedef void (*spdylay_on_stream_close_callback)
(spdylay_session *session, int32_t stream_id, spdylay_status_code status_code,
 void *user_data);

/*
 * Callback function invoked when request from remote peer is
 * received.  In other words, frame with FIN flag set is received.  In
 * HTTP, this means HTTP request, including request body, is fully
 * received.
 */
typedef void (*spdylay_on_request_recv_callback)
(spdylay_session *session, int32_t stream_id, void *user_data);

typedef struct {
  spdylay_send_callback send_callback;
  spdylay_recv_callback recv_callback;
  spdylay_on_ctrl_recv_callback on_ctrl_recv_callback;
  spdylay_on_invalid_ctrl_recv_callback on_invalid_ctrl_recv_callback;
  spdylay_on_data_chunk_recv_callback on_data_chunk_recv_callback;
  spdylay_on_data_recv_callback on_data_recv_callback;
  spdylay_before_ctrl_send_callback before_ctrl_send_callback;
  spdylay_on_ctrl_send_callback on_ctrl_send_callback;
  spdylay_on_ctrl_not_send_callback on_ctrl_not_send_callback;
  spdylay_on_data_send_callback on_data_send_callback;
  spdylay_on_stream_close_callback on_stream_close_callback;
  spdylay_on_request_recv_callback on_request_recv_callback;
} spdylay_session_callbacks;

/*
 * Initializes |*session_ptr| for client use, using the protocol
 * version |version|. The all members of |callbacks| are copied to
 * |*session_ptr|. Therefore |*session_ptr| does not store
 * |callbacks|. |user_data| is an arbitrary user supplied data, which
 * will be passed to the callback functions.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 * SPDYLAY_ERR_ZLIB
 *     The z_stream initialization failed.
 * SPDYLAY_ERR_UNSUPPORTED_VERSION
 *     The version is not supported.
 */
int spdylay_session_client_new(spdylay_session **session_ptr,
                               uint16_t version,
                               const spdylay_session_callbacks *callbacks,
                               void *user_data);

/*
 * Initializes |*session_ptr| for server use, using the protocol
 * version |version|. The all members of |callbacks| are copied to
 * |*session_ptr|. Therefore |*session_ptr| does not store
 * |callbacks|. |user_data| is an arbitrary user supplied data, which
 * will be passed to the callback functions.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 * SPDYLAY_ERR_ZLIB
 *     The z_stream initialization failed.
 * SPDYLAY_ERR_UNSUPPORTED_VERSION
 *     The version is not supported.
 */
int spdylay_session_server_new(spdylay_session **session_ptr,
                               uint16_t version,
                               const spdylay_session_callbacks *callbacks,
                               void *user_data);

/*
 * Frees any resources allocated for |session|.
 */
void spdylay_session_del(spdylay_session *session);

/*
 * Sends pending frames to the remote peer.
 *
 * This function retrieves the highest prioritized frame from the
 * outbound queue and sends it to the remote peer. It does this as
 * many as possible until the user callback send_callback returns
 * SPDYLAY_ERR_WOULDBLOCK or the outbound queue becomes empty.  This
 * function calls several callback functions which are passed when
 * initializing the |session|. Here is the simple time chart which
 * tells when each callback is invoked:
 *
 * 1. Get the next frame to send from outbound queue.
 * 2. Prepare transmission of the frame.
 * 3. If the control frame cannot be sent because some preconditions
 *    are not met (e.g., SYN_STREAM cannot be sent after GOAWAY),
 *    on_ctrl_not_send_callback is invoked. Skip following steps.
 * 4. If the frame is SYN_STREAM, the stream is opened here.
 * 5. before_ctrl_send_callback is invoked.
 * 6. send_callback is invoked one or more times (while the frame is
 *    completely sent).
 * 7. If the frame is a control frame, on_ctrl_send_callback is invoked.
 * 8. If the frame is a DATA frame, on_data_send_callback is invoked.
 * 9. If the transmission of the frame triggers closure of the stream,
 *    the stream is closed and on_stream_close_callback is invoked.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 * SPDYLAY_ERR_CALLBACK_FAILURE
 *     The callback function failed.
 */
int spdylay_session_send(spdylay_session *session);

/*
 * Receives frames from the remote peer.
 *
 * This function receives as many frames as possible until the user
 * callback recv_callback returns SPDYLAY_ERR_WOULDBLOCK. This
 * function calls several callback functions which are passed when
 * initializing the |session|. Here is the simple time chart which
 * tells when each callback is invoked:
 *
 * 1. recv_callback is invoked one or more times to receive frame header.
 * 2. If the frame is DATA frame:
 *   2.1. recv_callback is invoked to receive DATA payload. For each
 *        chunk of data, on_data_chunk_recv_callback is invoked.
 *   2.2. If one DATA frame is completely received, on_data_recv_callback
 *        is invoked.
 *        If the frame is the final frame of the request,
 *        on_request_recv_callback is invoked.
 *        If the reception of the frame triggers the closure of the stream,
 *        on_stream_close_callback is invoked.
 * 3. If the frame is the control frame:
 *   3.1. recv_callback is invoked one or more times to receive whole frame.
 *   3.2. If the received frame is valid, on_ctrl_recv_callback is invoked.
 *        If the frame is the final frame of the request,
 *        on_request_recv_callback is invoked.
 *        If the reception of the frame triggers the closure of the stream,
 *        on_stream_close_callback is invoked.
 *   3.3. If the received frame is unpacked but is interpreted as invalid,
 *        on_invalid_ctrl_recv_callback is invoked.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_EOF
 *     The remote peer did shutdown on the connection.
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 * SPDYLAY_ERR_CALLBACK_FAILURE
 *     The callback function failed.
 */
int spdylay_session_recv(spdylay_session *session);

/*
 * Put back previously deferred DATA frame in the stream |stream_id|
 * to outbound queue.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_INVALID_ARGUMENT
 *     The stream does not exist or no deferred data exist.
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_session_resume_data(spdylay_session *session, int32_t stream_id);

/*
 * Returns nonzero value if |session| want to receive data from the
 * remote peer.
 *
 * If both spdylay_session_want_read() and
 * spdylay_session_want_write() return 0, the application should drop
 * the connection.
 */
int spdylay_session_want_read(spdylay_session *session);

/*
 * Returns nonzero value if |session| want to send data to the remote
 * peer, or 0.
 *
 * If both spdylay_session_want_read() and
 * spdylay_session_want_write() return 0, the application should drop
 * the connection.
 */
int spdylay_session_want_write(spdylay_session *session);

/*
 * Returns stream_user_data for the stream |stream_id|. The
 * stream_user_data is provided by spdylay_submit_request().  If the
 * stream is initiated by the remote endpoint, stream_user_data is
 * always NULL. If the stream is initiated by the local endpoint and
 * NULL is given in spdylay_submit_request(), then this function
 * returns NULL. If the stream does not exist, this function returns
 * NULL.
 */
void* spdylay_session_get_stream_user_data(spdylay_session *session,
                                           int32_t stream_id);

/*
 * Submits SYN_STREAM frame and optionally one or more DATA
 * frames.
 *
 * |pri| is priority of this request. 0 is the highest priority.  If
 * the |session| is initialized with the version SPDYLAY_PROTO_SPDY2,
 * the lowest priority is 3.  If the |session| is initialized with the
 * version SPDYLAY_PROTO_SPDY3, the lowest priority is 7.
 *
 * The |nv| must include following name/value pairs:
 *
 * ":method"
 *     HTTP method (e.g., "GET", "POST", "HEAD", etc)
 * ":scheme"
 *     URI scheme (e.g., "https")
 * ":path"
 *     Absolute path and parameters of this request (e.g., "/foo",
 *     "/foo;bar;haz?h=j&y=123")
 * ":version"
 *     HTTP version (e.g., "HTTP/1.1")
 * ":host"
 *     The hostport portion of the URI for this request (e.g.,
 *     "example.org:443"). This is the same as the HTTP "Host" header
 *     field.
 *
 * If the |session| is initialized with the version
 * SPDYLAY_PROTO_SPDY2, the above names are translated to "method",
 * "scheme", "url", "version" and "host" respectively.
 *
 * This function creates copies of all name/value pairs in |nv|.  It
 * also lower-cases all names in |nv|.
 *
 * If |data_prd| is not NULL, it provides data which will be sent in
 * subsequent DATA frames. In this case, a method that allows request
 * message bodies
 * (http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html#sec9) must
 * be specified with "method" key in |nv| (e.g. POST). If |data_prd|
 * is NULL, SYN_STREAM have FLAG_FIN. The |stream_user_data| is data
 * associated to the stream opened by this request and can be an
 * arbitrary pointer, which can be retrieved by
 * spdylay_session_get_stream_user_data().
 *
 * Since the library reorders the frames and tries to send the highest
 * prioritized one first and the SPDY specification requires the
 * stream ID must be strictly increasing, the stream ID of this
 * request cannot be known until it is about to sent.  To know the
 * stream ID of the request, the application can use
 * before_ctrl_send_callback. This callback is called just before the
 * frame is sent. For SYN_STREAM frame, the argument frame has stream
 * ID assigned. Also since the stream is already opened,
 * spdylay_session_get_stream_user_data() can be used to get
 * |stream_user_data| to identify which SYN_STREAM we are processing.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_INVALID_FRAME
 *     |pri| is invalid.
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_request(spdylay_session *session, uint8_t pri,
                           const char **nv,
                           const spdylay_data_provider *data_prd,
                           void *stream_user_data);

/*
 * Submits SYN_REPLY frame and optionally one or more DATA frames
 * against stream |stream_id|.
 *
 * The |nv| must include following name/value pairs:
 *
 * ":status"
 *     HTTP status code (e.g., "200" or "200 OK")
 * ":version"
 *     HTTP response version (e.g., "HTTP/1.1")
 *
 * If the |session| is initialized with the version
 * SPDYLAY_PROTO_SPDY2, the above names are translated to "status" and
 * "version" respectively.
 *
 * This function creates copies of all name/value pairs in |nv|.  It
 * also lower-cases all names in |nv|.
 *
 * If |data_prd| is not NULL, it provides data which will be sent in
 * subsequent DATA frames. If |data_prd| is NULL, SYN_REPLY will have
 * FLAG_FIN.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_response(spdylay_session *session,
                            int32_t stream_id, const char **nv,
                            const spdylay_data_provider *data_prd);

/*
 * Submits SYN_STREAM frame. The |flags| is bitwise OR of the
 * following values:
 *
 * SPDYLAY_CTRL_FLAG_FIN
 * SPDYLAY_CTRL_FLAG_UNIDIRECTIONAL
 *
 * If |flags| includes SPDYLAY_CTRL_FLAG_FIN, this frame has FIN flag
 * set.
 *
 * The |assoc_stream_id| is used for server-push. If |session| is
 * initialized for client use, |assoc_stream_id| is ignored. The |pri|
 * is the priority of this frame and it must be between 0 and 3,
 * inclusive. 0 is the highest. The |nv| is the name/value pairs in
 * this frame. The |stream_user_data| is a pointer to an arbitrary
 * data which is associated to the stream this frame will open.
 *
 * This function is low-level in a sense that the application code can
 * specify flags and assoc-stream-ID directly. For usual HTTP request,
 * spdylay_submit_request() is useful.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_INVALID_FRAME
 *     |pri| is invalid.
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_syn_stream(spdylay_session *session, uint8_t flags,
                              int32_t assoc_stream_id, uint8_t pri,
                              const char **nv, void *stream_user_data);

/*
 * Submits SYN_REPLY frame. The |flags| is bitwise OR of the following
 * values:
 *
 * SPDYLAY_CTRL_FLAG_FIN
 *
 * If |flags| includes SPDYLAY_CTRL_FLAG_FIN, this frame has FIN flag
 * set.
 *
 * The stream this frame belongs to is given in |stream_id|. The |nv|
 * is the name/value pairs in this frame.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_syn_reply(spdylay_session *session, uint8_t flags,
                             int32_t stream_id, const char **nv);

/*
 * Submits HEADERS frame. The |flags| is bitwise OR of the following
 * values:
 *
 * SPDYLAY_CTRL_FLAG_FIN
 *
 * If |flags| includes SPDYLAY_CTRL_FLAG_FIN, this frame has FIN flag
 * set.
 *
 * The stream this frame belongs to is given in |stream_id|. The |nv|
 * is the name/value pairs in this frame.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_headers(spdylay_session *session, uint8_t flags,
                           int32_t stream_id, const char **nv);

/*
 * Submits 1 or more DATA frames to the stream |stream_id|.  The data
 * to be sent are provided by |data_prd|.  Depending on the length of
 * data, 1 or more DATA frames will be sent.  If |flags| contains
 * SPDYLAY_DATA_FLAG_FIN, the last DATA frame has FLAG_FIN set.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_data(spdylay_session *session, int32_t stream_id,
                        uint8_t flags, const spdylay_data_provider *data_prd);

/*
 * Submits RST_STREAM frame to cancel/reject stream |stream_id| with
 * status code |status_code|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_rst_stream(spdylay_session *session, int32_t stream_id,
                              uint32_t status_code);

/*
 * Submits PING frame.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_ping(spdylay_session *session);

/*
 * Submits GOAWAY frame. The status code |status_code| is ignored if
 * the protocol version is SPDYLAY_PROTO_SPDY2.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * SPDYLAY_ERR_NOMEM
 *     Out of memory.
 */
int spdylay_submit_goaway(spdylay_session *session, uint32_t status_code);

/*
 * A helper function for dealing with NPN in client side.
 * |in| contains server's protocol in preferable order.
 * The format of |in| is length-prefixed and not null-terminated.
 * For example, "spdy/2" are "http/1.1" stored in |in| like this:
 *
 *  in[0] = 6
 *  in[1..6] = "spdy/2"
 *  in[7] = 8
 *  in[8..15] = "http/1.1"
 *  inlen = 16
 *
 * The selection algorithm is as follows:
 *
 * 1. If server's list contains SPDY versions the spdylay library
 *    supports, this function selects one of them and returns its SPDY
 *    protocol version which can be used directly with
 *    spdylay_session_client_new() and spdylay_session_server_new()
 *    . The following steps are not taken.
 *
 * 2. If server's list contains "http/1.1", this function selects
 *    "http/1.1" and returns 0. The following step is not taken.
 *
 * 3. This function selects nothing and returns -1. (So called
 *    non-overlap case). In this case, |out| and |outlen| are left
 *    untouched.
 *
 * When spdylay supports updated version of SPDY in the future, this
 * function may select updated protocol and application code which
 * relies on spdylay for SPDY stuff needs not be modified.
 *
 * Selecting "spdy/2" means that "spdy/2" is written into |*out| and
 * length of "spdy/2" (which is 6) is assigned to |*outlen|.
 *
 * See http://technotes.googlecode.com/git/nextprotoneg.html for more
 * details about NPN.
 *
 * To use this method you should do something like:
 *
 * static int select_next_proto_cb(SSL* ssl,
 *                                 unsigned char **out, unsigned char *outlen,
 *                                 const unsigned char *in, unsigned int inlen,
 *                                 void *arg)
 * {
 *   int version;
 *   version = spdylay_select_next_protocol(out, outlen, in, inlen);
 *   if(version > 0) {
 *     ((MyType*)arg)->spdy_version = version;
 *   }
 *   return SSL_TLSEXT_ERR_OK;
 * }
 * ...
 * SSL_CTX_set_next_proto_select_cb(ssl_ctx, select_next_proto_cb, my_obj);
 */
int spdylay_select_next_protocol(unsigned char **out, unsigned char *outlen,
                                 const unsigned char *in, unsigned int inlen);

/*
 * Returns spdy version which spdylay library supports from given
 * protocol name. The |proto| is the pointer to the protocol name and
 * |protolen| is its length. Currently, "spdy/2" and "spdy/3" are
 * supported.
 *
 * This function returns nonzero spdy version if it succeeds, or 0.
 */
uint16_t spdylay_npn_get_version(const unsigned char *proto, size_t protolen);

#ifdef __cplusplus
}
#endif

#endif /* SPDYLAY_H */
