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

/** section: Classes
 * class MysqlConnection
 *
 * MySQL connection class, base version
 **/
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

    static Handle<Value> ConnectErrnoGetter(Local<String> property, const AccessorInfo &info);

    static Handle<Value> ConnectErrorGetter(Local<String> property, const AccessorInfo &info);

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
    };
    static void EIO_After_Connect(uv_work_t *req);
    static void EIO_Connect(uv_work_t *req);
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
    struct local_infile_data {
      char * buffer;
      size_t length;
      size_t position;
    };

    struct query_request {
        bool ok;
        bool connection_closed;
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

        local_infile_data * infile_data;
    };
    static int CustomLocalInfileInit(void ** ptr,
                                     const char * filename,
                                     void * userdata);
    static int CustomLocalInfileRead(void * ptr,
                                     char * buf,
                                     unsigned int buf_len);
    static void CustomLocalInfileEnd(void * ptr);
    static int CustomLocalInfileError(void * ptr,
                                      char * error_msg,
                                      unsigned int error_msg_len);
    static void SetCorrectLocalInfileHandlers(local_infile_data * infile_data,
                                              MYSQL * conn);
    static void RestoreLocalInfileHandlers(local_infile_data * infile_data,
                                           MYSQL * conn);
    static local_infile_data * PrepareLocalInfileData(Handle<Value> buffer);
    static void EIO_After_Query(uv_work_t *req);
    static void EIO_Query(uv_work_t *req);
    static Handle<Value> Query(const Arguments& args);

    /*!
     * Callback function for uv_close(uv_handle_t* handle), if needed
     */
    NODE_ADDON_SHIM_STOP_IO_WATCH_ONCLOSE(EV_After_QuerySend_OnWatchHandleClose)
    static void EV_After_QuerySend(NODE_ADDON_SHIM_IO_WATCH_CALLBACK_ARGUMENTS);
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

