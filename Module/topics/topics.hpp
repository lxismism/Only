/**
 * @file topic.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <cstdint>
#include <cstring>

/*静态内存配置*/

//最大消息结构体字节数
#ifndef TOPICS_MAX_MESSAGE_SIZE
#define TOPICS_MAX_MESSAGE_SIZE 32U
#endif

//最大历史长度（订阅者缓冲队列最大长度）
#ifndef TOPICS_MAX_HISTORY_LEN
#define TOPICS_MAX_HISTORY_LEN 8U
#endif

//最大topic数量
#ifndef TOPICS_MAX_TOPICS
#define TOPICS_MAX_TOPICS 8U
#endif

//某个topics上能搭载的最大订阅
#ifndef TOPICS_MAX_SUBS_PER_TOPIC
#define TOPICS_MAX_SUBS_PER_TOPIC 4U
#endif

typedef struct publish_data_t {
    uint8_t *data;
    int len;
} publish_data;

struct internal_topic;
struct subscriber_state;

class TopicPublisher{
public:
    explicit TopicPublisher(const char *topic);
    bool IsValid() const {return topic_ != nullptr;}
    bool Publish(uint8_t *data, int len) const;

private:
    internal_topic *topic_{nullptr};
};

class TopicSubscriber{
public:
    TopicSubscriber(const char *topic, uint32_t buffer_len);
    bool IsValid() const {return sub_ != nullptr;}
    bool TryGet(publish_data *out) const;

private:
    subscriber_state *sub_{nullptr};
};

