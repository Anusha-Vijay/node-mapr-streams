#ifndef NMSTREAMS_PRODUCER_H
#define NMSTREAMS_PRODUCER_H

#include <napi.h>
#include <rdkafka.h>

using namespace Napi;
using namespace std;

class mproducer : public Napi::ObjectWrap<mproducer>
{
public:
    //typedef mstreams super;
    explicit mproducer(const CallbackInfo& callbackInfo); // constructor
    ~mproducer() {
        //rd_kafka_destroy(_client_handle);
    } // destructor

    bool producerInitDone;

    // to be set in the Initialize block when the class has been created
    static Napi::FunctionReference constructor;

    /* being called in from the addon cc to initlize the class */
    static void Initialize(Napi::Env& env, Napi::Object& target)
    {
        Napi::HandleScope scope(env);
        Function ctor =
                DefineClass(env,
                            "mproducer",
                            {
                                    // create a instance method named load and save reference in &mproducer::Load
                                    InstanceMethod("get", &mproducer::get),
                                    InstanceMethod("produce", &mproducer::produce),
                                    InstanceMethod("poll", &mproducer::poll)
                            });
        // its a persistent and not an intermittent constructor
        constructor = Napi::Persistent(ctor);
        constructor.SuppressDestruct();
        // set the local member variable with the instance object
        target.Set("mproducer", ctor);
    }

    static void msg_delivery_cb (rd_kafka_t *rk,
                                     const rd_kafka_message_t *rkmessage, void *opaque);
    Napi::Value get(const CallbackInfo&);
    Napi::Value produce(const CallbackInfo&);
    Napi::Value poll(const CallbackInfo&);

private:
    rd_kafka_conf_t *_conf;
    rd_kafka_t *_client_handle;
    rd_kafka_topic_conf_t *_topic_conf;
    rd_kafka_topic_t *_client_topic_handle;
};

class merrorworker : public Napi::AsyncWorker {
public:
    merrorworker(const Napi::Function& callback, Napi::Error error)
            : Napi::AsyncWorker(callback), error(error) {}

protected:
    void Execute() override {}
    void OnOK() override {
        Napi::Env env = Env();
        Callback().MakeCallback(Receiver().Value(), {error.Value(), env.Undefined()});
    }

    void OnError(const Napi::Error& e) override {
        Napi::Env env = Env();
        Callback().MakeCallback(Receiver().Value(), {e.Value(), env.Undefined()});
    }

private:
    Napi::Error error;
};

class mproducerworker : public Napi::AsyncWorker {
public:
    int32_t partition;
    void *payload;
    size_t len;
    void *key;
    size_t keylen;
    void *msg_opaque;
    bool produced;
    std::string result;

    mproducerworker(Function &callback, int32_t partition,
                    void *payload, size_t len, void *key, size_t keylen,
                       void *msg_opaque);
    ~mproducerworker() {}

protected:
    void Execute() override;

    void OnOK() override;

    void OnError(const Error &e) override;

};

#endif
