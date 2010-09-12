/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

extern "C" void init(Handle<Object> target) {
    MysqlConnection::Init(target);
    MysqlResult::Init(target);
    MysqlStatement::Init(target);
}

