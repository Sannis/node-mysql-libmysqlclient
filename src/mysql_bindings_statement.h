/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_STATEMENT_H  // NOLINT
#define NODE_MYSQL_STATEMENT_H

#include <mysql/mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

static Persistent<String> prepare_symbol;

class MysqlSyncConn::MysqlSyncStmt : public node::EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

  protected:
    MYSQL_STMT *_stmt;

    MysqlSyncStmt();

    explicit MysqlSyncStmt(MYSQL_STMT *my_stmt):
                                    _stmt(my_stmt), EventEmitter() {}

    ~MysqlSyncStmt();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Prepare(const Arguments& args);
};

#endif  // NODE_MYSQL_STATEMENT_H  // NOLINT

