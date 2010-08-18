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

    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlConn"));

    ADD_PROTOTYPE_METHOD(connection, async, Async);

    ADD_PROTOTYPE_METHOD(connection, affectedRows, AffectedRows);
    ADD_PROTOTYPE_METHOD(connection, autoCommit, AutoCommit);
    ADD_PROTOTYPE_METHOD(connection, changeUser, ChangeUser);
    ADD_PROTOTYPE_METHOD(connection, commit, Commit);
    ADD_PROTOTYPE_METHOD(connection, connect, Connect);
    ADD_PROTOTYPE_METHOD(connection, connected, Connected);
    ADD_PROTOTYPE_METHOD(connection, connectErrno, ConnectErrno);
    ADD_PROTOTYPE_METHOD(connection, connectError, ConnectError);
    ADD_PROTOTYPE_METHOD(connection, close, Close);
    ADD_PROTOTYPE_METHOD(connection, debug, Debug);
    ADD_PROTOTYPE_METHOD(connection, dumpDebugInfo, DumpDebugInfo);
    ADD_PROTOTYPE_METHOD(connection, errno, Errno);
    ADD_PROTOTYPE_METHOD(connection, error, Error);
    ADD_PROTOTYPE_METHOD(connection, escape, Escape);
    ADD_PROTOTYPE_METHOD(connection, fieldCount, FieldCount);
    ADD_PROTOTYPE_METHOD(connection, getCharset, GetCharset);
    ADD_PROTOTYPE_METHOD(connection, getCharsetName, GetCharsetName);
    ADD_PROTOTYPE_METHOD(connection, getInfo, GetInfo);
    ADD_PROTOTYPE_METHOD(connection, getInfoString, GetInfoString);
    ADD_PROTOTYPE_METHOD(connection, getWarnings, GetWarnings);
    ADD_PROTOTYPE_METHOD(connection, initStatement, InitStatement);
    ADD_PROTOTYPE_METHOD(connection, lastInsertId, LastInsertId);
    ADD_PROTOTYPE_METHOD(connection, multiMoreResults, MultiMoreResults);
    ADD_PROTOTYPE_METHOD(connection, multiNextResult, MultiNextResult);
    ADD_PROTOTYPE_METHOD(connection, multiRealQuery, MultiRealQuery);
    ADD_PROTOTYPE_METHOD(connection, ping, Ping);
    ADD_PROTOTYPE_METHOD(connection, query, Query);
    ADD_PROTOTYPE_METHOD(connection, queryAsync, QueryAsync);
    ADD_PROTOTYPE_METHOD(connection, realQuery, RealQuery);
    ADD_PROTOTYPE_METHOD(connection, rollback, Rollback);
    ADD_PROTOTYPE_METHOD(connection, selectDb, SelectDb);
    ADD_PROTOTYPE_METHOD(connection, setCharset, SetCharset);
    ADD_PROTOTYPE_METHOD(connection, setSsl, SetSsl);
    ADD_PROTOTYPE_METHOD(connection, sqlState, SqlState);
    ADD_PROTOTYPE_METHOD(connection, stat, Stat);
    ADD_PROTOTYPE_METHOD(connection, storeResult, StoreResult);
    ADD_PROTOTYPE_METHOD(connection, threadId, ThreadId);
    ADD_PROTOTYPE_METHOD(connection, threadSafe, ThreadSafe);
    ADD_PROTOTYPE_METHOD(connection, threadKill, ThreadKill);
    ADD_PROTOTYPE_METHOD(connection, useResult, UseResult);
    ADD_PROTOTYPE_METHOD(connection, warningCount, WarningCount);

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

    connect_errno = mysql_errno(_conn);
    connect_error = mysql_error(_conn);

    if (unsuccessful) {
        mysql_close(_conn);
        connected = false;
        _conn = NULL;
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

/* Example of async function based on libeio */
int MysqlConn::EIO_After_Async(eio_req *req) {
    ev_unref(EV_DEFAULT_UC);
    struct async_request *async_req = (struct async_request *)(req->data);

    Local<Value> argv[0];
    int argc = 0;

    TryCatch try_catch;

    async_req->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    async_req->callback.Dispose();
    async_req->conn->Unref();
    free(async_req);

    return 0;
}

int MysqlConn::EIO_Async(eio_req *req) {
    req->result = 0;

    return 0;
}

Handle<Value> MysqlConn::Async(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    REQ_FUN_ARG(0, callback);

    struct async_request *async_req = (struct async_request *)
        calloc(1, sizeof(struct async_request));

    if (!async_req) {
      V8::LowMemoryNotification();
      return THREXC("Could not allocate enough memory");
    }

    async_req->callback = Persistent<Function>::New(callback);
    async_req->conn = conn;

    eio_custom(EIO_Async, EIO_PRI_DEFAULT, EIO_After_Async, async_req);

    ev_ref(EV_DEFAULT_UC);
    conn->Ref();

    return Undefined();
}
/* Example of async function? based on libeio [E] */

Handle<Value> MysqlConn::AffectedRows(const Arguments& args) {
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

Handle<Value> MysqlConn::AutoCommit(const Arguments& args) {
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

Handle<Value> MysqlConn::ChangeUser(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected yet");
    }

    if ( (args.Length() < 3 || !args[0]->IsString()) ||
         (!args[1]->IsString() || !args[2]->IsString()) ) {
        return THREXC("Must give user, password and database as arguments");
    }

    String::Utf8Value user(args[0]->ToString());
    String::Utf8Value password(args[1]->ToString());
    String::Utf8Value dbname(args[2]->ToString());

    bool r = mysql_change_user(conn->_conn, *user, *password, *dbname);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::Commit(const Arguments& args) {
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

Handle<Value> MysqlConn::Connect(const Arguments& args) {
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

Handle<Value> MysqlConn::Connected(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    return scope.Close(conn->connected ? True() : False());
}

Handle<Value> MysqlConn::ConnectErrno(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    Local<Value> js_result = Integer::New(conn->connect_errno);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ConnectError(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    Local<Value> js_result = String::New(conn->connect_error);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::Close(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    conn->Close();

    return Undefined();
}

Handle<Value> MysqlConn::Debug(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.debug() must be a string");
    }

    String::Utf8Value debug(args[0]);

    mysql_debug(*debug);

    return Undefined();
}

Handle<Value> MysqlConn::DumpDebugInfo(const Arguments& args) {
    HandleScope scope;
    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    bool r = !mysql_dump_debug_info(conn->_conn);

    return scope.Close(r ? True() : False());
}

Handle<Value> MysqlConn::Errno(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    uint32_t errno = mysql_errno(conn->_conn);

    Local<Value> js_result = Integer::New(errno);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::Error(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    const char *error = mysql_error(conn->_conn);

    Local<Value> js_result = String::New(error);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::Escape(const Arguments& args) {
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

Handle<Value> MysqlConn::FieldCount(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = Integer::New(mysql_field_count(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetCharset(const Arguments& args) {
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

Handle<Value> MysqlConn::GetCharsetName(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = String::New(
                                    mysql_character_set_name(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetInfo(const Arguments& args) {
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

Handle<Value> MysqlConn::GetInfoString(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    const char *info = mysql_info(conn->_conn);

    Local<Value> js_result = String::New(info ? info : "");

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::GetWarnings(const Arguments& args) {
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

Handle<Value> MysqlConn::InitStatement(const Arguments& args) {
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

Handle<Value> MysqlConn::LastInsertId(const Arguments& args) {
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

Handle<Value> MysqlConn::MultiMoreResults(const Arguments& args) {
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

Handle<Value> MysqlConn::MultiNextResult(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (!mysql_more_results(conn->_conn)) {
        return THREXC("There is no next result set."
                        "Please, call MultiMoreResults() to check "
                        "whether to call this function/method");
    }

    if (!mysql_next_result(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

Handle<Value> MysqlConn::MultiRealQuery(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.multiRealQuery() "
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

Handle<Value> MysqlConn::Ping(const Arguments& args) {
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

Handle<Value> MysqlConn::Query(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of conn.query() must be a string");
    }

    int result_mode = MYSQLSYNC_STORE_RESULT;

    if (args.Length() == 2) {
        result_mode = MYSQLSYNC_USE_RESULT;
    }

    String::Utf8Value query(args[0]->ToString());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    MYSQLSYNC_DISABLE_MQ;

    int r = mysql_query(conn->_conn, *query);
    if (r != 0) {
        return scope.Close(False());
    }

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result;

    switch (result_mode) {
        case MYSQLSYNC_STORE_RESULT:
            my_result = mysql_store_result(conn->_conn);
            break;
        case MYSQLSYNC_USE_RESULT:
            my_result = mysql_use_result(conn->_conn);
            break;
    }

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

int MysqlConn::EIO_After_Query(eio_req *req) {
    ev_unref(EV_DEFAULT_UC);
    struct query_request *query_req = (struct query_request *)(req->data);

    int argc = 1; /* node.js convention, there is always one argument */
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
    int r = mysql_real_query(
                    conn->_conn,
                    query_req->query,
                    query_req->query_length);
    if (r) {
        req->result = 1;
    } else {
        req->int1 = 1;
        req->result = 0;

        query_req->field_count = mysql_field_count(conn->_conn);
        /* If no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN */
        if (!query_req->field_count) {
            req->int1 = 0;
        } else {
            MYSQL_RES *my_result = mysql_store_result(conn->_conn);
            if (my_result) {
                query_req->my_result = my_result;
            } else {
                req->result = 1;
            }
        }
    }
    pthread_mutex_unlock(&conn->query_lock);
    return 0;
}

Handle<Value> MysqlConn::QueryAsync(const Arguments& args) {
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

    query_req->query_length = query.length();
    query_req->query =
        reinterpret_cast<char *>(calloc(query_req->query_length + 1,
        sizeof(char))); //NOLINT

    if (snprintf(query_req->query, query_req->query_length + 1, "%s", *query) !=
                                                      query_req->query_length) {
        return THREXC("Snprintf() error");
    }

    query_req->callback = Persistent<Function>::New(callback);
    query_req->conn = conn;

    eio_custom(EIO_Query, EIO_PRI_DEFAULT, EIO_After_Query, query_req);

    ev_ref(EV_DEFAULT_UC);
    conn->Ref();

    return Undefined();
}

Handle<Value> MysqlConn::Rollback(const Arguments& args) {
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

Handle<Value> MysqlConn::RealQuery(const Arguments& args) {
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

    int r = mysql_real_query(conn->_conn, *query, query.length());

    if (r != 0) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> MysqlConn::SelectDb(const Arguments& args) {
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

Handle<Value> MysqlConn::SetCharset(const Arguments& args) {
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

Handle<Value> MysqlConn::SetSsl(const Arguments& args) {
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

Handle<Value> MysqlConn::SqlState(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    Local<Value> js_result = String::New(mysql_sqlstate(conn->_conn));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::Stat(const Arguments& args) {
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

Handle<Value> MysqlConn::StoreResult(const Arguments& args) {
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

Handle<Value> MysqlConn::ThreadId(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    Local<Value> js_result = Integer::New(thread_id);

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::ThreadKill(const Arguments& args) {
    HandleScope scope;

    MysqlConn *conn = OBJUNWRAP<MysqlConn>(args.This());

    if (!conn->_conn) {
        return THREXC("Not connected");
    }

    if (args.Length() == 0 || !args[0]->IsNumber()) {
        return THREXC("First arg of conn.threadKill() must be a pid");
    }

    if (mysql_kill(conn->_conn, args[0]->IntegerValue())) {
        return scope.Close(False());
    } else {
        return scope.Close(True());
    }
}

Handle<Value> MysqlConn::ThreadSafe(const Arguments& args) {
    HandleScope scope;

    if (mysql_thread_safe()) {
        return scope.Close(True());
    } else {
        return scope.Close(False());
    }
}

Handle<Value> MysqlConn::UseResult(const Arguments& args) {
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

Handle<Value> MysqlConn::WarningCount(const Arguments& args) {
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

