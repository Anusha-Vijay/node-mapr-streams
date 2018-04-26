#include "mstreams.h"

Napi::FunctionReference mstreams::constructor;

mstreams::mstreams(const Napi::CallbackInfo &callbackInfo) : ObjectWrap(callbackInfo) {
    _conf = rd_kafka_conf_new();
    if (_conf == NULL) {
        throw Napi::Error::New(callbackInfo.Env(), "Failed to create conf");
    }
}

Napi::Value mstreams::create(const Napi::CallbackInfo& info) {
    string clientType = info[0].As<Napi::String>().Utf8Value();
    char errstr[1024];
    if(clientType == "producer") {
        _client_handle = rd_kafka_new(RD_KAFKA_PRODUCER, _conf, errstr, sizeof(errstr));
        if (_client_handle == NULL) {
            throw Napi::Error::New(info.Env(), "Failed to create producer");
        }
    } else if (clientType == "consumer") {
        _client_handle = rd_kafka_new(RD_KAFKA_CONSUMER, _conf, errstr, sizeof(errstr));
        throw Napi::Error::New(info.Env(), "Failed to create consumer");
    } else {
        throw Napi::Error::New(info.Env(), "Either producer or consumer client expected");
    }
    connectionType = clientType;


    return Napi::String::New(info.Env(), std::string(rd_kafka_name(_client_handle)));
}