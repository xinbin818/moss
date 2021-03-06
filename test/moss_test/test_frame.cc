/**
 * Copyright 2019 Linkworld Open Team
 * */
#include <cstring>
#include "./frame.h"
#include "gtest/gtest.h"

TEST(Frame, StreamFrameConvert) {
  FrameStream frame;
  frame.bits = 0x07;
  frame.id = 0;
  frame.offset = 64;
  frame.length = 5;
  frame.stream_data = new char[5]{0x01, 0x02, 0x03, 0x04, 0x05};
  GenericFrameLayout gfl;
  EXPECT_EQ(ConvertFrameToGFL(&frame, FrameType::kStream, &gfl), 0);
  EXPECT_EQ(gfl.frame_type_bits, 0x07 | 0x08);
  char gfl_data[] = {
      0x00,                         // Stream ID
      0x40, 0x40,                   // Offset
      0x05,                         // Length
      0x01, 0x02, 0x03, 0x04, 0x05  // Stream Data
  };
  EXPECT_EQ(gfl.data_len, 9);
  EXPECT_EQ(memcmp(gfl.data, gfl_data, 9), 0);
  //  test convert bin to frame;
  FrameStream recv_frame;
  FrameType TypeFrame;
  ConvertGFLToFrame(&gfl, &recv_frame, &TypeFrame);
  EXPECT_EQ(TypeFrame, FrameType::kStream);

  EXPECT_EQ(recv_frame.bits, 0x07 | 0x08);
  EXPECT_EQ(recv_frame.id, 0);
  EXPECT_EQ(recv_frame.length, 5);
  EXPECT_EQ(recv_frame.offset, 64);
}

TEST(Frame, StreamFrameConvertWithoutData) {
  FrameStream frame;
  frame.bits = 0x4;
  frame.id = 1073741823;               // 4 bytes
  frame.offset = 4611686018427387903;  // 8 bytes
  frame.stream_data = new char[5]{0x01, 0x02, 0x03, 0x04,
                                  0x05};  // test: do not contain data in gfl
  GenericFrameLayout gfl;
  EXPECT_EQ(ConvertFrameToGFL(&frame, FrameType::kStream, &gfl), 0);
  EXPECT_EQ(gfl.frame_type_bits, 0x04 | 0x08);
  char gfl_data[] = {
      0xbf, 0xff, 0xff, 0xff,                         // Stream ID
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff  // Offset
  };
  EXPECT_EQ(gfl.data_len, 12);
  EXPECT_EQ(memcmp(gfl.data, gfl_data, 12), 0);
  //  test convert bin to frame;
  FrameStream recv_frame;
  FrameType TypeFrame;
  ConvertGFLToFrame(&gfl, &recv_frame, &TypeFrame);
  EXPECT_EQ(TypeFrame, FrameType::kStream);
  EXPECT_EQ(recv_frame.bits, 0x04 | 0x08);
  EXPECT_EQ(recv_frame.id, 1073741823);
  EXPECT_EQ(recv_frame.offset, 4611686018427387903);
}

TEST(Frame, StreamDataBlockedFrameConvert) {
  FrameStreamDataBlocked frame;
  frame.stream_id = 12345;
  frame.stream_data_limit = 0;
  GenericFrameLayout gfl;
  EXPECT_EQ(ConvertFrameToGFL(&frame, FrameType::kStreamDataBlocked, &gfl), 0);
  EXPECT_EQ(gfl.frame_type_bits, 0x15);
  char gfl_data[] = {
      0x70, 0x39,  // Stream ID
      0x00         // Stream Data Limit
  };
  EXPECT_EQ(gfl.data_len, 3);
  EXPECT_EQ(memcmp(gfl.data, gfl_data, 3), 0);
  //  test convert bin to frame;
  FrameStreamDataBlocked recv_frame;
  FrameType frame_type;
  ConvertGFLToFrame(&gfl, &recv_frame, &frame_type);
  EXPECT_EQ(frame_type, FrameType::kStreamDataBlocked);
  EXPECT_EQ(recv_frame.stream_id, frame.stream_id);
  EXPECT_EQ(recv_frame.stream_data_limit, frame.stream_data_limit);
}

TEST(Frame, ResetStreamFrameConvert) {
  FrameResetStream frame;
  frame.stream_id = 12345;
  frame.error_code = 12345;
  frame.final_size = 12345;
  GenericFrameLayout gfl;
  EXPECT_EQ(ConvertFrameToGFL(&frame, FrameType::kResetStream, &gfl), 0);
  EXPECT_EQ(gfl.frame_type_bits,0x04);
  char gfl_data[] = {
      0x70, 0x39,  // Stream ID
      0x70, 0x39,  // Application Error Code
      0x70, 0x39   // Final Size
  };
  EXPECT_EQ(gfl.data_len, 6);
  EXPECT_EQ(memcmp(gfl.data, gfl_data, 6), 0);
  //  test convert bin to frame;
  FrameResetStream recv_frame;
  FrameType frame_type;
  ConvertGFLToFrame(&gfl, &recv_frame, &frame_type);
  EXPECT_EQ(frame_type, FrameType::kResetStream);
  EXPECT_EQ(recv_frame.stream_id, frame.stream_id);
  EXPECT_EQ(recv_frame.error_code, frame.error_code);
  EXPECT_EQ(recv_frame.final_size, frame.final_size);
}
