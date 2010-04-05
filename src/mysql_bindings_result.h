/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_RESULT_H  // NOLINT
#define NODE_MYSQL_RESULT_H

#include <mysql/mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

#define mysql_result_is_unbuffered(r) \
((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT)

static Persistent<String> result_dataSeek_symbol;
static Persistent<String> result_fetchAll_symbol;
static Persistent<String> result_fetchArray_symbol;
static Persistent<String> result_fetchField_symbol;
static Persistent<String> result_fetchFieldDirect_symbol;
static Persistent<String> result_fetchFields_symbol;
static Persistent<String> result_fetchLengths_symbol;
static Persistent<String> result_fetchObject_symbol;
static Persistent<String> result_fieldCount_symbol;
static Persistent<String> result_fieldSeek_symbol;
static Persistent<String> result_fieldTell_symbol;
static Persistent<String> result_numRows_symbol;

class MysqlConn::MysqlResult : public node::EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

    static void AddFieldProperties(
                                    Local<Object> &js_field_obj,
                                    MYSQL_FIELD *field);

    static void SetFieldValue(
                                Handle<Value> &js_field,
                                MYSQL_FIELD field,
                                char* field_value);

  protected:
    MYSQL_RES *_res;

    uint32_t field_count;

    MysqlResult();

    explicit MysqlResult(MYSQL_RES *my_result, uint32_t my_field_count):
                                    EventEmitter(),
                                    _res(my_result),
                                    field_count(my_field_count) {}

    ~MysqlResult();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> DataSeek(const Arguments& args);

    static Handle<Value> FetchAll(const Arguments& args);

    static Handle<Value> FetchArray(const Arguments& args);

    static Handle<Value> FetchField(const Arguments& args);

    static Handle<Value> FetchFieldDirect(const Arguments& args);

    static Handle<Value> FetchFields(const Arguments& args);

    static Handle<Value> FetchLengths(const Arguments& args);

    static Handle<Value> FetchObject(const Arguments& args);

    static Handle<Value> FieldCount(const Arguments& args);

    static Handle<Value> FieldSeek(const Arguments& args);

    static Handle<Value> FieldTell(const Arguments& args);

    static Handle<Value> NumRows(const Arguments& args);
};

#endif  // NODE_MYSQL_RESULT_H  // NOLINT

