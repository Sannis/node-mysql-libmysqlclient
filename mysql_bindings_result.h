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

// static Persistent<String> fetchResult_symbol;

class MysqlSyncConn::MysqlSyncRes : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

  protected:
    MYSQL_RES *_res;

    MysqlSyncRes();

    explicit MysqlSyncRes(MYSQL_RES *my_result):
                                    _res(my_result), EventEmitter() {}

    ~MysqlSyncRes();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> FetchResult(const Arguments& args);
};

#endif  // NODE_MYSQL_RESULT_H  // NOLINT

