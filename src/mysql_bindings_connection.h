/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_CONNECTION_H  // NOLINT
#define NODE_MYSQL_CONNECTION_H

#include <mysql/mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADD_PROTOTYPE_METHOD(name, method) \
name ## _symbol = NODE_PSYMBOL(#name); \
NODE_SET_PROTOTYPE_METHOD(constructor_template, #name, method);

// Only for fixing some cpplint.py errors:
// Lines should be <= 80 characters long
// [whitespace/line_length] [2]
// Lines should very rarely be longer than 100 characters
// [whitespace/line_length] [4]
#define THREXC(str) ThrowException(Exception::Error(String::New(str)))

#define OBJUNWRAP ObjectWrap::Unwrap

#define REQ_STR_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsString()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a string"))); \
String::Utf8Value VAR(args[I]->ToString());

#define REQ_FUN_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsFunction()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a function"))); \
Local<Function> VAR = Local<Function>::Cast(args[I]);

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

using namespace v8; // NOLINT

static Persistent<String> async_symbol;

static Persistent<String> affectedRows_symbol;
static Persistent<String> autoCommit_symbol;
static Persistent<String> changeUser_symbol;
static Persistent<String> commit_symbol;
static Persistent<String> connect_symbol;
static Persistent<String> connectErrno_symbol;
static Persistent<String> connectError_symbol;
static Persistent<String> close_symbol;
static Persistent<String> debug_symbol;
static Persistent<String> dumpDebugInfo_symbol;
static Persistent<String> errno_symbol;
static Persistent<String> error_symbol;
static Persistent<String> escape_symbol;
static Persistent<String> fieldCount_symbol;
static Persistent<String> getCharset_symbol;
static Persistent<String> getCharsetName_symbol;
static Persistent<String> getInfo_symbol;
static Persistent<String> getInfoString_symbol;
static Persistent<String> getWarnings_symbol;
static Persistent<String> initStatement_symbol;
static Persistent<String> lastInsertId_symbol;
static Persistent<String> multiMoreResults_symbol;
static Persistent<String> multiNextResult_symbol;
static Persistent<String> multiRealQuery_symbol;
static Persistent<String> ping_symbol;
static Persistent<String> query_symbol;
static Persistent<String> queryAsync_symbol;
static Persistent<String> realQuery_symbol;
static Persistent<String> rollback_symbol;
static Persistent<String> selectDb_symbol;
static Persistent<String> setCharset_symbol;
static Persistent<String> setSsl_symbol;
static Persistent<String> sqlState_symbol;
static Persistent<String> stat_symbol;
static Persistent<String> storeResult_symbol;
static Persistent<String> threadId_symbol;
static Persistent<String> threadKill_symbol;
static Persistent<String> threadSafe_symbol;
static Persistent<String> useResult_symbol;
static Persistent<String> warningCount_symbol;

class MysqlConn : public node::EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    struct MysqlConnInfo {
        uint64_t client_version;
        const char *client_info;
        uint64_t server_version;
        const char *server_info;
        const char *host_info;
        uint32_t proto_info;
    };

    class MysqlResult;

    class MysqlStatement;

    bool Connect(const char* hostname,
                 const char* user,
                 const char* password,
                 const char* dbname,
                 uint32_t port,
                 const char* socket);

    void Close();

    MysqlConnInfo GetInfo();

  protected:
    MYSQL *_conn;

    bool multi_query;

    unsigned int connect_errno;
    const char *connect_error;

    MysqlConn();

    ~MysqlConn();

    static Handle<Value> New(const Arguments& args);

    /* Example of async function? based on libeio */
    struct async_request {
        Persistent<Function> callback;
        MysqlConn *conn;
    };
    static int EIO_After_Async(eio_req *req);
    static int EIO_Async(eio_req *req);
    static Handle<Value> Async(const Arguments& args);
    /* Example of async function? based on libeio [E] */

    static Handle<Value> AffectedRows(const Arguments& args);

    static Handle<Value> AutoCommit(const Arguments& args);

    static Handle<Value> ChangeUser(const Arguments& args);

    static Handle<Value> Commit(const Arguments& args);

    static Handle<Value> Connect(const Arguments& args);

    static Handle<Value> ConnectErrno(const Arguments& args);

    static Handle<Value> ConnectError(const Arguments& args);

    static Handle<Value> Close(const Arguments& args);

    static Handle<Value> Debug(const Arguments& args);

    static Handle<Value> DumpDebugInfo(const Arguments& args);

    static Handle<Value> Errno(const Arguments& args);

    static Handle<Value> Error(const Arguments& args);

    static Handle<Value> Escape(const Arguments& args);

    static Handle<Value> FieldCount(const Arguments& args);

    static Handle<Value> GetCharset(const Arguments& args);

    static Handle<Value> GetCharsetName(const Arguments& args);

    static Handle<Value> GetInfo(const Arguments& args);

    static Handle<Value> GetInfoString(const Arguments& args);

    static Handle<Value> GetWarnings(const Arguments& args);

    static Handle<Value> InitStatement(const Arguments& args);

    static Handle<Value> LastInsertId(const Arguments& args);

    static Handle<Value> MultiMoreResults(const Arguments& args);

    static Handle<Value> MultiNextResult(const Arguments& args);

    static Handle<Value> MultiRealQuery(const Arguments& args);

    static Handle<Value> Ping(const Arguments& args);

    static Handle<Value> Query(const Arguments& args);

    struct query_request {
        Persistent<Function> callback;
        MysqlConn *conn;
        int result_mode;
        int query_length;
        char query[1];
        MYSQL_RES *my_result;
    };
    static int EIO_After_Query(eio_req *req);
    static int EIO_Query(eio_req *req);
    static Handle<Value> QueryAsync(const Arguments& args);

    static Handle<Value> RealQuery(const Arguments& args);

    static Handle<Value> Rollback(const Arguments& args);

    static Handle<Value> SelectDb(const Arguments& args);

    static Handle<Value> SetCharset(const Arguments& args);

    static Handle<Value> SetSsl(const Arguments& args);

    static Handle<Value> SqlState(const Arguments& args);

    static Handle<Value> Stat(const Arguments& args);

    static Handle<Value> StoreResult(const Arguments& args);

    static Handle<Value> ThreadId(const Arguments& args);

    static Handle<Value> ThreadKill(const Arguments& args);

    static Handle<Value> ThreadSafe(const Arguments& args);

    static Handle<Value> UseResult(const Arguments& args);

    static Handle<Value> WarningCount(const Arguments& args);
};

extern "C" void init(Handle<Object> target);

#endif  // NODE_MYSQL_CONNECTION_H  // NOLINT

