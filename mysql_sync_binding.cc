/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/
#include <mysql/mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

using namespace v8;
using namespace node;

//static Persistent<String> connect_symbol;
//static Persistent<String> close_symbol;
//static Persistent<String> escape_symbol;
//static Persistent<String> fetch_result_symbol;
//static Persistent<String> query_symbol;

class MysqlDbSync : public EventEmitter
{
  public:
    static void Init(v8::Handle<v8::Object> target) 
    {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        //connect_symbol = NODE_PSYMBOL("connect");
        //close_symbol = NODE_PSYMBOL("close");
        //escape_symbol = NODE_PSYMBOL("escape");
        //fetch_result_symbol = NODE_PSYMBOL("fetch_result");
        //query_symbol = NODE_PSYMBOL("query");

        NODE_SET_PROTOTYPE_METHOD(t, "connect", Connect);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "escape", Escape);
        NODE_SET_PROTOTYPE_METHOD(t, "fetch_result", FetchResult);
        NODE_SET_PROTOTYPE_METHOD(t, "query", Query);

        target->Set(v8::String::NewSymbol("MysqlDbSync"), t->GetFunction());
    }
    
    bool Connect(const char* servername, const char* user, const char* password, const char* database, unsigned int port, const char* socket)
    {
        if(_connection)
        {
            return false;
        }

        _connection = mysql_init(NULL);
        
        if(!_connection)
        {
            return false;
        }
        
        if(!mysql_real_connect(_connection, servername, user, password, database, port, socket, 0))
        {
            return false;
        }

        return true;
    }
    
    void Close()
    {
        if (_connection)
        {
            mysql_close(_connection);
            _connection = NULL;
        }
    }
    
    bool Escape(const char* query, int length)
    {
        //TODO: Implement this
        
        return false;
    }
    
    void FetchResult()
    {
        //TODO: Implement this
    }
    
    const char* ErrorMessage ( )
    {
        return mysql_error(_connection);
    }
    
    bool Query(const char* query, int length)
    {
        if(!_connection)
        {
            return false;
        }
        
        int r = mysql_real_query(_connection, query, length);
        
        if (r == 0) {
            return true;
        }
        
        return false;
    }

  protected:
    MYSQL *_connection;
    
    MysqlDbSync()
    {
        _connection = NULL;
    }

    ~MysqlDbSync()
    {
        if (_connection) mysql_close(_connection);
    }
    
    static Handle<Value> New (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = new MysqlDbSync();
        connection->Wrap(args.This());

        return args.This();
    }
    
    static Handle<Value> Connect (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        if (args.Length() < 3 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) {
            return ThrowException(String::New("Must give servername, user and password strings as argument"));
        }

        String::Utf8Value servername(args[0]->ToString());
        String::Utf8Value user(args[1]->ToString());
        String::Utf8Value password(args[2]->ToString());
        String::Utf8Value database(args[3]->ToString());
        unsigned int port = args[4]->IntegerValue();
        String::Utf8Value socket(args[5]->ToString());

        bool r = connection->Connect(*servername, *user, *password, *database, port, (args[5]->IsString() ? *socket : NULL) );

        if (!r) {
            return ThrowException(Exception::Error(String::New(connection->ErrorMessage())));
        }

        return Undefined();
    }
    
    static Handle<Value> Close (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());
        
        connection->Close();
        
        return Undefined();
    }
    
    static Handle<Value> Escape (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());
        
        //TODO: Implement this
        
        return Undefined();
    }
    
    static Handle<Value> FetchResult (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());
        
        //TODO: Implement this
        
        return Undefined();
    }

    static Handle<Value> Query (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        if (args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(String::New("First argument of MysqlDbSync->Query() must be a string")));
        }

        String::Utf8Value query(args[0]->ToString());

        bool r = connection->Query(*query, query.length());

        if (!r) {
            return ThrowException(Exception::Error(String::New(connection->ErrorMessage())));
        }

        return Undefined();
    }
};

extern "C" void init (v8::Handle<Object> target)
{
    MysqlDbSync::Init(target);
}

