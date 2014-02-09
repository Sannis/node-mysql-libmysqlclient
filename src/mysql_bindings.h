/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

#ifndef SRC_MYSQL_BINDINGS_H_
#define SRC_MYSQL_BINDINGS_H_

#include <v8.h>
#include <node_buffer.h>

#include "nan.h"

using namespace v8; // NOLINT

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
#define OBJUNWRAP ObjectWrap::Unwrap
#define V8STR(str) String::New(str)
#define V8STR2(str, len) String::New(str, len)
#define MALLOC_ARRAY(name, type, size) type* name = (type*) malloc(sizeof(type) * size);

#define REQ_INT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsInt32()) \
return NanThrowTypeError("Argument " #I " must be an integer"); \
int32_t VAR = args[I]->Int32Value();

#define REQ_UINT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsUint32()) \
return NanThrowTypeError("Argument " #I " must be an integer"); \
uint32_t VAR = args[I]->Uint32Value();

#define REQ_NUMBER_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsNumber()) \
return NanThrowTypeError("Argument " #I " must be a number"); \
double VAR = args[I]->NumberValue();

#define REQ_STR_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsString()) \
return NanThrowTypeError("Argument " #I " must be a string"); \
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
return NanThrowTypeError("Argument " #I " must be a boolean"); \
bool VAR = args[I]->BooleanValue();

#define REQ_ARRAY_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsArray()) \
return NanThrowTypeError("Argument " #I " must be an array"); \
Local<Array> VAR = Local<Array>::Cast(args[I]);

#define REQ_EXT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsExternal()) \
return NanThrowTypeError("Argument " #I " must be an external"); \
Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_FUN_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsFunction()) \
return NanThrowTypeError("Argument " #I " must be a function"); \
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
    #define DEBUG_PRINTF(...) printf("\nDEBUG_PRINTF: "); printf(__VA_ARGS__); printf("\n")
    #define DEBUG_ARGS() for (int i = 0; i < args.Length(); i++) { \
        printf( \
            "DEBUG_ARGS[%d]: is_null(%d) is_bool(%d) is_number(%d) is_string(%d) is_object(%d) is_function(%d) is_external(%d)\n", \
            i, \
            args[i]->IsNull() ? 1 : 0, \
            args[i]->IsBoolean() ? 1 : 0, \
            args[i]->IsNumber() ? 1 : 0, \
            args[i]->IsString() ? 1 : 0, \
            args[i]->IsObject() ? 1 : 0, \
            args[i]->IsFunction() ? 1 : 0, \
            args[i]->IsExternal() ? 1 : 0 \
        ); \
    } \
    printf("DEBUG_ARGS END\n\n");
#else
    #define DEBUG_PRINTF(...) (void)0
    #define DEBUG_ARGS() (void)0
#endif

#endif  // SRC_MYSQL_BINDINGS_H_

