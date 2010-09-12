/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

#ifndef NODE_MYSQL_STATEMENT_H  // NOLINT
#define NODE_MYSQL_STATEMENT_H

#include <mysql.h>

#include <v8.h>
#include <node.h>
#include <node_events.h>

static Persistent<String> statement_prepareSync_symbol;

class MysqlStatement : public node::EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

  protected:
    MYSQL_STMT *_stmt;

    MysqlStatement();

    explicit MysqlStatement(MYSQL_STMT *my_stmt):
                                    EventEmitter(), _stmt(my_stmt) {}

    ~MysqlStatement();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> PrepareSync(const Arguments& args);
};

#endif  // NODE_MYSQL_STATEMENT_H  // NOLINT

