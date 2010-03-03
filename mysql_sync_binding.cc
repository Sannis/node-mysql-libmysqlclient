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

        NODE_SET_PROTOTYPE_METHOD(t, "connect", Connect);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);

        target->Set(v8::String::NewSymbol("MysqlDbSync"), t->GetFunction());
    }
    
    bool Connect(const char* servername, const char* user, const char* password, const char* database, unsigned int port, const char* socket)
    {printf("bool Connect()\n");
        if (_connection) return false;

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
    {printf("void Close()\n");
        if (_connection)
        {printf("Inside if (_connection)\n");
            mysql_close(_connection);
            _connection = NULL;
        }
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
    {printf("static Handle<Value> New\n");
        HandleScope scope;
        
        MysqlDbSync *connection = new MysqlDbSync();
        connection->Wrap(args.This());

        return args.This();
    }
    
    static Handle<Value> Connect (const Arguments& args)
    {printf("static Handle<Value> Connect\n");
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        //TODO: Check arguments
        /*if (args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(String::New("Must give conninfo string as argument"));
        }*/

        //TODO: Learn v8 types conversions
        String::Utf8Value servername(args[0]->ToString());
        String::Utf8Value user(args[1]->ToString());
        String::Utf8Value password(args[2]->ToString());
        String::Utf8Value database(args[3]->ToString());
        //Local<Integer> port(args[4]->ToInteger()); port.Value()
        String::Utf8Value socket(args[5]->ToString());

        bool r = connection->Connect(*servername, *user, *password, *database, 3306, *socket);

        //TODO:
        /*if (!r) {
            return ThrowException(Exception::Error(String::New(connection->ErrorMessage())));
        }*/

        return Undefined();
    }
    
    static Handle<Value> Close (const Arguments& args)
    {printf("static Handle<Value> Close\n");
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());
        
        connection->Close();
        
        return Undefined();
    }

};

extern "C" void init (v8::Handle<Object> target)
{
    MysqlDbSync::Init(target);
}

