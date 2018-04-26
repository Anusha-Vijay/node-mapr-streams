#include <cstdlib>
#include "mconsumer.h"

Napi::FunctionReference mconsumer::constructor;

mconsumer::mconsumer(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    consumerInitDone = false;
    char errstr[1024];
    _conf = rd_kafka_conf_new();
    if(_conf == NULL) {
        throw Napi::Error::New(info.Env(), "Failed to create consumer conf");
    }

    if(RD_KAFKA_CONF_OK != rd_kafka_conf_set(_conf,
                                             "group.id", "defaultConsumerGroup",
                                             errstr, sizeof(errstr))) {
        throw Napi::Error::New(info.Env(), "rd_kafka_conf_set() failed");
    }
    _topic_conf = rd_kafka_topic_conf_new();

    if (RD_KAFKA_CONF_OK != rd_kafka_topic_conf_set(_topic_conf, "auto.offset.reset",
                                                    "earliest" ,errstr, sizeof(errstr))) {
        throw Napi::Error::New(info.Env(), "rd_kafka_topic_conf_set() failed with error");
    }

    rd_kafka_conf_set_default_topic_conf(_conf, _topic_conf);

    _client_handle = rd_kafka_new(RD_KAFKA_CONSUMER, _conf, errstr, sizeof(errstr));
    if(_client_handle == NULL) {
        throw Napi::Error::New(info.Env(), "Failed to create consumer");
    }

    rd_kafka_poll_set_consumer(_client_handle);
    consumerInitDone = true;
}

Napi::Value mconsumer::get(const Napi::CallbackInfo &info) {
    auto mconsumerPtr = External<mconsumer>::New(info.Env(), this);
    auto instance = mconsumer::constructor.New({ this->Value(), mconsumerPtr });
    return instance;
}

Napi::Value mconsumer::consume(const Napi::CallbackInfo &info) {
    char errstr[1024];
    string topic = info[0].As<Napi::String>().Utf8Value();
    int32_t partition = info[1].As<Napi::Number>().Int32Value();
    int32_t timeout = info[2].As<Napi::Number>().Int32Value();
    string group = info[3].As<Napi::String>().Utf8Value();
    Napi::Function cb = info[4].As<Napi::Function>();

    if(RD_KAFKA_CONF_OK != rd_kafka_conf_set(_conf,
                                             "group.id", group.c_str(),
                                             errstr, sizeof(errstr))) {
        throw Napi::Error::New(info.Env(), "rd_kafka_conf_set() failed");
    }

    rd_kafka_topic_partition_list_t *tp_list = rd_kafka_topic_partition_list_new(0);
    rd_kafka_topic_partition_t* tpObj = rd_kafka_topic_partition_list_add(tp_list,
                                                                          topic.c_str(), partition);
    if (NULL == tpObj) {
        throw Napi::Error::New(info.Env(), "Could not add the topic partition to the list");
    }

    errCode = rd_kafka_subscribe(_client_handle, tp_list);
    if (errCode  != RD_KAFKA_RESP_ERR_NO_ERROR) {
        throw Napi::Error::New(info.Env(), "Topic partition subscription failed");
    }

    rd_kafka_topic_partition_list_destroy(tp_list);

    int msg_count = 0;
    int retry = 100;
    while(1) {
        rd_kafka_message_t *msg = rd_kafka_consumer_poll(_client_handle, timeout);
        if (msg != NULL) {
            if (msg->err == RD_KAFKA_RESP_ERR_NO_ERROR) {
                msg_count++;
                Napi::Env env = info.Env();
                Napi::Object object = Napi::Object::New(env);

//                auto * keyData = (char *) malloc(sizeof(char) * msg->key_len);
//                auto * valueData = (char *) malloc(sizeof(char) * msg->len);
//                sprintf(keyData, "%.*s", (int)msg->key_len, (char*)msg->key);
//                sprintf(valueData, "%.*s", (int) msg->len, (char*)msg->payload);

                object.Set(Napi::String::New(env, "key"),
                           Napi::String::New(env, std::string((char*)msg->key, msg->key_len)));
                object.Set(Napi::String::New(env, "payload"),
                           Napi::String::New(env, std::string((char*)msg->payload, msg->len)));

//                object.Set(Napi::String::New(env, "key"),
//                           Napi::String::New(env, std::string(keyData)));
//                object.Set(Napi::String::New(env, "payload"),
//                           Napi::String::New(env, std::string(valueData)));

                cb.MakeCallback(env.Global(),
                               {env.Null(), object });
            } else {
                retry--;
                break;
            }
            rd_kafka_message_destroy(msg);
        } else {
            retry--;
            break;
        }
        if(retry <= 0)
            break;
    }

    int retVal = rd_kafka_commit(_client_handle, NULL, false/*async*/);
    if(retVal != RD_KAFKA_RESP_ERR_NO_ERROR) {
        throw Napi::Error::New(info.Env(), "rd_kafka_commit() failed");
    }

    rd_kafka_consumer_close(_client_handle);
    rd_kafka_destroy(_client_handle);

    return Napi::String::New(info.Env(), std::string(rd_kafka_name(_client_handle)));
}