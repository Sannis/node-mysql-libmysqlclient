/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_statement.h"

Persistent<FunctionTemplate> MysqlStatement::constructor_template;

void MysqlStatement::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    // Constructor
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlStatement"));

    Local<ObjectTemplate> instance_template =
        constructor_template->InstanceTemplate();

    // Methods
    ADD_PROTOTYPE_METHOD(statement, prepareSync, PrepareSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlStatement"),
                constructor_template->GetFunction());
}

MysqlStatement::MysqlStatement(): EventEmitter() {
    _stmt = NULL;
}

MysqlStatement::~MysqlStatement() {
    if (_stmt) {
        mysql_stmt_close(_stmt);
    }
}

Handle<Value> MysqlStatement::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_stmt);
    MYSQL_STMT *stmt = static_cast<MYSQL_STMT*>(js_stmt->Value());
    MysqlStatement *my_stmt = new MysqlStatement(stmt);
    my_stmt->Wrap(args.This());

    return args.This();
}

Handle<Value> MysqlStatement::PrepareSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    REQ_STR_ARG(0, query)

    int query_len = args[0]->ToString()->Utf8Length();

    if (mysql_stmt_prepare(stmt->_stmt, *query, query_len)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

