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

/*!
 * Init V8 structures for MysqlConnection class
 */
Persistent<FunctionTemplate> MysqlConnection::constructor_template;

void MysqlConnection::Init(Handle<Object> target) {
    NanScope();

    // Constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
    tpl->SetClassName(NanSymbol("MysqlConnection"));

    // Instance template
    Local<ObjectTemplate> instance_template = tpl->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Instance properties
    instance_template->SetAccessor(V8STR("connectErrno"), ConnectErrnoGetter);
    instance_template->SetAccessor(V8STR("connectError"), ConnectErrorGetter);

    // Prototype methods
    NODE_SET_PROTOTYPE_METHOD(tpl, "affectedRowsSync",     AffectedRowsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "autoCommitSync",       AutoCommitSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "changeUserSync",       ChangeUserSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "commitSync",           CommitSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connect",              Connect);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connectSync",          ConnectSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connectedSync",        ConnectedSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "closeSync",            CloseSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "debugSync",            DebugSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dumpDebugInfoSync",    DumpDebugInfoSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "errnoSync",            ErrnoSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "errorSync",            ErrorSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "escapeSync",           EscapeSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fieldCountSync",       FieldCountSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCharsetSync",       GetCharsetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCharsetNameSync",   GetCharsetNameSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getClientInfoSync",    GetClientInfoSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getInfoSync",          GetInfoSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getInfoStringSync",    GetInfoStringSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getWarningsSync",      GetWarningsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "initSync",             InitSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "initStatementSync",    InitStatementSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "lastInsertIdSync",     LastInsertIdSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "multiMoreResultsSync", MultiMoreResultsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "multiNextResultSync",  MultiNextResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "multiRealQuerySync",   MultiRealQuerySync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "pingSync",             PingSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "query",                Query);
    NODE_SET_PROTOTYPE_METHOD(tpl, "querySend",            QuerySend);
    NODE_SET_PROTOTYPE_METHOD(tpl, "querySync",            QuerySync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "realConnectSync",      RealConnectSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "realQuerySync",        RealQuerySync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "rollbackSync",         RollbackSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "selectDbSync",         SelectDbSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setCharsetSync",       SetCharsetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setOptionSync",        SetOptionSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setSslSync",           SetSslSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "sqlStateSync",         SqlStateSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "statSync",             StatSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "storeResultSync",      StoreResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "threadIdSync",         ThreadIdSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "threadSafeSync",       ThreadSafeSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "useResultSync",        UseResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "warningCountSync",     WarningCountSync);

    // Make it visible in JavaScript land
    target->Set(NanSymbol("MysqlConnection"), tpl->GetFunction());
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
    NanScope();

    MysqlConnection *conn = new MysqlConnection();
    conn->Wrap(args.Holder());

    NanReturnValue(args.Holder());
}

/** read-only
 * MysqlConnection#connectErrno -> Integer
 *
 * Gets last connect error number
 **/
NAN_GETTER(MysqlConnection::ConnectErrnoGetter) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    NanReturnValue(Integer::NewFromUnsigned(conn->connect_errno));
}

/** read-only
 * MysqlConnection#connectError -> String
 *
 * Gets last connect error string
 **/
NAN_GETTER(MysqlConnection::ConnectErrorGetter) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    NanReturnValue(V8STR(conn->connect_error ? conn->connect_error : ""));
}

/**
 * MysqlConnection#affectedRowsSync() -> Integer
 *
 * Gets number of affected rows in previous operation
 **/
NAN_METHOD(MysqlConnection::AffectedRowsSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

    if (affected_rows == ((my_ulonglong)-1)) {
        NanReturnValue(Integer::New(-1));
    }

    NanReturnValue(Integer::New(affected_rows));
}

/**
 * MysqlConnection#autoCommitSync(mode) -> Boolean
 * - mode (Boolean): Mode flag
 *
 * Sets autocommit mode
 **/
NAN_METHOD(MysqlConnection::AutoCommitSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_BOOL_ARG(0, autocomit)

    if (mysql_autocommit(conn->_conn, autocomit)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
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
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, user)

    // TODO(Sannis): Check logic
    if ( (args.Length() < 2) || (!args[0]->IsString()) ||
         (!args[1]->IsString()) ) {
        return NanThrowTypeError("Must give at least user and password as arguments");
    }
    String::Utf8Value password(args[1]->ToString());

    if ( (args.Length() == 3) && (!args[2]->IsString()) ) {
        return NanThrowTypeError("Must give string value as third argument, dbname");
    }
    String::Utf8Value dbname(args[2]->ToString());

    bool r = mysql_change_user(conn->_conn,
                               *user,
                               args[1]->IsString() ? *password : NULL,
                               args[2]->IsString() ? *dbname : NULL);

    if (r) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#commitSync() -> Boolean
 *
 * Commits the current transaction
 **/
NAN_METHOD(MysqlConnection::CommitSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_commit(conn->_conn)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Connect
 */
void MysqlConnection::EIO_After_Connect(uv_work_t *req) {
    NanScope();

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
        argv[0] = NanNewLocal(Null());
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
    NanScope();

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

        NanReturnUndefined();
    }

    connect_request *conn_req = new connect_request;

    conn_req->nan_callback = new NanCallback(callback.As<Function>());

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

    NanReturnUndefined();
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
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());
    if (conn->_conn) {
        return NanThrowError("Already initialized. Use conn.realConnectSync() after conn.initSync()");
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
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#connectedSync() -> Boolean
 *
 * Returns current connected status
 **/
NAN_METHOD(MysqlConnection::ConnectedSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    NanReturnValue(conn->connected ? True() : False());
}

/**
 * MysqlConnection#closeSync()
 *
 * Closes database connection
 **/
NAN_METHOD(MysqlConnection::CloseSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    conn->Close();

    NanReturnUndefined();
}

/**
 * MysqlConnection#debugSync(message)
 * - message (String): Debug message
 *
 * Performs debug logging
 **/
NAN_METHOD(MysqlConnection::DebugSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, debug)

    mysql_debug(*debug);

    NanReturnUndefined();
}

/**
 * MysqlConnection#dumpDebugInfoSync() -> Boolean
 *
 * Dump debugging information into the log
 **/
NAN_METHOD(MysqlConnection::DumpDebugInfoSync) {
    NanScope();
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    NanReturnValue(mysql_dump_debug_info(conn->_conn) ? False() : True());
}

/**
 * MysqlConnection#errnoSync() -> Integer
 *
 * Returns the last error code
 **/
NAN_METHOD(MysqlConnection::ErrnoSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    NanReturnValue(Integer::NewFromUnsigned(mysql_errno(conn->_conn)));
}

/**
 * MysqlConnection#errorSync() -> String
 *
 * Returns the last error message
 **/
NAN_METHOD(MysqlConnection::ErrorSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *error = mysql_error(conn->_conn);

    NanReturnValue(V8STR(error));
}

/**
 * MysqlConnection#escapeSync(string) -> String
 * - string (String): String to escape
 *
 * Escapes special characters in a string for use in an SQL statement,
 * taking into account the current charset of the connection
 **/
NAN_METHOD(MysqlConnection::EscapeSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, str)

    unsigned long len = static_cast<unsigned long>(str.length());
    char *result = new char[2*len + 1];
    if (!result) {
        V8::LowMemoryNotification();
        return NanThrowError("Not enough memory");
    }

    len = mysql_real_escape_string(conn->_conn, result, *str, len);
    Local<Value> js_result = V8STR2(result, len);

    delete[] result;

    NanReturnValue(js_result);
}

/**
 * MysqlConnection#fieldCountSync() -> Integer
 *
 * Returns the number of columns for the most recent query
 **/
NAN_METHOD(MysqlConnection::FieldCountSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    NanReturnValue(
                Integer::NewFromUnsigned(
                    mysql_field_count(conn->_conn)));
}

/**
 * MysqlConnection#getCharsetSync() -> Object
 *
 * Returns a character set object
 **/
NAN_METHOD(MysqlConnection::GetCharsetSync) {
    NanScope();

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

    NanReturnValue(js_result);
}

/**
 * MysqlConnection#getCharsetNameSync() -> String
 *
 * Returns a character set name
 **/
NAN_METHOD(MysqlConnection::GetCharsetNameSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    NanReturnValue(V8STR(mysql_character_set_name(conn->_conn)));
}

/**
 * MysqlConnection#getClientInfoSync() -> Object
 *
 * Returns the MySQL client version and information
 **/
NAN_METHOD(MysqlConnection::GetClientInfoSync) {
    NanScope();

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("client_info"),
                   V8STR(mysql_get_client_info()));
    js_result->Set(V8STR("client_version"),
                   Integer::NewFromUnsigned(mysql_get_client_version()));

    NanReturnValue(js_result);
}

/**
 * MysqlConnection#getInfoSync() -> Object
 *
 * Returns the MySQL client, server, host and protocol version and information
 **/
NAN_METHOD(MysqlConnection::GetInfoSync) {
    NanScope();

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

    NanReturnValue(js_result);
}

/**
 * MysqlConnection#getInfoStringSync() -> String
 *
 * Retrieves information about the most recently executed query
 **/
NAN_METHOD(MysqlConnection::GetInfoStringSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *info = mysql_info(conn->_conn);

    NanReturnValue(V8STR(info ? info : ""));
}

/**
 * MysqlConnection#getWarningsSync() -> Array
 *
 * Gets result of SHOW WARNINGS
 **/
NAN_METHOD(MysqlConnection::GetWarningsSync) {
    NanScope();

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

    NanReturnValue(js_result);
}

/**
 * MysqlConnection#initSync() -> Boolean
 *
 * Initializes MysqlConnection
 **/
NAN_METHOD(MysqlConnection::InitSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (conn->_conn) {
        return NanThrowError("Already initialized");
    }

    conn->_conn = mysql_init(NULL);

    if (!conn->_conn) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#initStatementSync() -> MysqlStatement|Boolean
 *
 * Initializes a statement and returns it
 **/
NAN_METHOD(MysqlConnection::InitStatementSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

    if (!my_statement) {
        NanReturnValue(False());
    }

    Local<Object> local_js_result = MysqlStatement::NewInstance(my_statement);
    Persistent<Object> persistent_js_result;
    NanAssignPersistent(Object, persistent_js_result, local_js_result);

    NanReturnValue(persistent_js_result);
}

/**
 * MysqlConnection#lastInsertIdSync() -> Integer
 *
 * Returns the auto generated id used in the last query
 **/
NAN_METHOD(MysqlConnection::LastInsertIdSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    my_ulonglong insert_id = 0;

    if ( (result = mysql_store_result(conn->_conn)) == 0 &&
         mysql_field_count(conn->_conn) == 0 &&
         mysql_insert_id(conn->_conn) != 0) {
        insert_id = mysql_insert_id(conn->_conn);
    }

    NanReturnValue(Integer::New(insert_id));
}

/**
 * MysqlConnection#multiMoreResultsSync() -> Boolean
 *
 * Checks if there are any more query results from a multi query
 **/
NAN_METHOD(MysqlConnection::MultiMoreResultsSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_more_results(conn->_conn)) {
        NanReturnValue(True());
    }

    NanReturnValue(False());
}

/**
 * MysqlConnection#multiNextResultSync() -> Boolean
 *
 * Prepare next result from multi query
 **/
NAN_METHOD(MysqlConnection::MultiNextResultSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (!mysql_more_results(conn->_conn)) {
        return NanThrowError("There is no next result set. Please, call MultiMoreResultsSync() to check whether to call this function/method");
    }

    if (!mysql_next_result(conn->_conn)) {
        NanReturnValue(True());
    }

    NanReturnValue(False());
}

/**
 * MysqlConnection#multiRealQuerySync(query) -> Boolean
 * - query (String): Query
 *
 * Performs a multi query on the database
 **/
NAN_METHOD(MysqlConnection::MultiRealQuerySync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_ENABLE_MQ;
    unsigned int query_len = static_cast<unsigned int>(query.length());
    if (mysql_real_query(conn->_conn, *query, query_len) != 0) {
        MYSQLCONN_DISABLE_MQ;
        NanReturnValue(False());
    }
    MYSQLCONN_DISABLE_MQ;

    NanReturnValue(True());
}

/**
 * MysqlConnection#pingSync() -> Boolean
 *
 * Pings a server connection,
 * or tries to reconnect if the connection has gone down
 **/
NAN_METHOD(MysqlConnection::PingSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_ping(conn->_conn)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/*!
 * EIO wrapper functions for MysqlConnection::Query
 */
void MysqlConnection::EIO_After_Query(uv_work_t *req) {
    NanScope();

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
        argv[0] = V8EXC("Connection is closed by closeSync() during query");
    } else if (!query_req->ok) {
        unsigned int error_string_length = strlen(query_req->my_error) + 20;
        char* error_string = new char[error_string_length];
        snprintf(error_string, error_string_length, "Query error #%d: %s", query_req->my_errno, query_req->my_error);

        argv[0] = V8EXC(error_string);
        delete[] error_string;
    } else {
        argc = 2;
        argv[0] = NanNewLocal(Null());
        if (query_req->have_result_set) {
            Local<Object> local_js_result = MysqlResult::NewInstance(query_req->conn->_conn, query_req->my_result, query_req->field_count);
            argv[1] = local_js_result;
        } else {
            Local<Object> local_js_result = Object::New();
            local_js_result->Set(V8STR("affectedRows"),
                           Integer::New(query_req->affected_rows));
            local_js_result->Set(V8STR("insertId"),
                           Integer::New(query_req->insert_id));
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
    NanScope();

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

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

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
        query_req->nan_callback = new NanCallback(optional_callback.As<Function>());
    } else {
        DEBUG_PRINTF("Query: !optional_callback->IsFunction()");
        query_req->nan_callback = NULL;
    }

    query_req->conn = conn;
    conn->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = query_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Query, (uv_after_work_cb)EIO_After_Query);

    NanReturnUndefined();
}

/*!
 * Callback function for MysqlConnection::QuerySend
 */
void MysqlConnection::EV_After_QuerySend(uv_poll_t* handle, int status, int events) {
    NanScope();

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
    NanScope();

    DEBUG_PRINTF("QuerySend: in");
    DEBUG_ARGS();

    REQ_STR_ARG(0, query);
    OPTIONAL_FUN_ARG(1, optional_callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    query_req->query = new char[query_len + 1];

    // Copy query from V8 var to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    if (optional_callback->IsFunction()) {
        DEBUG_PRINTF("QuerySend: optional_callback->IsFunction()\n\n\n");
        query_req->nan_callback = new NanCallback(optional_callback.As<Function>());
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

    NanReturnUndefined();
}


/**
 * MysqlConnection#querySync(query) -> MysqlResult
 * - query (String): Query
 *
 * Performs a query on the database.
 **/
NAN_METHOD(MysqlConnection::QuerySync) {
    NanScope();

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
        NanReturnValue(False());
    }

    if (!my_result) {
        if (field_count == 0) {
            // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
            NanReturnValue(True());
        } else {
            // Error
            NanReturnValue(False());
        }
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, field_count);

    NanReturnValue(local_js_result);
}

/**
 * MysqlConnection#rollbackSync() -> Boolean
 *
 * Rolls back current transaction
 **/
NAN_METHOD(MysqlConnection::RollbackSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_rollback(conn->_conn)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
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
    NanScope();

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
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#realQuerySync() -> Boolean
 * - query (String): Query
 *
 * Executes an SQL query
 **/
NAN_METHOD(MysqlConnection::RealQuerySync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    unsigned int query_len = static_cast<unsigned int>(query.length());

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, *query, query_len);
    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#selectDbSync(database) -> Boolean
 * - database (String): Database to use
 *
 * Selects the default database for database queries
 **/
NAN_METHOD(MysqlConnection::SelectDbSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, dbname)

    if (mysql_select_db(conn->_conn, *dbname)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#setCharsetSync() -> Boolean
 * - charset (String): Charset
 *
 * Sets the default client character set
 **/
NAN_METHOD(MysqlConnection::SetCharsetSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, charset)

    if (mysql_set_character_set(conn->_conn, *charset)) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/**
 * MysqlConnection#setOptionSync(key, value) -> Boolean
 * - key (Integer): Option key
 * - value (Integer|String): Option value
 *
 * Sets connection options
 **/
NAN_METHOD(MysqlConnection::SetOptionSync) {
    NanScope();

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
            return NanThrowError("This option isn't used because Windows");
            break;
        case MYSQL_OPT_GUESS_CONNECTION:
        case MYSQL_SET_CLIENT_IP:
        case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
        case MYSQL_OPT_USE_REMOTE_CONNECTION:
            return NanThrowError("This option isn't used because not embedded");
            break;
        case MYSQL_OPT_USE_RESULT:
        default:
            return NanThrowError("This option isn't supported by MySQL");
    }

    if (r) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
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
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_INITIALIZED;

    if (args.Length() < 5) {
        return NanThrowError("setSslSync() require 5 string or null arguments");
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

    NanReturnUndefined();
}

/**
 * MysqlConnection#sqlStateSync() -> String
 *
 * Returns the SQLSTATE error from previous MySQL operation
 **/
NAN_METHOD(MysqlConnection::SqlStateSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    NanReturnValue(V8STR(mysql_sqlstate(conn->_conn)));
}

/**
 * MysqlConnection#statSync() -> String
 *
 * Gets the current system status
 **/
NAN_METHOD(MysqlConnection::StatSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *stat = mysql_stat(conn->_conn);

    NanReturnValue(V8STR(stat ? stat : ""));
}

/**
 * MysqlConnection#storeResultSync() -> MysqlResult
 *
 * Transfers a result set from the last query
 **/
NAN_METHOD(MysqlConnection::StoreResultSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        NanReturnValue(True());
    }

    MYSQL_RES *my_result = mysql_store_result(conn->_conn);

    if (!my_result) {
        NanReturnValue(False());
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, mysql_field_count(conn->_conn));
    Persistent<Object> persistent_js_result;
    NanAssignPersistent(Object, persistent_js_result, local_js_result);

    NanReturnValue(persistent_js_result);
}

/**
 * MysqlConnection#threadIdSync() -> Integer
 *
 * Returns the thread ID for the current connection
 **/
NAN_METHOD(MysqlConnection::ThreadIdSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    NanReturnValue(Integer::New(thread_id));
}

/**
 * MysqlConnection#threadSafeSync() -> Boolean
 *
 * Returns whether thread safety is given or not
 **/
NAN_METHOD(MysqlConnection::ThreadSafeSync) {
    NanScope();

    if (mysql_thread_safe()) {
        NanReturnValue(True());
    } else {
        NanReturnValue(False());
    }
}

/**
 * MysqlConnection#useResultSync() -> MysqlResult
 *
 * Initiates a result set retrieval for the last query
 **/
NAN_METHOD(MysqlConnection::UseResultSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        NanReturnValue(True());
    }

    MYSQL_RES *my_result = mysql_use_result(conn->_conn);

    if (!my_result) {
        NanReturnValue(False());
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(conn->_conn, my_result, mysql_field_count(conn->_conn));
    Persistent<Object> persistent_js_result;
    NanAssignPersistent(Object, persistent_js_result, local_js_result);

    NanReturnValue(persistent_js_result);
}

/**
 * MysqlConnection#warningCountSync() -> Integer
 *
 * Returns the number of warnings from the last query for the given link
 **/
NAN_METHOD(MysqlConnection::WarningCountSync) {
    NanScope();

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.Holder());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint32_t warning_count = mysql_warning_count(conn->_conn);

    NanReturnValue(Integer::NewFromUnsigned(warning_count));
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
