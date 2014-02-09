/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef SRC_MYSQL_BINDINGS_STATEMENT_H_
#define SRC_MYSQL_BINDINGS_STATEMENT_H_

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

#include "./mysql_bindings.h"

#define MYSQLSTMT_MUSTBE_INITIALIZED \
    if (stmt->state < STMT_INITIALIZED) { \
        return NanThrowError("Statement not initialized"); \
    }

#define MYSQLSTMT_MUSTBE_PREPARED \
    if (stmt->state < STMT_PREPARED) { \
        return NanThrowError("Statement not prepared"); \
    }

#define MYSQLSTMT_MUSTBE_EXECUTED \
    if (stmt->state < STMT_EXECUTED) { \
        return NanThrowError("Statement not executed"); \
    }

#define MYSQLSTMT_MUSTBE_STORED \
    if (stmt->state < STMT_STORED_RESULT) { \
        return NanThrowError("Statement result not stored"); \
    }

/** section: Classes
 * class MysqlStatement
 *
 * MySQL prepared statement class
 **/
class MysqlStatement : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    static Local<Object> NewInstance(MYSQL_STMT *my_statement);

  private:
    MYSQL_STMT *_stmt;

    MYSQL_BIND *binds;
    MYSQL_BIND *result_binds;
    unsigned long param_count;

    enum MysqlStatementState {
        STMT_CLOSED,
        STMT_INITIALIZED,
        STMT_PREPARED,
        STMT_BINDED_PARAMS,
        STMT_EXECUTED,
        STMT_BINDED_RESULT,
        STMT_STORED_RESULT,
    };
    MysqlStatementState state;

    MysqlStatement(MYSQL_STMT *my_stmt);

    ~MysqlStatement();

    // Constructor

    static NAN_METHOD(New);

    // Properties

    static NAN_GETTER(ParamCountGetter);

    // Methods

    static NAN_METHOD(AffectedRowsSync);

    static NAN_METHOD(AttrGetSync);

    static NAN_METHOD(AttrSetSync);

    static NAN_METHOD(BindParamsSync);

    static NAN_METHOD(BindResultSync);

    static NAN_METHOD(CloseSync);

    static NAN_METHOD(DataSeekSync);

    static NAN_METHOD(ErrnoSync);

    static NAN_METHOD(ErrorSync);

    struct execute_request {
        bool ok;

        NanCallback *nan_callback;
        MysqlStatement* stmt;
    };

    static void EIO_After_Execute(uv_work_t* req);

    static void EIO_Execute(uv_work_t* req);

    static NAN_METHOD(Execute);

    static NAN_METHOD(ExecuteSync);

    struct fetch_request {
        bool ok;
        bool empty_resultset;

        NanCallback *nan_callback;
        MysqlStatement* stmt;

        MYSQL_RES* meta;
        unsigned long field_count;
    };
    static void EIO_After_FetchAll(uv_work_t* req);
    static void EIO_FetchAll(uv_work_t* req);
    static NAN_METHOD(FetchAll);

    static NAN_METHOD(FetchAllSync);

    static void EIO_After_Fetch(uv_work_t* req);
    static void EIO_Fetch(uv_work_t* req);
    static NAN_METHOD(Fetch);

    static NAN_METHOD(FetchSync);

    static NAN_METHOD(FieldCountSync);

    static NAN_METHOD(FreeResultSync);

    static void FreeMysqlBinds(MYSQL_BIND *binds, unsigned long size, bool params);

    static Local<Value> GetFieldValue(void* ptr, unsigned long& length, MYSQL_FIELD& field);

    static NAN_METHOD(LastInsertIdSync);

    static NAN_METHOD(NextResultSync);

    static NAN_METHOD(NumRowsSync);

    static NAN_METHOD(PrepareSync);

    static NAN_METHOD(ResetSync);

    static NAN_METHOD(ResultMetadataSync);

    static NAN_METHOD(SendLongDataSync);

    static NAN_METHOD(StoreResultSync);

    struct store_result_request {
        bool ok;

        NanCallback *nan_callback;
        MysqlStatement* stmt;
    };
    static void EIO_After_StoreResult(uv_work_t* req);
    static void EIO_StoreResult(uv_work_t* req);
    static NAN_METHOD(StoreResult);

    static NAN_METHOD(SqlStateSync);
};

#endif  // SRC_MYSQL_BINDINGS_STATEMENT_H_

