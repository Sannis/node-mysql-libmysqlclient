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
        return NanThrowError("Not connected"); \
    }

#define MYSQLCONN_MUSTBE_INITIALIZED \
    if (!conn->_conn) { \
        return NanThrowError("Not initialized"); \
    }

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

  private:
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

    static NAN_METHOD(New);

    // Properties

    static NAN_GETTER(ConnectErrnoGetter);

    static NAN_GETTER(ConnectErrorGetter);

    // Methods

    static NAN_METHOD(AffectedRowsSync);

    static NAN_METHOD(AutoCommitSync);

    static NAN_METHOD(ChangeUserSync);

    static NAN_METHOD(CommitSync);

    struct connect_request {
        bool ok;

        NanCallback *nan_callback;
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
    static NAN_METHOD(Connect);

    static NAN_METHOD(ConnectSync);

    static NAN_METHOD(ConnectedSync);

    static NAN_METHOD(CloseSync);

    static NAN_METHOD(DebugSync);

    static NAN_METHOD(DumpDebugInfoSync);

    static NAN_METHOD(ErrnoSync);

    static NAN_METHOD(ErrorSync);

    static NAN_METHOD(EscapeSync);

    static NAN_METHOD(FieldCountSync);

    static NAN_METHOD(GetCharsetSync);

    static NAN_METHOD(GetCharsetNameSync);

    static NAN_METHOD(GetClientInfoSync);

    static NAN_METHOD(GetInfoSync);

    static NAN_METHOD(GetInfoStringSync);

    static NAN_METHOD(GetWarningsSync);

    static NAN_METHOD(InitSync);

    static NAN_METHOD(InitStatementSync);

    static NAN_METHOD(LastInsertIdSync);

    static NAN_METHOD(MultiMoreResultsSync);

    static NAN_METHOD(MultiNextResultSync);

    static NAN_METHOD(MultiRealQuerySync);

    static NAN_METHOD(PingSync);
    struct local_infile_data {
      char * buffer;
      size_t length;
      size_t position;
    };

    struct query_request {
        bool ok;
        bool connection_closed;
        bool have_result_set;

        NanCallback *nan_callback;
        MysqlConnection *conn;

        char *query;
        unsigned int query_len;

        MYSQL_RES *my_result;
        uint32_t field_count;
        my_ulonglong affected_rows;
        my_ulonglong insert_id;

        unsigned int my_errno;
        const char *my_error;

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
    static NAN_METHOD(Query);

    static void EV_After_QuerySend(uv_poll_t* handle, int status, int events);
    static void EV_After_QuerySend_OnWatchHandleClose(uv_handle_t* handle);
    static NAN_METHOD(QuerySend);

    static NAN_METHOD(QuerySync);

    static NAN_METHOD(RealConnectSync);

    static NAN_METHOD(RealQuerySync);

    static NAN_METHOD(RollbackSync);

    static NAN_METHOD(SelectDbSync);

    static NAN_METHOD(SetCharsetSync);

    static NAN_METHOD(SetOptionSync);

    static NAN_METHOD(SetSslSync);

    static NAN_METHOD(SqlStateSync);

    static NAN_METHOD(StatSync);

    static NAN_METHOD(StoreResultSync);

    static NAN_METHOD(ThreadIdSync);

    static NAN_METHOD(ThreadSafeSync);

    static NAN_METHOD(UseResultSync);

    static NAN_METHOD(WarningCountSync);
};

#endif  // SRC_MYSQL_BINDINGS_CONNECTION_H_

