/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_statement.h"

Persistent<FunctionTemplate> MysqlSyncConn::MysqlSyncStmt::constructor_template;

void MysqlSyncConn::MysqlSyncStmt::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlSyncStmt"));

    ADD_PROTOTYPE_METHOD(prepare, Prepare);
}

MysqlSyncConn::MysqlSyncStmt::MysqlSyncStmt(): EventEmitter() {
    _stmt = NULL;
}

MysqlSyncConn::MysqlSyncStmt::~MysqlSyncStmt() {
    if (_stmt) {
        mysql_stmt_close(_stmt);
    }
}

Handle<Value> MysqlSyncConn::MysqlSyncStmt::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_stmt);
    MYSQL_STMT *stmt = static_cast<MYSQL_STMT*>(js_stmt->Value());
    MysqlSyncStmt *my_stmt = new MysqlSyncStmt(stmt);
    my_stmt->Wrap(args.This());

    return args.This();
}

Handle<Value> MysqlSyncConn::MysqlSyncStmt::Prepare(const Arguments& args) {
    HandleScope scope;

    MysqlSyncStmt *stmt = OBJUNWRAP<MysqlSyncStmt>(args.This());

    if (args.Length() == 0 || !args[0]->IsString()) {
        return THREXC("First arg of stmt.prepare() must be a string");
    }

    String::Utf8Value query(args[0]);

    int query_len = args[0]->ToString()->Utf8Length();

    if (mysql_stmt_prepare(stmt->_stmt, *query, query_len)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

