/*
Copyright by node-mysql-libmysqlclient contributors.
See contributors list in README

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_CONNECTION_H  // NOLINT
#define NODE_MYSQL_CONNECTION_H

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#define ADD_PROTOTYPE_METHOD(class, name, method) \
class ## _ ## name ## _symbol = NODE_PSYMBOL(#name); \
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

static Persistent<String> connection_async_symbol;

static Persistent<String> connection_affectedRowsSync_symbol;
static Persistent<String> connection_autoCommitSync_symbol;
static Persistent<String> connection_changeUserSync_symbol;
static Persistent<String> connection_commitSync_symbol;
static Persistent<String> connection_connectSync_symbol;
static Persistent<String> connection_connectedSync_symbol;
static Persistent<String> connection_connectErrnoSync_symbol;
static Persistent<String> connection_connectErrorSync_symbol;
static Persistent<String> connection_closeSync_symbol;
static Persistent<String> connection_debugSync_symbol;
static Persistent<String> connection_dumpDebugInfoSync_symbol;
static Persistent<String> connection_errnoSync_symbol;
static Persistent<String> connection_errorSync_symbol;
static Persistent<String> connection_escapeSync_symbol;
static Persistent<String> connection_fieldCountSync_symbol;
static Persistent<String> connection_getCharsetSync_symbol;
static Persistent<String> connection_getCharsetNameSync_symbol;
static Persistent<String> connection_getInfoSync_symbol;
static Persistent<String> connection_getInfoStringSync_symbol;
static Persistent<String> connection_getWarningsSync_symbol;
static Persistent<String> connection_initStatementSync_symbol;
static Persistent<String> connection_lastInsertIdSync_symbol;
static Persistent<String> connection_multiMoreResultsSync_symbol;
static Persistent<String> connection_multiNextResultSync_symbol;
static Persistent<String> connection_multiRealQuerySync_symbol;
static Persistent<String> connection_pingSync_symbol;
static Persistent<String> connection_querySync_symbol;
static Persistent<String> connection_query_symbol;
static Persistent<String> connection_realQuerySync_symbol;
static Persistent<String> connection_rollbackSync_symbol;
static Persistent<String> connection_selectDbSync_symbol;
static Persistent<String> connection_setCharsetSync_symbol;
static Persistent<String> connection_setSslSync_symbol;
static Persistent<String> connection_sqlStateSync_symbol;
static Persistent<String> connection_statSync_symbol;
static Persistent<String> connection_storeResultSync_symbol;
static Persistent<String> connection_threadIdSync_symbol;
static Persistent<String> connection_threadKillSync_symbol;
static Persistent<String> connection_threadSafeSync_symbol;
static Persistent<String> connection_useResultSync_symbol;
static Persistent<String> connection_warningCountSync_symbol;

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
    bool connected;

    pthread_mutex_t query_lock;

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
        String::Utf8Value *hostname;
        String::Utf8Value *user;
        String::Utf8Value *password;
        String::Utf8Value *dbname;
        uint32_t port;
        String::Utf8Value *socket;
    };
    static int EIO_After_Async(eio_req *req);
    static int EIO_Async(eio_req *req);
    static Handle<Value> Async(const Arguments& args);
    /* Example of async function? based on libeio [E] */

    static Handle<Value> AffectedRowsSync(const Arguments& args);

    static Handle<Value> AutoCommitSync(const Arguments& args);

    static Handle<Value> ChangeUserSync(const Arguments& args);

    static Handle<Value> CommitSync(const Arguments& args);

    static Handle<Value> ConnectSync(const Arguments& args);

    static Handle<Value> ConnectedSync(const Arguments& args);

    static Handle<Value> ConnectErrnoSync(const Arguments& args);

    static Handle<Value> ConnectErrorSync(const Arguments& args);

    static Handle<Value> CloseSync(const Arguments& args);

    static Handle<Value> DebugSync(const Arguments& args);

    static Handle<Value> DumpDebugInfoSync(const Arguments& args);

    static Handle<Value> ErrnoSync(const Arguments& args);

    static Handle<Value> ErrorSync(const Arguments& args);

    static Handle<Value> EscapeSync(const Arguments& args);

    static Handle<Value> FieldCountSync(const Arguments& args);

    static Handle<Value> GetCharsetSync(const Arguments& args);

    static Handle<Value> GetCharsetNameSync(const Arguments& args);

    static Handle<Value> GetInfoSync(const Arguments& args);

    static Handle<Value> GetInfoStringSync(const Arguments& args);

    static Handle<Value> GetWarningsSync(const Arguments& args);

    static Handle<Value> InitStatementSync(const Arguments& args);

    static Handle<Value> LastInsertIdSync(const Arguments& args);

    static Handle<Value> MultiMoreResultsSync(const Arguments& args);

    static Handle<Value> MultiNextResultSync(const Arguments& args);

    static Handle<Value> MultiRealQuerySync(const Arguments& args);

    static Handle<Value> PingSync(const Arguments& args);

    static Handle<Value> QuerySync(const Arguments& args);

    struct query_request {
        Persistent<Function> callback;
        MysqlConn *conn;
        int query_length;
        char *query;
        MYSQL_RES *my_result;
        uint32_t field_count;
    };
    static int EIO_After_Query(eio_req *req);
    static int EIO_Query(eio_req *req);
    static Handle<Value> Query(const Arguments& args);

    static Handle<Value> RealQuerySync(const Arguments& args);

    static Handle<Value> RollbackSync(const Arguments& args);

    static Handle<Value> SelectDbSync(const Arguments& args);

    static Handle<Value> SetCharsetSync(const Arguments& args);

    static Handle<Value> SetSslSync(const Arguments& args);

    static Handle<Value> SqlStateSync(const Arguments& args);

    static Handle<Value> StatSync(const Arguments& args);

    static Handle<Value> StoreResultSync(const Arguments& args);

    static Handle<Value> ThreadIdSync(const Arguments& args);

    static Handle<Value> ThreadKillSync(const Arguments& args);

    static Handle<Value> ThreadSafeSync(const Arguments& args);

    static Handle<Value> UseResultSync(const Arguments& args);

    static Handle<Value> WarningCountSync(const Arguments& args);
};

extern "C" void init(Handle<Object> target);

#endif  // NODE_MYSQL_CONNECTION_H  // NOLINT

