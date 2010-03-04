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

//static Persistent<String> affectedRows_symbol;
//static Persistent<String> connect_symbol;
//static Persistent<String> close_symbol;
//static Persistent<String> escape_symbol;
//static Persistent<String> fetchResult_symbol;
//static Persistent<String> getInfo_symbol;
//static Persistent<String> lastInsertId_symbol;
//static Persistent<String> query_symbol;

class MysqlDbSync : public EventEmitter
{
  public:
    struct MysqlDbSyncInfo
    {
        unsigned long client_version;
        const char *client_info;
        unsigned long server_version;
        const char *server_info;
        const char *host_info;
        unsigned int proto_info;
    };
    
    static void Init(v8::Handle<v8::Object> target) 
    {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        //connect_symbol = NODE_PSYMBOL("affectedRows");
        //connect_symbol = NODE_PSYMBOL("connect");
        //close_symbol = NODE_PSYMBOL("close");
        //escape_symbol = NODE_PSYMBOL("escape");
        //fetchResult_symbol = NODE_PSYMBOL("fetchResult");
        //getInfo_symbol = NODE_PSYMBOL("getInfo");
        //lastInsertId_symbol = NODE_PSYMBOL("lastInsertId");
        //query_symbol = NODE_PSYMBOL("query");

        NODE_SET_PROTOTYPE_METHOD(t, "affectedRows", AffectedRows);
        NODE_SET_PROTOTYPE_METHOD(t, "connect", Connect);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "escape", Escape);
        NODE_SET_PROTOTYPE_METHOD(t, "fetchResult", FetchResult);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo", GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "lastInsertId", LastInsertId);
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
    
    const char* ErrorMessage ( )
    {
        return mysql_error(_connection);
    }
    
    MysqlDbSyncInfo GetInfo ()
    {
        MysqlDbSyncInfo info;
        
        info.client_version = mysql_get_client_version();
        info.client_info = mysql_get_client_info();
        info.server_version = mysql_get_server_version(_connection);
        info.server_info = mysql_get_server_info(_connection);
        info.host_info = mysql_get_host_info(_connection);
        info.proto_info = mysql_get_proto_info(_connection);
        
        return info;
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
    
    static Handle<Value> AffectedRows (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        if(!connection->_connection)
        {
            return ThrowException(String::New("Not connected"));
        }

        my_ulonglong affected_rows = mysql_affected_rows(connection->_connection);

        if(affected_rows == -1)
        {
            return ThrowException(String::New("Error occured in mysql_affected_rows()"));
        }

        Local<Value> js_result = Integer::New(affected_rows);
        
        return scope.Close(js_result);
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
        
        if(!connection->_connection)
        {
            return ThrowException(String::New("Not connected"));
        }
       
        if (args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(String::New("Nothing to escape"));
        }

        String::Utf8Value str(args[0]);
        
        int len = args[0]->ToString()->Utf8Length();
        char *result = new char[2*len + 1];
        if(!result)
        {
            return ThrowException(String::New("Not enough memory"));
        }

        int length = mysql_real_escape_string(connection->_connection, result, *str, len);
        Local<Value> js_result = String::New(result, length);
        
        delete[] result;
        return scope.Close(js_result);
    }
    
    static Handle<Value> FetchResult (const Arguments& args)
    {
        HandleScope scope;
   
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        MYSQL_RES *result = mysql_use_result(connection->_connection);
     
        if(!result)
        {
            return scope.Close(False()); 
        }

        MYSQL_FIELD *fields = mysql_fetch_fields(result);
        unsigned int num_fields = mysql_num_fields(result);
        MYSQL_ROW result_row;
        //my_ulonglong num_rows = mysql_num_rows(result); // Only use this with mysql_store_result() instead of mysql_use_result()
        int i = 0, j = 0;

        Local<Array> js_result = Array::New();
        Local<Object> js_result_row;
        Local<Value> js_field;
        
        i = 0;
        while( result_row = mysql_fetch_row(result) )
        {
            js_result_row = Object::New();
            
            for( j = 0; j < num_fields; j++ )
            {
                switch(fields[j].type)
                {
                  MYSQL_TYPE_BIT:
                  
                  MYSQL_TYPE_TINY:
                  MYSQL_TYPE_SHORT:
                  MYSQL_TYPE_LONG:
                    
                  MYSQL_TYPE_LONGLONG:
                  MYSQL_TYPE_INT24:
                    js_field = String::New(result_row[j])->ToInteger();
                    break;
                  MYSQL_TYPE_DECIMAL:
                  MYSQL_TYPE_FLOAT:
                  MYSQL_TYPE_DOUBLE:
                    js_field = String::New(result_row[j])->ToNumber();
                    break;
                  //TODO: Handle other types, dates in first order
                  /*  MYSQL_TYPE_NULL,   MYSQL_TYPE_TIMESTAMP,
                    MYSQL_TYPE_DATE,   MYSQL_TYPE_TIME,
                    MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
                    MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR,*/
                    /*MYSQL_TYPE_NEWDECIMAL=246,
                    MYSQL_TYPE_ENUM=247,
                    MYSQL_TYPE_SET=248,
                    MYSQL_TYPE_TINY_BLOB=249,
                    MYSQL_TYPE_MEDIUM_BLOB=250,
                    MYSQL_TYPE_LONG_BLOB=251,
                    MYSQL_TYPE_BLOB=252,*/
                  MYSQL_TYPE_VAR_STRING:
                  MYSQL_TYPE_STRING:
                    js_field = String::New(result_row[j]);
                    break;
                    /*MYSQL_TYPE_GEOMETRY=255*/
                  default:
                    js_field = String::New(result_row[j]);
                }
                
                js_result_row->Set(String::New(fields[j].name), js_field);
            }
            
            js_result->Set(Integer::New(i), js_result_row);
            
            i++;
        }
 
        return scope.Close(js_result); 
    }
    
    static Handle<Value> GetInfo (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());
        
        MysqlDbSyncInfo info = connection->GetInfo();
        
        Local<Object> result = Object::New();
        
        result->Set(String::New("client_version"), Integer::New(info.client_version));
        result->Set(String::New("client_info"), String::New(info.client_info));
        result->Set(String::New("server_version"), Integer::New(info.server_version));
        result->Set(String::New("server_info"), String::New(info.server_info));
        result->Set(String::New("host_info"), String::New(info.host_info));
        result->Set(String::New("proto_info"), Integer::New(info.proto_info));
               
        return scope.Close(result); 
    }

    static Handle<Value> LastInsertId (const Arguments& args)
    {
        HandleScope scope;
        
        MysqlDbSync *connection = ObjectWrap::Unwrap<MysqlDbSync>(args.This());

        if(!connection->_connection)
        {
            return ThrowException(String::New("Not connected"));
        }

        MYSQL_RES *result;
        my_ulonglong insert_id = 0;
        
        if( (result = mysql_store_result(connection->_connection)) == 0 &&
             mysql_field_count(connection->_connection) == 0 &&
             mysql_insert_id(connection->_connection) != 0)
        {
            insert_id = mysql_insert_id(connection->_connection);
        }

        Local<Value> js_result = Integer::New(insert_id);
        
        return scope.Close(js_result);
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

