#ifndef MSTREAMS_MCONSUMER_H
#define MSTREAMS_MCONSUMER_H

#include <napi.h>
#include <rdkafka.h>

using namespace Napi;
using namespace std;

class mconsumer : public Napi::ObjectWrap<mconsumer>
{
public:
    explicit mconsumer(const CallbackInfo& callbackInfo);
    ~mconsumer() {
        //rd_kafka_destroy(_client_handle);
    }

    bool consumerInitDone;

    static Napi::FunctionReference constructor;

    static void Initialize(Napi::Env& env, Napi::Object& target)
    {
        Napi::HandleScope scope(env);
        Function ctor =
                DefineClass(env,
                            "mconsumer",
                            {
                                    InstanceMethod("get", &mconsumer::get),
                                    InstanceMethod("consume", &mconsumer::consume),
                            });

        constructor = Napi::Persistent(ctor);
        constructor.SuppressDestruct();
        target.Set("mconsumer", ctor);
    }

    Napi::Value get(const CallbackInfo&);
    Napi::Value consume(const CallbackInfo&);

private:
    rd_kafka_t *_client_handle;
    rd_kafka_conf_t *_conf;
    rd_kafka_topic_conf_t *_topic_conf;
    rd_kafka_resp_err_t errCode;
};

#endif
