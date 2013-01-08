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
#include<stdio.h>
/*!
 * Init V8 structures for MysqlConnection class
 */
Persistent<FunctionTemplate> MysqlConnection::constructor_template;

void MysqlConnection::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(MysqlConnection::New);

    // Constructor template
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->SetClassName(String::NewSymbol("MysqlConnection"));

    // Instance template
    Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Properties
    instance_template->SetAccessor(V8STR("connectErrno"), MysqlConnection::ConnectErrnoGetter);
    instance_template->SetAccessor(V8STR("connectError"), MysqlConnection::ConnectErrorGetter);

    // Methods
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "affectedRowsSync",     MysqlConnection::AffectedRowsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "autoCommitSync",       MysqlConnection::AutoCommitSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "changeUserSync",       MysqlConnection::ChangeUserSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "commitSync",           MysqlConnection::CommitSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "connect",              MysqlConnection::Connect);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "connectSync",          MysqlConnection::ConnectSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "connectedSync",        MysqlConnection::ConnectedSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "closeSync",            MysqlConnection::CloseSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "debugSync",            MysqlConnection::DebugSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "dumpDebugInfoSync",    MysqlConnection::DumpDebugInfoSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "errnoSync",            MysqlConnection::ErrnoSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "errorSync",            MysqlConnection::ErrorSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "escapeSync",           MysqlConnection::EscapeSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fieldCountSync",       MysqlConnection::FieldCountSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getCharsetSync",       MysqlConnection::GetCharsetSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getCharsetNameSync",   MysqlConnection::GetCharsetNameSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getClientInfoSync",    MysqlConnection::GetClientInfoSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getInfoSync",          MysqlConnection::GetInfoSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getInfoStringSync",    MysqlConnection::GetInfoStringSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getWarningsSync",      MysqlConnection::GetWarningsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "initSync",             MysqlConnection::InitSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "initStatementSync",    MysqlConnection::InitStatementSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "lastInsertIdSync",     MysqlConnection::LastInsertIdSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "multiMoreResultsSync", MysqlConnection::MultiMoreResultsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "multiNextResultSync",  MysqlConnection::MultiNextResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "multiRealQuerySync",   MysqlConnection::MultiRealQuerySync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "pingSync",             MysqlConnection::PingSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "query",                MysqlConnection::Query);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "querySend",            MysqlConnection::QuerySend);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "querySync",            MysqlConnection::QuerySync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "realConnectSync",      MysqlConnection::RealConnectSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "realQuerySync",        MysqlConnection::RealQuerySync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "rollbackSync",         MysqlConnection::RollbackSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "selectDbSync",         MysqlConnection::SelectDbSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setCharsetSync",       MysqlConnection::SetCharsetSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setOptionSync",        MysqlConnection::SetOptionSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setSslSync",           MysqlConnection::SetSslSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "sqlStateSync",         MysqlConnection::SqlStateSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "statSync",             MysqlConnection::StatSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "storeResultSync",      MysqlConnection::StoreResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "threadIdSync",         MysqlConnection::ThreadIdSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "threadSafeSync",       MysqlConnection::ThreadSafeSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "useResultSync",        MysqlConnection::UseResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "warningCountSync",     MysqlConnection::WarningCountSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlConnection"), constructor_template->GetFunction());
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
    DEBUG_PRINTF("Close: pthread_mutex_lock\n");
    pthread_mutex_lock(&this->query_lock);
    DEBUG_PRINTF("Close: pthread_mutex_lock'ed\n");
    if (this->_conn) {

        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        // this->multi_query = false;
        this->opt_reconnect = false;
        this->connect_errno = 0;
        this->connect_error = NULL;
    }
    DEBUG_PRINTF("Close: pthread_mutex_unlock\n");
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
Handle<Value> MysqlConnection::New(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = new MysqlConnection();
    conn->Wrap(args.Holder());

    return args.Holder();
}

/** read-only
 * MysqlConnection#connectErrno -> Integer
 *
 * Gets last connect error number
 **/
Handle<Value> MysqlConnection::ConnectErrnoGetter(Local<String> property, const AccessorInfo &info) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    return scope.Close(Integer::NewFromUnsigned(conn->connect_errno));
}

/** read-only
 * MysqlConnection#connectError -> String
 *
 * Gets last connect error string
 **/
Handle<Value> MysqlConnection::ConnectErrorGetter(Local<String> property, const AccessorInfo &info) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    return scope.Close(V8STR(conn->connect_error ? conn->connect_error : ""));
}

/**
 * MysqlConnection#affectedRowsSync() -> Integer
 *
 * Gets number of affected rows in previous operation
 **/
Handle<Value> MysqlConnection::AffectedRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

    if (affected_rows == ((my_ulonglong)-1)) {
        return scope.Close(Integer::New(-1));
    }

    return scope.Close(Integer::New(affected_rows));
}

/**
 * MysqlConnection#autoCommitSync(mode) -> Boolean
 * - mode (Boolean): Mode flag
 *
 * Sets autocommit mode
 **/
Handle<Value> MysqlConnection::AutoCommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_BOOL_ARG(0, autocomit)

    if (mysql_autocommit(conn->_conn, autocomit)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#changeUserSync(user[, password[, database]]) -> Boolean
 * - user (String): Username
 * - password (String): Password
 * - database (String): Database to use
 *
 * Changes the user and causes the database to become the default
 **/
Handle<Value> MysqlConnection::ChangeUserSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, user)

    // TODO(Sannis): Check logic
    if ( (args.Length() < 2) || (!args[0]->IsString()) ||
         (!args[1]->IsString()) ) {
        return THRTYPEEXC("Must give at least user and password as arguments");
    }
    String::Utf8Value password(args[1]->ToString());

    if ( (args.Length() == 3) && (!args[2]->IsString()) ) {
        return THRTYPEEXC("Must give string value as third argument, dbname");
    }
    String::Utf8Value dbname(args[2]->ToString());

    bool r = mysql_change_user(conn->_conn,
                               *user,
                               args[1]->IsString() ? *password : NULL,
                               args[2]->IsString() ? *dbname : NULL);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#commitSync() -> Boolean
 *
 * Commits the current transaction
 **/
Handle<Value> MysqlConnection::CommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_commit(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Connect
 */
void MysqlConnection::EIO_After_Connect(uv_work_t *req) {
    HandleScope scope;

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

        argv[0] = V8EXC(error_string);
        delete[] error_string;
    } else {
        argv[0] = Local<Value>::New(Null());
    }

    node::MakeCallback(Context::GetCurrent()->Global(), conn_req->callback, argc, argv);

    conn_req->callback.Dispose();

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
Handle<Value> MysqlConnection::Connect(const Arguments& args) {
    HandleScope scope;

    REQ_FUN_ARG(args.Length() - 1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (conn->_conn) {
        const int argc = 1;
        Local<Value> argv[argc];

        argv[0] = V8EXC("Already initialized. "
                        "Use conn.realConnectSync() after conn.initSync()");

        TryCatch try_catch;

        callback->Call(Context::GetCurrent()->Global(), argc, argv);

        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }

        return Undefined();
    }

    connect_request *conn_req = new connect_request;

    conn_req->callback = Persistent<Function>::New(callback);
    conn_req->conn = conn;
    conn->Ref();

    String::Utf8Value *hostname = new String::Utf8Value(args[0]->ToString());
    String::Utf8Value *user     = new String::Utf8Value(args[1]->ToString());
    String::Utf8Value *password = new String::Utf8Value(args[2]->ToString());
    String::Utf8Value *dbname   = new String::Utf8Value(args[3]->ToString());
    uint32_t port               =                       args[4]->Uint32Value();
    String::Utf8Value *socket   = new String::Utf8Value(args[5]->ToString());
    uint64_t flags              =                       args[6]->Uint32Value();

    conn_req->hostname = args[0]->IsString() ? hostname : NULL;
    conn_req->user     = args[1]->IsString() ? user     : NULL;
    conn_req->password = args[2]->IsString() ? password : NULL;
    conn_req->dbname   = args[3]->IsString() ? dbname   : NULL;
    conn_req->port     = args[4]->IsUint32() ? port     : 0;
    conn_req->socket   = args[5]->IsString() ? socket   : NULL;
    conn_req->flags    = args[6]->IsUint32() ? flags    : 0;

    uv_work_t *_req = new uv_work_t; \
    _req->data = conn_req; \
    uv_queue_work(uv_default_loop(), _req, EIO_Connect, (uv_after_work_cb)EIO_After_Connect);

    return Undefined();
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
Handle<Value> MysqlConnection::ConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());
    if (conn->_conn) {
        return THREXC("Already initialized. "
                      "Use conn.realConnectSync() after conn.initSync()");
    }

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->Uint32Value();
    String::Utf8Value socket(args[5]->ToString());
    uint64_t flags = args[6]->Uint32Value();

    bool r = conn->Connect(
        args[0]->IsString() ? *hostname : NULL,
        args[1]->IsString() ? *user     : NULL,
        args[2]->IsString() ? *password : NULL,
        args[3]->IsString() ? *dbname   : NULL,
        args[4]->IsUint32() ? port      : 0,
        args[5]->IsString() ? *socket   : NULL,
        args[6]->IsUint32() ? flags     : 0
    );

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#connectedSync() -> Boolean
 *
 * Returns current connected status
 **/
Handle<Value> MysqlConnection::ConnectedSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    return scope.Close(conn->connected ? True() : False());
}

/**
 * MysqlConnection#closeSync()
 *
 * Closes database connection
 **/
Handle<Value> MysqlConnection::CloseSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    conn->Close();

    return Undefined();
}

/**
 * MysqlConnection#debugSync(message)
 * - message (String): Debug message
 *
 * Performs debug logging
 **/
Handle<Value> MysqlConnection::DebugSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, debug)

    mysql_debug(*debug);

    return Undefined();
}

/**
 * MysqlConnection#dumpDebugInfoSync() -> Boolean
 *
 * Dump debugging information into the log
 **/
Handle<Value> MysqlConnection::DumpDebugInfoSync(const Arguments& args) {
    HandleScope scope;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(mysql_dump_debug_info(conn->_conn) ? False() : True());
}

/**
 * MysqlConnection#errnoSync() -> Integer
 *
 * Returns the last error code
 **/
Handle<Value> MysqlConnection::ErrnoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(Integer::NewFromUnsigned(mysql_errno(conn->_conn)));
}

/**
 * MysqlConnection#errorSync() -> String
 *
 * Returns the last error message
 **/
Handle<Value> MysqlConnection::ErrorSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *error = mysql_error(conn->_conn);

    return scope.Close(V8STR(error));
}

/**
 * MysqlConnection#escapeSync(string) -> String
 * - string (String): String to escape
 *
 * Escapes special characters in a string for use in an SQL statement,
 * taking into account the current charset of the connection
 **/
Handle<Value> MysqlConnection::EscapeSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, str)

    unsigned long len = static_cast<unsigned long>(str.length());
    char *result = new char[2*len + 1];
    if (!result) {
        V8::LowMemoryNotification();
        return THREXC("Not enough memory");
    }

    len = mysql_real_escape_string(conn->_conn, result, *str, len);
    Local<Value> js_result = V8STR2(result, len);

    delete[] result;

    return scope.Close(js_result);
}

/**
 * MysqlConnection#fieldCountSync() -> Integer
 *
 * Returns the number of columns for the most recent query
 **/
Handle<Value> MysqlConnection::FieldCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(
                Integer::NewFromUnsigned(
                    mysql_field_count(conn->_conn)));
}

/**
 * MysqlConnection#getCharsetSync() -> Object
 *
 * Returns a character set object
 **/
Handle<Value> MysqlConnection::GetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MY_CHARSET_INFO cs;

    mysql_get_character_set_info(conn->_conn, &cs);

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("charset"), V8STR(cs.csname ? cs.csname : ""));
    js_result->Set(V8STR("collation"), V8STR(cs.name ? cs.name : ""));
    js_result->Set(V8STR("dir"), V8STR(cs.dir ? cs.dir : ""));
    js_result->Set(V8STR("min_length"), Integer::NewFromUnsigned(cs.mbminlen));
    js_result->Set(V8STR("max_length"), Integer::NewFromUnsigned(cs.mbmaxlen));
    js_result->Set(V8STR("number"), Integer::NewFromUnsigned(cs.number));
    js_result->Set(V8STR("state"), Integer::NewFromUnsigned(cs.state));
    js_result->Set(V8STR("comment"), V8STR(cs.comment ? cs.comment : ""));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#getCharsetNameSync() -> String
 *
 * Returns a character set name
 **/
Handle<Value> MysqlConnection::GetCharsetNameSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(V8STR(mysql_character_set_name(conn->_conn)));
}

/**
 * MysqlConnection#getClientInfoSync() -> Object
 *
 * Returns the MySQL client version and information
 **/
Handle<Value> MysqlConnection::GetClientInfoSync(const Arguments& args) {
    HandleScope scope;

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("client_info"),
                   V8STR(mysql_get_client_info()));
    js_result->Set(V8STR("client_version"),
                   Integer::NewFromUnsigned(mysql_get_client_version()));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#getInfoSync() -> Object
 *
 * Returns the MySQL client, server, host and protocol version and information
 **/
Handle<Value> MysqlConnection::GetInfoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("client_info"),
                   V8STR(mysql_get_client_info()));
    js_result->Set(V8STR("client_version"),
                   Integer::NewFromUnsigned(mysql_get_client_version()));
    js_result->Set(V8STR("server_info"),
                   V8STR(mysql_get_server_info(conn->_conn)));
    js_result->Set(V8STR("server_version"),
                   Integer::NewFromUnsigned(
                       mysql_get_server_version(conn->_conn)));
    js_result->Set(V8STR("host_info"),
                   V8STR(mysql_get_host_info(conn->_conn)));
    js_result->Set(V8STR("proto_info"),
                   Integer::NewFromUnsigned(mysql_get_proto_info(conn->_conn)));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#getInfoStringSync() -> String
 *
 * Retrieves information about the most recently executed query
 **/
Handle<Value> MysqlConnection::GetInfoStringSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *info = mysql_info(conn->_conn);

    return scope.Close(V8STR(info ? info : ""));
}

/**
 * MysqlConnection#getWarningsSync() -> Array
 *
 * Gets result of SHOW WARNINGS
 **/
Handle<Value> MysqlConnection::GetWarningsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    MYSQL_ROW row;
    int i = 0;

    Local<Object> js_warning = Object::New();
    Local<Array> js_result = Array::New();

    if (mysql_warning_count(conn->_conn)) {
        if (mysql_real_query(conn->_conn, "SHOW WARNINGS", 13) == 0) {
            result = mysql_store_result(conn->_conn);

            while ((row = mysql_fetch_row(result))) {
                js_warning->Set(V8STR("errno"), V8STR(row[1])->ToInteger());

                js_warning->Set(V8STR("reason"), V8STR(row[2]));

                js_result->Set(Integer::New(i), js_warning);

                i++;
            }

            mysql_free_result(result);
        }
    }

    return scope.Close(js_result);
}

/**
 * MysqlConnection#initSync() -> Boolean
 *
 * Initializes MysqlConnection
 **/
Handle<Value> MysqlConnection::InitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (conn->_conn) {
        return THREXC("Already initialized");
    }

    conn->_conn = mysql_init(NULL);

    if (!conn->_conn) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#initStatementSync() -> MysqlStatement|Boolean
 *
 * Initializes a statement and returns it
 **/
Handle<Value> MysqlConnection::InitStatementSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

    if (!my_statement) {
        return scope.Close(False());
    }

    Local<Value> arg = External::New(my_statement);
    Persistent<Object> js_result(MysqlStatement::constructor_template->
                             GetFunction()->NewInstance(1, &arg));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#lastInsertIdSync() -> Integer
 *
 * Returns the auto generated id used in the last query
 **/
Handle<Value> MysqlConnection::LastInsertIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    my_ulonglong insert_id = 0;

    if ( (result = mysql_store_result(conn->_conn)) == 0 &&
         mysql_field_count(conn->_conn) == 0 &&
         mysql_insert_id(conn->_conn) != 0) {
        insert_id = mysql_insert_id(conn->_conn);
    }

    return scope.Close(Integer::New(insert_id));
}

/**
 * MysqlConnection#multiMoreResultsSync() -> Boolean
 *
 * Checks if there are any more query results from a multi query
 **/
Handle<Value> MysqlConnection::MultiMoreResultsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_more_results(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

/**
 * MysqlConnection#multiNextResultSync() -> Boolean
 *
 * Prepare next result from multi query
 **/
Handle<Value> MysqlConnection::MultiNextResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (!mysql_more_results(conn->_conn)) {
        return THREXC("There is no next result set."
                        "Please, call MultiMoreResultsSync() to check "
                        "whether to call this function/method");
    }

    if (!mysql_next_result(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

/**
 * MysqlConnection#multiRealQuerySync(query) -> Boolean
 * - query (String): Query
 *
 * Performs a multi query on the database
 **/
Handle<Value> MysqlConnection::MultiRealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_ENABLE_MQ;
    unsigned int query_len = static_cast<unsigned int>(query.length());
    if (mysql_real_query(conn->_conn, *query, query_len) != 0) {
        MYSQLCONN_DISABLE_MQ;
        return scope.Close(False());
    }
    MYSQLCONN_DISABLE_MQ;

    return scope.Close(True());
}

/**
 * MysqlConnection#pingSync() -> Boolean
 *
 * Pings a server connection,
 * or tries to reconnect if the connection has gone down
 **/
Handle<Value> MysqlConnection::PingSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_ping(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Query
 */
void MysqlConnection::EIO_After_Query(uv_work_t *req) {
    HandleScope scope;

    struct query_request *query_req = (struct query_request *)(req->data);

    // We can't use const int argc here because argv is used
    // for both MysqlResult creation and callback call
    int argc = 1; // node.js convention, there is always at least one argument for callback
    Local<Value> argv[3];
    DEBUG_PRINTF("EIO_After_Query: in\n");
    if (!query_req->conn->_conn || !query_req->conn->connected || query_req->connection_closed) {
        DEBUG_PRINTF("EIO_After_Query: !query_req->conn->_conn || !query_req->conn->connected || query_req->connection_closed\n");
        // Check connection
        // If closeSync() is called after query(),
        // than connection is destroyed here
        // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
        argv[0] = V8EXC("Connection is closed by closeSync() during query");
    } else if (!query_req->ok) {
        unsigned int error_string_length = strlen(query_req->error) + 20;
        char* error_string = new char[error_string_length];
        snprintf(error_string, error_string_length, "Query error #%d: %s",
                 query_req->errno, query_req->error);

        argv[0] = V8EXC(error_string);
        delete[] error_string;
    } else {
        if (query_req->have_result_set) {
            argv[0] = External::New(query_req->conn->_conn);
            argv[1] = External::New(query_req->my_result);
            argv[2] = Integer::NewFromUnsigned(query_req->field_count);
            Persistent<Object> js_result(MysqlResult::constructor_template->
                                     GetFunction()->NewInstance(3, argv));

            argv[1] = Local<Object>::New(js_result);
        } else {
            Local<Object> js_result = Object::New();
            js_result->Set(V8STR("affectedRows"),
                           Integer::New(query_req->affected_rows));
            js_result->Set(V8STR("insertId"),
                           Integer::New(query_req->insert_id));
            argv[1] = Local<Object>::New(js_result);
        }
        argc = 2;
        argv[0] = Local<Value>::New(Null());
    }

    if (query_req->callback->IsFunction()) {
        DEBUG_PRINTF("EIO_After_Query: node::MakeCallback\n");
        node::MakeCallback(
            Context::GetCurrent()->Global(),
            Persistent<Function>::Cast(query_req->callback),
            argc, argv
        );

        query_req->callback.Dispose();
    }

    // See comment above
    DEBUG_PRINTF("EIO_After_Query: Unref?\n");
    if (!query_req->conn->_conn || !query_req->conn->connected) {
        DEBUG_PRINTF("EIO_After_Query: Unref\n");
        query_req->conn->Unref();
        DEBUG_PRINTF("EIO_After_Query: Unref'ed\n");
    }

    delete[] query_req->query;
    delete query_req;

    delete req;
}

void MysqlConnection::EIO_Query(uv_work_t *req) {
    struct query_request *query_req = (struct query_request *)(req->data);

    MysqlConnection *conn = query_req->conn;

    DEBUG_PRINTF("EIO_Query: pthread_mutex_lock\n");
    pthread_mutex_lock(&conn->query_lock);
    DEBUG_PRINTF("EIO_Query: pthread_mutex_lock'ed\n");

    // Check connection
    // If closeSync() is called after query(),
    // than connection is destroyed here
    // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
    if (!conn->_conn || !conn->connected) {
        query_req->ok = false;
        query_req->connection_closed = true;

        DEBUG_PRINTF("EIO_Query: !conn->_conn || !conn->connected\n");
        DEBUG_PRINTF("EIO_Query: pthread_mutex_unlock\n");
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

    int errno = mysql_errno(conn->_conn);
    if (r != 0 || errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->errno = errno;
        query_req->error = mysql_error(conn->_conn);
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
                query_req->errno = errno;
                query_req->error = mysql_error(conn->_conn);
            }
        }
    }
    DEBUG_PRINTF("EIO_Query: pthread_mutex_unlock\n");
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
Handle<Value> MysqlConnection::Query(const Arguments& args) {
    HandleScope scope;

    REQ_STR_ARG(0, query);
    OPTIONAL_BUFFER_ARG(1, local_infile_buffer);

    Handle<Value> callback;
    if (local_infile_buffer->IsNull()) {
      OPTIONAL_FUN_ARG(1, possibly_callback);
      callback = possibly_callback;
    } else {
      OPTIONAL_FUN_ARG(2, possibly_callback);
      callback = possibly_callback;
    }

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;
    unsigned int query_len = static_cast<unsigned int>(query.length());

    query_req->query = new char[query_len + 1];
    query_req->query_len = query_len;
    query_req->infile_data = MysqlConnection::PrepareLocalInfileData(local_infile_buffer);
    // Copy query from V8 value to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    query_req->callback = Persistent<Value>::New(callback);
    query_req->conn = conn;
    conn->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = query_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Query, (uv_after_work_cb)EIO_After_Query);

    return Undefined();
}

/*!
 * Callback function for MysqlConnection::QuerySend
 */
void MysqlConnection::EV_After_QuerySend(NODE_ADDON_SHIM_IO_WATCH_CALLBACK_ARGUMENTS) {
    HandleScope scope;

    uv_work_t *_req = new uv_work_t;

    // Fake uv_work_t struct for EIO_After_Query call
#if NODE_VERSION_AT_LEAST(0, 7, 9)
    _req->data = handle->data;
#else
    _req->data = io_watcher->data;
#endif

    // Stop IO watcher
    NODE_ADDON_SHIM_STOP_IO_WATCH(EV_After_QuerySend_OnWatchHandleClose)

    struct query_request *query_req = (struct query_request *)(_req->data);

    MysqlConnection *conn = query_req->conn;

    // Check connection
    // If closeSync() is called after query(),
    // than connection is destroyed here
    // https://github.com/Sannis/node-mysql-libmysqlclient/issues/157
    if (!conn->_conn || !conn->connected) {
        query_req->ok = false;
        query_req->connection_closed = true;

        DEBUG_PRINTF("EV_After_QuerySend: !conn->_conn || !conn->connected\n");

        // The callback part, just call the existing code
        EIO_After_Query(_req);

        return;
    }
    query_req->connection_closed = false;

    // The query part, mostly same with EIO_Query
    // TODO merge the same parts with EIO_Query
    int r = mysql_read_query_result(conn->_conn);
    int errno = mysql_errno(conn->_conn);
    if (r != 0 || errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->errno = errno;
        query_req->error = mysql_error(conn->_conn);
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
                query_req->errno = errno;
                query_req->error = mysql_error(query_req->conn->_conn);
            }
        }
    }

    // The callback part, just call the existing code
    EIO_After_Query(_req);
}

/**
 * MysqlConnection#querySend(query, callback)
 * - query (String): Query
 * - callback (Function): Callback function, gets (errro, result)
 *
 * Performs a query on the database.
 * Uses mysql_send_query.
 */
Handle<Value> MysqlConnection::QuerySend(const Arguments& args) {
    HandleScope scope;

    REQ_STR_ARG(0, query);
    OPTIONAL_FUN_ARG(1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    query_req->query = new char[query_len + 1];

    // Copy query from V8 var to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    query_req->callback = Persistent<Value>::New(callback);
    query_req->conn = conn;
    conn->Ref();

    // Send query
    mysql_send_query(conn->_conn, query_req->query, query_len + 1);

    // Init IO watcher
    NODE_ADDON_SHIM_START_IO_READABLE_WATCH(query_req, EV_After_QuerySend, conn->_conn->net.fd)

    return Undefined();
}


/**
 * MysqlConnection#querySync(query) -> MysqlResult
 * - query (String): Query
 *
 * Performs a query on the database.
 **/
Handle<Value> MysqlConnection::QuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    REQ_STR_ARG(0, query)
    OPTIONAL_BUFFER_ARG(1, local_infile_buffer);

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    MYSQL_RES *my_result = NULL;
    unsigned int field_count;

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
        return scope.Close(False());
    }

    if (!my_result) {
        if (field_count == 0) {
            // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
            return scope.Close(True());
        } else {
            // Error
            return scope.Close(False());
        }
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(field_count);
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#rollbackSync() -> Boolean
 *
 * Rolls back current transaction
 **/
Handle<Value> MysqlConnection::RollbackSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_rollback(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
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
Handle<Value> MysqlConnection::RealConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_INITIALIZED;

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->Uint32Value();
    String::Utf8Value socket(args[5]->ToString());
    uint64_t flags = args[6]->Uint32Value();
    bool r = conn->RealConnect(args[0]->IsString() ? *hostname : NULL,
                               args[1]->IsString() ? *user     : NULL,
                               args[2]->IsString() ? *password : NULL,
                               args[3]->IsString() ? *dbname   : NULL,
                               args[4]->IsUint32() ? port      : 0,
                               args[5]->IsString() ? *socket   : NULL,
                               args[6]->IsUint32() ? flags     : 0);

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#realQuerySync() -> Boolean
 * - query (String): Query
 *
 * Executes an SQL query
 **/
Handle<Value> MysqlConnection::RealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    unsigned int query_len = static_cast<unsigned int>(query.length());

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, *query, query_len);
    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#selectDbSync(database) -> Boolean
 * - database (String): Database to use
 *
 * Selects the default database for database queries
 **/
Handle<Value> MysqlConnection::SelectDbSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, dbname)

    if (mysql_select_db(conn->_conn, *dbname)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#setCharsetSync() -> Boolean
 * - charset (String): Charset
 *
 * Sets the default client character set
 **/
Handle<Value> MysqlConnection::SetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, charset)

    if (mysql_set_character_set(conn->_conn, *charset)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * MysqlConnection#setOptionSync(key, value) -> Boolean
 * - key (Integer): Option key
 * - value (Integer|String): Option value
 *
 * Sets connection options
 **/
Handle<Value> MysqlConnection::SetOptionSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

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
            return THREXC("This option isn't used because Windows");
            break;
        case MYSQL_OPT_GUESS_CONNECTION:
        case MYSQL_SET_CLIENT_IP:
        case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
        case MYSQL_OPT_USE_REMOTE_CONNECTION:
            return THREXC("This option isn't used because not embedded");
            break;
        case MYSQL_OPT_USE_RESULT:
        default:
            return THREXC("This option isn't supported by MySQL");
    }

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
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
Handle<Value> MysqlConnection::SetSslSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_INITIALIZED;

    if (args.Length() < 5) {
        return THREXC("setSslSync() require 5 string or null arguments");
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

    return scope.Close(Undefined());
}

/**
 * MysqlConnection#sqlStateSync() -> String
 *
 * Returns the SQLSTATE error from previous MySQL operation
 **/
Handle<Value> MysqlConnection::SqlStateSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(V8STR(mysql_sqlstate(conn->_conn)));
}

/**
 * MysqlConnection#statSync() -> String
 *
 * Gets the current system status
 **/
Handle<Value> MysqlConnection::StatSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *stat = mysql_stat(conn->_conn);

    return scope.Close(V8STR(stat ? stat : ""));
}

/**
 * MysqlConnection#storeResultSync() -> MysqlResult
 *
 * Transfers a result set from the last query
 **/
Handle<Value> MysqlConnection::StoreResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_store_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#threadIdSync() -> Integer
 *
 * Returns the thread ID for the current connection
 **/
Handle<Value> MysqlConnection::ThreadIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    return scope.Close(Integer::New(thread_id));
}

/**
 * MysqlConnection#threadSafeSync() -> Boolean
 *
 * Returns whether thread safety is given or not
 **/
Handle<Value> MysqlConnection::ThreadSafeSync(const Arguments& args) {
    HandleScope scope;

    if (mysql_thread_safe()) {
        return scope.Close(True());
    } else {
        return scope.Close(False());
    }
}

/**
 * MysqlConnection#useResultSync() -> MysqlResult
 *
 * Initiates a result set retrieval for the last query
 **/
Handle<Value> MysqlConnection::UseResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_use_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * MysqlConnection#warningCountSync() -> Integer
 *
 * Returns the number of warnings from the last query for the given link
 **/
Handle<Value> MysqlConnection::WarningCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint32_t warning_count = mysql_warning_count(conn->_conn);

    return scope.Close(Integer::NewFromUnsigned(warning_count));
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
void MysqlConnection::SetCorrectLocalInfileHandlers(local_infile_data * infile_data,
                                                    MYSQL * conn) {
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
void MysqlConnection::RestoreLocalInfileHandlers(local_infile_data * infile_data,
                                                 MYSQL * conn) {
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
