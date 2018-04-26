#ifndef MSTREAMS_MSTREAMS_H
#define MSTREAMS_MSTREAMS_H

#include <napi.h>
#include <rdkafka.h>

using namespace Napi;
using namespace std;

class mstreams : public Napi::ObjectWrap<mstreams>
{
public:
    explicit mstreams(const Napi::CallbackInfo& callbackInfo); // constructor
    ~mstreams() {
        rd_kafka_destroy(_client_handle);
    }

    string connectionType;

    // to be set in the Initialize block when the class has been created
    static Napi::FunctionReference constructor;

    /* being called in from the addon cc to initlize the class */
    static void Initialize(Napi::Env& env, Napi::Object& target)
    {
        Napi::HandleScope scope(env);
        Function ctor =
                DefineClass(env,
                            "mstreams",
                            {
                                    // create a instance method named load and save reference in &mproducer::Load
                                    InstanceMethod("create", &mstreams::create),
                            });
        // its a persistent and not an intermittent constructor
        constructor = Napi::Persistent(ctor);
        constructor.SuppressDestruct();
        // set the local member variable with the instance object
        target.Set("mstreams", ctor);
    }

    Napi::Value create(const CallbackInfo&);

    inline rd_kafka_conf_t* getGlobalConf() { return _conf; }

protected:
    rd_kafka_conf_t *_conf;
    rd_kafka_t *_client_handle;

};

#endif //STREAMS_STREAMS_H