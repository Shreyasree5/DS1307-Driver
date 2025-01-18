#include "ds1307.h"

// enum {
//     MSG_ALL = 0,
//     MSG_GET_TIME,
//     MSG_SET_TIME,
//     MSG_SET_TIME_ACK,
//     MSG_GET_TIME_ACK,
// } MsgType;

struct message {
    long msg_type;
    struct date_time_to_set payload;
};

