#ifndef HARDCODE_H
#define HARDCODE_H

#include <sys/types.h>

extern const size_t SZ_CONNECTION_START_PKT;
extern const char CONNECTION_START_PKT[];

extern const size_t SZ_CONNECTION_TUNE_PKT;
extern const char CONNECTION_TUNE_PKT[];

extern const size_t SZ_CONNECTION_OPEN_OK_PKT;
extern const char CONNECTION_OPEN_OK_PKT[];

extern const size_t SZ_CONNECTION_CLOSE_OK_PKT;
extern const char CONNECTION_CLOSE_OK_PKT[];

extern const size_t SZ_CHANNEL_OPEN_OK_PKT;
extern const char CHANNEL_OPEN_OK_PKT[];

extern const size_t SZ_CHANNEL_CLOSE_OK_PKT;
extern const char CHANNEL_CLOSE_OK_PKT[];

extern const size_t SZ_QUEUE_DECLARE_OK_PKT1;
extern const char QUEUE_DECLARE_OK_PKT1[];

extern const size_t SZ_QUEUE_DECLARE_OK_PKT2;
extern const char QUEUE_DECLARE_OK_PKT2[];

extern const size_t SZ_BASIC_CONSUME_OK_PKT;
extern const char BASIC_CONSUME_OK_PKT[];

extern const size_t SZ_BASIC_DELIVER_PKT;
extern const char BASIC_DELIVER_PKT[]; 

extern const size_t SZ_BASIC_QOS_OK_PKT;
extern const char BASIC_QOS_OK_PKT[];

#endif
