/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#define BUILDING_NODE_EXTENSION

#include <node.h>

/**
 * Include headers
 *
 * @ignore
 */
#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

/**
 * Init V8 structures
 *
 * Classes to populate in JavaScript:
 *
 * * MysqlConnection
 * * MysqlResult
 * * MysqlStatement
 */
void InitMysqlLibmysqlclient(Handle<Object> target) {
    HandleScope scope;

    MysqlConnection::Init(target);
    MysqlResult::Init(target);
    MysqlStatement::Init(target);
}

NODE_MODULE(mysql_bindings, InitMysqlLibmysqlclient)
