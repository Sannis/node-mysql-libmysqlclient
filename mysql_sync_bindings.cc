/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#include "./mysql_sync_bindings.h"

// Only for fixing some cpplint.py errors:
// Lines should be <= 80 characters long
// [whitespace/line_length] [2]
// Lines should very rarely be longer than 100 characters
// [whitespace/line_length] [4]
#define THREXC(str) ThrowException(String::New(str))

#define OBJUNWRAP ObjectWrap::Unwrap

#define REQ_EXT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsExternal()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " invalid"))); \
Local<External> VAR = Local<External>::Cast(args[I]);

#define MYSQLSYNC_STORE_RESULT 0
#define MYSQLSYNC_USE_RESULT   1

#define MYSQLSYNC_DISABLE_MQ if (conn->multi_query) { \
    mysql_set_server_option(conn->_conn, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
    conn->multi_query = false; \
}

#define MYSQLSYNC_ENABLE_MQ if (!conn->multi_query) { \
    mysql_set_server_option(conn->_conn, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
    conn->multi_query = true; \
}

    void MysqlSyncConn::Init(Handle<Object> target) {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        // affectedRows_symbol = NODE_PSYMBOL("affectedRows");
        // changeUser_symbol = NODE_PSYMBOL("changeUser");
        // connect_symbol = NODE_PSYMBOL("connect");
        // connectErrno_symbol = NODE_PSYMBOL("connectErrno");
        // connectError_symbol = NODE_PSYMBOL("connectError");
        // close_symbol = NODE_PSYMBOL("close");
        // debug_symbol = NODE_PSYMBOL("debug");
        // dumpDebugInfo_symbol = NODE_PSYMBOL("dumpDebugInfo");
        // errno_symbol = NODE_PSYMBOL("errno");
        // error_symbol = NODE_PSYMBOL("error");
        // escape_symbol = NODE_PSYMBOL("escape");
        // fieldCount_symbol = NODE_PSYMBOL("fieldCount");
        // getCharset_symbol = NODE_PSYMBOL("getCharset");
        // getCharsetName_symbol = NODE_PSYMBOL("getCharsetName");
        // getInfo_symbol = NODE_PSYMBOL("getInfo");
        // getInfoString_symbol = NODE_PSYMBOL("getInfoString");
        // getWarnings_symbol = NODE_PSYMBOL("getWarnings");
        // initStatement_symbol = NODE_PSYMBOL("initStatement");
        // lastInsertId_symbol = NODE_PSYMBOL("lastInsertId");
        // multiMoreResults_symbol = NODE_PSYMBOL("multiMoreResults");
        // multiNextResult_symbol = NODE_PSYMBOL("multiNextResult");
        // multiRealQuery_symbol = NODE_PSYMBOL("multiRealQuery");
        // ping_symbol = NODE_PSYMBOL("ping");
        // query_symbol = NODE_PSYMBOL("query");
        // realQuery_symbol = NODE_PSYMBOL("realQuery");
        // selectDb_symbol = NODE_PSYMBOL("selectDb");
        // setCharset_symbol = NODE_PSYMBOL("setCharset");
        // setSsl_symbol = NODE_PSYMBOL("setSsl");
        // sqlState_symbol = NODE_PSYMBOL("sqlState");
        // stat_symbol = NODE_PSYMBOL("stat");
        // storeResult_symbol = NODE_PSYMBOL("storeResult");
        // useResult_symbol = NODE_PSYMBOL("useResult");
        // warningCount_symbol = NODE_PSYMBOL("warningCount");

        NODE_SET_PROTOTYPE_METHOD(t, "affectedRows", AffectedRows);
        NODE_SET_PROTOTYPE_METHOD(t, "changeUser", ChangeUser);
        NODE_SET_PROTOTYPE_METHOD(t, "connect", Connect);
        NODE_SET_PROTOTYPE_METHOD(t, "connectErrno", ConnectErrno);
        NODE_SET_PROTOTYPE_METHOD(t, "connectError", ConnectError);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "debug", Debug);
        NODE_SET_PROTOTYPE_METHOD(t, "dumpDebugInfo", DumpDebugInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "errno", Errno);
        NODE_SET_PROTOTYPE_METHOD(t, "error", Error);
        NODE_SET_PROTOTYPE_METHOD(t, "escape", Escape);
        NODE_SET_PROTOTYPE_METHOD(t, "fieldCount", FieldCount);
        NODE_SET_PROTOTYPE_METHOD(t, "getCharset", GetCharset);
        NODE_SET_PROTOTYPE_METHOD(t, "getCharsetName", GetCharsetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo", GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfoString", GetInfoString);
        NODE_SET_PROTOTYPE_METHOD(t, "getWarnings", GetWarnings);
        NODE_SET_PROTOTYPE_METHOD(t, "initStatement", InitStatement);
        NODE_SET_PROTOTYPE_METHOD(t, "lastInsertId", LastInsertId);
        NODE_SET_PROTOTYPE_METHOD(t, "multiMoreResults", MultiMoreResults);
        NODE_SET_PROTOTYPE_METHOD(t, "multiNextResult", MultiNextResult);
        NODE_SET_PROTOTYPE_METHOD(t, "multiRealQuery", MultiRealQuery);
        NODE_SET_PROTOTYPE_METHOD(t, "ping", Ping);
        NODE_SET_PROTOTYPE_METHOD(t, "query", Query);
        NODE_SET_PROTOTYPE_METHOD(t, "realQuery", RealQuery);
        NODE_SET_PROTOTYPE_METHOD(t, "selectDb", SelectDb);
        NODE_SET_PROTOTYPE_METHOD(t, "setCharset", SetCharset);
        NODE_SET_PROTOTYPE_METHOD(t, "setSsl", SetSsl);
        NODE_SET_PROTOTYPE_METHOD(t, "sqlState", SqlState);
        NODE_SET_PROTOTYPE_METHOD(t, "stat", Stat);
        NODE_SET_PROTOTYPE_METHOD(t, "storeResult", StoreResult);
        NODE_SET_PROTOTYPE_METHOD(t, "useResult", UseResult);
        NODE_SET_PROTOTYPE_METHOD(t, "warningCount", WarningCount);

        target->Set(String::NewSymbol("MysqlSyncConn"), t->GetFunction());

        MysqlSyncRes::Init(target);
        MysqlSyncStmt::Init(target);
    }

    bool MysqlSyncConn::Connect(const char* hostname,
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
            return false;
        }

        return true;
    }

    void MysqlSyncConn::Close() {
        if (_conn) {
            mysql_close(_conn);
            _conn = NULL;
        }
    }

    MysqlSyncConn::MysqlSyncConnInfo MysqlSyncConn::GetInfo() {
        MysqlSyncConnInfo info;

        info.client_version = mysql_get_client_version();
        info.client_info = mysql_get_client_info();
        info.server_version = mysql_get_server_version(_conn);
        info.server_info = mysql_get_server_info(_conn);
        info.host_info = mysql_get_host_info(_conn);
        info.proto_info = mysql_get_proto_info(_conn);

        return info;
    }

    MysqlSyncConn::MysqlSyncConn(): EventEmitter() {
        _conn = NULL;
        multi_query = false;
    }

    MysqlSyncConn::~MysqlSyncConn() {
        if (_conn) {
            mysql_close(_conn);
        }
    }

    Handle<Value> MysqlSyncConn::New(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = new MysqlSyncConn();
        conn->Wrap(args.This());

        return args.This();
    }

    Handle<Value> MysqlSyncConn::AffectedRows(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

        if (affected_rows == -1) {
            return THREXC("Error occured in mysql_affected_rows()");
        }

        Local<Value> js_result = Integer::New(affected_rows);

        return scope.Close(js_result);
    }

    // TODO(Sannis): Write test for this method
    Handle<Value> MysqlSyncConn::ChangeUser(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::Connect(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if ( (args.Length() < 3 || !args[0]->IsString()) ||
             (!args[1]->IsString() || !args[2]->IsString()) ) {
            return THREXC("Must give hostname, user and password as arguments");
        }

        String::Utf8Value hostname(args[0]->ToString());
        String::Utf8Value user(args[1]->ToString());
        String::Utf8Value password(args[2]->ToString());
        String::Utf8Value dbname(args[3]->ToString());
        uint32_t port = args[4]->IntegerValue();
        String::Utf8Value socket(args[5]->ToString());

        bool r = conn->Connect(
                        *hostname,
                        (
                            args[1]->IsString() ?
                            *user : NULL),
                        (
                            args[1]->IsString() &&
                            args[2]->IsString() ?
                            *password : NULL),
                        (
                            args[1]->IsString() &&
                            args[2]->IsString() &&
                            args[3]->IsString() ?
                            *dbname : NULL),
                        (
                            args[1]->IsString() &&
                            args[2]->IsString() &&
                            args[3]->IsString() &&
                            args[4]->IsString() ?
                            port : 0),
                        (
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

    Handle<Value> MysqlSyncConn::ConnectErrno(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        Local<Value> js_result = Integer::New(conn->connect_errno);

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::ConnectError(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        Local<Value> js_result = String::New(conn->connect_error);

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::Close(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        conn->Close();

        return Undefined();
    }

    Handle<Value> MysqlSyncConn::Debug(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::DumpDebugInfo(const Arguments& args) {
        HandleScope scope;
        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        bool r = !mysql_dump_debug_info(conn->_conn);

        return scope.Close(r ? True() : False());
    }

    Handle<Value> MysqlSyncConn::Errno(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        uint32_t errno = mysql_errno(conn->_conn);

        Local<Value> js_result = Integer::New(errno);

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::Error(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        const char *error = mysql_error(conn->_conn);

        Local<Value> js_result = String::New(error);

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::Escape(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::FieldCount(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        Local<Value> js_result = Integer::New(mysql_field_count(conn->_conn));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::GetCharset(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::GetCharsetName(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        Local<Value> js_result = String::New(
                                        mysql_character_set_name(conn->_conn));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::GetInfo(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        MysqlSyncConnInfo info = conn->GetInfo();

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

    Handle<Value> MysqlSyncConn::GetInfoString(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        const char *info = mysql_info(conn->_conn);

        Local<Value> js_result = String::New(info ? info : "");

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::GetWarnings(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::InitStatement(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

        if (!my_statement) {
            return scope.Close(False());
        }

        Local<Value> arg = External::New(my_statement);
        Persistent<Object> js_result(MysqlSyncStmt::constructor_template->
                                 GetFunction()->NewInstance(1, &arg));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::LastInsertId(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    // TODO(Sannis): Write test for this method
    Handle<Value> MysqlSyncConn::MultiMoreResults(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        if (mysql_more_results(conn->_conn)) {
            return scope.Close(True());
        }

        return scope.Close(False());
    }

    // TODO(Sannis): Write test for this method
    Handle<Value> MysqlSyncConn::MultiNextResult(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    // TODO(Sannis): Write test for this method
    Handle<Value> MysqlSyncConn::MultiRealQuery(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    // TODO(Sannis): Write test for this method
    Handle<Value> MysqlSyncConn::Ping(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected yet");
        }

        bool r = mysql_ping(conn->_conn);

        if (r) {
            return scope.Close(False());
        }

        return scope.Close(True());
    }

    Handle<Value> MysqlSyncConn::Query(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (args.Length() == 0 || !args[0]->IsString()) {
            return THREXC("First arg of conn.query() must be a string");
        }

        int result_mode = MYSQLSYNC_STORE_RESULT;

        if (args.Length() == 1) {
            result_mode = MYSQLSYNC_USE_RESULT;
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

        Local<Value> arg = External::New(my_result);
        Persistent<Object> js_result(MysqlSyncRes::constructor_template->
                                 GetFunction()->NewInstance(1, &arg));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::RealQuery(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::SelectDb(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::SetCharset(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    // TODO(Sannis): How to write a test for this function?
    Handle<Value> MysqlSyncConn::SetSsl(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::SqlState(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        Local<Value> js_result = String::New(mysql_sqlstate(conn->_conn));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::Stat(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

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

    Handle<Value> MysqlSyncConn::StoreResult(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!mysql_field_count(conn->_conn)) {
            /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
            return scope.Close(True());
        }

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        if (!my_result) {
            return scope.Close(False());
        }

        Local<Value> arg = External::New(my_result);
        Persistent<Object> js_result(MysqlSyncRes::constructor_template->
                                 GetFunction()->NewInstance(1, &arg));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::UseResult(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!mysql_field_count(conn->_conn)) {
            /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
            return scope.Close(True());
        }

        MYSQL_RES *my_result = mysql_use_result(conn->_conn);

        if (!my_result) {
            return scope.Close(False());
        }

        Local<Value> arg = External::New(my_result);
        Persistent<Object> js_result(MysqlSyncRes::constructor_template->
                                 GetFunction()->NewInstance(1, &arg));

        return scope.Close(js_result);
    }

    Handle<Value> MysqlSyncConn::WarningCount(const Arguments& args) {
        HandleScope scope;

        MysqlSyncConn *conn = OBJUNWRAP<MysqlSyncConn>(args.This());

        if (!conn->_conn) {
            return THREXC("Not connected");
        }

        uint32_t warning_count = mysql_warning_count(conn->_conn);

        Local<Value> js_result = Integer::New(warning_count);

        return scope.Close(js_result);
    }

        void MysqlSyncConn::MysqlSyncRes::Init(Handle<Object> target) {
            HandleScope scope;

            Local<FunctionTemplate> t = FunctionTemplate::New(New);
            constructor_template = Persistent<FunctionTemplate>::New(t);

            t->Inherit(EventEmitter::constructor_template);
            t->InstanceTemplate()->SetInternalFieldCount(1);

            // fetchResult_symbol = NODE_PSYMBOL("fetchResult");

            NODE_SET_PROTOTYPE_METHOD(t, "fetchResult", FetchResult);
        }

        MysqlSyncConn::MysqlSyncRes::MysqlSyncRes(): EventEmitter() {}

        MysqlSyncConn::MysqlSyncRes::~MysqlSyncRes() {}

        Handle<Value> MysqlSyncConn::MysqlSyncRes::New(const Arguments& args) {
            HandleScope scope;

            REQ_EXT_ARG(0, js_res);
            MYSQL_RES *res = static_cast<MYSQL_RES*>(js_res->Value());
            MysqlSyncRes *my_res = new MysqlSyncRes(res);
            my_res->Wrap(args.This());

            return args.This();
        }

        Handle<Value> MysqlSyncConn::MysqlSyncRes::FetchResult(const Arguments& args) {
            HandleScope scope;

            MysqlSyncRes *res = OBJUNWRAP<MysqlSyncRes>(args.This());

            if (!res->_res) {
                return scope.Close(False());
            }

            MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
            uint32_t num_fields = mysql_num_fields(res->_res);
            MYSQL_ROW result_row;
            // Only use this with
            // mysql_store_result() instead of mysql_use_result()
            // my_ulonglong num_rows = mysql_num_rows(result);
            int i = 0, j = 0;

            Local<Array> js_result = Array::New();
            Local<Object> js_result_row;
            Local<Value> js_field;

            i = 0;
            while ( result_row = mysql_fetch_row(res->_res) ) {
                js_result_row = Object::New();

                for ( j = 0; j < num_fields; j++ ) {
                    switch (fields[j].type) {
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
                      // TODO(Sannis): Handle other types, dates in first order
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

        void MysqlSyncConn::MysqlSyncStmt::Init(Handle<Object> target) {
            HandleScope scope;

            Local<FunctionTemplate> t = FunctionTemplate::New(New);
            constructor_template = Persistent<FunctionTemplate>::New(t);

            t->Inherit(EventEmitter::constructor_template);
            t->InstanceTemplate()->SetInternalFieldCount(1);

            // prepare_symbol = NODE_PSYMBOL("prepare");

            NODE_SET_PROTOTYPE_METHOD(t, "prepare", Prepare);
        }

        MysqlSyncConn::MysqlSyncStmt::MysqlSyncStmt(): EventEmitter() {
            _stmt = NULL;
        }

        MysqlSyncConn::MysqlSyncStmt::~MysqlSyncStmt() {
            if (_stmt) {
                mysql_stmt_close(_stmt);
            }
        }

        Handle<Value> MysqlSyncConn::MysqlSyncStmt::New(const Arguments& args) {
            HandleScope scope;

            REQ_EXT_ARG(0, js_stmt);
            MYSQL_STMT *stmt = static_cast<MYSQL_STMT*>(js_stmt->Value());
            MysqlSyncStmt *my_stmt = new MysqlSyncStmt(stmt);
            my_stmt->Wrap(args.This());

            return args.This();
        }

        Handle<Value> MysqlSyncConn::MysqlSyncStmt::Prepare(const Arguments& args) {
            HandleScope scope;

            MysqlSyncStmt *stmt = OBJUNWRAP<MysqlSyncStmt>(args.This());

            if (args.Length() == 0 || !args[0]->IsString()) {
                return THREXC("First arg of stmt.prepare() must be a string");
            }

            String::Utf8Value query(args[0]);

            int query_len = args[0]->ToString()->Utf8Length();

            if (mysql_stmt_prepare(stmt->_stmt, *query, query_len)) {
                return scope.Close(False());
            }

            return scope.Close(True());
        }

extern "C" void init(Handle<Object> target) {
    MysqlSyncConn::Init(target);
}

