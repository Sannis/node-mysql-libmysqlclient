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

/* Backport MakeCallback from Node v0.7.8 */
#if NODE_VERSION_AT_LEAST(0, 7, 8)
  // Nothing
#else
    namespace node {
        Handle<Value>
        MakeCallback(const Handle<Object> object,
                     const Handle<Function> callback,
                     int argc,
                     Handle<Value> argv[]) {
          HandleScope scope;

          // TODO Hook for long stack traces to be made here.

          TryCatch try_catch;

          Local<Value> ret = callback->Call(object, argc, argv);

          if (try_catch.HasCaught()) {
            FatalException(try_catch);
            return Undefined();
          }

          return scope.Close(ret);
        }
    }  // namespace node
#endif

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
