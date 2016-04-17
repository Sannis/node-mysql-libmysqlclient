/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/*!
 * Include headers
 */
#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"
using namespace Nan ;
/*!
 * Init V8 structures for MysqlConnection class
 */
Nan::Persistent<FunctionTemplate> MysqlConnection::constructor_template;

void MysqlConnection::Init(Handle<Object> target) {
    Nan::HandleScope scope;
	// Constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New<String>("MysqlConnection").ToLocalChecked());
	
    constructor_template.Reset(tpl) ; 
    
    //AssignPersistent(FunctionTemplate, constructor_template, tpl);
    //tpl->SetClassName(Nan::NanSymbol("MysqlConnection"));

    // Instance template
    v8::Local<v8::ObjectTemplate> instance_template = tpl->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Instance properties
    Nan::SetAccessor(instance_template,Nan::New<String>("connectErrno").ToLocalChecked(),ConnectErrnoGetter);
    Nan::SetAccessor(instance_template,Nan::New<String>("connectError").ToLocalChecked(), ConnectErrorGetter);

    // Prototype methods
    Nan::SetPrototypeMethod(tpl, "affectedRowsSync",     AffectedRowsSync);
    Nan::SetPrototypeMethod(tpl, "autoCommitSync",       AutoCommitSync);
    Nan::SetPrototypeMethod(tpl, "changeUserSync",       ChangeUserSync);
    Nan::SetPrototypeMethod(tpl, "commitSync",           CommitSync);
    Nan::SetPrototypeMethod(tpl, "connect",              Connect);
    Nan::SetPrototypeMethod(tpl, "connectSync",          ConnectSync);
    Nan::SetPrototypeMethod(tpl, "connectedSync",        ConnectedSync);
    Nan::SetPrototypeMethod(tpl, "closeSync",            CloseSync);
    Nan::SetPrototypeMethod(tpl, "debugSync",            DebugSync);
    Nan::SetPrototypeMethod(tpl, "dumpDebugInfoSync",    DumpDebugInfoSync);
    Nan::SetPrototypeMethod(tpl, "errnoSync",            ErrnoSync);
    Nan::SetPrototypeMethod(tpl, "errorSync",            ErrorSync);
    Nan::SetPrototypeMethod(tpl, "escapeSync",           EscapeSync);
    Nan::SetPrototypeMethod(tpl, "fieldCountSync",       FieldCountSync);
    Nan::SetPrototypeMethod(tpl, "getCharsetSync",       GetCharsetSync);
    Nan::SetPrototypeMethod(tpl, "getCharsetNameSync",   GetCharsetNameSync);
    Nan::SetPrototypeMethod(tpl, "getClientInfoSync",    GetClientInfoSync);
    Nan::SetPrototypeMethod(tpl, "getInfoSync",          GetInfoSync);
    Nan::SetPrototypeMethod(tpl, "getInfoStringSync",    GetInfoStringSync);
    Nan::SetPrototypeMethod(tpl, "getWarningsSync",      GetWarningsSync);
    Nan::SetPrototypeMethod(tpl, "initSync",             InitSync);
    Nan::SetPrototypeMethod(tpl, "initStatementSync",    InitStatementSync);
    Nan::SetPrototypeMethod(tpl, "lastInsertIdSync",     LastInsertIdSync);
    Nan::SetPrototypeMethod(tpl, "multiMoreResultsSync", MultiMoreResultsSync);
    Nan::SetPrototypeMethod(tpl, "multiNextResultSync",  MultiNextResultSync);
    Nan::SetPrototypeMethod(tpl, "multiRealQuerySync",   MultiRealQuerySync);
    Nan::SetPrototypeMethod(tpl, "pingSync",             PingSync);
    Nan::SetPrototypeMethod(tpl, "query",                Query);
    Nan::SetPrototypeMethod(tpl, "querySend",            QuerySend);
    Nan::SetPrototypeMethod(tpl, "querySync",            QuerySync);
    Nan::SetPrototypeMethod(tpl, "realConnectSync",      RealConnectSync);
    Nan::SetPrototypeMethod(tpl, "realQuerySync",        RealQuerySync);
    Nan::SetPrototypeMethod(tpl, "rollbackSync",         RollbackSync);
    Nan::SetPrototypeMethod(tpl, "selectDbSync",         SelectDbSync);
    Nan::SetPrototypeMethod(tpl, "setCharsetSync",       SetCharsetSync);
    Nan::SetPrototypeMethod(tpl, "setOptionSync",        SetOptionSync);
    Nan::SetPrototypeMethod(tpl, "setSslSync",           SetSslSync);
    Nan::SetPrototypeMethod(tpl, "sqlStateSync",         SqlStateSync);
    Nan::SetPrototypeMethod(tpl, "statSync",             StatSync);
    Nan::SetPrototypeMethod(tpl, "storeResultSync",      StoreResultSync);
    Nan::SetPrototypeMethod(tpl, "threadIdSync",         ThreadIdSync);
    Nan::SetPrototypeMethod(tpl, "threadSafeSync",       ThreadSafeSync);
    Nan::SetPrototypeMethod(tpl, "useResultSync",        UseResultSync);
    Nan::SetPrototypeMethod(tpl, "warningCountSync",     WarningCountSync);
    // Make it visible in JavaScript land
    target->Set(Nan::New<String>("MysqlConnection").ToLocalChecked(), tpl->GetFunction());
}

bool MysqlConnection::Connect(const char* hostname,
                              const char* user,
                              const char* password,
                              const char* dbname,
                              uint32_t port,
                              const char* socket,
                              uint64_t flags) {
    if (this->_conn) {
        return false;
    }

    this->_conn = mysql_init(NULL);

    if (!this->_conn) {
        this->connected = false;
        return false;
    }

    bool unsuccessful = !mysql_real_connect(this->_conn,
                            hostname,
                            user,
                            password,
                            dbname,
                            port,
                            socket,
                            flags);

    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->connected = false;
        this->_conn = NULL;
        return false;
    }

    this->connected = true;
    return true;
}

bool MysqlConnection::RealConnect(const char* hostname,
                            const char* user,
                            const char* password,
                            const char* dbname,
                            uint32_t port,
                            const char* socket,
                            uint64_t flags) {
    if (!this->_conn) {
        return false;
    }

    if (this->connected) {
        return false;
    }

    bool unsuccessful = !mysql_real_connect(this->_conn,
                                            hostname,
                                            user,
                                            password,
                                            dbname,
                                            port,
                                            socket,
                                            flags);

    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        return false;
    }

    // MYSQL_OPT_RECONNECT option is modified by mysql_real_connect
    // due to bug in MySQL < 5.1.6
    // Save it state and repeat mysql_options after connect
    // See issue #66
#if MYSQL_VERSION_ID >= 50013 && MYSQL_VERSION_ID < 50106
    unsuccessful = mysql_options(this->_conn, MYSQL_OPT_RECONNECT,
                            static_cast<const char*>(&this->opt_reconnect));
    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        return false;
    }
#endif

    this->connected = true;
    return true;
}

void MysqlConnection::Close() {
    pthread_mutex_lock(&this->query_lock);
    if (this->_conn) {
        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        // this->multi_query = false;
        this->opt_reconnect = false;
        this->connect_errno = 0;
        this->connect_error = NULL;
    }
    pthread_mutex_unlock(&this->query_lock);
}

MysqlConnection::MysqlConnection(): ObjectWrap() {
    this->_conn = NULL;
    this->connected = false;
    this->multi_query = false;
    this->opt_reconnect = false;
    this->connect_errno = 0;
    this->connect_error = NULL;
    pthread_mutex_init(&this->query_lock, NULL);
}

MysqlConnection::~MysqlConnection() {
    this->Close();
    pthread_mutex_destroy(&this->query_lock);
}

/**
 * new MysqlConnection()
 *
 * Creates new MysqlConnection object
 **/
NAN_METHOD(MysqlConnection::New) {
    Nan::HandleScope scope;

    MysqlConnection *conn = new MysqlConnection();
    conn->Wrap(info.Holder());

    info.GetReturnValue().Set(info.This());
}

/** read-only
 * MysqlConnection#connectErrno -> Integer
 *
 * Gets last connect error number
 **/
NAN_GETTER(MysqlConnection::ConnectErrnoGetter) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
	
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());
	
    info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate,conn->connect_errno));
}

/** read-only
 * MysqlConnection#connectError -> String
 *
 * Gets last connect error string
 **/
NAN_GETTER(MysqlConnection::ConnectErrorGetter) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
	
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());
	if(conn->connect_error) 
		info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,conn->connect_error)) ; 
	else info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,"")) ; 
    //info.GetReturnValue().Set(Nan::New<String>(conn->connect_error ? conn->connect_error : ""));
}

/**
 * MysqlConnection#affectedRowsSync() -> Integer
 *
 * Gets number of affected rows in previous operation
 **/
NAN_METHOD(MysqlConnection::AffectedRowsSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

    if (affected_rows == ((my_ulonglong)-1)) {
        info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate,-1));
    }

    info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate,affected_rows));
}

/**
 * MysqlConnection#autoCommitSync(mode) -> Boolean
 * - mode (Boolean): Mode flag
 *
 * Sets autocommit mode
 **/
NAN_METHOD(MysqlConnection::AutoCommitSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_BOOL_ARG(0, autocomit)

    if (mysql_autocommit(conn->_conn, autocomit)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#changeUserSync(user[, password[, database]]) -> Boolean
 * - user (String): Username
 * - password (String): Password
 * - database (String): Database to use
 *
 * Changes the user and causes the database to become the default
 **/
NAN_METHOD(MysqlConnection::ChangeUserSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, user)

    // TODO(Sannis): Check logic
    if ( (info.Length() < 2) || (!info[0]->IsString()) ||
         (!info[1]->IsString()) ) {
        return Nan::ThrowTypeError("Must give at least user and password as arguments");
    }
    String::Utf8Value password(info[1]->ToString());

    if ( (info.Length() == 3) && (!info[2]->IsString()) ) {
        return Nan::ThrowTypeError("Must give string value as third argument, dbname");
    }
    String::Utf8Value dbname(info[2]->ToString());

    bool r = mysql_change_user(conn->_conn,
                               *user,
                               info[1]->IsString() ? *password : NULL,
                               info[2]->IsString() ? *dbname : NULL);

    if (r) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#commitSync() -> Boolean
 *
 * Commits the current transaction
 **/
NAN_METHOD(MysqlConnection::CommitSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_commit(conn->_conn)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Connect
 */
void MysqlConnection::EIO_After_Connect(uv_work_t *req) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    const int argc = 1;
    Local<Value> argv[argc];

    if (!conn_req->ok) {
        unsigned int error_string_length = strlen(conn_req->conn->connect_error) + 25;
        char* error_string = new char[error_string_length];
        snprintf(
            error_string, error_string_length,
            "Connection error #%d: %s",
            conn_req->conn->connect_errno, conn_req->conn->connect_error);

        argv[0] = V8EXC(error_string,isolate);
        delete[] error_string;
    } else {
        argv[0] = Nan::Null();
    }

    conn_req->nan_callback->Call(argc, argv);
    delete conn_req->nan_callback;

    conn_req->conn->Unref();

    delete conn_req;

    delete req;
}

void MysqlConnection::EIO_Connect(uv_work_t *req) {
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    conn_req->ok = conn_req->conn->Connect(
        conn_req->hostname ? **(conn_req->hostname) : NULL,
        conn_req->user ? **(conn_req->user) : NULL,
        conn_req->password ? **(conn_req->password) : NULL,
        conn_req->dbname ? **(conn_req->dbname) : NULL,
        conn_req->port,
        conn_req->socket ? **(conn_req->socket) : NULL,
        conn_req->flags
    ) ? true : false;

    delete conn_req->hostname;
    delete conn_req->user;
    delete conn_req->password;
    delete conn_req->socket;
}

/**
 * MysqlConnection#connect([hostname[, user[, password[, database[, port[, socket]]]]]], callback)
 * - hostname (String): Hostname
 * - user (String): Username
 * - password (String): Password
 * - database (String): Database to use
 * - port (Integer): Connection port
 * - socket (String): Connection socket
 * - callback (Function): Callback function, gets (error)
 *
 * Connects to the MySQL server
 **/
NAN_METHOD(MysqlConnection::Connect) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    REQ_FUN_ARG(info.Length() - 1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    if (conn->_conn) {
        const int argc = 1;
        v8::Local<Value> argv[argc];

        argv[0] = V8EXC("Already initialized. "
                        "Use conn.realConnectSync() after conn.initSync()",isolate);

        v8::TryCatch try_catch;

        callback->Call(Nan::GetCurrentContext()->Global(), argc, argv);

        if (try_catch.HasCaught()) {
            node::FatalException(isolate,try_catch);
        }

        info.GetReturnValue().Set(Nan::Undefined());
    }

    connect_request *conn_req = new connect_request;

    conn_req->nan_callback = new Nan::Callback(callback.As<Function>());

    conn_req->conn = conn;
    conn->Ref();

    String::Utf8Value *hostname = new String::Utf8Value(info[0]->ToString());
    String::Utf8Value *user     = new String::Utf8Value(info[1]->ToString());
    String::Utf8Value *password = new String::Utf8Value(info[2]->ToString());
    String::Utf8Value *dbname   = new String::Utf8Value(info[3]->ToString());
    uint32_t port               =                       info[4]->Uint32Value();
    String::Utf8Value *socket   = new String::Utf8Value(info[5]->ToString());
    uint64_t flags              =                       info[6]->Uint32Value();

    conn_req->hostname = info[0]->IsString() ? hostname : NULL;
    conn_req->user     = info[1]->IsString() ? user     : NULL;
    conn_req->password = info[2]->IsString() ? password : NULL;
    conn_req->dbname   = info[3]->IsString() ? dbname   : NULL;
    conn_req->port     = info[4]->IsUint32() ? port     : 0;
    conn_req->socket   = info[5]->IsString() ? socket   : NULL;
    conn_req->flags    = info[6]->IsUint32() ? flags    : 0;

    uv_work_t *_req = new uv_work_t; \
    _req->data = conn_req; \
    uv_queue_work(uv_default_loop(), _req, EIO_Connect, (uv_after_work_cb)EIO_After_Connect);

    info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlConnection#connectSync([hostname[, user[, password[, database[, port[, socket]]]]]]) -> Boolean
 * - hostname (String): Hostname
 * - user (String): Username
 * - password (String): Password
 * - database (String): Database to use
 * - port (Integer): Connection port
 * - socket (String): Connection socket
 *
 * Connects to the MySQL server
 **/
NAN_METHOD(MysqlConnection::ConnectSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());
    if (conn->_conn) {
        return Nan::ThrowError("Already initialized. Use conn.realConnectSync() after conn.initSync()");
    }

    String::Utf8Value hostname(info[0]->ToString());
    String::Utf8Value user(info[1]->ToString());
    String::Utf8Value password(info[2]->ToString());
    String::Utf8Value dbname(info[3]->ToString());
    uint32_t port = info[4]->Uint32Value();
    String::Utf8Value socket(info[5]->ToString());
    uint64_t flags = info[6]->Uint32Value();

    bool r = conn->Connect(
        info[0]->IsString() ? *hostname : NULL,
        info[1]->IsString() ? *user     : NULL,
        info[2]->IsString() ? *password : NULL,
        info[3]->IsString() ? *dbname   : NULL,
        info[4]->IsUint32() ? port      : 0,
        info[5]->IsString() ? *socket   : NULL,
        info[6]->IsUint32() ? flags     : 0
    );

    if (!r) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#connectedSync() -> Boolean
 *
 * Returns current connected status
 **/
NAN_METHOD(MysqlConnection::ConnectedSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    info.GetReturnValue().Set(conn->connected ? True() : False());
}

/**
 * MysqlConnection#closeSync()
 *
 * Closes database connection
 **/
NAN_METHOD(MysqlConnection::CloseSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    conn->Close();

    info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlConnection#debugSync(message)
 * - message (String): Debug message
 *
 * Performs debug logging
 **/
NAN_METHOD(MysqlConnection::DebugSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, debug)

    mysql_debug(*debug);

    info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlConnection#dumpDebugInfoSync() -> Boolean
 *
 * Dump debugging information into the log
 **/
NAN_METHOD(MysqlConnection::DumpDebugInfoSync) {
    Nan::HandleScope scope;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    info.GetReturnValue().Set(mysql_dump_debug_info(conn->_conn) ? False() : True());
}

/**
 * MysqlConnection#errnoSync() -> Integer
 *
 * Returns the last error code
 **/
NAN_METHOD(MysqlConnection::ErrnoSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate,mysql_errno(conn->_conn)));
}

/**
 * MysqlConnection#errorSync() -> String
 *
 * Returns the last error message
 **/
NAN_METHOD(MysqlConnection::ErrorSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *error = mysql_error(conn->_conn);

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,error));
}

/**
 * MysqlConnection#escapeSync(string) -> String
 * - string (String): String to escape
 *
 * Escapes special characters in a string for use in an SQL statement,
 * taking into account the current charset of the connection
 **/
NAN_METHOD(MysqlConnection::EscapeSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, str)

    unsigned long len = static_cast<unsigned long>(str.length());
    char *result = new char[2*len + 1];
    if (!result) {
        isolate->LowMemoryNotification();
        return Nan::ThrowError("Not enough memory");
    }

    len = mysql_real_escape_string(conn->_conn, result, *str, len);
    Local<Value> js_result = v8::String::NewFromUtf8(isolate,result);

    delete[] result;

    info.GetReturnValue().Set(js_result);
}

/**
 * MysqlConnection#fieldCountSync() -> Integer
 *
 * Returns the number of columns for the most recent query
 **/
NAN_METHOD(MysqlConnection::FieldCountSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    info.GetReturnValue().Set(
                Integer::NewFromUnsigned(isolate,
                    mysql_field_count(conn->_conn)));
}

/**
 * MysqlConnection#getCharsetSync() -> Object
 *
 * Returns a character set object
 **/
NAN_METHOD(MysqlConnection::GetCharsetSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MY_CHARSET_INFO cs;

    mysql_get_character_set_info(conn->_conn, &cs);

    Local<Object> js_result = v8::Object::New(isolate);

    js_result->Set(v8::String::NewFromUtf8(isolate,"charset"), v8::String::NewFromUtf8(isolate,cs.csname ? cs.csname : ""));
    js_result->Set(v8::String::NewFromUtf8(isolate,"collation"), v8::String::NewFromUtf8(isolate,cs.name ? cs.name : ""));
    js_result->Set(v8::String::NewFromUtf8(isolate,"dir"), v8::String::NewFromUtf8(isolate,cs.dir ? cs.dir : ""));
    js_result->Set(v8::String::NewFromUtf8(isolate,"min_length"), Integer::NewFromUnsigned(isolate,cs.mbminlen));
    js_result->Set(v8::String::NewFromUtf8(isolate,"max_length"), Integer::NewFromUnsigned(isolate,cs.mbmaxlen));
    js_result->Set(v8::String::NewFromUtf8(isolate,"number"), Integer::NewFromUnsigned(isolate,cs.number));
    js_result->Set(v8::String::NewFromUtf8(isolate,"state"), Integer::NewFromUnsigned(isolate,cs.state));
    js_result->Set(v8::String::NewFromUtf8(isolate,"comment"), v8::String::NewFromUtf8(isolate,cs.comment ? cs.comment : ""));

    info.GetReturnValue().Set(js_result);
}

/**
 * MysqlConnection#getCharsetNameSync() -> String
 *
 * Returns a character set name
 **/
NAN_METHOD(MysqlConnection::GetCharsetNameSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,mysql_character_set_name(conn->_conn)));
}

/**
 * MysqlConnection#getClientInfoSync() -> Object
 *
 * Returns the MySQL client version and information
 **/
NAN_METHOD(MysqlConnection::GetClientInfoSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    Local<Object> js_result = v8::Object::New(isolate);

    js_result->Set(v8::String::NewFromUtf8(isolate,"client_info"),
                   v8::String::NewFromUtf8(isolate,mysql_get_client_info()));
    js_result->Set(v8::String::NewFromUtf8(isolate,"client_version"),
                   Integer::NewFromUnsigned(isolate,mysql_get_client_version()));

    info.GetReturnValue().Set(js_result);
}

/**
 * MysqlConnection#getInfoSync() -> Object
 *
 * Returns the MySQL client, server, host and protocol version and information
 **/
NAN_METHOD(MysqlConnection::GetInfoSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    Local<Object> js_result = Object::New(isolate);

    js_result->Set(v8::String::NewFromUtf8(isolate,"client_info"),
                   v8::String::NewFromUtf8(isolate,mysql_get_client_info()));
    js_result->Set(v8::String::NewFromUtf8(isolate,"client_version"),
                   Integer::NewFromUnsigned(isolate,mysql_get_client_version()));
    js_result->Set(v8::String::NewFromUtf8(isolate,"server_info"),
                   v8::String::NewFromUtf8(isolate,mysql_get_server_info(conn->_conn)));
    js_result->Set(v8::String::NewFromUtf8(isolate,"server_version"),
                   Integer::NewFromUnsigned(isolate,
                       mysql_get_server_version(conn->_conn)));
    js_result->Set(v8::String::NewFromUtf8(isolate,"host_info"),
                   v8::String::NewFromUtf8(isolate,mysql_get_host_info(conn->_conn)));
    js_result->Set(v8::String::NewFromUtf8(isolate,"proto_info"),
                   Integer::NewFromUnsigned(isolate,mysql_get_proto_info(conn->_conn)));

    info.GetReturnValue().Set(js_result);
}

/**
 * MysqlConnection#getInfoStringSync() -> String
 *
 * Retrieves information about the most recently executed query
 **/
NAN_METHOD(MysqlConnection::GetInfoStringSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *infos = mysql_info(conn->_conn);

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,infos ? infos : ""));
}

/**
 * MysqlConnection#getWarningsSync() -> Array
 *
 * Gets result of SHOW WARNINGS
 **/
NAN_METHOD(MysqlConnection::GetWarningsSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    MYSQL_ROW row;
    int i = 0;

    Local<Object> js_warning = Object::New(isolate);
    Local<Array> js_result = Array::New(isolate);

    if (mysql_warning_count(conn->_conn)) {
        if (mysql_real_query(conn->_conn, "SHOW WARNINGS", 13) == 0) {
            result = mysql_store_result(conn->_conn);

            while ((row = mysql_fetch_row(result))) {
                js_warning->Set(v8::String::NewFromUtf8(isolate,"errno"), v8::String::NewFromUtf8(isolate,row[1])->ToInteger());

                js_warning->Set(v8::String::NewFromUtf8(isolate,"reason"), v8::String::NewFromUtf8(isolate,row[2]));

                js_result->Set(Integer::New(isolate,i), js_warning);

                i++;
            }

            mysql_free_result(result);
        }
    }

    info.GetReturnValue().Set(js_result);
}

/**
 * MysqlConnection#initSync() -> Boolean
 *
 * Initializes MysqlConnection
 **/
NAN_METHOD(MysqlConnection::InitSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    if (conn->_conn) {
        return Nan::ThrowError("Already initialized");
    }

    conn->_conn = mysql_init(NULL);

    if (!conn->_conn) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#initStatementSync() -> MysqlStatement|Boolean
 *
 * Initializes a statement and returns it
 **/
NAN_METHOD(MysqlConnection::InitStatementSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

    if (!my_statement) {
        info.GetReturnValue().Set(False());
    }
	
    Local<Object> local_js_result = MysqlStatement::NewInstance(my_statement);
    Nan::Persistent<Object> persistent_js_result ;
    persistent_js_result.Reset(local_js_result);
    //NanAssignPersistent(Object, persistent_js_result, local_js_result);
	v8::Local<v8::Object> ret = Nan::New(persistent_js_result) ;  
    info.GetReturnValue().Set(ret);
}

/**
 * MysqlConnection#lastInsertIdSync() -> Integer
 *
 * Returns the auto generated id used in the last query
 **/
NAN_METHOD(MysqlConnection::LastInsertIdSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
	
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    my_ulonglong insert_id = 0;

    if ( (result = mysql_store_result(conn->_conn)) == 0 &&
         mysql_field_count(conn->_conn) == 0 &&
         mysql_insert_id(conn->_conn) != 0) {
        insert_id = mysql_insert_id(conn->_conn);
    }

    info.GetReturnValue().Set(Integer::New(isolate,insert_id));
}

/**
 * MysqlConnection#multiMoreResultsSync() -> Boolean
 *
 * Checks if there are any more query results from a multi query
 **/
NAN_METHOD(MysqlConnection::MultiMoreResultsSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_more_results(conn->_conn)) {
        info.GetReturnValue().Set(True());
    }

    info.GetReturnValue().Set(False());
}

/**
 * MysqlConnection#multiNextResultSync() -> Boolean
 *
 * Prepare next result from multi query
 **/
NAN_METHOD(MysqlConnection::MultiNextResultSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (!mysql_more_results(conn->_conn)) {
        return Nan::ThrowError("There is no next result set. Please, call MultiMoreResultsSync() to check whether to call this function/method");
    }

    if (!mysql_next_result(conn->_conn)) {
        info.GetReturnValue().Set(True());
    }

    info.GetReturnValue().Set(False());
}

/**
 * MysqlConnection#multiRealQuerySync(query) -> Boolean
 * - query (String): Query
 *
 * Performs a multi query on the database
 **/
NAN_METHOD(MysqlConnection::MultiRealQuerySync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_ENABLE_MQ;
    unsigned int query_len = static_cast<unsigned int>(query.length());
    if (mysql_real_query(conn->_conn, *query, query_len) != 0) {
        MYSQLCONN_DISABLE_MQ;
        info.GetReturnValue().Set(False());
    }
    MYSQLCONN_DISABLE_MQ;

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#pingSync() -> Boolean
 *
 * Pings a server connection,
 * or tries to reconnect if the connection has gone down
 **/
NAN_METHOD(MysqlConnection::PingSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_ping(conn->_conn)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Query
 */
void MysqlConnection::EIO_After_Query(uv_work_t *req) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    struct query_request *query_req = (struct query_request *)(req->data);

    // We can't use const int argc here because argv is used
    // for both MysqlResult creation and callback call
    int argc = 1; // node.js convention, there is always at least one argument for callback
    Local<Value> argv[2];
    DEBUG_PRINTF("EIO_After_Query: in");
    if (!query_req->conn->_conn || !query_req->conn->connected || query_req->connection_closed) {
        DEBUG_PRINTF("EIO_After_Query: !query_req->conn->_conn || !query_req->conn->connected || query_req->connection_closed");
        // Check connection
        // If closeSync() is called after query(),
        // than connection is destroyed here
        // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
        argv[0] = V8EXC("Connection is closed by closeSync() during query",isolate);
    } else if (!query_req->ok) {
        unsigned int error_string_length = strlen(query_req->my_error) + 20;
        char* error_string = new char[error_string_length];
        snprintf(error_string, error_string_length, "Query error #%d: %s", query_req->my_errno, query_req->my_error);

        argv[0] = V8EXC(error_string,isolate);
        delete[] error_string;
    } else {
        argc = 2;
        argv[0] = Nan::Null();
        if (query_req->have_result_set) {
            Local<Object> local_js_result = MysqlResult::NewInstance(query_req->conn->_conn, query_req->my_result, query_req->field_count);
            argv[1] = local_js_result;
        } else {
            Local<Object> local_js_result = Object::New(isolate);
            local_js_result->Set(v8::String::NewFromUtf8(isolate,"affectedRows"),
                           Integer::New(isolate,query_req->affected_rows));
            local_js_result->Set(v8::String::NewFromUtf8(isolate,"insertId"),
                           Integer::New(isolate,query_req->insert_id));
            argv[1] = local_js_result;
        }
    }

    if (query_req->nan_callback) {
        DEBUG_PRINTF("EIO_After_Query: query_req->nan_callback->Call()");
        query_req->nan_callback->Call(argc, argv);
        DEBUG_PRINTF("EIO_After_Query: delete query_req->nan_callback");
        delete query_req->nan_callback;
    }

    // See comment above
    DEBUG_PRINTF("EIO_After_Query: Unref?");
    if (!query_req->conn->_conn || !query_req->conn->connected) {
        DEBUG_PRINTF("EIO_After_Query: Unref");
        query_req->conn->Unref();
        DEBUG_PRINTF("EIO_After_Query: Unref'ed");
    }

    DEBUG_PRINTF("EIO_After_Query: delete[] query_req->query");
    delete[] query_req->query;
    DEBUG_PRINTF("EIO_After_Query: delete query_req");
    delete query_req;

    DEBUG_PRINTF("EIO_After_Query: delete req");
    delete req;
}

void MysqlConnection::EIO_Query(uv_work_t *req) {
    struct query_request *query_req = (struct query_request *)(req->data);

    MysqlConnection *conn = query_req->conn;

    pthread_mutex_lock(&conn->query_lock);

    // Check connection
    // If closeSync() is called after query(),
    // than connection is destroyed here
    // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
    if (!conn->_conn || !conn->connected) {
        query_req->ok = false;
        query_req->connection_closed = true;

        pthread_mutex_unlock(&conn->query_lock);
        return;
    }
    query_req->connection_closed = false;

    MYSQLCONN_DISABLE_MQ;

    // we are protected with mutex, so set CURRENT request data
    // in connection (common object for ALL queries)
    SetCorrectLocalInfileHandlers(query_req->infile_data, conn->_conn);
    int r = mysql_real_query(conn->_conn, query_req->query, query_req->query_len);

    // clean after ourselves
    RestoreLocalInfileHandlers(query_req->infile_data, conn->_conn);

    int my_errno = mysql_errno(conn->_conn);
    if (r != 0 || my_errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->my_errno = my_errno;
        query_req->my_error = mysql_error(conn->_conn);
    } else {
        query_req->ok = true;

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        query_req->field_count = mysql_field_count(conn->_conn);

        if (my_result) {
            // Valid result set (may be empty, of cause)
            query_req->have_result_set = true;
            query_req->my_result = my_result;
        } else {
            if (query_req->field_count == 0) {
                // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
                query_req->have_result_set = false;
                // UPDATE or DELETE?
                query_req->affected_rows = mysql_affected_rows(conn->_conn);
                // INSERT?
                query_req->insert_id = mysql_insert_id(conn->_conn);
            } else {
                // Result store error
                query_req->ok = false;
                query_req->my_errno = my_errno;
                query_req->my_error = mysql_error(conn->_conn);
            }
        }
    }

    pthread_mutex_unlock(&conn->query_lock);
}

/**
 * MysqlConnection#query(query, callback)
 * - query (String): Query
 * - callback (Function): Callback function, gets (error, result)
 *
 * Performs a query on the database.
 * Uses mysql_real_query.
 **/
NAN_METHOD(MysqlConnection::Query) {
    Nan::HandleScope scope;

    DEBUG_PRINTF("Query: in");
    DEBUG_ARGS();

    REQ_STR_ARG(0, query);
    OPTIONAL_BUFFER_ARG(1, optional_local_infile_buffer);

    int optional_callback_arg_number;
    if (optional_local_infile_buffer->IsNull()) {
        DEBUG_PRINTF("Query: local_infile_buffer->IsNull()");
        optional_callback_arg_number = 1;
    } else {
        DEBUG_PRINTF("Query: !local_infile_buffer->IsNull()");
        optional_callback_arg_number = 2;
    }
    OPTIONAL_FUN_ARG(optional_callback_arg_number, optional_callback);

    DEBUG_PRINTF("Query: continue");

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;
    unsigned int query_len = static_cast<unsigned int>(query.length());

    query_req->query = new char[query_len + 1];
    query_req->query_len = query_len;
    query_req->infile_data = MysqlConnection::PrepareLocalInfileData(optional_local_infile_buffer);
    // Copy query from V8 value to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    if (optional_callback->IsFunction()) {
        DEBUG_PRINTF("Query: optional_callback->IsFunction()");
        query_req->nan_callback = new Nan::Callback(optional_callback.As<Function>());
    } else {
        DEBUG_PRINTF("Query: !optional_callback->IsFunction()");
        query_req->nan_callback = NULL;
    }

    query_req->conn = conn;
    conn->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = query_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Query, (uv_after_work_cb)EIO_After_Query);

    info.GetReturnValue().Set(Nan::Undefined());
}

/*!
 * Callback function for MysqlConnection::QuerySend
 */
void MysqlConnection::EV_After_QuerySend(uv_poll_t* handle, int status, int events) {
    Nan::HandleScope scope;

    // Fake uv_work_t struct for EIO_After_Query call
    uv_work_t *fake_req = new uv_work_t;
    fake_req->data = handle->data;

    // Stop IO watcher
    uv_poll_stop(handle);
    uv_close((uv_handle_t *)handle, EV_After_QuerySend_OnWatchHandleClose);

    struct query_request *query_req = (struct query_request *)handle->data;

    MysqlConnection *conn = query_req->conn;

    // Check connection
    // If closeSync() is called after query(),
    // than connection is destroyed here
    // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
    if (!conn->_conn || !conn->connected) {
        query_req->ok = false;
        query_req->connection_closed = true;

        DEBUG_PRINTF("EV_After_QuerySend: !conn->_conn || !conn->connected");

        // The callback part, just call the existing code
        EIO_After_Query(fake_req);
        return;
    }
    query_req->connection_closed = false;

    // The query part, mostly same with EIO_Query
    // TODO merge the same parts with EIO_Query
    int r = mysql_read_query_result(conn->_conn);
    int my_errno = mysql_errno(conn->_conn);
    if (r != 0 || my_errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->my_errno = my_errno;
        query_req->my_error = mysql_error(conn->_conn);
    } else {
        query_req->ok = true;

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        query_req->field_count = mysql_field_count(conn->_conn);

        if (my_result) {
            // Valid result set (may be empty, of cause)
            query_req->have_result_set = true;
            query_req->my_result = my_result;
        } else {
            if (query_req->field_count == 0) {
                // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
                query_req->have_result_set = false;
                // UPDATE or DELETE?
                query_req->affected_rows = mysql_affected_rows(query_req->conn->_conn);
                // INSERT?
                query_req->insert_id = mysql_insert_id(query_req->conn->_conn);
            } else {
                // Result store error
                query_req->ok = false;
                query_req->my_errno = my_errno;
                query_req->my_error = mysql_error(query_req->conn->_conn);
            }
        }
    }

    // The callback part, just call the existing code
    EIO_After_Query(fake_req);
}

void MysqlConnection::EV_After_QuerySend_OnWatchHandleClose(uv_handle_t* handle) {
    delete handle;
}

/**
 * MysqlConnection#querySend(query, callback)
 * - query (String): Query
 * - callback (Function): Callback function, gets (errro, result)
 *
 * Performs a query on the database.
 * Uses mysql_send_query.
 **/
NAN_METHOD(MysqlConnection::QuerySend) {
    Nan::HandleScope scope;

    DEBUG_PRINTF("QuerySend: in");
    DEBUG_ARGS();

    REQ_STR_ARG(0, query);
    OPTIONAL_FUN_ARG(1, optional_callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    query_req->query = new char[query_len + 1];

    // Copy query from V8 var to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    if (optional_callback->IsFunction()) {
        DEBUG_PRINTF("QuerySend: optional_callback->IsFunction()\n\n\n");
        query_req->nan_callback = new Nan::Callback(optional_callback.As<Function>());
    } else {
        DEBUG_PRINTF("QuerySend: !optional_callback->IsFunction()\n\n\n");
        query_req->nan_callback = NULL;
    }

    query_req->conn = conn;
    conn->Ref();

    // Send query
    mysql_send_query(conn->_conn, query_req->query, query_len + 1);

    // Init IO watcher
    uv_poll_t* handle = new uv_poll_t;
    handle->data = query_req;
    uv_poll_init(uv_default_loop(), handle, conn->_conn->net.fd);
    uv_poll_start(handle, UV_READABLE, EV_After_QuerySend);

    info.GetReturnValue().Set(Nan::Undefined());
}


/**
 * MysqlConnection#querySync(query) -> MysqlResult
 * - query (String): Query
 *
 * Performs a query on the database.
 **/
NAN_METHOD(MysqlConnection::QuerySync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    REQ_STR_ARG(0, query)
    OPTIONAL_BUFFER_ARG(1, local_infile_buffer);

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    MYSQL_RES *my_result = NULL;
    unsigned int field_count = 0 ;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    local_infile_data * infile_data = PrepareLocalInfileData(local_infile_buffer);
    // Only one query can be executed on a connection at a time
    pthread_mutex_lock(&conn->query_lock);
    SetCorrectLocalInfileHandlers(infile_data, conn->_conn);
    int r = mysql_real_query(conn->_conn, *query, query_len);
    RestoreLocalInfileHandlers(infile_data, conn->_conn);
    if (r == 0) {
        my_result = mysql_store_result(conn->_conn);
        field_count = mysql_field_count(conn->_conn);
    }

    pthread_mutex_unlock(&conn->query_lock);
    if (r != 0) {
        // Query error
        info.GetReturnValue().Set(False());
    }

    if (!my_result) {
        if (field_count == 0) {
            // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
            info.GetReturnValue().Set(True());
        } else {
            // Error
            info.GetReturnValue().Set(False());
        }
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, field_count);

    info.GetReturnValue().Set(local_js_result);
}

/**
 * MysqlConnection#rollbackSync() -> Boolean
 *
 * Rolls back current transaction
 **/
NAN_METHOD(MysqlConnection::RollbackSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_rollback(conn->_conn)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#realConnectSync([hostname[, user[, password[, database[, port[, socket]]]]]]) -> Boolean
 * - hostname (String): Hostname
 * - user (String): Username
 * - password (String): Password
 * - database (String): Database to use
 * - port (Integer): Connection port
 * - socket (String): Connection socket
 *
 * Open a new connection to the MySQL server
 **/
NAN_METHOD(MysqlConnection::RealConnectSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_INITIALIZED;

    String::Utf8Value hostname(info[0]->ToString());
    String::Utf8Value user(info[1]->ToString());
    String::Utf8Value password(info[2]->ToString());
    String::Utf8Value dbname(info[3]->ToString());
    uint32_t port = info[4]->Uint32Value();
    String::Utf8Value socket(info[5]->ToString());
    uint64_t flags = info[6]->Uint32Value();
    bool r = conn->RealConnect(info[0]->IsString() ? *hostname : NULL,
                               info[1]->IsString() ? *user     : NULL,
                               info[2]->IsString() ? *password : NULL,
                               info[3]->IsString() ? *dbname   : NULL,
                               info[4]->IsUint32() ? port      : 0,
                               info[5]->IsString() ? *socket   : NULL,
                               info[6]->IsUint32() ? flags     : 0);

    if (!r) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#realQuerySync() -> Boolean
 * - query (String): Query
 *
 * Executes an SQL query
 **/
NAN_METHOD(MysqlConnection::RealQuerySync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    unsigned int query_len = static_cast<unsigned int>(query.length());

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, *query, query_len);
    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#selectDbSync(database) -> Boolean
 * - database (String): Database to use
 *
 * Selects the default database for database queries
 **/
NAN_METHOD(MysqlConnection::SelectDbSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, dbname)

    if (mysql_select_db(conn->_conn, *dbname)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#setCharsetSync() -> Boolean
 * - charset (String): Charset
 *
 * Sets the default client character set
 **/
NAN_METHOD(MysqlConnection::SetCharsetSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, charset)

    if (mysql_set_character_set(conn->_conn, *charset)) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#setOptionSync(key, value) -> Boolean
 * - key (Integer): Option key
 * - value (Integer|String): Option value
 *
 * Sets connection options
 **/
NAN_METHOD(MysqlConnection::SetOptionSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    REQ_INT_ARG(0, option_integer_key);
    mysql_option option_key = static_cast<mysql_option>(option_integer_key);
    int r = 1;

    switch (option_key) {
        case MYSQL_OPT_CONNECT_TIMEOUT:
        case MYSQL_OPT_PROTOCOL:
        case MYSQL_OPT_READ_TIMEOUT:
        case MYSQL_OPT_WRITE_TIMEOUT:
        case MYSQL_OPT_RECONNECT:
        case MYSQL_OPT_COMPRESS:
        case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
        case MYSQL_REPORT_DATA_TRUNCATION:
        case MYSQL_SECURE_AUTH:
            {
            REQ_INT_ARG(1, option_integer_value);
            r = mysql_options(conn->_conn,
                              option_key,
                              static_cast<const char *>(
                                static_cast<const void *>(
                                  &option_integer_value)));
            // MYSQL_OPT_RECONNECT option is modified by mysql_real_connect
            // due to bug in MySQL < 5.1.6
            // Save it state and repeat mysql_options after connect
            // See issue #66
#if MYSQL_VERSION_ID >= 50013 && MYSQL_VERSION_ID < 50106
            if (!r && (option_key == MYSQL_OPT_RECONNECT)) {
                conn->opt_reconnect = option_integer_value;
            }
#endif
            }
            break;
        case MYSQL_READ_DEFAULT_FILE:
        case MYSQL_READ_DEFAULT_GROUP:
        case MYSQL_INIT_COMMAND:
        case MYSQL_SET_CHARSET_NAME:
        case MYSQL_SET_CHARSET_DIR:
            {
            REQ_STR_ARG(1, option_string_value);
            r = mysql_options(conn->_conn, option_key, *option_string_value);
            }
            break;
        case MYSQL_OPT_LOCAL_INFILE:
            r  = mysql_options(conn->_conn, option_key, NULL);
            break;
        case MYSQL_OPT_NAMED_PIPE:
        case MYSQL_SHARED_MEMORY_BASE_NAME:
            return Nan::ThrowError("This option isn't used because Windows");
            break;
        case MYSQL_OPT_GUESS_CONNECTION:
        case MYSQL_SET_CLIENT_IP:
        case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
        case MYSQL_OPT_USE_REMOTE_CONNECTION:
            return Nan::ThrowError("This option isn't used because not embedded");
            break;
        case MYSQL_OPT_USE_RESULT:
        default:
            return Nan::ThrowError("This option isn't supported by MySQL");
    }

    if (r) {
        info.GetReturnValue().Set(False());
    }

    info.GetReturnValue().Set(True());
}

/**
 * MysqlConnection#setSslSync()
 *
 * Sets SSL options
 * Used for establishing secure connections
 *
 * - key (String): Key file
 * - cert (String): Certificate file
 * - ca (String): Certificate authority file
 * - capath(String): CA certificates in PEM directory
 * - cipher (String): Allowable ciphers
 **/
NAN_METHOD(MysqlConnection::SetSslSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_INITIALIZED;

    if (info.Length() < 5) {
        return Nan::ThrowError("setSslSync() require 5 string or null arguments");
    }

    OPTIONAL_STR_ARG(0, key)
    OPTIONAL_STR_ARG(1, cert)
    OPTIONAL_STR_ARG(2, ca)
    OPTIONAL_STR_ARG(3, capath)
    OPTIONAL_STR_ARG(4, cipher)

    mysql_ssl_set(
        conn->_conn,
        key,
        cert,
        ca,
        capath,
        cipher
    );

    info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlConnection#sqlStateSync() -> String
 *
 * Returns the SQLSTATE error from previous MySQL operation
 **/
NAN_METHOD(MysqlConnection::SqlStateSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,mysql_sqlstate(conn->_conn)));
}

/**
 * MysqlConnection#statSync() -> String
 *
 * Gets the current system status
 **/
NAN_METHOD(MysqlConnection::StatSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *stat = mysql_stat(conn->_conn);

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,stat ? stat : ""));
}

/**
 * MysqlConnection#storeResultSync() -> MysqlResult
 *
 * Transfers a result set from the last query
 **/
NAN_METHOD(MysqlConnection::StoreResultSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        info.GetReturnValue().Set(True());
    }

    MYSQL_RES *my_result = mysql_store_result(conn->_conn);

    if (!my_result) {
        info.GetReturnValue().Set(False());
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, mysql_field_count(conn->_conn));
    Nan::Persistent<Object> persistent_js_result;
    persistent_js_result.Reset(local_js_result) ; 
    
    v8::Local<v8::Object> ret = Nan::New(persistent_js_result) ; 
    
    //NanAssignPersistent(Object, persistent_js_result, local_js_result);

    info.GetReturnValue().Set(ret);
}

/**
 * MysqlConnection#threadIdSync() -> Integer
 *
 * Returns the thread ID for the current connection
 **/
NAN_METHOD(MysqlConnection::ThreadIdSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    info.GetReturnValue().Set(Integer::New(isolate,thread_id));
}

/**
 * MysqlConnection#threadSafeSync() -> Boolean
 *
 * Returns whether thread safety is given or not
 **/
NAN_METHOD(MysqlConnection::ThreadSafeSync) {
    Nan::HandleScope scope;

    if (mysql_thread_safe()) {
        info.GetReturnValue().Set(True());
    } else {
        info.GetReturnValue().Set(False());
    }
}

/**
 * MysqlConnection#useResultSync() -> MysqlResult
 *
 * Initiates a result set retrieval for the last query
 **/
NAN_METHOD(MysqlConnection::UseResultSync) {
    Nan::HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        info.GetReturnValue().Set(True());
    }

    MYSQL_RES *my_result = mysql_use_result(conn->_conn);

    if (!my_result) {
        info.GetReturnValue().Set(False());
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, mysql_field_count(conn->_conn));
	Nan::Persistent<Object> persistent_js_result;
    persistent_js_result.Reset(local_js_result) ;  
    v8::Local<v8::Object> ret = Nan::New(persistent_js_result) ; 
    info.GetReturnValue().Set(ret);
}

/**
 * MysqlConnection#warningCountSync() -> Integer
 *
 * Returns the number of warnings from the last query for the given link
 **/
NAN_METHOD(MysqlConnection::WarningCountSync) {
    Nan::HandleScope scope;
	v8::Isolate * isolate ;
	isolate = v8::Isolate::GetCurrent() ;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint32_t warning_count = mysql_warning_count(conn->_conn);

    info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate,warning_count));
}
int MysqlConnection::CustomLocalInfileInit(void ** ptr, const char * filename, void * userdata) {
  *ptr = userdata;
  return 0;
}
int MysqlConnection::CustomLocalInfileRead(void * ptr, char * buf, unsigned int buf_len) {
  local_infile_data * infile_data = static_cast<local_infile_data *>(ptr);
  if (!infile_data->buffer || !infile_data->length) {
    return 0;
  }
  if (infile_data->position >= infile_data->length) {
    return 0;
  }
  size_t copy_len = infile_data->length - infile_data->position;
  copy_len = copy_len < buf_len ? copy_len : buf_len;
  memcpy(buf, infile_data->buffer + infile_data->position, copy_len);
  infile_data->position += copy_len;
  return copy_len;
}
void MysqlConnection::CustomLocalInfileEnd(void * ptr) {
}
int MysqlConnection::CustomLocalInfileError(void * ptr,
                                            char *error_msg,
                                            unsigned int error_msg_len) {
  return 0;
}

MysqlConnection::local_infile_data * MysqlConnection::PrepareLocalInfileData(Handle<Value> buffer) {
  local_infile_data * infile_data;
  if (buffer->IsNull()) {
    return NULL;
  }
  infile_data = static_cast<local_infile_data*>(malloc(sizeof(*infile_data)));
  infile_data->length = node::Buffer::Length(buffer->ToObject());
  infile_data->position = 0;
  infile_data->buffer = static_cast<char *>(malloc(infile_data->length));
  memcpy(infile_data->buffer, node::Buffer::Data(buffer->ToObject()), infile_data->length);
  return infile_data;
}
void MysqlConnection::SetCorrectLocalInfileHandlers(local_infile_data * infile_data, MYSQL * conn) {
  if (infile_data) {
    mysql_set_local_infile_handler(conn,
                                   MysqlConnection::CustomLocalInfileInit,
                                   MysqlConnection::CustomLocalInfileRead,
                                   MysqlConnection::CustomLocalInfileEnd,
                                   MysqlConnection::CustomLocalInfileError,
                                   infile_data);
  } else {
    // default infile handlers uses thread local storage,
    // so to be shure everything is ok, init thread specific data.
    // it's cheap enough, it initializes  everything needed only
    // first time, all subsequent calls do nothing
    mysql_thread_init();
    mysql_set_local_infile_default(conn);
  }
}
void MysqlConnection::RestoreLocalInfileHandlers(local_infile_data * infile_data, MYSQL * conn) {
  if (infile_data) {
    mysql_set_local_infile_default(conn);
    if (infile_data->buffer) {
      free(infile_data->buffer);
    }
    free(infile_data);
  } else {
    mysql_thread_end();
  }
}
