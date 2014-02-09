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
        return NanThrowError("Result has been freed."); \
    }

/** section: Classes
 * class MysqlResult
 *
 * MySQL results class
 **/
class MysqlResult : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    static Local<Object> NewInstance(MYSQL *my_conn, MYSQL_RES *my_result, uint32_t field_count);

    static void AddFieldProperties(Local<Object> &js_field_obj, MYSQL_FIELD *field);

    static Local<Value> GetFieldValue(MYSQL_FIELD field, char* field_value, unsigned long field_length);

    struct fetch_options {
        bool results_as_array;
        bool results_nest_tables;
    };
    static fetch_options GetFetchOptions(Local<Object> options);

    void Free();

  private:
    MYSQL *_conn;
    MYSQL_RES *_res;

    uint32_t field_count;

    MysqlResult();

    explicit MysqlResult(MYSQL *my_connection, MYSQL_RES *my_result, uint32_t my_field_count):
        ObjectWrap(),
        _conn(my_connection),
        _res(my_result),
        field_count(my_field_count) {}

    ~MysqlResult();

    // Constructor

    static NAN_METHOD(New);

    // Properties

    static NAN_GETTER(FieldCountGetter);

    // Methods

    static NAN_METHOD(DataSeekSync);

    struct fetchAll_request {
        bool ok;
        
        NanCallback *nan_callback;
        MysqlResult *res;

        MYSQL_FIELD *fields;
        uint32_t num_fields;

        fetch_options fo;
    };
    static void EIO_After_FetchAll(uv_work_t *req);
    static void EIO_FetchAll(uv_work_t *req);
    static NAN_METHOD(FetchAll);

    static NAN_METHOD(FetchAllSync);

    static NAN_METHOD(FetchFieldSync);

    static NAN_METHOD(FetchFieldDirectSync);

    static NAN_METHOD(FetchFieldsSync);

    static NAN_METHOD(FetchLengthsSync);

    static NAN_METHOD(FetchRowSync);

    static NAN_METHOD(FieldSeekSync);

    static NAN_METHOD(FieldTellSync);

    static NAN_METHOD(FreeSync);

    static NAN_METHOD(NumRowsSync);
};

#endif  // SRC_MYSQL_BINDINGS_RESULT_H_

