/*
Copyright by node-mysql-libmysqlclient contributors.
See contributors list in README

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_RESULT_H  // NOLINT
#define NODE_MYSQL_RESULT_H

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

#define mysql_result_is_unbuffered(r) \
((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT)

static Persistent<String> result_dataSeekSync_symbol;
static Persistent<String> result_fetchAllSync_symbol;
static Persistent<String> result_fetchArraySync_symbol;
static Persistent<String> result_fetchFieldSync_symbol;
static Persistent<String> result_fetchFieldDirectSync_symbol;
static Persistent<String> result_fetchFieldsSync_symbol;
static Persistent<String> result_fetchLengthsSync_symbol;
static Persistent<String> result_fetchObjectSync_symbol;
static Persistent<String> result_fieldCountSync_symbol;
static Persistent<String> result_fieldSeekSync_symbol;
static Persistent<String> result_fieldTellSync_symbol;
static Persistent<String> result_freeSync_symbol;
static Persistent<String> result_numRowsSync_symbol;

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

    void Free();

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

    static Handle<Value> DataSeekSync(const Arguments& args);

    static Handle<Value> FetchAllSync(const Arguments& args);

    static Handle<Value> FetchArraySync(const Arguments& args);

    static Handle<Value> FetchFieldSync(const Arguments& args);

    static Handle<Value> FetchFieldDirectSync(const Arguments& args);

    static Handle<Value> FetchFieldsSync(const Arguments& args);

    static Handle<Value> FetchLengthsSync(const Arguments& args);

    static Handle<Value> FetchObjectSync(const Arguments& args);

    static Handle<Value> FieldCountSync(const Arguments& args);

    static Handle<Value> FieldSeekSync(const Arguments& args);

    static Handle<Value> FieldTellSync(const Arguments& args);

    static Handle<Value> FreeSync(const Arguments& args);

    static Handle<Value> NumRowsSync(const Arguments& args);
};

#endif  // NODE_MYSQL_RESULT_H  // NOLINT

