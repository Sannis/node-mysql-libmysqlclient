/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef SRC_MYSQL_BINDINGS_H_
#define SRC_MYSQL_BINDINGS_H_

#include <v8.h>
#include<node_buffer.h>
/*!
 * Use this header file to conditionally invoke different libev/libeio/libuv functions
 * depending on the node version that the module is being compiled for.
 */
#include "./node_addon_shim.h"

/*!
 * Useful macroses for utility operations
 * such as argument checking and C=+ <-> V8 type conversions
 */

// Only for fixing some cpplint.py errors:
// Lines should be <= 80 characters long
// [whitespace/line_length] [2]
// Lines should very rarely be longer than 100 characters
// [whitespace/line_length] [4]
#define V8EXC(str) Exception::Error(String::New(str))
#define THREXC(str) ThrowException(Exception::Error(String::New(str)))
#define THRTYPEEXC(str) ThrowException(Exception::TypeError(String::New(str)))
#define OBJUNWRAP ObjectWrap::Unwrap
#define V8STR(str) String::New(str)
#define V8STR2(str, len) String::New(str, len)
#define MALLOC_ARRAY(name, type, size) type* name = (type*) malloc(sizeof(type) * size);

#define REQ_INT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsInt32()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an integer"))); \
int32_t VAR = args[I]->Int32Value();

#define REQ_UINT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsUint32()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an integer"))); \
uint32_t VAR = args[I]->Uint32Value();

#define REQ_NUMBER_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsNumber()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an integer"))); \
double VAR = args[I]->NumberValue();

#define REQ_STR_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsString()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a string"))); \
String::Utf8Value VAR(args[I]->ToString());

#define OPTIONAL_STR_ARG(I, VAR) \
String::Utf8Value Utf8Value_##VAR(args[I]->ToString()); \
char *VAR; \
if (args[I]->IsString()) { \
    VAR = *(Utf8Value_##VAR); \
} else { \
    VAR = NULL; \
}

#define REQ_BOOL_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsBoolean()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a boolean"))); \
bool VAR = args[I]->BooleanValue();

#define REQ_ARRAY_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsArray()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an array"))); \
Local<Array> VAR = Local<Array>::Cast(args[I]);

#define REQ_EXT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsExternal()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an external"))); \
Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_FUN_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsFunction()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a function"))); \
Local<Function> VAR = Local<Function>::Cast(args[I]);

#define OPTIONAL_FUN_ARG(I, VAR) \
Handle<Value> VAR; \
if (args.Length() > (I) && args[I]->IsFunction()) {\
    VAR = args[I]; \
} else { \
    VAR = Null(); \
}

#define OPTIONAL_BUFFER_ARG(I, VAR) \
Handle<Value> VAR; \
if (args.Length() > (I) \
 && args[I]->IsObject() \
 && node::Buffer::HasInstance(args[I]) \
) { \
  VAR = args[I]->ToObject(); \
} else { \
  VAR = Null(); \
}

#ifdef DEBUG
    #define DEBUG_PRINTF(...) fprintf(stdout, __VA_ARGS__)
#else
    #define DEBUG_PRINTF(...) (void)0
#endif

/* Backport MakeCallback from Node v0.7.8 */
#if NODE_VERSION_AT_LEAST(0, 7, 8)
  // Nothing
#else
    namespace node {
        v8::Handle<v8::Value>
        MakeCallback(const v8::Handle<v8::Object> object,
                     const v8::Handle<v8::Function> callback,
                     int argc,
                     v8::Handle<v8::Value> argv[]);
    }  // namespace node
#endif

#endif  // SRC_MYSQL_BINDINGS_H_

