#include <sstream>
#include "mproducer.h"

Napi::FunctionReference mproducer::constructor;
static std::vector<Napi::Function> callbacks;


extern "C" void mproducer::msg_delivery_cb (rd_kafka_t *rk,
                      const rd_kafka_message_t *rkmessage, void *opaque) {

    if (rkmessage->err != RD_KAFKA_RESP_ERR_NO_ERROR) {

    } else {
        /*
         * printf("Message [%.*s] Delivered %s\n",(int)rkmessage->len,(const char*)rkmessage->payload,
         * rd_kafka_err2str(rkmessage->err));
         * printf("Key [%.*s] Delivered %s\n",(int)rkmessage->key_len,(const char*)rkmessage->key,
         * rd_kafka_err2str(rkmessage->err));
        */
        while(callbacks.size() > 0) {

            Napi::Function f = callbacks.back();
            Napi::Env env = f.Env();

            Napi::Object object = Napi::Object::New(env);
            object.Set(Napi::String::New(env, "key"),
                       Napi::String::New(env, std::string((char*)rkmessage->key, rkmessage->key_len)));
            object.Set(Napi::String::New(env, "payload"),
                       Napi::String::New(env, std::string((char*)rkmessage->payload, rkmessage->len)));
//
//            /*
//             * approach 1 works! - launch subworkers in other threads - async queue
//             * mproducerworker* mproducerworker1 = new mproducerworker(f, rkmessage->partition, payload1,
//             * rkmessage->len, key1, rkmessage->key_len,
//             * rkmessage->_private);
//             * mproducerworker1->Queue();
//            */
//
//            // approach 2 works! - immediate with same js thread
            f.MakeCallback(env.Global(), {env.Null(), object/*Napi::String::New(env, key1)*/});
            if(callbacks.size() > 0)
                callbacks.pop_back();
        }
    }

}

mproducer::mproducer(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    char errstr[1024];

    _conf = rd_kafka_conf_new();
    if (_conf == NULL) {
        throw Napi::Error::New(info.Env(), "Failed to create conf");
    }

    rd_kafka_conf_set_dr_msg_cb(_conf, mproducer::msg_delivery_cb);

    _client_handle = rd_kafka_new(RD_KAFKA_PRODUCER, _conf, errstr, sizeof(errstr));
    if (_client_handle == NULL) {
        producerInitDone = false;
        throw Napi::Error::New(info.Env(), "Failed to create producer");
    }


    producerInitDone = true;
}

Napi::Value mproducer::get(const Napi::CallbackInfo& info) {
    auto mproducerPtr = External<mproducer>::New(info.Env(), this);
    auto instance = mproducer::constructor.New({ this->Value(), mproducerPtr });
    return instance;
}

Napi::Value mproducer::produce(const Napi::CallbackInfo &info) {
    string topic = info[0].As<Napi::String>().Utf8Value();
    int32_t partition = info[1].As<Napi::Number>().Int32Value();
    string message = info[2].As<Napi::String>().Utf8Value();
    string key = info[3].As<Napi::String>().Utf8Value();
    Napi::ArrayBuffer opaque = info[4].As<Napi::ArrayBuffer>();
    Napi::Function cb = info[5].As<Napi::Function>();
    Napi::Function cb1  = Persistent<Napi::Function>(cb).Value();

    _topic_conf = rd_kafka_topic_conf_new();

    if (_topic_conf == NULL) {
        throw Napi::Error::New(info.Env(), "Failed to create new topic conf");
    }

    _client_topic_handle = rd_kafka_topic_new(_client_handle, topic.c_str(), _topic_conf);
    if (_client_topic_handle == NULL) {
        throw Napi::Error::New(info.Env(), "Failed to create new topic handle");
    }


    callbacks.push_back(cb1);

    if (rd_kafka_produce(_client_topic_handle,
                         partition,
                         RD_KAFKA_MSG_F_COPY,
                         (void *) string(message).c_str(),
                         message.length(),
                         string(key).c_str(),
                         key.length(),
                         opaque.Data()) == -1) {
        throw Napi::Error::New(info.Env(), "Failed to produce");
    }

    while (rd_kafka_outq_len(_client_handle) > 0)
        rd_kafka_poll(_client_handle, 5000);

    return Napi::String::New(info.Env(), string(rd_kafka_name(_client_handle)));
}

Napi::Value mproducer::poll(const Napi::CallbackInfo &info) {

    int32_t interval = info[0].As<Napi::Number>().Int32Value();
    while (rd_kafka_outq_len(_client_handle) > 0)
        rd_kafka_poll(_client_handle, interval);
    return Napi::String::New(info.Env(), string(rd_kafka_name(_client_handle)));
}

mproducerworker::mproducerworker(Napi::Function& callback,
               int32_t partition,
               void *payload,
               size_t len,
               void *key,
               size_t keylen,
               void *msg_opaque
) : Napi::AsyncWorker(callback), partition(partition),
    payload(payload), len(len), key(key), keylen(keylen), msg_opaque(msg_opaque), produced(false) {

}

void mproducerworker::Execute() {
    auto * key1 = static_cast<char *>(malloc(keylen));
    auto * payload1 = static_cast<char *>(malloc(len));
    strcpy(key1, static_cast<const char *>(key));
    strcpy(payload1, static_cast<const char *>(payload));
    result = key1;

}

void mproducerworker::OnOK() {
    Napi::Env env = Env();
    Callback().MakeCallback(Receiver().Value(),
                            {env.Null(), Napi::String::New(env, result)});
}

void mproducerworker::OnError(const Napi::Error& e) {
    Napi::Env env = Env();
    Callback().MakeCallback(Receiver().Value(), {e.Value(), env.Undefined()});
}