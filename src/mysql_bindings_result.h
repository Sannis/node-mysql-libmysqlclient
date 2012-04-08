/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef SRC_MYSQL_BINDINGS_RESULT_H_
#define SRC_MYSQL_BINDINGS_RESULT_H_

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <node_buffer.h>

#include <cstring>

#include "./mysql_bindings.h"

#define mysql_result_is_unbuffered(r) \
((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT)

#define MYSQLRES_MUSTBE_VALID \
    if (!res->_res) { \
        return THREXC("Result has been freed."); \
    }

static Persistent<String> result_dataSeekSync_symbol;
static Persistent<String> result_fetchAll_symbol;
static Persistent<String> result_fetchAllSync_symbol;
static Persistent<String> result_fetchArraySync_symbol;
static Persistent<String> result_fetchFieldSync_symbol;
static Persistent<String> result_fetchFieldDirectSync_symbol;
static Persistent<String> result_fetchFieldsSync_symbol;
static Persistent<String> result_fetchLengthsSync_symbol;
static Persistent<String> result_fetchObjectSync_symbol;
static Persistent<String> result_fieldSeekSync_symbol;
static Persistent<String> result_fieldTellSync_symbol;
static Persistent<String> result_freeSync_symbol;
static Persistent<String> result_numRowsSync_symbol;

class MysqlResult : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    static void AddFieldProperties(
                                    Local<Object> &js_field_obj,
                                    MYSQL_FIELD *field);

    static Local<Value> GetFieldValue(MYSQL_FIELD field,
                                       char* field_value,
                                       unsigned long field_length);

    void Free();

  protected:
    MYSQL *_conn;
    MYSQL_RES *_res;

    uint32_t field_count;

    MysqlResult();

    explicit MysqlResult(MYSQL *my_connection,
                          MYSQL_RES *my_result,
                          uint32_t my_field_count):
                                                ObjectWrap(),
                                                _conn(my_connection),
                                                _res(my_result),
                                                field_count(my_field_count) {}

    ~MysqlResult();

    // Constructor

    static Handle<Value> New(const Arguments& args);

    // Properties

    static Handle<Value> FieldCountGetter(Local<String> property,
                                           const AccessorInfo &info);

    // Methods

    static Handle<Value> DataSeekSync(const Arguments& args);

    struct fetchAll_request {
        bool ok;
        
        Persistent<Function> callback;
        MysqlResult *res;

        MYSQL_FIELD *fields;
        uint32_t num_fields;
        bool results_array;
        bool results_structured;
    };
    static async_rtn EIO_After_FetchAll(uv_work_t *req);
    static async_rtn EIO_FetchAll(uv_work_t *req);
    static Handle<Value> FetchAll(const Arguments& args);

    static Handle<Value> FetchAllSync(const Arguments& args);

    static Handle<Value> FetchArraySync(const Arguments& args);

    static Handle<Value> FetchFieldSync(const Arguments& args);

    static Handle<Value> FetchFieldDirectSync(const Arguments& args);

    static Handle<Value> FetchFieldsSync(const Arguments& args);

    static Handle<Value> FetchLengthsSync(const Arguments& args);

    static Handle<Value> FetchObjectSync(const Arguments& args);

    static Handle<Value> FieldSeekSync(const Arguments& args);

    static Handle<Value> FieldTellSync(const Arguments& args);

    static Handle<Value> FreeSync(const Arguments& args);

    static Handle<Value> NumRowsSync(const Arguments& args);
};

#endif  // SRC_MYSQL_BINDINGS_RESULT_H_

