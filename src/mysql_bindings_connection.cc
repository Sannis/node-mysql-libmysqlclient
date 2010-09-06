/*
Copyright by node-mysql-libmysqlclient contributors.
See contributors list in README

See license text in LICENSE file
*/

#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

Persistent<FunctionTemplate> MysqlConn::constructor_template;

void MysqlConn::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    // Constructor
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlConn"));

    Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate(); // NOLINT

    // Constants
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_INIT_COMMAND);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_COMPRESS);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_CONNECT_TIMEOUT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_LOCAL_INFILE);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_PROTOCOL);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_READ_TIMEOUT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_RECONNECT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_WRITE_TIMEOUT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_READ_DEFAULT_FILE);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_READ_DEFAULT_GROUP);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_SET_CHARSET_DIR);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_SET_CHARSET_NAME);

    // Properties
    instance_template->SetAccessor(String::New("connectErrno"),
                                   ConnectErrnoGetter);
    instance_template->SetAccessor(String::New("connectError"),
                                   ConnectErrorGetter);

    // Methods
    ADD_PROTOTYPE_METHOD(connection, affectedRowsSync, AffectedRowsSync);
    ADD_PROTOTYPE_METHOD(connection, autoCommitSync, AutoCommitSync);
    ADD_PROTOTYPE_METHOD(connection, changeUserSync, ChangeUserSync);
    ADD_PROTOTYPE_METHOD(connection, commitSync, CommitSync);
    ADD_PROTOTYPE_METHOD(connection, connect, Connect);
    ADD_PROTOTYPE_METHOD(connection, connectSync, ConnectSync);
    ADD_PROTOTYPE_METHOD(connection, connectedSync, ConnectedSync);
    ADD_PROTOTYPE_METHOD(connection, closeSync, CloseSync);
    ADD_PROTOTYPE_METHOD(connection, debugSync, DebugSync);
    ADD_PROTOTYPE_METHOD(connection, dumpDebugInfoSync, DumpDebugInfoSync);
    ADD_PROTOTYPE_METHOD(connection, errnoSync, ErrnoSync);
    ADD_PROTOTYPE_METHOD(connection, errorSync, ErrorSync);
    ADD_PROTOTYPE_METHOD(connection, escapeSync, EscapeSync);
    ADD_PROTOTYPE_METHOD(connection, fieldCountSync, FieldCountSync);
    ADD_PROTOTYPE_METHOD(connection, getCharsetSync, GetCharsetSync);
    ADD_PROTOTYPE_METHOD(connection, getCharsetNameSync, GetCharsetNameSync);
    ADD_PROTOTYPE_METHOD(connection, getInfoSync, GetInfoSync);
    ADD_PROTOTYPE_METHOD(connection, getInfoStringSync, GetInfoStringSync);
    ADD_PROTOTYPE_METHOD(connection, getWarningsSync, GetWarningsSync);
    ADD_PROTOTYPE_METHOD(connection, initSync, InitSync);
    ADD_PROTOTYPE_METHOD(connection, initStatementSync, InitStatementSync);
    ADD_PROTOTYPE_METHOD(connection, lastInsertIdSync, LastInsertIdSync);
    ADD_PROTOTYPE_METHOD(connection, multiMoreResultsSync, MultiMoreResultsSync); // NOLINT
    ADD_PROTOTYPE_METHOD(connection, multiNextResultSync, MultiNextResultSync);
    ADD_PROTOTYPE_METHOD(connection, multiRealQuerySync, MultiRealQuerySync);
    ADD_PROTOTYPE_METHOD(connection, pingSync, PingSync);
    ADD_PROTOTYPE_METHOD(connection, query, Query);
    ADD_PROTOTYPE_METHOD(connection, querySync, QuerySync);
    ADD_PROTOTYPE_METHOD(connection, realConnectSync, RealConnectSync);
    ADD_PROTOTYPE_METHOD(connection, realQuerySync, RealQuerySync);
    ADD_PROTOTYPE_METHOD(connection, rollbackSync, RollbackSync);
    ADD_PROTOTYPE_METHOD(connection, selectDbSync, SelectDbSync);
    ADD_PROTOTYPE_METHOD(connection, setCharsetSync, SetCharsetSync);
    ADD_PROTOTYPE_METHOD(connection, setOptionSync, SetOptionSync);
    ADD_PROTOTYPE_METHOD(connection, setSslSync, SetSslSync);
    ADD_PROTOTYPE_METHOD(connection, sqlStateSync, SqlStateSync);
    ADD_PROTOTYPE_METHOD(connection, statSync, StatSync);
    ADD_PROTOTYPE_METHOD(connection, storeResultSync, StoreResultSync);
    ADD_PROTOTYPE_METHOD(connection, threadIdSync, ThreadIdSync);
    ADD_PROTOTYPE_METHOD(connection, threadSafeSync, ThreadSafeSync);
    ADD_PROTOTYPE_METHOD(connection, useResultSync, UseResultSync);
    ADD_PROTOTYPE_METHOD(connection, warningCountSync, WarningCountSync);

    target->Set(String::NewSymbol("MysqlConn"),
                constructor_template->GetFunction());

    MysqlResult::Init(target);
    MysqlStatement::Init(target);
}

bool MysqlConn::Connect(const char* hostname,
                        const char* user,
                        const char* password,
                        const char* dbname,
                        uint32_t port,
                        const char* socket) {
    if (_conn) {
        return false;
    }

    _conn = mysql_init(NULL);

    if (!_conn) {
        connected = false;
        return false;
    }

    bool unsuccessful = !mysql_real_connect(_conn,
                            hostname,
                            user,
                            password,
                            dbname,
                            port,
                            socket,
                            0);

    if (unsuccessful) {
        connect_errno = mysql_errno(_conn);
        connect_error = mysql_error(_conn);

        mysql_close(_conn);
        connected = false;
        _conn = NULL;
        return false;
    }

    connected = true;
    return true;
}

bool MysqlConn::RealConnect(const char* hostname,
                            const char* user,
                            const char* password,
                            const char* dbname,
                            uint32_t port,
                            const char* socket) {
    if (!_conn) {
        return false;
    }

    if (connected) {
        return false;
    }

    bool unsuccessful = !mysql_real_connect(_conn,
                                            hostname,
                                            user,
                                            password,
                                            dbname,
                                            port,
                                            socket,
                                            0);

    if (unsuccessful) {
        connect_errno = mysql_errno(_conn);
        connect_error = mysql_error(_conn);

        mysql_close(_conn);
        connected = false;
        return false;
    }

    connected = true;
    return true;
}

void MysqlConn::Close() {
    if (_conn) {
        mysql_close(_conn);
        connected = false;
        _conn = NULL;
    }
}

MysqlConn::MysqlConnInfo MysqlConn::GetInfo() {
    MysqlConnInfo info;

    info.client_version = mysql_get_client_version();
    info.client_info = mysql_get_client_info();
    info.server_version = mysql_get_server_version(_conn);
    info.server_info = mysql_get_server_info(_conn);
    info.host_info = mysql_get_host_info(_conn);
    info.proto_info = mysql_get_proto_info(_conn);

    return info;
}

MysqlConn::MysqlConn(): EventEmitter() {
    _conn = NULL;
    multi_query = false;
    pthread_mutex_init(&query_lock, NULL);
}

MysqlConn::~MysqlConn() {
    this->Close();
    pthread_mutex_destroy(&query_lock);
}

Handle<Value> MysqlConn::New(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = new MysqlConn();
    conn->Wrap(args.This());

    return args.This();
}

Handle<Value> MysqlConn::ConnectErrnoGetter(Local<String> property,
                                            const AccessorInfo &info) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(info.Holder());

    Local<Value> js_result = Integer::New(conn->connect_errno);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ConnectErrorGetter(Local<String> property,
                                            const AccessorInfo &info) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(info.Holder());

    Local<Value> js_result = String::New(conn->connect_error);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::AffectedRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

    if (affected_rows == ((my_ulonglong)-1)) {
        return THREXC("Error occured in mysql_affected_rows(), -1 returned");
    }

    Local<Value> js_result = Integer::New(affected_rows);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::AutoCommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if ( args.Length() < 1 ) {
        return THREXC("Must give boolean automode flag as argument");
    }

    if (mysql_autocommit(conn->_conn, args[0]->BooleanValue())) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::ChangeUserSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected yet");
    }

    if ( (args.Length() < 2) || (!args[0]->IsString()) ||
         (!args[1]->IsString()) ) {
        return THREXC("Must give at least user and password as arguments");
    }
    
    if ( (args.Length() == 3) && (!args[2]->IsString()) ) {
        return THREXC("Must give string value as third argument, database");
    }

    String::Utf8Value user(args[0]->ToString());
    String::Utf8Value password(args[1]->ToString());
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

Handle<Value> MysqlConn::CommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (mysql_commit(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

int MysqlConn::EIO_After_Connect(eio_req *req) {
    ev_unref(EV_DEFAULT_UC);
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    Local<Value> argv[1];
    int argc = 1;

    if (req->result) {
      argv[0] = Local<Value>::New(Integer::New(conn_req->conn->connect_errno));
    } else {
      argv[0] = Local<Value>::New(Null());
    }

    TryCatch try_catch;

    conn_req->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    conn_req->callback.Dispose();
    conn_req->conn->Unref();
    free(conn_req);

    return 0;
}

int MysqlConn::EIO_Connect(eio_req *req) {
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    req->result = conn_req->conn->Connect(
                    conn_req->hostname ? **(conn_req->hostname) : NULL,
                    conn_req->user ? **(conn_req->user) : NULL,
                    conn_req->password ? **(conn_req->password) : NULL,
                    conn_req->dbname ? **(conn_req->dbname) : NULL,
                    conn_req->port,
                    conn_req->socket ? **(conn_req->socket) : NULL) ? 0 : 1;

    delete conn_req->hostname;
    delete conn_req->user;
    delete conn_req->password;
    delete conn_req->socket;

    return 0;
}

Handle<Value> MysqlConn::Connect(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    struct connect_request *conn_req =
         (struct connect_request *)calloc(1, sizeof(struct connect_request));

    if (!conn_req) {
      V8::LowMemoryNotification();
      return THREXC("Could not allocate enough memory");
    }

    REQ_FUN_ARG(args.Length() - 1, callback);
    conn_req->callback = Persistent<Function>::New(callback);
    conn_req->conn = conn;

    conn_req->hostname = args.Length() > 1 && args[0]->IsString() ?
        new String::Utf8Value(args[0]->ToString()) : NULL;
    conn_req->user = args.Length() > 2 && args[1]->IsString() ?
        new String::Utf8Value(args[1]->ToString()) : NULL;
    conn_req->password = args.Length() > 3 && args[2]->IsString() ?
        new String::Utf8Value(args[2]->ToString()) : NULL;
    conn_req->dbname = args.Length() > 4 && args[3]->IsString() ?
        new String::Utf8Value(args[3]->ToString()) : NULL;
    conn_req->port = args.Length() > 5 ? args[4]->IntegerValue() : 0;
    conn_req->socket = args.Length() > 6 && args[5]->IsString() ?
      new String::Utf8Value(args[5]->ToString()) : NULL;

    eio_custom(EIO_Connect, EIO_PRI_DEFAULT, EIO_After_Connect, conn_req);

    ev_ref(EV_DEFAULT_UC);
    conn->Ref();

    return Undefined();
}

Handle<Value> MysqlConn::ConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->IntegerValue();
    String::Utf8Value socket(args[5]->ToString());

    bool r = conn->Connect(
                    (
                        args[0]->IsString() ?
                        *hostname : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() ?
                        *user : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() ?
                        *password : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() ?
                        *dbname : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() &&
                        args[4]->IsString() ?
                        port : 0),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() &&
                        args[4]->IsString() &&
                        args[5]->IsString() ?
                        *socket : NULL));

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::ConnectedSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    return scope.Close(conn->connected ? True() : False());
}

Handle<Value> MysqlConn::CloseSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    conn->Close();

    return Undefined();
}

Handle<Value> MysqlConn::DebugSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.debugSync() must be a string");
    }

    String::Utf8Value debug(args[0]);

    mysql_debug(*debug);

    return Undefined();
}

Handle<Value> MysqlConn::DumpDebugInfoSync(const Arguments& args) {
    HandleScope scope;
    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    bool r = !mysql_dump_debug_info(conn->_conn);

    return scope.Close(r ? True() : False());
}

Handle<Value> MysqlConn::ErrnoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    uint32_t errno = mysql_errno(conn->_conn);

    Local<Value> js_result = Integer::New(errno);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ErrorSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    const char *error = mysql_error(conn->_conn);

    Local<Value> js_result = String::New(error);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::EscapeSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("Nothing to escape");
    }

    String::Utf8Value str(args[0]);

    int len = args[0]->ToString()->Utf8Length();
    char *result = new char[2*len + 1];
    if (!result) {
        return THREXC("Not enough memory");
    }

    int length = mysql_real_escape_string(conn->_conn, result, *str, len);
    Local<Value> js_result = String::New(result, length);

    delete[] result;

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::FieldCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = Integer::New(mysql_field_count(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MY_CHARSET_INFO cs;

    mysql_get_character_set_info(conn->_conn, &cs);

    Local<Object> js_result = Object::New();

    js_result->Set(String::New("charset"),
                   String::New(cs.csname ? cs.csname : ""));

    js_result->Set(String::New("collation"),
                   String::New(cs.name ? cs.name : ""));

    js_result->Set(String::New("dir"),
                   String::New(cs.dir ? cs.dir : ""));

    js_result->Set(String::New("min_length"),
                   Integer::New(cs.mbminlen));

    js_result->Set(String::New("max_length"),
                   Integer::New(cs.mbmaxlen));

    js_result->Set(String::New("number"),
                   Integer::New(cs.number));

    js_result->Set(String::New("state"),
                   Integer::New(cs.state));

    js_result->Set(String::New("comment"),
                   String::New(cs.comment ? cs.comment : ""));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetCharsetNameSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = String::New(
                                    mysql_character_set_name(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetInfoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MysqlConnInfo info = conn->GetInfo();

    Local<Object> js_result = Object::New();

    js_result->Set(String::New("client_version"),
                   Integer::New(info.client_version));

    js_result->Set(String::New("client_info"),
                   String::New(info.client_info));

    js_result->Set(String::New("server_version"),
                   Integer::New(info.server_version));

    js_result->Set(String::New("server_info"),
                   String::New(info.server_info));

    js_result->Set(String::New("host_info"),
                   String::New(info.host_info));

    js_result->Set(String::New("proto_info"),
                   Integer::New(info.proto_info));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetInfoStringSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    const char *info = mysql_info(conn->_conn);

    Local<Value> js_result = String::New(info ? info : "");

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetWarningsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQL_RES *result;
    MYSQL_ROW row;
    int i = 0;

    Local<Object> js_warning = Object::New();
    Local<Array> js_result = Array::New();

    if (mysql_warning_count(conn->_conn)) {
        if (mysql_real_query(conn->_conn, "SHOW WARNINGS", 13) == 0) {
            result = mysql_store_result(conn->_conn);

            while ((row = mysql_fetch_row(result))) {
                js_warning->Set(String::New("errno"),
                                String::New(row[1])->ToInteger());

                js_warning->Set(String::New("reason"),
                                String::New(row[2]));

                js_result->Set(Integer::New(i), js_warning);

                i++;
            }

            mysql_free_result(result);
        }
    }

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::InitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (conn->_conn) {
        return THREXC("Already initialized");
    }

    conn->_conn = mysql_init(NULL);

    if (!conn->_conn) {
        return scope.Close(False());
    }
    
    return scope.Close(True());
}

Handle<Value> MysqlConn::InitStatementSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

    if (!my_statement) {
        return scope.Close(False());
    }

    Local<Value> arg = External::New(my_statement);
    Persistent<Object> js_result(MysqlStatement::constructor_template->
                             GetFunction()->NewInstance(1, &arg));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::LastInsertIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQL_RES *result;
    my_ulonglong insert_id = 0;

    if ( (result = mysql_store_result(conn->_conn)) == 0 &&
         mysql_field_count(conn->_conn) == 0 &&
         mysql_insert_id(conn->_conn) != 0) {
        insert_id = mysql_insert_id(conn->_conn);
    }

    Local<Value> js_result = Integer::New(insert_id);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::MultiMoreResultsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (mysql_more_results(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

Handle<Value> MysqlConn::MultiNextResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

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

Handle<Value> MysqlConn::MultiRealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.multiRealQuerySync() "
                        "must be a string");
    }

    String::Utf8Value query(args[0]->ToString());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQLSYNC_ENABLE_MQ;

    int r = mysql_real_query(conn->_conn, *query, query.length());

    if (r != 0) {
        MYSQLSYNC_DISABLE_MQ;

        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::PingSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected yet");
    }

    bool r = mysql_ping(conn->_conn);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

int MysqlConn::EIO_After_Query(eio_req *req) {
    ev_unref(EV_DEFAULT_UC);
    struct query_request *query_req = (struct query_request *)(req->data);

    int argc = 1;
    Local<Value> argv[2];
    HandleScope scope;

    if (req->result) {
        argv[0] = Exception::Error(String::New("Error on query execution"));
    } else {
        if (req->int1) {
            argv[0] = External::New(query_req->my_result);
            argv[1] = Integer::New(query_req->field_count);
            Persistent<Object> js_result(MysqlResult::constructor_template->
                                     GetFunction()->NewInstance(2, argv));

            argv[1] = Local<Value>::New(scope.Close(js_result));
            argc = 2;
        }
        argv[0] = Local<Value>::New(Null());
    }

    TryCatch try_catch;

    query_req->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    query_req->callback.Dispose();
    query_req->conn->Unref();
    free(query_req->query);
    free(query_req);

    return 0;
}

int MysqlConn::EIO_Query(eio_req *req) {
    struct query_request *query_req = (struct query_request *)(req->data);

    MysqlConn *conn = query_req->conn;

    if (!conn->_conn) {
        req->result = 1;
        return 0;
    }

    MYSQLSYNC_DISABLE_MQ;

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_query(conn->_conn, query_req->query);
    if (r != 0) {
        // Query error
        req->result = 1;
    } else {
        req->result = 0;

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        query_req->field_count = mysql_field_count(conn->_conn);

        if (!my_result) {
            if (query_req->field_count == 0) {
                // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
                req->int1 = 0;
            } else {
                // Result store error
                req->result = 1;
            }
        } else {
            req->int1 = 1;
            query_req->my_result = my_result;
        }
    }
    pthread_mutex_unlock(&conn->query_lock);
    return 0;
}

Handle<Value> MysqlConn::Query(const Arguments& args) {
    HandleScope scope;

    REQ_STR_ARG(0, query);
    REQ_FUN_ARG(1, callback);

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    struct query_request *query_req = (struct query_request *)
        calloc(1, sizeof(struct query_request));

    if (!query_req) {
        V8::LowMemoryNotification();
        return THREXC("Could not allocate enough memory");
    }

    query_req->query =
        reinterpret_cast<char *>(calloc(query.length() + 1,
        sizeof(char))); //NOLINT

    if (snprintf(query_req->query, query.length() + 1, "%s", *query) !=
                                                      query.length()) {
        return THREXC("Snprintf() error");
    }

    query_req->callback = Persistent<Function>::New(callback);
    query_req->conn = conn;

    eio_custom(EIO_Query, EIO_PRI_DEFAULT, EIO_After_Query, query_req);

    ev_ref(EV_DEFAULT_UC);
    conn->Ref();

    return Undefined();
}

Handle<Value> MysqlConn::QuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.query() must be a string");
    }

    String::Utf8Value query(args[0]->ToString());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQLSYNC_DISABLE_MQ;
    MYSQL_RES *my_result = NULL;
    int field_count;


    //only one query can be executed on a connection at a time
    pthread_mutex_lock(&conn->query_lock);

    int r = mysql_query(conn->_conn, *query);
    if (r == 0) {
        my_result = mysql_store_result(conn->_conn);
        field_count = mysql_field_count(conn->_conn);
    }

    pthread_mutex_unlock(&conn->query_lock);

    if(r != 0) {
        // Query error
        // TODO(Sannis): change this to THREXC()?
        return scope.Close(False());
    }
    if (!my_result) {
        if (field_count == 0) {
            // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
            return scope.Close(True());
        } else {
            // Error
            // TODO(Sannis): change this to THREXC()?
            return scope.Close(False());
        }
    }

    int argc = 2;
    Local<Value> argv[2];
    argv[0] = External::New(my_result);
    argv[1] = Integer::New(field_count);
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::RollbackSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (mysql_rollback(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::RealConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not initialized, execute conn.initSync() before conn.realConnectSync()"); // NOLINT
    }

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->IntegerValue();
    String::Utf8Value socket(args[5]->ToString());

    bool r = conn->RealConnect(
                    (
                        args[0]->IsString() ?
                        *hostname : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() ?
                        *user : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() ?
                        *password : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() ?
                        *dbname : NULL),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() &&
                        args[4]->IsString() ?
                        port : 0),
                    (
                        args[0]->IsString() &&
                        args[1]->IsString() &&
                        args[2]->IsString() &&
                        args[3]->IsString() &&
                        args[4]->IsString() &&
                        args[5]->IsString() ?
                        *socket : NULL));

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::RealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.realQuery() must be a string");
    }

    String::Utf8Value query(args[0]->ToString());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQLSYNC_DISABLE_MQ;


    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, *query, query.length());
    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::SelectDbSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("Must give database name as argument");
    }

    String::Utf8Value dbname(args[0]);

    bool r = mysql_select_db(conn->_conn, *dbname);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::SetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("Must give charset name as argument");
    }

    String::Utf8Value charset(args[0]);

    bool r = mysql_set_character_set(conn->_conn, *charset);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::SetOptionSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    REQ_INT_ARG(0, option_integer_key);
    mysql_option option_key = static_cast<mysql_option>(option_integer_key);
    int r = 1;

    switch (option_key) {
        case MYSQL_OPT_CONNECT_TIMEOUT:
        case MYSQL_OPT_LOCAL_INFILE:
        case MYSQL_OPT_PROTOCOL:
        case MYSQL_OPT_READ_TIMEOUT:
        case MYSQL_OPT_WRITE_TIMEOUT:
        case MYSQL_OPT_RECONNECT:
        case MYSQL_OPT_COMPRESS:
            {
            REQ_INT_ARG(1, option_integer_value);
            r = mysql_options(conn->_conn, option_key, &option_integer_value);
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
        default:
            return THREXC("This option isn't supported");
    }

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::SetSslSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() < 5 ||
        !args[0]->IsString() ||
        !args[1]->IsString() ||
        !args[2]->IsString() ||
        !args[3]->IsString() ||
        !args[4]->IsString()) {
        return THREXC("Must give 5 arguments");
    }

    String::Utf8Value key(args[0]->ToString());
    String::Utf8Value cert(args[1]->ToString());
    String::Utf8Value ca(args[2]->ToString());
    String::Utf8Value capath(args[3]->ToString());
    String::Utf8Value cipher(args[4]->ToString());

    mysql_ssl_set(conn->_conn, *key, *cert, *ca, *capath, *cipher);

    return scope.Close(Undefined());
}

Handle<Value> MysqlConn::SqlStateSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = String::New(mysql_sqlstate(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::StatSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    const char *stat = mysql_stat(conn->_conn);

    if (stat) {
        Local<Value> js_result = String::New(stat);

        return scope.Close(js_result);
    } else {
        return scope.Close(False());
    }
}

Handle<Value> MysqlConn::StoreResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_store_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    int argc = 2;
    Local<Value> argv[2];
    argv[0] = External::New(my_result);
    argv[1] = Integer::New(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ThreadIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    Local<Value> js_result = Integer::New(thread_id);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ThreadSafeSync(const Arguments& args) {
    HandleScope scope;

    if (mysql_thread_safe()) {
        return scope.Close(True());
    } else {
        return scope.Close(False());
    }
}

Handle<Value> MysqlConn::UseResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_use_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    int argc = 2;
    Local<Value> argv[2];
    argv[0] = External::New(my_result);
    argv[1] = Integer::New(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::WarningCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    uint32_t warning_count = mysql_warning_count(conn->_conn);

    Local<Value> js_result = Integer::New(warning_count);

    return scope.Close(js_result);
}

extern "C" void init(Handle<Object> target) {
    MysqlConn::Init(target);
}

