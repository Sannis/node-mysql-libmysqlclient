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

#define mysqli_result_is_unbuffered(r) \
((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT)

static Persistent<String> fetchAll_symbol;
static Persistent<String> numRows_symbol;

class MysqlConn::MysqlResult : public node::EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

  protected:
    MYSQL_RES *_res;

    MysqlResult();

    explicit MysqlResult(MYSQL_RES *my_result):
                                    EventEmitter(), _res(my_result) {}

    ~MysqlResult();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> FetchAll(const Arguments& args);

    static Handle<Value> NumRows(const Arguments& args);
};

#endif  // NODE_MYSQL_RESULT_H  // NOLINT

