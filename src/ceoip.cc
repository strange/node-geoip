// Copyright 2010 Gustaf Sjöberg <gs@distrop.com>
#include <GeoIP.h>
#include <GeoIPCity.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>
#include <assert.h>

using namespace v8;
using namespace node;

static Persistent<String> connected_symbol;
static Persistent<String> closed_symbol;
static Persistent<String> error_symbol;
static Persistent<String> result_symbol;

class Connection : public EventEmitter {
  public:
    static void
    Initialize(v8::Handle<v8::Object> target) {
      Local<FunctionTemplate> t = FunctionTemplate::New(Connection::New);
      t->Inherit(EventEmitter::constructor_template);
      t->InstanceTemplate()->SetInternalFieldCount(1);

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

    void Connect(const char *dbpath, int db_opts_bitmask) {
      HandleScope scope;

      if(db_opts_bitmask == 0){
        db_opts_bitmask = 0 | GEOIP_STANDARD;
      }
      gi = GeoIP_open(dbpath, db_opts_bitmask);

      Emit((gi ? connected_symbol : error_symbol), 0, NULL);
    }

    void Close() {
      HandleScope scope;

      if (gi != NULL) {
        GeoIP_delete(gi);
        gi = NULL;
      }

      Emit(closed_symbol, 0, NULL);
    }

    void Query(const char *query) {
      HandleScope scope;

      assert(gi);
      GeoIPRecord *record;
      record = GeoIP_record_by_addr(gi, query);

      if (record) {
        Local<Value> result = BuildResult(record);
        GeoIPRecord_delete(record);
        Emit(result_symbol, 1, &result);
      } else {
        Emit(result_symbol, 0, NULL);
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
      connection->Connect(*dbpath, args[1]->ToUint32()->Value());

      return Undefined();
    }

    static Handle<Value> Close(const Arguments &args) {
      HandleScope scope;

      Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
      connection->Close();

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
      connection->Query(*query);

      return Undefined();
    }

  private:
    Local<Value>BuildResult(GeoIPRecord *record) {
      HandleScope scope;

      Local<Array> result = Array::New();

      if (record->longitude != NULL) {
        result->Set(
          String::New("longitude"),
          Number::New(record->longitude)
        );
      }
      if (record->latitude != NULL) {
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
      if (record->metro_code != NULL) {
        result->Set(
          String::New("metro_code"),
          Number::New(record->metro_code)
        );
      }
      if (record->country_name != NULL) {
        result->Set(
          String::New("country"),
          String::New(record->country_name)
        );
      }
      if (record->city != NULL) {
        result->Set(
          String::New("city"),
          String::New(record->city)
        );
      }
      if (record->area_code != NULL) {
        result->Set(
          String::New("area_code"),
          Number::New(record->area_code)
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
