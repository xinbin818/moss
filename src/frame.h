/**
 * Copyright 2019 Linkworld Open Team
 * */
#ifndef SRC_FRAME_H_
#define SRC_FRAME_H_
#include <cstddef>
#include <cstdint>

// variable-length integer type
typedef uint64_t vint;

// STREAM Frame
// type: 0x08 to 0x0f
// STREAM frames implicitly create a stream and carry stream data. The STREAM
// frame takes the form 0b00001XXX (or the set of values from 0x08 to 0x0f). The
// value of the three low-order bits of the frame type determines the fields
// that are present in the frame.
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         Stream ID (i)                       ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         [Offset (i)]                        ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         [Length (i)]                        ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        Stream Data (*)                      ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FrameStream {
  // frame type bits
  // * The OFF bit (0x04) in the frame type is set to indicate that there is an
  // Offset field present. When set to 1, the Offset field is present. When set
  // to 0, the Offset field is absent and the Stream Data starts at an offset of
  // 0 (that is, the frame contains the first bytes of the stream, or the end of
  // a stream that includes no data).
  // * The LEN bit (0x02) in the frame type is set to indicate that there is a
  // Length field present. If this bit is set to 0, the Length field is absent
  // and the Stream Data field extends to the end of the packet. If this bit is
  // set to 1, the Length field is present.
  // * The FIN bit (0x01) of the frame type
  // is set only on frames that contain the final size of the stream. Setting
  // this bit indicates that the frame marks the end of the stream.
  vint bits;
  // Stream ID: A variable-length integer indicating the stream ID of the
  // stream
  vint id;
  // Offset: A variable-length integer specifying the byte offset in the stream
  // for the data in this STREAM frame. This field is present when the OFF bit
  // is set to 1. When the Offset field is absent, the offset is 0.
  vint offset;
  // Length: A variable-length integer specifying the length of the Stream Data
  // field in this STREAM frame. This field is present when the LEN bit is set
  // to 1. When the LEN bit is set to 0, the Stream Data field consumes all the
  // remaining bytes in the packet.
  vint length;
  // Stream Data: The bytes from the designated stream to be delivered.
  char* stream_data;
};

// STREAM_DATA_BLOCKED Frame
// type: 0x15
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        Stream ID (i)                        ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                    Stream Data Limit (i)                    ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FrameStreamDataBlocked {
  // Stream ID: A variable-length integer indicating the stream which is flow
  // control blocked.
  vint stream_id;
  // Stream Data Limit: A variable-length integer indicating the offset of the
  // stream at which the blocking occurred.
  vint stream_data_limit;
};

// RESET_STREAM Frame
// type: 0x04
// An endpoint uses a RESET_STREAM frame (type=0x04) to abruptly terminate the
// sending part of a stream.
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        Stream ID (i)                        ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                  Application Error Code (i)                 ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        Final Size (i)                       ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FrameResetStream {
  // Stream ID: A variable-length integer encoding of the Stream ID of the
  // stream being terminated.
  vint stream_id;
  // Application Error Code: A variable-length integer containing the
  // application protocol error code which indicates why the stream is being
  // closed.
  vint error_code;
  // Final Size: A variable-length integer indicating the final size of the
  // stream by the RESET_STREAM sender, in unit of bytes.
  vint final_size;
};

// Convert data to frame.
// Example:
//    char *data;// where data received from the network
//    void *frame;
//    ConvertDataToFrame(data,sizeof(data),frame);
int ConvertDataToFrame(const char* data, const size_t len, void* frame);

// Convert frame to data.
// new memory outside the function
// Example:
//    size_t buff_size;
//    char* buff = new char[buff_size];
//    void* frame;
//    ConvertFrameToData(frame,buff,buff_size);
int ConvertFrameToData(const void* frame, const char* buff,
                       const size_t buff_size);

// Convert frame to data.
// new memory inside the function
// Example:
//    int* len;
//    char* data;
//    void* frame;
//    ConvertFrameToData(frame,data,len);
int ConvertFrameToData(const void* frame, char* data, size_t* len);

#endif  // SRC_FRAME_H_
