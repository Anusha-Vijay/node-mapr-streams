
/* Common exports entry to initialize functions to sub modules */
#ifndef MSTREAMS_ADDON_H
#define MSTREAMS_ADDON_H

#include "mstreams.h"
#include "mproducer.h"
#include "mconsumer.h"

Napi::Object init(Napi::Env env, Napi::Object exports) {

  /* initialize/export mproducer module */
    mstreams::Initialize(env, exports);
    mproducer::Initialize(env, exports);
    mconsumer::Initialize(env, exports);

  return exports;
}

/* note to me - addon and the entry method
   signature for the Init method changed
   before Napi::Env env, Napi::Object exports, Napi::Object module
   module parameter should be removed as the signature is now:
   napi_value regfunc(napi_env env, napi_value exports)
   REF: https://github.com/nodejs/node-addon-api/issues/145
*/
//Module registration - modname, regfunc
NODE_API_MODULE(mstreams, init);

#endif