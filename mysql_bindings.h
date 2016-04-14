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
#define V8EXC(str,isolate) Exception::Error(v8::String::NewFromUtf8(isolate,str))
#define OBJUNWRAP ObjectWrap::Unwrap
#define V8STR(str,isolate) v8::String::NewFromUtf8(isolate,str)
#define V8STR2(str, len,isolate) String::NewFromUtf8(isolate,str)
#define MALLOC_ARRAY(name, type, size) type* name = (type*) malloc(sizeof(type) * size);

#define REQ_INT_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsInt32()) \
return Nan::ThrowTypeError("Argument " #I " must be an integer"); \
int32_t VAR = info[I]->Int32Value();

#define REQ_UINT_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsUint32()) \
return Nan::ThrowTypeError("Argument " #I " must be an integer"); \
uint32_t VAR = info[I]->Uint32Value();

#define REQ_NUMBER_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsNumber()) \
return Nan::ThrowTypeError("Argument " #I " must be a number"); \
double VAR = info[I]->NumberValue();

#define REQ_STR_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsString()) \
return Nan::ThrowTypeError("Argument " #I " must be a string"); \
String::Utf8Value VAR(info[I]->ToString());

#define OPTIONAL_STR_ARG(I, VAR) \
String::Utf8Value Utf8Value_##VAR(info[I]->ToString()); \
char *VAR; \
if (info[I]->IsString()) { \
    VAR = *(Utf8Value_##VAR); \
} else { \
    VAR = NULL; \
}

#define REQ_BOOL_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsBoolean()) \
return Nan::ThrowTypeError("Argument " #I " must be a boolean"); \
bool VAR = info[I]->BooleanValue();

#define REQ_ARRAY_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsArray()) \
return Nan::ThrowTypeError("Argument " #I " must be an array"); \
Local<Array> VAR = Local<Array>::Cast(info[I]);

#define REQ_EXT_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsExternal()) \
return Nan::ThrowTypeError("Argument " #I " must be an external"); \
Local<External> VAR = Local<External>::Cast(info[I]);

#define REQ_FUN_ARG(I, VAR) \
if (info.Length() <= (I) || !info[I]->IsFunction()) \
return Nan::ThrowTypeError("Argument " #I " must be a function"); \
Local<Function> VAR = Local<Function>::Cast(info[I]);

#define OPTIONAL_FUN_ARG(I, VAR) \
Handle<Value> VAR; \
if (info.Length() > (I) && info[I]->IsFunction()) {\
    VAR = info[I]; \
} else { \
    VAR = Null(); \
}

#define OPTIONAL_BUFFER_ARG(I, VAR) \
Handle<Value> VAR; \
if (info.Length() > (I) \
 && info[I]->IsObject() \
 && node::Buffer::HasInstance(info[I]) \
) { \
    VAR = info[I]->ToObject(); \
} else { \
    VAR = Null(); \
}

#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf("\nDEBUG_PRINTF: "); printf(__VA_ARGS__); printf("\n")
    #define DEBUG_ARGS() for (int i = 0; i < info.Length(); i++) { \
        printf( \
            "DEBUG_info[%d]: is_null(%d) is_bool(%d) is_number(%d) is_string(%d) is_object(%d) is_function(%d) is_external(%d)\n", \
            i, \
            info[i]->IsNull() ? 1 : 0, \
            info[i]->IsBoolean() ? 1 : 0, \
            info[i]->IsNumber() ? 1 : 0, \
            info[i]->IsString() ? 1 : 0, \
            info[i]->IsObject() ? 1 : 0, \
            info[i]->IsFunction() ? 1 : 0, \
            info[i]->IsExternal() ? 1 : 0 \
        ); \
    } \
    printf("DEBUG_ARGS END\n\n");
#else
    #define DEBUG_PRINTF(...) (void)0
    #define DEBUG_ARGS() (void)0
#endif

#endif  // SRC_MYSQL_BINDINGS_H_

