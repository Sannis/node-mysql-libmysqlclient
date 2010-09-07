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
#include <node_events.h>

#include "./mysql_bindings.h"

static Persistent<String> statement_affectedRowsSync_symbol;
static Persistent<String> statement_closeSync_symbol;
static Persistent<String> statement_errnoSync_symbol;
static Persistent<String> statement_errorSync_symbol;
static Persistent<String> statement_executeSync_symbol;
static Persistent<String> statement_prepareSync_symbol;
static Persistent<String> statement_resetSync_symbol;
static Persistent<String> statement_storeResultSync_symbol;
static Persistent<String> statement_sqlStateSync_symbol;

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

    static Handle<Value> AffectedRowsSync(const Arguments& args);

    static Handle<Value> CloseSync(const Arguments& args);

    static Handle<Value> ErrnoSync(const Arguments& args);

    static Handle<Value> ErrorSync(const Arguments& args);

    static Handle<Value> ExecuteSync(const Arguments& args);

    static Handle<Value> PrepareSync(const Arguments& args);

    static Handle<Value> ResetSync(const Arguments& args);

    static Handle<Value> StoreResultSync(const Arguments& args);

    static Handle<Value> SqlStateSync(const Arguments& args);
};

#endif  // SRC_MYSQL_BINDINGS_STATEMENT_H_

