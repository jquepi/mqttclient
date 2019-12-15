/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2019-12-16 02:25:55
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <stdio.h>
#include <string.h>

#include "MQTTPacket.h"
#include "list.h"
#include "memory.h"
#include "network.h"
#include "error.h"
#include "mutex.h"

#define     MAX_PACKET_ID           65535
#define     MAX_MESSAGE_HANDLERS    5
#define     MQTT_TOPIC_LEN_MAX      128
#define     MQTT_REPUB_NUM_MAX      20
#define     MQTT_SUB_NUM_MAX        10
#define     DEFAULT_BUF_SIZE        1024
#define     DEFAULT_CMD_TIMEOUT     2000
#define     MAX_CMD_TIMEOUT         5000
#define     MIN_CMD_TIMEOUT         500


typedef enum mqtt_qos {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
    SUBFAIL = 0x80
} mqtt_qos_t;

typedef enum client_state {
	CLIENT_STATE_INVALID = 0,                    //client无效状态
	CLIENT_STATE_INITIALIZED = 1,                //client初始化状态
	CLIENT_STATE_CONNECTED = 2,                  //client已经连接状态
	CLIENT_STATE_DISCONNECTED = 3,               //client连接丢失状态
	CLIENT_STATE_DISCONNECTED_RECONNECTING = 4,  //client正在重连状态
}client_state_t;

typedef struct mqtt_connack_data {
    unsigned char rc;
    unsigned char session_present;
} mqtt_connack_data_t;

typedef struct mqtt_message {
    mqtt_qos_t          qos;
    unsigned char       retained;
    unsigned char       dup;
    unsigned short      id;
    void                *payload;
    size_t              payloadlen;
} mqtt_message_t;

typedef struct message_data {
    mqtt_message_t      *message;
    MQTTString          *topic_name;
} message_data_t;

typedef void (*message_handler_t)(void* client, message_data_t* msg);

typedef struct message_handlers {
    list_t              list;
    const char*         topic_filter;
    message_handler_t   handler;
} message_handlers_t;

typedef struct mqtt_client {
    unsigned int                packet_id;
    unsigned int                cmd_timeout;
    size_t                      read_buf_size;
    size_t                      write_buf_size;
    char                        *read_buf;
    char                        *write_buf;
    platform_mutex_t            write_lock;
    list_t                      msg_handler_list;
    void                        (*default_message_handler)(void*, message_data_t*);
    network_t                   *network;
    client_state_t              client_state;
    unsigned int                keep_alive_interval;
    int                         clean_session;
    char                        connection_state;
    char                        ping_outstanding;
    platform_timer_t            ping_timer;
    platform_timer_t            last_sent;
    platform_timer_t            last_received;
    connect_params_t            connect_data;
    // MQTTPacket_connectData      connect_data;
} mqtt_client_t;


typedef void (*disconnect_handler_t)(mqtt_client_t *, void *);
typedef void (*reconnect_handler_t)(mqtt_client_t *, void *);

typedef struct client_init_params{
	unsigned int            cmd_timeout;
    size_t                  read_buf_size;
    size_t                  write_buf_size;
    connect_params_t        connect_params;
	disconnect_handler_t    disconnect_handler;
	reconnect_handler_t     reconnect_handler;
	void                    *disconnect_handler_data;
	void                    *reconnect_handler_data;
} client_init_params_t;

int mqtt_set_connect_data(mqtt_client_t* c, MQTTPacket_connectData* connect_data);
int mqtt_init(mqtt_client_t* c, client_init_params_t* init);
int mqtt_release(mqtt_client_t* c);


#endif /* _MQTTCLIENT_H_ */