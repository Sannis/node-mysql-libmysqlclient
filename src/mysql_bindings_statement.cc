/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/**
 * Include headers
 *
 * @ignore
 */
#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

/**
 * Init V8 structures for MysqlStatement class
 *
 * @ignore
 */
Persistent<FunctionTemplate> MysqlStatement::constructor_template;

void MysqlStatement::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(MysqlStatement::New);

    // Constructor template
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->SetClassName(String::NewSymbol("MysqlStatement"));

    // Instance template
    Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Properties
    instance_template->SetAccessor(V8STR("paramCount"), ParamCountGetter);

    // Methods
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "affectedRowsSync",   MysqlStatement::AffectedRowsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "attrGetSync",        MysqlStatement::AttrGetSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "attrSetSync",        MysqlStatement::AttrSetSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "bindParamsSync",     MysqlStatement::BindParamsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "bindResultSync",     MysqlStatement::BindResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "closeSync",          MysqlStatement::CloseSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "dataSeekSync",       MysqlStatement::DataSeekSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "errnoSync",          MysqlStatement::ErrnoSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "errorSync",          MysqlStatement::ErrorSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "execute",            MysqlStatement::Execute);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "executeSync",        MysqlStatement::ExecuteSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchAll",           MysqlStatement::FetchAll);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchAllSync",       MysqlStatement::FetchAllSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchSync",          MysqlStatement::FetchSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetch",              MysqlStatement::Fetch);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fieldCountSync",     MysqlStatement::FieldCountSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "freeResultSync",     MysqlStatement::FreeResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "lastInsertIdSync",   MysqlStatement::LastInsertIdSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "nextResultSync",     MysqlStatement::NextResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "numRowsSync",        MysqlStatement::NumRowsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "prepareSync",        MysqlStatement::PrepareSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "resetSync",          MysqlStatement::ResetSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "resultMetadataSync", MysqlStatement::ResultMetadataSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "sendLongDataSync",   MysqlStatement::SendLongDataSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "storeResultSync",    MysqlStatement::StoreResultSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "storeResult",        MysqlStatement::StoreResult);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "sqlStateSync",       MysqlStatement::SqlStateSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setStringSize",      MysqlStatement::SqlStateSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlStatement"), constructor_template->GetFunction());
}

MysqlStatement::MysqlStatement(MYSQL_STMT *my_stmt): ObjectWrap() {
    this->_stmt = my_stmt;
    this->binds = NULL;
    this->param_count = 0;
    this->state = STMT_INITIALIZED;
}

MysqlStatement::~MysqlStatement() {
    if (this->_stmt) {
        if (this->state >= STMT_PREPARED) {
            FreeMysqlBinds(this->binds, this->param_count, true);
        }
        mysql_stmt_free_result(this->_stmt);
        mysql_stmt_close(this->_stmt);
    }
}

/**
 * Creates new MySQL statement object
 *
 * @constructor
 */
Handle<Value> MysqlStatement::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_stmt);
    MYSQL_STMT *my_stmt = static_cast<MYSQL_STMT*>(js_stmt->Value());
    MysqlStatement *binding_stmt = new MysqlStatement(my_stmt);
    binding_stmt->Wrap(args.Holder());

    return args.Holder();
}

/**
 * Returns the number of parameter for the given statement
 *
 * @getter
 * @return {Integer}
 */
Handle<Value> MysqlStatement::ParamCountGetter(Local<String> property,
                                                       const AccessorInfo &info) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(info.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    return scope.Close(Integer::New(stmt->param_count));
}

/**
 * Returns the total number of rows changed, deleted, or inserted by the last executed statement
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::AffectedRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt->_stmt);

    if (affected_rows == ((my_ulonglong)-1)) {
        return scope.Close(Integer::New(-1));
    }

    return scope.Close(Integer::New(affected_rows));
}

/**
 * Used to get the current value of a statement attribute
 *
 * @param {Integer} attr
 * @return {Boolean|Integer}
 */
Handle<Value> MysqlStatement::AttrGetSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    REQ_INT_ARG(0, attr_integer_key)
    enum_stmt_attr_type attr_key =
                        static_cast<enum_stmt_attr_type>(attr_integer_key);

    // TODO(Sannis): Possible error, see Integer::NewFromUnsigned, 32/64
    unsigned long attr_value; // NOLINT

    if (mysql_stmt_attr_get(stmt->_stmt, attr_key, &attr_value)) {
        return THREXC("This attribute isn't supported by libmysqlclient");
    }

    switch (attr_key) {
        case STMT_ATTR_UPDATE_MAX_LENGTH:
            return scope.Close(Boolean::New(attr_value));
            break;
        case STMT_ATTR_CURSOR_TYPE:
        case STMT_ATTR_PREFETCH_ROWS:
            return scope.Close(Integer::NewFromUnsigned(attr_value));
            break;
        default:
            return THREXC("This attribute isn't supported yet");
    }

    return THREXC("Control reaches end of non-void function :-D");
}

/**
 * Used to modify the behavior of a prepared statement
 *
 * @param {Integer} attr
 * @param {Boolean|Integer} value
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::AttrSetSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    REQ_INT_ARG(0, attr_integer_key)
    enum_stmt_attr_type attr_key =
                        static_cast<enum_stmt_attr_type>(attr_integer_key);
    int r = 1;

    switch (attr_key) {
        case STMT_ATTR_UPDATE_MAX_LENGTH:
            {
            REQ_BOOL_ARG(1, attr_bool_value);
            r = mysql_stmt_attr_set(stmt->_stmt, attr_key, &attr_bool_value);
            }
            break;
        case STMT_ATTR_CURSOR_TYPE:
        case STMT_ATTR_PREFETCH_ROWS:
            {
            REQ_UINT_ARG(1, attr_uint_value);
            r = mysql_stmt_attr_set(stmt->_stmt, attr_key, &attr_uint_value);
            }
            break;
        default:
            return THREXC("This attribute isn't supported yet");
    }

    if (r) {
        return THREXC("This attribute isn't supported by libmysqlclient");
    }

    return scope.Close(True());
}

/**
 * Binds variables to a prepared statement as parameters
 *
 * @param {Array} params
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::BindParamsSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    REQ_ARRAY_ARG(0, js_params);

    uint32_t i = 0;
    Local<Value> js_param;

    // For debug
    /*String::Utf8Value *str;
    for (i = 0; i < js_params->Length(); i++) {
        str = new String::Utf8Value(js_params->Get(Integer::New(i))->ToString());
        printf("%d: %s\n", i, **str);
    }*/

    if (js_params->Length() != stmt->param_count) {
        return THREXC("Array length doesn't match number of parameters in prepared statement"); // NOLINT
    }

    int *int_data;
    unsigned int *uint_data;
    double *double_data;
    unsigned long *str_length; // NOLINT
    char *str_data;
    time_t date_timet;
    struct tm date_timeinfo;
    MYSQL_TIME *date_data;

    for (i = 0; i < js_params->Length(); i++) {
        js_param = js_params->Get(i);

        if (js_param->IsUndefined()) {
            return THREXC("All arguments must be defined");
        }

        if (js_param->IsNull()) {
            int_data = new int;
            *int_data = 0;

            stmt->binds[i].buffer_type = MYSQL_TYPE_NULL;
            stmt->binds[i].buffer = int_data;
            // TODO(Sannis): Fix this error
            stmt->binds[i].is_null = 0;
        } else if (js_param->IsInt32()) {
            int_data = new int;
            *int_data = js_param->Int32Value();

            stmt->binds[i].buffer_type = MYSQL_TYPE_LONG;
            stmt->binds[i].buffer = int_data;
            stmt->binds[i].is_null = 0;
            stmt->binds[i].is_unsigned = false;
        } else if (js_param->IsBoolean()) {
            // I assume, booleans are usually stored as TINYINT(1)
            int_data = new int;
            *int_data = js_param->Int32Value();

            stmt->binds[i].buffer_type = MYSQL_TYPE_TINY;
            stmt->binds[i].buffer = int_data;
            stmt->binds[i].is_null = 0;
            stmt->binds[i].is_unsigned = false;
        } else if (js_param->IsUint32()) {
            uint_data = new unsigned int;
            *uint_data = js_param->Uint32Value();

            stmt->binds[i].buffer_type = MYSQL_TYPE_LONG;
            stmt->binds[i].buffer = uint_data;
            stmt->binds[i].is_null = 0;
            stmt->binds[i].is_unsigned = true;
        } else if (js_param->IsNumber()) {
            double_data = new double;
            *double_data = js_param->NumberValue();

            stmt->binds[i].buffer_type = MYSQL_TYPE_DOUBLE;
            stmt->binds[i].buffer = double_data;
            stmt->binds[i].is_null = 0;
        } else if (js_param->IsDate()) {
            date_data = new MYSQL_TIME;
            date_timet = static_cast<time_t>(js_param->NumberValue()/1000);
            if (!gmtime_r(&date_timet, &date_timeinfo)) {
                return THREXC("Error occured in gmtime_r()");
            }
            date_data->year = date_timeinfo.tm_year + 1900;
            date_data->month = date_timeinfo.tm_mon + 1;
            date_data->day = date_timeinfo.tm_mday;
            date_data->hour = date_timeinfo.tm_hour;
            date_data->minute = date_timeinfo.tm_min;
            date_data->second = date_timeinfo.tm_sec;

            stmt->binds[i].buffer_type = MYSQL_TYPE_DATETIME;
            stmt->binds[i].buffer = date_data;
            stmt->binds[i].is_null = 0;
        } else {  // js_param->IsString() and other
            // TODO(Sannis): Simplify this if possible
            str_data = strdup(**(new String::Utf8Value(js_param->ToString())));
            str_length = new unsigned long; // NOLINT
            *str_length = js_param->ToString()->Length();

            stmt->binds[i].buffer_type = MYSQL_TYPE_STRING;
            stmt->binds[i].buffer =  str_data;
            stmt->binds[i].is_null = 0;
            stmt->binds[i].length = str_length;
        }
    }

    if (mysql_stmt_bind_param(stmt->_stmt, stmt->binds)) {
      return scope.Close(False());
    }

    stmt->state = STMT_BINDED_PARAMS;

    return scope.Close(True());
}

/**
 * Bind resultset buffers
 */
Handle<Value> MysqlStatement::BindResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    MYSQL_RES *meta_result;
    MYSQL_FIELD *fields;
    MYSQL_BIND *bind;
    unsigned int i = 0, field_count = 0;
    uint32_t buf_length = 0;
    enum_field_types type;
    void* ptr = NULL;
    unsigned long *length;
    my_bool *is_null;
    my_bool *error;

    meta_result = mysql_stmt_result_metadata(stmt->_stmt);
    if (meta_result == NULL) {
        return scope.Close(False());
    }

    field_count = mysql_stmt_field_count(stmt->_stmt);
    fields = meta_result->fields;

    bind = new MYSQL_BIND[field_count];
    memset(bind, 0, sizeof(MYSQL_BIND) * field_count);

    length = new unsigned long[field_count];
    is_null = new my_bool[field_count];
    // TODO(estliberitas): this should be handled later
    error = new my_bool[field_count];

    while (i < field_count) {
        type = fields[i].type;

        if (
        type == MYSQL_TYPE_TINY ||                     // TINYINT
        type == MYSQL_TYPE_NULL) {                     // NULL
            buf_length = sizeof(signed char);
            ptr = new signed char;
        } else if (
        type == MYSQL_TYPE_SHORT ||                    // SMALLINT
        type == MYSQL_TYPE_SHORT) {                    // YEAR
            buf_length = sizeof(short int);
            ptr = new short int;
        } else if (
        type == MYSQL_TYPE_INT24 ||                    // MEDIUMINT
        type == MYSQL_TYPE_LONG) {                     // INT
            buf_length = sizeof(int);
            ptr = new int;
        } else if (type == MYSQL_TYPE_LONGLONG) {      // BIGINT
            buf_length = sizeof(long long);
            ptr = new long long;
        } else if (type == MYSQL_TYPE_FLOAT) {         // FLOAT
            buf_length = sizeof(float);
            ptr = new float;
        } else if (type == MYSQL_TYPE_DOUBLE) {        // DOUBLE, REAL
            buf_length = sizeof(double);
            ptr = new double;
        } else if (
        type == MYSQL_TYPE_DECIMAL ||                  // DECIMAL, NUMERIC
        type == MYSQL_TYPE_NEWDECIMAL ||               // NEWDECIMAL
        type == MYSQL_TYPE_STRING ||                   // CHAR, BINARY
        type == MYSQL_TYPE_VAR_STRING ||               // VARCHAR, VARBINARY
        type == MYSQL_TYPE_TINY_BLOB ||                // TINYBLOB, TINYTEXT
        type == MYSQL_TYPE_BLOB ||                     // BLOB, TEXT
        type == MYSQL_TYPE_MEDIUM_BLOB ||              // MEDIUMBLOB, MEDIUMTEXT
        type == MYSQL_TYPE_LONG_BLOB ||                // LONGBLOB, LONGTEXT
        type == MYSQL_TYPE_BIT ||                      // BIT
        type == MYSQL_TYPE_SET ||                      // SET
        type == MYSQL_TYPE_ENUM ||                     // ENUM
        type == MYSQL_TYPE_GEOMETRY) {                 // Spatial fields
            buf_length = sizeof(char) * fields[i].length;
            ptr = new char[fields[i].length];
        } else if (
        type == MYSQL_TYPE_TIME ||                     // TIME
        type == MYSQL_TYPE_DATE ||                     // DATE
        type == MYSQL_TYPE_NEWDATE ||                  // Newer const used in MySQL > 5.0
        type == MYSQL_TYPE_DATETIME ||                 // DATETIME
        type == MYSQL_TYPE_TIMESTAMP) {                // TIMESTAMP
            buf_length = sizeof(MYSQL_TIME);
            ptr = new MYSQL_TIME;
        } else {                                       // For others we bind char buffer
            buf_length = sizeof(char) * fields[i].length;
            ptr = new char[fields[i].length];
        }

        bind[i].is_null = &is_null[i];
        bind[i].length = &length[i];
        bind[i].error = &error[i];
        bind[i].buffer = ptr;
        bind[i].buffer_type = type;
        bind[i].buffer_length = buf_length;

        i++;
    }

    if (mysql_stmt_bind_result(stmt->_stmt, bind)) {
        FreeMysqlBinds(bind, field_count, false);
        return scope.Close(False());
    }

    stmt->result_binds = bind;
    stmt->state = STMT_BINDED_RESULT;

    return scope.Close(True());
}


/**
 * Closes a prepared statement
 *
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::CloseSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    if (mysql_stmt_close(stmt->_stmt)) {
        return scope.Close(False());
    }

    stmt->state = STMT_CLOSED;
    stmt->_stmt = NULL;

    return scope.Close(True());
}

/**
 * Seeks to an arbitrary row in statement result set
 *
 * @param {Integer} offset
 */
Handle<Value> MysqlStatement::DataSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_STORED;

    REQ_NUMBER_ARG(0, offset_double)
    REQ_UINT_ARG(0, offset_uint)

    if (offset_double < 0 || offset_uint >= mysql_stmt_num_rows(stmt->_stmt)) {
        return THREXC("Invalid row offset");
    }

    mysql_stmt_data_seek(stmt->_stmt, offset_uint);

    return Undefined();
}

/**
 * Returns the error code for the most recent statement call
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::ErrnoSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    uint32_t errno = mysql_stmt_errno(stmt->_stmt);

    return scope.Close(Integer::New(errno));
}

/**
 * Returns a string description for last statement error
 *
 * @return {String}
 */
Handle<Value> MysqlStatement::ErrorSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    const char *error = mysql_stmt_error(stmt->_stmt);

    return scope.Close(V8STR(error));
}

/**
 * After function for Execute() method
 */
void MysqlStatement::EIO_After_Execute(uv_work_t *req) {
    struct execute_request* execute_req = (struct execute_request *) (req->data);
    MysqlStatement* stmt = execute_req->stmt;

    Local<Value> argv[1];

    if (!execute_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else {
        stmt->state = STMT_EXECUTED;
        argv[0] = Local<Value>::New(Null());
    }

    node::MakeCallback(Context::GetCurrent()->Global(), execute_req->callback, 1, argv);
    execute_req->callback.Dispose();
    execute_req->stmt->Unref();

    delete execute_req;
    delete req;
}

/**
 * Thread function for Execute() method
 */
void MysqlStatement::EIO_Execute(uv_work_t *req) {
    struct execute_request* execute_req = (struct execute_request *) (req->data);
    MysqlStatement* stmt = execute_req->stmt;

    if (mysql_stmt_execute(stmt->_stmt)) {
        execute_req->ok = false;
    } else {
        execute_req->ok = true;
    }
}

Handle<Value> MysqlStatement::Execute(const Arguments& args) {
    HandleScope scope;

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    MYSQLSTMT_MUSTBE_PREPARED;

    execute_request* execute_req = new execute_request;

    execute_req->callback = Persistent<Function>::New(callback);
    execute_req->stmt = stmt;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = execute_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Execute, EIO_After_Execute);

    return Undefined();
}

/**
 * Executes a prepared query
 *
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::ExecuteSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    if (mysql_stmt_execute(stmt->_stmt)) {
        return scope.Close(False());
    }

    stmt->state = STMT_EXECUTED;
    return scope.Close(True());
}

void MysqlStatement::EIO_After_FetchAll(uv_work_t* req) {
    HandleScope scope;

    struct fetch_request* fetchAll_req = (struct fetch_request *) (req->data);
    MysqlStatement* stmt = fetchAll_req->stmt;

    MYSQL_FIELD* fields;
    int argc = 1, error = 0;
    uint32_t i = 0, j = 0;
    void *ptr = 0;
    my_ulonglong row_count = 0;
    Local<Array> js_result;
    Local<Value> argv[2];
    Local<Object> js_result_row;

    if (!fetchAll_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else if (fetchAll_req->empty_resultset) {
        argc = 2;
        argv[0] = argv[1] = Local<Value>::New(Null());
    } else {
        fields = fetchAll_req->meta->fields;

        row_count = mysql_stmt_num_rows(stmt->_stmt);
        js_result = Array::New(row_count);

        while (row_count && !error) {
            error = mysql_stmt_fetch(stmt->_stmt);
            // TODO(estliberitas): handle following case properly
            if (error == MYSQL_DATA_TRUNCATED) {
                error = 0;
            } else if (error == MYSQL_NO_DATA) {
                FreeMysqlBinds(stmt->result_binds, fetchAll_req->field_count, false);
                break;
            } else if (error) {
                break;
            }

            js_result_row = Object::New();

            DEBUG_PRINTF("Fetching row #%d\n", i);

            while (j < fetchAll_req->field_count) {
                ptr = stmt->result_binds[j].buffer;

                DEBUG_PRINTF("Is null %d\n", *(stmt->result_binds[j].is_null));

                Local<Value> js_field;
                if (*(stmt->result_binds[j].is_null)) {
                    js_field = Local<Value>::New(Null());
                } else {
                    js_field = GetFieldValue(ptr, *(stmt->result_binds[j].length), fields[j]);
                }

                js_result_row->Set(V8STR(fields[j].name), js_field);
                j++;
            }
            j = 0;

            js_result->Set(Integer::NewFromUnsigned(i), js_result_row);
            i++;
        }

        if (error && error != MYSQL_NO_DATA) {
            argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
        } else {
            argc = 2;
            argv[0] = Local<Value>::New(Null());
            argv[1] = js_result;
        }
    }

    if (fetchAll_req->meta != NULL && mysql_stmt_num_rows(stmt->_stmt)) {
        mysql_free_result(fetchAll_req->meta);
    }

    node::MakeCallback(Context::GetCurrent()->Global(), fetchAll_req->callback, argc, argv);
    fetchAll_req->callback.Dispose();
    fetchAll_req->stmt->Unref();

    delete fetchAll_req;
    delete req;
}

void MysqlStatement::EIO_FetchAll(uv_work_t *req) {
    struct fetch_request* fetchAll_req = (struct fetch_request *) (req->data);
    MysqlStatement* stmt = fetchAll_req->stmt;

    unsigned int field_count = 0;

    fetchAll_req->ok = true;

    MYSQL_RES* meta = mysql_stmt_result_metadata(stmt->_stmt);
    if (meta == NULL) {
        fetchAll_req->empty_resultset = true;
    } else {
        field_count = mysql_stmt_field_count(stmt->_stmt);

        fetchAll_req->empty_resultset = false;
        fetchAll_req->field_count = field_count;
        fetchAll_req->meta = meta;
    }
}

Handle<Value> MysqlStatement::FetchAll(const Arguments& args) {
    HandleScope scope;

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return THREXC("Resultset buffers not binded");
    }

    fetch_request *fetchAll_req = new fetch_request;

    fetchAll_req->callback = Persistent<Function>::New(callback);
    fetchAll_req->stmt = stmt;
    fetchAll_req->meta = NULL;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = fetchAll_req;
    uv_queue_work(uv_default_loop(), _req, EIO_FetchAll, EIO_After_FetchAll);

    return Undefined();
}

/**
 * MysqlStatement#fetchAllSync() -> Object
 *
 * Returns row data from statement result
 **/
Handle<Value> MysqlStatement::FetchAllSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return THREXC("Resultset buffers not binded");
    }

    MYSQL_RES* meta;
    MYSQL_FIELD* fields;
    int error = 0;
    uint32_t i = 0, j = 0;
    void *ptr = 0;
    unsigned int field_count;
    my_ulonglong row_count;

    // Get fields count for binding buffers
    field_count = mysql_stmt_field_count(stmt->_stmt);

    // Get meta data for binding buffers
    meta = mysql_stmt_result_metadata(stmt->_stmt);
    if (meta == NULL) {
        return scope.Close(Null());
    }

    fields = meta->fields;
    row_count = mysql_stmt_num_rows(stmt->_stmt);

    Local<Array> js_result = Array::New(row_count);
    Local<Object> js_result_row;

    while (row_count && !error) {
        error = mysql_stmt_fetch(stmt->_stmt);
        // TODO: handle following case properly
        if (error == MYSQL_DATA_TRUNCATED) {
            error = 0;
        } else if (error == MYSQL_NO_DATA) {
            FreeMysqlBinds(stmt->result_binds, field_count, false);
            break;
        } else if (error) {
            break;
        }

        js_result_row = Object::New();

        DEBUG_PRINTF("Fetching row #%d\n", i);

        while (j < field_count) {
            ptr = stmt->result_binds[j].buffer;

            DEBUG_PRINTF("Is null %d\n", *(stmt->result_binds[j].is_null));

            Local<Value> js_field;
            if (*(stmt->result_binds[j].is_null)) {
                js_field = Local<Value>::New(Null());
            } else {
                js_field = GetFieldValue(ptr, *(stmt->result_binds[j].length), fields[j]);
            }

            js_result_row->Set(V8STR(fields[j].name), js_field);
            j++;
        }
        j = 0;

        js_result->Set(Integer::NewFromUnsigned(i), js_result_row);
        i++;
    }

    if (meta != NULL) {
        mysql_free_result(meta);
    }

    if (error && error != MYSQL_NO_DATA) {
        return THREXC(mysql_stmt_error(stmt->_stmt));
    } else {
        return scope.Close(js_result);
    }
}

void MysqlStatement::EIO_After_Fetch(uv_work_t* req) {
    HandleScope scope;

    struct fetch_request* fetch_req = (struct fetch_request *) (req->data);
    MysqlStatement* stmt = fetch_req->stmt;

    MYSQL_FIELD* fields;
    int argc = 1;
    uint32_t i = 0;
    void *ptr = 0;
    Local<Value> argv[2];
    Local<Object> js_result_row;

    if (!fetch_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else if (fetch_req->empty_resultset) {
        argc = 2;
        argv[0] = argv[1] = Local<Value>::New(Null());
    } else {
        fields = fetch_req->meta->fields;
        js_result_row = Object::New();

        while (i < fetch_req->field_count) {
            ptr = stmt->result_binds[i].buffer;

            DEBUG_PRINTF("Is null %d\n", *(stmt->result_binds[i].is_null));

            Local<Value> js_field;
            if (*(stmt->result_binds[i].is_null)) {
                js_field = Local<Value>::New(Null());
            } else {
                js_field = GetFieldValue(ptr, *(stmt->result_binds[i].length), fields[i]);
            }

            js_result_row->Set(V8STR(fields[i].name), js_field);
            i++;
        }
        argc = 2;
        argv[0] = Local<Value>::New(Null());
        argv[1] = js_result_row;
    }

    if (fetch_req->meta != NULL) {
        mysql_free_result(fetch_req->meta);
    }

    node::MakeCallback(Context::GetCurrent()->Global(), fetch_req->callback, argc, argv);
    fetch_req->callback.Dispose();
    fetch_req->stmt->Unref();

    delete fetch_req;
    delete req;
}

void MysqlStatement::EIO_Fetch(uv_work_t *req) {
    struct fetch_request* fetch_req = (struct fetch_request *) (req->data);
    MysqlStatement* stmt = fetch_req->stmt;

    MYSQL_RES* meta;
    int error = 0;

    fetch_req->ok = true;
    fetch_req->empty_resultset = false;

    // Get meta data for binding buffers
    meta = mysql_stmt_result_metadata(stmt->_stmt);
    if (meta == NULL) {
        fetch_req->empty_resultset = true;
        return;
    } else {
        fetch_req->meta = meta;
    }

    if (!mysql_stmt_num_rows(stmt->_stmt)) {
        fetch_req->empty_resultset = true;
        return;
    } else {
        fetch_req->field_count = mysql_stmt_field_count(stmt->_stmt);
    }

    error = mysql_stmt_fetch(stmt->_stmt);
    if (error == MYSQL_DATA_TRUNCATED) {
        // TODO: handle MYSQL_DATA_TRUNCATED case properly
    } else if (error == MYSQL_NO_DATA) {
        FreeMysqlBinds(stmt->result_binds, fetch_req->field_count, false);

        fetch_req->empty_resultset = true;
    } else if (error) {
        fetch_req->ok = false;
    }
}

Handle<Value> MysqlStatement::Fetch(const Arguments& args) {
    HandleScope scope;

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return THREXC("Resultset buffers not binded");
    }

    fetch_request *fetch_req = new fetch_request;

    fetch_req->callback = Persistent<Function>::New(callback);
    fetch_req->stmt = stmt;
    fetch_req->meta = NULL;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = fetch_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Fetch, EIO_After_Fetch);

    return Undefined();
}

/**
 * Fetch row
 */
Handle<Value> MysqlStatement::FetchSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    if (stmt->state < STMT_BINDED_RESULT) {
        return THREXC("Resultset buffers not binded");
    }

    MYSQL_RES* meta;
    MYSQL_FIELD* fields;
    int error = 0;
    uint32_t i = 0;
    void *ptr = 0;
    unsigned int field_count;
    Local<Value> js_result_row;

    // Get fields count for binding buffers
    field_count = mysql_stmt_field_count(stmt->_stmt);

    // Get meta data for binding buffers
    meta = mysql_stmt_result_metadata(stmt->_stmt);
    if (meta == NULL) {
        return scope.Close(Null());
    }

    fields = meta->fields;

    if (!mysql_stmt_num_rows(stmt->_stmt)) {
        return scope.Close(Null());
    }

    error = mysql_stmt_fetch(stmt->_stmt);
    if (error == MYSQL_DATA_TRUNCATED) {
        // TODO: handle MYSQL_DATA_TRUNCATED case properly
        error = 0;
    }

    if (error == MYSQL_NO_DATA) {
        FreeMysqlBinds(stmt->result_binds, field_count, false);

        error = 0;
        js_result_row = Local<Value>::New(Null());
    } else if (!error) {
        js_result_row = Object::New();

        while (i < field_count) {
            ptr = stmt->result_binds[i].buffer;

            DEBUG_PRINTF("Is null %d\n", *(stmt->result_binds[i].is_null));

            Local<Value> js_field;
            if (*(stmt->result_binds[i].is_null)) {
                js_field = Local<Value>::New(Null());
            } else {
                js_field = GetFieldValue(ptr, *(stmt->result_binds[i].length), fields[i]);
            }

            js_result_row->ToObject()->Set(V8STR(fields[i].name), js_field);
            i++;
        }
    }

    if (meta != NULL) {
        mysql_free_result(meta);
    }

    if (error) {
        return THREXC(mysql_stmt_error(stmt->_stmt));
    } else {
        return scope.Close(js_result_row);
    }
}

/**
 * Returns the number of field in the given statement
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::FieldCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    return scope.Close(Integer::New(mysql_stmt_field_count(stmt->_stmt)));
}

/**
 * Frees stored result memory for the given statement handle
 *
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::FreeResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    return scope.Close(!mysql_stmt_free_result(stmt->_stmt) ? True() : False());
}

/**
 * Helper for freeing memory of
 */
void MysqlStatement::FreeMysqlBinds(MYSQL_BIND *binds, unsigned long size, bool params) {
    uint64_t i = 0;
    enum_field_types type;
    if (!params && size) {
        delete[] static_cast<my_bool *>(binds[0].error);
        delete[] static_cast<my_bool *>(binds[0].is_null);
        delete[] static_cast<unsigned long *>(binds[0].length);
    }

    while (i < size) {
        type = binds[i].buffer_type;

        if (
        type == MYSQL_TYPE_TINY ||                     // TINYINT
        type == MYSQL_TYPE_NULL) {                     // NULL
            delete static_cast<signed char *>(binds[i].buffer);
        } else if (
        type == MYSQL_TYPE_SHORT ||                    // SMALLINT
        type == MYSQL_TYPE_SHORT) {                    // YEAR
            if (binds[i].is_unsigned) {
                delete static_cast<unsigned short *>(binds[i].buffer);
            } else {
                delete static_cast<short *>(binds[i].buffer);
            }
        } else if (
        type == MYSQL_TYPE_INT24 ||                    // MEDIUMINT
        type == MYSQL_TYPE_LONG) {                     // INT
            if (binds[i].is_unsigned) {
                delete static_cast<unsigned int *>(binds[i].buffer);
            } else {
                delete static_cast<int *>(binds[i].buffer);
            }
        } else if (type == MYSQL_TYPE_LONGLONG) {      // BIGINT
            if (binds[i].is_unsigned) {
                delete static_cast<unsigned long long *>(binds[i].buffer);
            } else {
                delete static_cast<long long *>(binds[i].buffer);
            }
        } else if (type == MYSQL_TYPE_FLOAT) {         // FLOAT
            delete static_cast<float *>(binds[i].buffer);
        } else if (type == MYSQL_TYPE_DOUBLE) {        // DOUBLE, REAL
            delete static_cast<double *>(binds[i].buffer);
        } else if (
        type == MYSQL_TYPE_DECIMAL ||                  // DECIMAL, NUMERIC
        type == MYSQL_TYPE_NEWDECIMAL ||               // NEWDECIMAL
        type == MYSQL_TYPE_STRING ||                   // CHAR, BINARY
        type == MYSQL_TYPE_VAR_STRING ||               // VARCHAR, VARBINARY
        type == MYSQL_TYPE_TINY_BLOB ||                // TINYBLOB, TINYTEXT
        type == MYSQL_TYPE_BLOB ||                     // BLOB, TEXT
        type == MYSQL_TYPE_MEDIUM_BLOB ||              // MEDIUMBLOB, MEDIUMTEXT
        type == MYSQL_TYPE_LONG_BLOB ||                // LONGBLOB, LONGTEXT
        type == MYSQL_TYPE_BIT ||                      // BIT
        type == MYSQL_TYPE_SET ||                      // SET
        type == MYSQL_TYPE_ENUM ||                     // ENUM
        type == MYSQL_TYPE_GEOMETRY) {                 // Spatial fields
            delete[] static_cast<char *>(binds[i].buffer);
            if (params) {
                delete static_cast<unsigned long *>(binds[i].length); // NOLINT
            }
        } else if (
        type == MYSQL_TYPE_TIME ||                     // TIME
        type == MYSQL_TYPE_DATE ||                     // DATE
        type == MYSQL_TYPE_NEWDATE ||                  // Newer const used in MySQL > 5.0
        type == MYSQL_TYPE_DATETIME ||                 // DATETIME
        type == MYSQL_TYPE_TIMESTAMP) {                // TIMESTAMP
            delete static_cast<MYSQL_TIME *>(binds[i].buffer);
        } else {                                       // For others we bind char buffer
            delete[] static_cast<char *>(binds[i].buffer);
        }
        i++;
    }
    delete[] binds;
}

/**
 * Helper for FetchAll(), FetchAllSync() methods. Converts raw data to JS type.
 */
Local<Value> MysqlStatement::GetFieldValue(void* ptr, unsigned long& length, MYSQL_FIELD& field) {
    unsigned int type = field.type;
    if (type == MYSQL_TYPE_TINY) {             // TINYINT
        int32_t val = *((signed char *) ptr);
        // handle as boolean
        if (length == 1) {
            DEBUG_PRINTF("TINYINT(1) %d\n", val);
            return Local<Value>::New(Boolean::New(val));
        // handle as integer
        } else {
            DEBUG_PRINTF("TINYINT(>1) %d\n", val);
            return Integer::New(val);
        }
    } else if (
    type == MYSQL_TYPE_SHORT ||                // SMALLINT
    type == MYSQL_TYPE_SHORT) {                // YEAR
        DEBUG_PRINTF("SMALLINT %d\n", *((short int*) ptr));
        if (field.flags & UNSIGNED_FLAG) {
            return Integer::NewFromUnsigned((uint32_t) *((unsigned short int *) ptr));
        } else {
            return Integer::New((int32_t) *((short int *) ptr));
        }
    } else if (
    type == MYSQL_TYPE_INT24 ||                // MEDIUMINT
    type == MYSQL_TYPE_LONG) {                 // INT
        DEBUG_PRINTF("INT %d\n", *((int *) ptr));
        if (field.flags & UNSIGNED_FLAG) {
            return Integer::NewFromUnsigned((uint32_t) *((unsigned int *) ptr));
        } else {
            return Integer::New((int32_t) *((int *) ptr));
        }
    } else if (type == MYSQL_TYPE_LONGLONG) {  // BIGINT
        DEBUG_PRINTF("BIGINT %lld\n", *((long long int*) ptr));
        return Number::New((double) *((long long int *) ptr));
    } else if (type == MYSQL_TYPE_FLOAT) {     // FLOAT
        DEBUG_PRINTF("FLOAT %f\n", *((float *) ptr));
        return Number::New(*((float *) ptr));
    } else if (type == MYSQL_TYPE_DOUBLE) {    // DOUBLE, REAL
        DEBUG_PRINTF("DOUBLE %f\n", *((double *) ptr));
        return Number::New(*((double *) ptr));
    } else if (
    type == MYSQL_TYPE_DECIMAL ||              // DECIMAL, NUMERIC
    type == MYSQL_TYPE_NEWDECIMAL ||           // NEWDECIMAL
    type == MYSQL_TYPE_STRING ||               // CHAR, BINARY
    type == MYSQL_TYPE_VAR_STRING ||           // VARCHAR, VARBINARY
    type == MYSQL_TYPE_TINY_BLOB ||            // TINYBLOB, TINYTEXT
    type == MYSQL_TYPE_BLOB ||                 // BLOB, TEXT
    type == MYSQL_TYPE_MEDIUM_BLOB ||          // MEDIUMBLOB, MEDIUMTEXT
    type == MYSQL_TYPE_LONG_BLOB ||            // LONGBLOB, LONGTEXT
    type == MYSQL_TYPE_BIT ||                  // BIT
    type == MYSQL_TYPE_ENUM ||                 // ENUM
    type == MYSQL_TYPE_GEOMETRY) {             // Spatial fields
        char *data = (char *) ptr;
        // create buffer
        if (field.flags & BINARY_FLAG) {
            DEBUG_PRINTF("Blob, length: (%lu)\n", length);

            // taken from: http://sambro.is-super-awesome.com/2011/03/03/creating-a-proper-buffer-in-a-node-c-addon/
            node::Buffer *slowBuffer = node::Buffer::New(length);
            memcpy(node::Buffer::Data(slowBuffer), data, length);
            v8::Local<v8::Object> globalObj = v8::Context::GetCurrent()->Global();
            v8::Local<v8::Function> bufferConstructor = v8::Local<v8::Function>::Cast(globalObj->Get(v8::String::New("Buffer")));
            v8::Handle<v8::Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(length), v8::Integer::New(0) };
            return bufferConstructor->NewInstance(3, constructorArgs);
        // create string
        } else {
            DEBUG_PRINTF("String, length: %lu/%lu\n", length, field.length);
            return V8STR2(data, length);
        }
    } else if (
    type == MYSQL_TYPE_TIME ||                 // TIME
    type == MYSQL_TYPE_DATE ||                 // DATE
    type == MYSQL_TYPE_NEWDATE ||              // Newer const used in MySQL > 5.0
    type == MYSQL_TYPE_DATETIME ||             // DATETIME
    type == MYSQL_TYPE_TIMESTAMP) {            // TIMESTAMP
        MYSQL_TIME ts = *((MYSQL_TIME *) ptr);

        DEBUG_PRINTF(
            "Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            ts.year, ts.month, ts.day,
            ts.hour, ts.minute, ts.second);

        char time_string[24];
        sprintf(
            time_string,
            "%04d-%02d-%02d %02d:%02d:%02d GMT",
            ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);

        // First step is to get a handle to the global object:
        Local<v8::Object> globalObj = Context::GetCurrent()->Global();

        // Now we need to grab the Date constructor function:
        Local<v8::Function> dateConstructor = Local<Function>::Cast(globalObj->Get(V8STR("Date")));

        // Great. We can use this constructor function to allocate new Dates:
        const int argc = 1;
        Local<Value> argv[argc] = { V8STR(time_string) };

        // Now we have our constructor, and our constructor args. Let's create the Date:
        return dateConstructor->NewInstance(argc, argv);
    } else if (type == MYSQL_TYPE_SET) {       // SET
        // TODO(Sannis): Maybe memory leaks here
        char *pch, *last, *field_value = (char *) ptr;
        int k = 0;
        Local<Array> js_field_array = Array::New();

        pch = strtok_r(field_value, ",", &last);
        while (pch != NULL) {
            js_field_array->Set(Integer::New(k), V8STR(pch));
            pch = strtok_r(NULL, ",", &last);
            k++;
        }

        return js_field_array;
    } else {
        return V8STR2((char *) ptr, length);
    }
}

/**
 * Get the ID generated from the previous INSERT operation
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::LastInsertIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    return scope.Close(Integer::New(mysql_stmt_insert_id(stmt->_stmt)));
}

/**
 * Return the number of rows in statements result set
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::NextResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    return scope.Close(Integer::New(mysql_stmt_next_result(stmt->_stmt)));
}

/**
 * Return the number of rows in statements result set
 *
 * @return {Integer}
 */
Handle<Value> MysqlStatement::NumRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_STORED;  // TODO(Sannis): Or all result already fetched!

    return scope.Close(Integer::New(mysql_stmt_num_rows(stmt->_stmt)));
}

/**
 * Prepare statement by given query
 *
 * @param {String} query
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::PrepareSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    REQ_STR_ARG(0, query)

    // TODO(Sannis): Smth else? close/reset

    unsigned long int query_len = args[0]->ToString()->Utf8Length();

    if (mysql_stmt_prepare(stmt->_stmt, *query, query_len)) {
        return scope.Close(False());
    }

    if (stmt->binds) {
        delete[] stmt->binds;
    }

    stmt->param_count = mysql_stmt_param_count(stmt->_stmt);

    if (stmt->param_count > 0) {
        stmt->binds = new MYSQL_BIND[stmt->param_count];
        memset(stmt->binds, 0, stmt->param_count*sizeof(MYSQL_BIND));

        // TODO(Sannis): Smth else?
    }

    stmt->state = STMT_PREPARED;

    return scope.Close(True());
}

/**
 * Resets a prepared statement
 *
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::ResetSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    if (mysql_stmt_reset(stmt->_stmt)) {
        return scope.Close(False());
    }

    stmt->state = STMT_INITIALIZED;
    return scope.Close(True());
}

/**
 * Returns result set metadata from a prepared statement
 *
 * @return {MysqlResult}
 */
Handle<Value> MysqlStatement::ResultMetadataSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    MYSQL_RES *my_result = mysql_stmt_result_metadata(stmt->_stmt);

    if (!my_result) {
        return scope.Close(False());
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(stmt->_stmt->mysql); // MySQL connection handle
    argv[1] = External::New(my_result);
    argv[2] = Integer::New(mysql_stmt_field_count(stmt->_stmt));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * Send parameter data to the server in blocks (or "chunks")
 *
 * @param {Integer} parameter number, beginning with 0
 * @param {String} data
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::SendLongDataSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    REQ_INT_ARG(0, parameter_number);
    REQ_STR_ARG(1, data);

    if (mysql_stmt_send_long_data(stmt->_stmt,
                                  parameter_number, *data, data.length())) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Returns SQLSTATE error from previous statement operation
 *
 * @return {String}
 */
Handle<Value> MysqlStatement::SqlStateSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    return scope.Close(V8STR(mysql_stmt_sqlstate(stmt->_stmt)));
}

/**
 * After function for StoreResult() method
 */
void MysqlStatement::EIO_After_StoreResult(uv_work_t *req) {
    struct store_result_request* store_req = (struct store_result_request *) (req->data);
    MysqlStatement* stmt = store_req->stmt;

    Local<Value> argv[1];

    if (!store_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else {
        stmt->state = STMT_STORED_RESULT;
        argv[0] = Local<Value>::New(Null());
    }

    node::MakeCallback(Context::GetCurrent()->Global(), store_req->callback, 1, argv);
    store_req->callback.Dispose();
    store_req->stmt->Unref();

    delete store_req;
    delete req;
}

/**
 * Thread function for StoreResult() method
 */
void MysqlStatement::EIO_StoreResult(uv_work_t *req) {
    struct store_result_request* store_req = (struct store_result_request *) (req->data);
    MysqlStatement* stmt = store_req->stmt;

    store_req->ok = !mysql_stmt_store_result(stmt->_stmt);
}

Handle<Value> MysqlStatement::StoreResult(const Arguments& args) {
    HandleScope scope;

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    MYSQLSTMT_MUSTBE_EXECUTED;

    store_result_request* store_req = new store_result_request;

    store_req->callback = Persistent<Function>::New(callback);
    store_req->stmt = stmt;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = store_req;
    uv_queue_work(uv_default_loop(), _req, EIO_StoreResult, EIO_After_StoreResult);

    return Undefined();
}

/**
 * Transfers a result set from a prepared statement
 *
 * @return {Boolean}
 */
Handle<Value> MysqlStatement::StoreResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    if (mysql_stmt_store_result(stmt->_stmt) != 0) {
        return scope.Close(False());
    }

    stmt->state = STMT_STORED_RESULT;

    return scope.Close(True());
}