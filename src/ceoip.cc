// Copyright 2010 Gustaf Sjöberg <gs@distrop.com>
// Copyright 2012 Circonus, Inc. All rights reserved.
#include <GeoIP.h>
#include <GeoIPCity.h>

#include <v8.h>
#include <node.h>
#include <assert.h>
#include <iconv.h>

using namespace v8;
using namespace node;

static Persistent<String> connected_symbol;
static Persistent<String> closed_symbol;
static Persistent<String> error_symbol;
static Persistent<String> result_symbol;
static iconv_t cd;

class Connection : ObjectWrap {
  public:
    static void
    Initialize(v8::Handle<v8::Object> target) {
      Local<FunctionTemplate> t = FunctionTemplate::New(Connection::New);
      t->InstanceTemplate()->SetInternalFieldCount(1);
      t->SetClassName(String::New("ceoip.Connection"));

      cd = iconv_open("utf-8", "ISO-8859-1");
      closed_symbol = NODE_PSYMBOL("closed");
      connected_symbol = NODE_PSYMBOL("connected");
      error_symbol = NODE_PSYMBOL("error");
      result_symbol = NODE_PSYMBOL("result");

      NODE_SET_PROTOTYPE_METHOD(t, "connect", Connect);
      NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
      NODE_SET_PROTOTYPE_METHOD(t, "query", Query);

      NODE_DEFINE_CONSTANT(target, GEOIP_STANDARD);
      NODE_DEFINE_CONSTANT(target, GEOIP_MEMORY_CACHE);
      NODE_DEFINE_CONSTANT(target, GEOIP_CHECK_CACHE);
      NODE_DEFINE_CONSTANT(target, GEOIP_INDEX_CACHE);
      NODE_DEFINE_CONSTANT(target, GEOIP_MMAP_CACHE);

      target->Set(String::NewSymbol("Connection"), t->GetFunction());
    }

    void Connect(const Arguments &args, const char *dbpath, int db_opts_bitmask) {
      HandleScope scope;

      if(db_opts_bitmask == 0){
        db_opts_bitmask = 0 | GEOIP_STANDARD;
      }
      gi = GeoIP_open(dbpath, db_opts_bitmask);

      Handle<Value> argv[1] = { gi ? connected_symbol : error_symbol };
      MakeCallback(args.This(), "emit", 1, argv);
    }

    void CloseHandle(const Arguments &args) {
      HandleScope scope;

      if (gi != NULL) {
        GeoIP_delete(gi);
        gi = NULL;
      }
      Handle<Value> argv[1] = { closed_symbol };
      MakeCallback(args.This(), "emit", 1, argv);
    }

    void Query(const Arguments &args, const char *query) {
      HandleScope scope;

      assert(gi);
      GeoIPRecord *record;
      record = GeoIP_record_by_addr(gi, query);

      if (record) {
        Local<Value> result = BuildResult(record);
        GeoIPRecord_delete(record);
        Handle<Value> argv[2] = { result_symbol, result };
        MakeCallback(args.This(), "emit", 2, argv);
      } else {
        Handle<Value> argv[1] = { result_symbol };
        MakeCallback(args.This(), "emit", 1, argv);
      }
    }

  protected:
    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;

      Connection *connection = new Connection();
      connection->Wrap(args.This());

      return args.This();
    }

    static Handle<Value> Connect(const Arguments &args) {
      HandleScope scope;

      if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(
                Exception::TypeError(
                    String::New("Required argument: path to database.")));
      }
      if (args.Length() == 2 && !args[1]->IsNumber()){
        return ThrowException(
          Exception::TypeError(
            String::New("Second argument must be a bitmask")));
      }

      String::Utf8Value dbpath(args[0]->ToString());


      Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
      connection->Connect(args, *dbpath, args[1]->ToUint32()->Value());

      return Undefined();
    }

    static Handle<Value> Close(const Arguments &args) {
      HandleScope scope;

      Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
      connection->CloseHandle(args);

      return Undefined();
    }

    static Handle<Value> Query(const Arguments &args) {
      HandleScope scope;

      if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(
                Exception::TypeError(
                    String::New("Required argument: ip address.")));
      }

      String::Utf8Value query(args[0]->ToString());

      Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
      connection->Query(args, *query);

      return Undefined();
    }

  private:

#ifndef ICONV_SRC_CONST
#define ICONV_SRC_CONST
#endif
#define icv(a,b,blen) do { \
  ICONV_SRC_CONST char *in = a; \
  char *out = b; \
  size_t inlen = strlen(a); \
  size_t outlen = blen; \
  if(iconv(cd, &in, &inlen, &out, &outlen) == -1) b[0] = '\0'; \
  else *out = '\0'; \
} while(0)

    Local<Value>BuildResult(GeoIPRecord *record) {
      HandleScope scope;
      char outputbuff[1024];

      Local<Array> result = Array::New();

      if (record->longitude != (int)NULL) {
        result->Set(
          String::New("longitude"),
          Number::New(record->longitude)
        );
      }
      if (record->latitude != (int)NULL) {
        result->Set(
          String::New("latitude"),
          Number::New(record->latitude)
        );
      }
      if (record->country_code != NULL) {
        result->Set(
          String::New("country_code"),
          String::New(record->country_code)
        );
      }
      if (record->continent_code != NULL) {
        result->Set(
          String::New("continent_code"),
          String::New(record->continent_code)
        );
      }
      if (record->metro_code != (int)NULL) {
        result->Set(
          String::New("metro_code"),
          Number::New(record->metro_code)
        );
      }
      if (record->country_name != NULL) {
        icv(record->country_name, outputbuff, sizeof(outputbuff));
        result->Set(
          String::New("country"),
          String::New(outputbuff)
        );
      }
      if (record->city != NULL) {
        icv(record->city, outputbuff, sizeof(outputbuff));
        result->Set(
          String::New("city"),
          String::New(outputbuff)
        );
      }
      if (record->area_code != (int)NULL) {
        result->Set(
          String::New("area_code"),
          Number::New(record->area_code)
        );
      }
      if (record->region != NULL) {
        icv(record->region, outputbuff, sizeof(outputbuff));
        result->Set(
          String::New("region"),
          String::New(outputbuff)
        );
      }

      return scope.Close(result);
    }

    GeoIP *gi;
};

extern "C" void
init(Handle<Object> target) {
  HandleScope scope;
  Connection::Initialize(target);
}

NODE_MODULE(ceoip, init)
