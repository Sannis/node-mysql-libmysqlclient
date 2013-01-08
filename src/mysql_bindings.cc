/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION 1
#endif  // BUILDING_NODE_EXTENSION

#include <node.h>

/*!
 * Include headers
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

/*!
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

    //// Populate classes constructors
    MysqlConnection::Init(target);
    MysqlResult::Init(target);
    MysqlStatement::Init(target);
    
    //// Populate constants
    // Constants for connect flags
    NODE_DEFINE_CONSTANT(target, CLIENT_COMPRESS);
    NODE_DEFINE_CONSTANT(target, CLIENT_FOUND_ROWS);
    NODE_DEFINE_CONSTANT(target, CLIENT_IGNORE_SIGPIPE);
    NODE_DEFINE_CONSTANT(target, CLIENT_IGNORE_SPACE);
    NODE_DEFINE_CONSTANT(target, CLIENT_INTERACTIVE);
    // Not yet implemented
    // NODE_DEFINE_CONSTANT(target, CLIENT_LOCAL_FILES);
    NODE_DEFINE_CONSTANT(target, CLIENT_MULTI_RESULTS);
    NODE_DEFINE_CONSTANT(target, CLIENT_MULTI_STATEMENTS);
    NODE_DEFINE_CONSTANT(target, CLIENT_NO_SCHEMA);
    // Unused by MySQL
    // NODE_DEFINE_CONSTANT(target, CLIENT_ODBC);
    // This option should not be set by application programs;
    // it is set internally in the client library. Instead,
    // use setSslSync() before calling connect() or connectSync().
    // NODE_DEFINE_CONSTANT(target, CLIENT_SSL);
    // Known issue: mysql.CLIENT_REMEMBER_OPTIONS === -2147483648
    NODE_DEFINE_CONSTANT(target, CLIENT_REMEMBER_OPTIONS);

    // Constants for setOption
    NODE_DEFINE_CONSTANT(target, MYSQL_INIT_COMMAND);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_COMPRESS);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_CONNECT_TIMEOUT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_GUESS_CONNECTION);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_LOCAL_INFILE);
    // Unused, windows
    // NODE_DEFINE_CONSTANT(target, MYSQL_OPT_NAMED_PIPE);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_PROTOCOL);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_READ_TIMEOUT);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_RECONNECT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_SET_CLIENT_IP);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_SSL_VERIFY_SERVER_CERT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_EMBEDDED_CONNECTION);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_REMOTE_CONNECTION);
    // Unused by MySQL
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_RESULT);
    NODE_DEFINE_CONSTANT(target, MYSQL_OPT_WRITE_TIMEOUT);
    NODE_DEFINE_CONSTANT(target, MYSQL_READ_DEFAULT_FILE);
    NODE_DEFINE_CONSTANT(target, MYSQL_READ_DEFAULT_GROUP);
    NODE_DEFINE_CONSTANT(target, MYSQL_REPORT_DATA_TRUNCATION);
    NODE_DEFINE_CONSTANT(target, MYSQL_SECURE_AUTH);
    NODE_DEFINE_CONSTANT(target, MYSQL_SET_CHARSET_DIR);
    NODE_DEFINE_CONSTANT(target, MYSQL_SET_CHARSET_NAME);
    // Unused, windows
    // NODE_DEFINE_CONSTANT(target, MYSQL_SHARED_MEMORY_BASE_NAME);

    // Statements constants
    NODE_DEFINE_CONSTANT(target, STMT_ATTR_UPDATE_MAX_LENGTH);
    NODE_DEFINE_CONSTANT(target, STMT_ATTR_CURSOR_TYPE);
    NODE_DEFINE_CONSTANT(target, STMT_ATTR_PREFETCH_ROWS);
}

NODE_MODULE(mysql_bindings, InitMysqlLibmysqlclient)
