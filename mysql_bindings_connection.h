/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_CONNECTION_H
#define NODE_MYSQL_CONNECTION_H

#include <mysql/mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

// This line caused
// "Do not use namespace using-directives. Use using-declarations instead."
// [build/namespaces] [5] error in cpplint.py
using namespace v8;

// static Persistent<String> affectedRows_symbol;
// static Persistent<String> autoCommit_symbol;
// static Persistent<String> changeUser_symbol;
// static Persistent<String> commit_symbol;
// static Persistent<String> connect_symbol;
// static Persistent<String> connectErrno_symbol;
// static Persistent<String> connectError_symbol;
// static Persistent<String> close_symbol;
// static Persistent<String> debug_symbol;
// static Persistent<String> dumpDebugInfo_symbol;
// static Persistent<String> errno_symbol;
// static Persistent<String> error_symbol;
// static Persistent<String> escape_symbol;
// static Persistent<String> fieldCount_symbol;
// static Persistent<String> getCharset_symbol;
// static Persistent<String> getCharsetName_symbol;
// static Persistent<String> getInfo_symbol;
// static Persistent<String> getInfoString_symbol;
// static Persistent<String> getWarnings_symbol;
// static Persistent<String> initStatement_symbol;
// static Persistent<String> lastInsertId_symbol;
// static Persistent<String> multiMoreResults_symbol;
// static Persistent<String> multiNextResult_symbol;
// static Persistent<String> multiRealQuery_symbol;
// static Persistent<String> ping_symbol;
// static Persistent<String> query_symbol;
// static Persistent<String> realQuery_symbol;
// static Persistent<String> rollback_symbol;
// static Persistent<String> selectDb_symbol;
// static Persistent<String> setCharset_symbol;
// static Persistent<String> setSsl_symbol;
// static Persistent<String> sqlState_symbol;
// static Persistent<String> stat_symbol;
// static Persistent<String> storeResult_symbol;
// static Persistent<String> threadId_symbol;
// static Persistent<String> threadKill_symbol;
// static Persistent<String> threadSafe_symbol;
// static Persistent<String> useResult_symbol;
// static Persistent<String> warningCount_symbol;

class MysqlSyncConn : public node::EventEmitter {
  public:
    struct MysqlSyncConnInfo {
        uint64_t client_version;
        const char *client_info;
        uint64_t server_version;
        const char *server_info;
        const char *host_info;
        uint32_t proto_info;
    };
    
    class MysqlSyncRes;
    
    class MysqlSyncStmt;

    static void Init(Handle<Object> target);

    bool Connect(const char* hostname,
                 const char* user,
                 const char* password,
                 const char* dbname,
                 uint32_t port,
                 const char* socket);

    void Close();

    MysqlSyncConnInfo GetInfo();

  protected:
    MYSQL *_conn;

    bool multi_query;

    unsigned int connect_errno;
    const char *connect_error;

    MysqlSyncConn();

    ~MysqlSyncConn();

    static Handle<Value> New(const Arguments& args);

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

//Persistent<FunctionTemplate> MysqlSyncConn::MysqlSyncRes::constructor_template;
//Persistent<FunctionTemplate> MysqlSyncConn::MysqlSyncStmt::constructor_template;

extern "C" void init(Handle<Object> target);

#endif  // NODE_MYSQL_CONNECTION_H

