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

static Persistent<String> statement_prepareSync_symbol;

class MysqlStatement : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(Handle<Object> target);

  protected:
    MYSQL_STMT *_stmt;

    MysqlStatement();

    explicit MysqlStatement(MYSQL_STMT *my_stmt):
                                    ObjectWrap(), _stmt(my_stmt) {}

    ~MysqlStatement();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> PrepareSync(const Arguments& args);
};

#endif  // SRC_MYSQL_BINDINGS_STATEMENT_H_

