/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef SRC_MYSQL_BINDINGS_CONNECTION_H_
#define SRC_MYSQL_BINDINGS_CONNECTION_H_

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_version.h>

#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <cstring>

#include "./mysql_bindings.h"

#define MYSQLCONN_DISABLE_MQ \
if (conn->multi_query) { \
    mysql_set_server_option(conn->_conn, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
    conn->multi_query = false; \
}

#define MYSQLCONN_ENABLE_MQ \
if (!conn->multi_query) { \
    mysql_set_server_option(conn->_conn, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
    conn->multi_query = true; \
}

#define MYSQLCONN_MUSTBE_CONNECTED \
    if (!conn->_conn || !conn->connected) { \
        return THREXC("Not connected"); \
    }

#define MYSQLCONN_MUSTBE_INITIALIZED \
    if (!conn->_conn) { \
        return THREXC("Not initialized"); \
    }

using namespace v8; // NOLINT

static Persistent<String> connection_affectedRowsSync_symbol;
static Persistent<String> connection_autoCommitSync_symbol;
static Persistent<String> connection_changeUserSync_symbol;
static Persistent<String> connection_commitSync_symbol;
static Persistent<String> connection_connect_symbol;
static Persistent<String> connection_connectSync_symbol;
static Persistent<String> connection_connectedSync_symbol;
static Persistent<String> connection_closeSync_symbol;
static Persistent<String> connection_debugSync_symbol;
static Persistent<String> connection_dumpDebugInfoSync_symbol;
static Persistent<String> connection_errnoSync_symbol;
static Persistent<String> connection_errorSync_symbol;
static Persistent<String> connection_escapeSync_symbol;
static Persistent<String> connection_fieldCountSync_symbol;
static Persistent<String> connection_getCharsetSync_symbol;
static Persistent<String> connection_getCharsetNameSync_symbol;
static Persistent<String> connection_getClientInfoSync_symbol;
static Persistent<String> connection_getInfoSync_symbol;
static Persistent<String> connection_getInfoStringSync_symbol;
static Persistent<String> connection_getWarningsSync_symbol;
static Persistent<String> connection_initSync_symbol;
static Persistent<String> connection_initStatementSync_symbol;
static Persistent<String> connection_lastInsertIdSync_symbol;
static Persistent<String> connection_multiMoreResultsSync_symbol;
static Persistent<String> connection_multiNextResultSync_symbol;
static Persistent<String> connection_multiRealQuerySync_symbol;
static Persistent<String> connection_pingSync_symbol;
static Persistent<String> connection_query_symbol;
static Persistent<String> connection_querySend_symbol;
static Persistent<String> connection_querySync_symbol;
static Persistent<String> connection_realConnectSync_symbol;
static Persistent<String> connection_realQuerySync_symbol;
static Persistent<String> connection_rollbackSync_symbol;
static Persistent<String> connection_selectDbSync_symbol;
static Persistent<String> connection_setCharsetSync_symbol;
static Persistent<String> connection_setOptionSync_symbol;
static Persistent<String> connection_setSslSync_symbol;
static Persistent<String> connection_sqlStateSync_symbol;
static Persistent<String> connection_statSync_symbol;
static Persistent<String> connection_storeResultSync_symbol;
static Persistent<String> connection_threadIdSync_symbol;
static Persistent<String> connection_threadSafeSync_symbol;
static Persistent<String> connection_useResultSync_symbol;
static Persistent<String> connection_warningCountSync_symbol;

class MysqlConnection : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    bool Connect(const char* hostname,
                 const char* user,
                 const char* password,
                 const char* dbname,
                 uint32_t port,
                 const char* socket,
                 uint64_t flags);

    bool RealConnect(const char* hostname,
                 const char* user,
                 const char* password,
                 const char* dbname,
                 uint32_t port,
                 const char* socket,
                 uint64_t flags);

    void Close();

  protected:
    MYSQL *_conn;
    bool connected;

    pthread_mutex_t query_lock;

    bool multi_query;
    my_bool opt_reconnect;

    unsigned int connect_errno;
    const char *connect_error;

    MysqlConnection();

    ~MysqlConnection();

    // Constructor

    static Handle<Value> New(const Arguments& args);

    // Properties

    static Handle<Value> ConnectErrnoGetter(Local<String> property,
                                             const AccessorInfo &info);

    static Handle<Value> ConnectErrorGetter(Local<String> property,
                                             const AccessorInfo &info);

    // Methods

    static Handle<Value> AffectedRowsSync(const Arguments& args);

    static Handle<Value> AutoCommitSync(const Arguments& args);

    static Handle<Value> ChangeUserSync(const Arguments& args);

    static Handle<Value> CommitSync(const Arguments& args);

    struct connect_request {
        bool ok;
        
        Persistent<Function> callback;
        MysqlConnection *conn;

        String::Utf8Value *hostname;
        String::Utf8Value *user;
        String::Utf8Value *password;
        String::Utf8Value *dbname;
        uint32_t port;
        String::Utf8Value *socket;
        uint64_t flags;

        unsigned int errno;
        const char *error;
    };
    static async_rtn EIO_After_Connect(uv_work_t *req);
    static async_rtn EIO_Connect(uv_work_t *req);
    static Handle<Value> Connect(const Arguments& args);

    static Handle<Value> ConnectSync(const Arguments& args);

    static Handle<Value> ConnectedSync(const Arguments& args);

    static Handle<Value> CloseSync(const Arguments& args);

    static Handle<Value> DebugSync(const Arguments& args);

    static Handle<Value> DumpDebugInfoSync(const Arguments& args);

    static Handle<Value> ErrnoSync(const Arguments& args);

    static Handle<Value> ErrorSync(const Arguments& args);

    static Handle<Value> EscapeSync(const Arguments& args);

    static Handle<Value> FieldCountSync(const Arguments& args);

    static Handle<Value> GetCharsetSync(const Arguments& args);

    static Handle<Value> GetCharsetNameSync(const Arguments& args);

    static Handle<Value> GetClientInfoSync(const Arguments& args);

    static Handle<Value> GetInfoSync(const Arguments& args);

    static Handle<Value> GetInfoStringSync(const Arguments& args);

    static Handle<Value> GetWarningsSync(const Arguments& args);

    static Handle<Value> InitSync(const Arguments& args);

    static Handle<Value> InitStatementSync(const Arguments& args);

    static Handle<Value> LastInsertIdSync(const Arguments& args);

    static Handle<Value> MultiMoreResultsSync(const Arguments& args);

    static Handle<Value> MultiNextResultSync(const Arguments& args);

    static Handle<Value> MultiRealQuerySync(const Arguments& args);

    static Handle<Value> PingSync(const Arguments& args);

    struct query_request {
        bool ok;
        bool have_result_set;
        
        Persistent<Value> callback;
        MysqlConnection *conn;

        char *query;
        unsigned int query_len;
        
        MYSQL_RES *my_result;
        uint32_t field_count;
        my_ulonglong affected_rows;
        my_ulonglong insert_id;

        unsigned int errno;
        const char *error;
    };
    static async_rtn EIO_After_Query(uv_work_t *req);
    static async_rtn EIO_Query(uv_work_t *req);
    static Handle<Value> Query(const Arguments& args);

    static void EV_After_QuerySend(EV_P_ ev_io *io_watcher, int revents);
    static Handle<Value> QuerySend(const Arguments& args);

    static Handle<Value> QuerySync(const Arguments& args);

    static Handle<Value> RealConnectSync(const Arguments& args);

    static Handle<Value> RealQuerySync(const Arguments& args);

    static Handle<Value> RollbackSync(const Arguments& args);

    static Handle<Value> SelectDbSync(const Arguments& args);

    static Handle<Value> SetCharsetSync(const Arguments& args);

    static Handle<Value> SetOptionSync(const Arguments& args);

    static Handle<Value> SetSslSync(const Arguments& args);

    static Handle<Value> SqlStateSync(const Arguments& args);

    static Handle<Value> StatSync(const Arguments& args);

    static Handle<Value> StoreResultSync(const Arguments& args);

    static Handle<Value> ThreadIdSync(const Arguments& args);

    static Handle<Value> ThreadSafeSync(const Arguments& args);

    static Handle<Value> UseResultSync(const Arguments& args);

    static Handle<Value> WarningCountSync(const Arguments& args);
};

#endif  // SRC_MYSQL_BINDINGS_CONNECTION_H_

