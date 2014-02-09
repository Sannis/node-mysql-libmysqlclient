/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/*!
 * Include headers
 */
#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

/*!
 * Init V8 structures for MysqlResult class
 */
Persistent<FunctionTemplate> MysqlStatement::constructor_template;

void MysqlStatement::Init(Handle<Object> target) {
    NanScope();

    // Constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
    tpl->SetClassName(NanSymbol("MysqlStatement"));

    // Instance template
    Local<ObjectTemplate> instance_template = tpl->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Instance properties
    instance_template->SetAccessor(V8STR("paramCount"), ParamCountGetter);

    // Prototype methods
    NODE_SET_PROTOTYPE_METHOD(tpl, "affectedRowsSync",   AffectedRowsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "attrGetSync",        AttrGetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "attrSetSync",        AttrSetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "bindParamsSync",     BindParamsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "bindResultSync",     BindResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "closeSync",          CloseSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dataSeekSync",       DataSeekSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "errnoSync",          ErrnoSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "errorSync",          ErrorSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "execute",            Execute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "executeSync",        ExecuteSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetchAll",           FetchAll);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetchAllSync",       FetchAllSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetchSync",          FetchSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetch",              Fetch);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fieldCountSync",     FieldCountSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "freeResultSync",     FreeResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "lastInsertIdSync",   LastInsertIdSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "nextResultSync",     NextResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "numRowsSync",        NumRowsSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "prepareSync",        PrepareSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "resetSync",          ResetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "resultMetadataSync", ResultMetadataSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "sendLongDataSync",   SendLongDataSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "storeResultSync",    StoreResultSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "storeResult",        StoreResult);
    NODE_SET_PROTOTYPE_METHOD(tpl, "sqlStateSync",       SqlStateSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setStringSize",      SqlStateSync);

    // Make it visible in JavaScript land
    target->Set(NanSymbol("MysqlStatement"), tpl->GetFunction());
}

Local<Object> MysqlStatement::NewInstance(MYSQL_STMT *my_statement) {
    NanScope();

    Local<FunctionTemplate> tpl = NanPersistentToLocal(constructor_template);

    const int argc = 1;
    Local<Value> argv[argc];
    argv[0] = External::New(my_statement);

    Local<Object> instance = tpl->GetFunction()->NewInstance(argc, argv);

    return scope.Close(instance);
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
 * new MysqlStatement()
 *
 * Creates new MysqlStatement object
 **/
NAN_METHOD(MysqlStatement::New) {
    NanScope();

    REQ_EXT_ARG(0, js_stmt);
    MYSQL_STMT *my_stmt = static_cast<MYSQL_STMT*>(js_stmt->Value());
    MysqlStatement *binding_stmt = new MysqlStatement(my_stmt);
    binding_stmt->Wrap(args.Holder());

    NanReturnValue(args.Holder());
}

/** read-only
 * MysqlStatement#paramCount -> Integer
 *
 * Gets the number of parameter for the given statement
 **/
NAN_GETTER(MysqlStatement::ParamCountGetter) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    NanReturnValue(Integer::New(stmt->param_count));
}

/**
 * MysqlStatement#affectedRowsSync() -> Integer
 *
 * Gets number of affected rows in previous operation
 **/
NAN_METHOD(MysqlStatement::AffectedRowsSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt->_stmt);

    if (affected_rows == ((my_ulonglong)-1)) {
        NanReturnValue(Integer::New(-1));
    }

    NanReturnValue(Integer::New(affected_rows));
}

/**
 * MysqlStatement#attrGetSync(attr) -> Integer | Boolean
 *
 * Used to get the current value of a statement attribute
 **/
NAN_METHOD(MysqlStatement::AttrGetSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    REQ_INT_ARG(0, attr_integer_key)
    enum_stmt_attr_type attr_key =
                        static_cast<enum_stmt_attr_type>(attr_integer_key);

    // TODO(Sannis): Possible error, see Integer::NewFromUnsigned, 32/64
    unsigned long attr_value; // NOLINT

    if (mysql_stmt_attr_get(stmt->_stmt, attr_key, &attr_value)) {
        return NanThrowError("This attribute isn't supported by libmysqlclient");
    }

    switch (attr_key) {
        case STMT_ATTR_UPDATE_MAX_LENGTH:
            NanReturnValue(Boolean::New(attr_value));
            break;
        case STMT_ATTR_CURSOR_TYPE:
        case STMT_ATTR_PREFETCH_ROWS:
            NanReturnValue(Integer::NewFromUnsigned(attr_value));
            break;
        default:
            return NanThrowError("This attribute isn't supported yet");
    }

    return NanThrowError("Control reaches end of non-void function :-D");
}

/**
 * MysqlStatement#attrSetSync(attr, value) -> Boolean
 * - attr(Integer)
 * - value(Integer | Boolean)
 *
 * Used to modify the behavior of a prepared statement
 **/
NAN_METHOD(MysqlStatement::AttrSetSync) {
    NanScope();

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
            return NanThrowError("This attribute isn't supported yet");
    }

    if (r) {
        return NanThrowError("This attribute isn't supported by libmysqlclient");
    }

    NanReturnValue(True());
}

/**
 * MysqlStatement#bindParamsSync(params) -> Boolean
 * - params(Array)
 *
 * Binds variables to a prepared statement as parameters
 **/
NAN_METHOD(MysqlStatement::BindParamsSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;
    MYSQLSTMT_MUSTBE_PREPARED;

    REQ_ARRAY_ARG(0, js_params);

    uint32_t i = 0;
    Local<Value> js_param;

    if (js_params->Length() != stmt->param_count) {
        return NanThrowError("Array length doesn't match number of parameters in prepared statement"); // NOLINT
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
            return NanThrowError("All arguments must be defined");
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
                return NanThrowError("Error occured in gmtime_r()");
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
      NanReturnValue(False());
    }

    stmt->state = STMT_BINDED_PARAMS;

    NanReturnValue(True());
}

/**
 * MysqlStatement#bindResultSync() -> Boolean
 *
 * Bind result set buffers
 **/
NAN_METHOD(MysqlStatement::BindResultSync) {
    NanScope();

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
        NanReturnValue(False());
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
        NanReturnValue(False());
    }

    stmt->result_binds = bind;
    stmt->state = STMT_BINDED_RESULT;

    NanReturnValue(True());
}


/*! todo: finish
 * Closes a prepared statement
 *
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::CloseSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    if (mysql_stmt_close(stmt->_stmt)) {
        NanReturnValue(False());
    }

    stmt->state = STMT_CLOSED;
    stmt->_stmt = NULL;

    NanReturnValue(True());
}

/*! todo: finish
 * Seeks to an arbitrary row in statement result set
 *
 * @param {Integer} offset
 */
NAN_METHOD(MysqlStatement::DataSeekSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_STORED;

    REQ_NUMBER_ARG(0, offset_double)
    REQ_UINT_ARG(0, offset_uint)

    if (offset_double < 0 || offset_uint >= mysql_stmt_num_rows(stmt->_stmt)) {
        return NanThrowError("Invalid row offset");
    }

    mysql_stmt_data_seek(stmt->_stmt, offset_uint);

    NanReturnUndefined();
}

/*! todo: finish
 * Returns the error code for the most recent statement call
 *
 * @return {Integer}
 */
NAN_METHOD(MysqlStatement::ErrnoSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    NanReturnValue(Integer::New(mysql_stmt_errno(stmt->_stmt)));
}

/*! todo: finish
 * Returns a string description for last statement error
 *
 * @return {String}
 */
NAN_METHOD(MysqlStatement::ErrorSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    const char *error = mysql_stmt_error(stmt->_stmt);

    NanReturnValue(V8STR(error));
}

/*! todo: finish
 * After function for Execute() method
 */
void MysqlStatement::EIO_After_Execute(uv_work_t *req) {
    struct execute_request* execute_req = (struct execute_request *) (req->data);
    MysqlStatement* stmt = execute_req->stmt;

    const int argc = 1;
    Local<Value> argv[argc];

    if (!execute_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else {
        stmt->state = STMT_EXECUTED;
        argv[0] = NanNewLocal(Null());
    }

    execute_req->nan_callback->Call(argc, argv);
    delete execute_req->nan_callback;

    execute_req->stmt->Unref();

    delete execute_req;
    delete req;
}

/*! todo: finish
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

NAN_METHOD(MysqlStatement::Execute) {
    NanScope();

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    MYSQLSTMT_MUSTBE_PREPARED;

    execute_request* execute_req = new execute_request;

    execute_req->nan_callback = new NanCallback(callback.As<Function>());

    execute_req->stmt = stmt;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = execute_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Execute, (uv_after_work_cb)EIO_After_Execute);

    NanReturnUndefined();
}

/*! todo: finish
 * Executes a prepared query
 *
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::ExecuteSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    if (mysql_stmt_execute(stmt->_stmt)) {
        NanReturnValue(False());
    }

    stmt->state = STMT_EXECUTED;
    NanReturnValue(True());
}

void MysqlStatement::EIO_After_FetchAll(uv_work_t* req) {
    NanScope();

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
        argv[0] = argv[1] = NanNewLocal(Null());
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

            while (j < fetchAll_req->field_count) {
                ptr = stmt->result_binds[j].buffer;

                Local<Value> js_field;
                if (*(stmt->result_binds[j].is_null)) {
                    js_field = NanNewLocal(Null());
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
            argv[0] = NanNewLocal(Null());
            argv[1] = js_result;
        }
    }

    if (fetchAll_req->meta != NULL && mysql_stmt_num_rows(stmt->_stmt)) {
        mysql_free_result(fetchAll_req->meta);
    }

    fetchAll_req->nan_callback->Call(argc, argv);
    delete fetchAll_req->nan_callback;

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

NAN_METHOD(MysqlStatement::FetchAll) {
    NanScope();

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return NanThrowError("Resultset buffers not binded");
    }

    fetch_request *fetchAll_req = new fetch_request;

    fetchAll_req->nan_callback = new NanCallback(callback.As<Function>());

    fetchAll_req->stmt = stmt;
    fetchAll_req->meta = NULL;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = fetchAll_req;
    uv_queue_work(uv_default_loop(), _req, EIO_FetchAll, (uv_after_work_cb)EIO_After_FetchAll);

    NanReturnUndefined();
}

/*! todo: finish
 * MysqlStatement#fetchAllSync() -> Object
 *
 * Returns row data from statement result
 **/
NAN_METHOD(MysqlStatement::FetchAllSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return NanThrowError("Resultset buffers not binded");
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
        NanReturnValue(Null());
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

        while (j < field_count) {
            ptr = stmt->result_binds[j].buffer;

            Local<Value> js_field;
            if (*(stmt->result_binds[j].is_null)) {
                js_field = NanNewLocal(Null());
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
        return NanThrowError(mysql_stmt_error(stmt->_stmt));
    } else {
        NanReturnValue(js_result);
    }
}

void MysqlStatement::EIO_After_Fetch(uv_work_t* req) {
    NanScope();

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
        argv[0] = argv[1] = NanNewLocal(Null());
    } else {
        fields = fetch_req->meta->fields;
        js_result_row = Object::New();

        while (i < fetch_req->field_count) {
            ptr = stmt->result_binds[i].buffer;

            Local<Value> js_field;
            if (*(stmt->result_binds[i].is_null)) {
                js_field = NanNewLocal(Null());
            } else {
                js_field = GetFieldValue(ptr, *(stmt->result_binds[i].length), fields[i]);
            }

            js_result_row->Set(V8STR(fields[i].name), js_field);
            i++;
        }
        argc = 2;
        argv[0] = NanNewLocal(Null());
        argv[1] = js_result_row;
    }

    if (fetch_req->meta != NULL) {
        mysql_free_result(fetch_req->meta);
    }

    fetch_req->nan_callback->Call(argc, argv);
    delete fetch_req->nan_callback;

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

NAN_METHOD(MysqlStatement::Fetch) {
    NanScope();

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    if (stmt->state < STMT_BINDED_RESULT) {
        return NanThrowError("Resultset buffers not binded");
    }

    fetch_request *fetch_req = new fetch_request;

    fetch_req->nan_callback = new NanCallback(callback.As<Function>());

    fetch_req->stmt = stmt;
    fetch_req->meta = NULL;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = fetch_req;
    uv_queue_work(uv_default_loop(), _req, EIO_Fetch, (uv_after_work_cb)EIO_After_Fetch);

    NanReturnUndefined();
}

/*! todo: finish
 * Fetch row
 */
NAN_METHOD(MysqlStatement::FetchSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    if (stmt->state < STMT_BINDED_RESULT) {
        return NanThrowError("Resultset buffers not binded");
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
        NanReturnValue(Null());
    }

    fields = meta->fields;

    if (!mysql_stmt_num_rows(stmt->_stmt)) {
        NanReturnValue(Null());
    }

    error = mysql_stmt_fetch(stmt->_stmt);
    if (error == MYSQL_DATA_TRUNCATED) {
        // TODO: handle MYSQL_DATA_TRUNCATED case properly
        error = 0;
    }

    if (error == MYSQL_NO_DATA) {
        FreeMysqlBinds(stmt->result_binds, field_count, false);

        error = 0;
        js_result_row = NanNewLocal(Null());
    } else if (!error) {
        js_result_row = Object::New();

        while (i < field_count) {
            ptr = stmt->result_binds[i].buffer;

            Local<Value> js_field;
            if (*(stmt->result_binds[i].is_null)) {
                js_field = NanNewLocal(Null());
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
        return NanThrowError(mysql_stmt_error(stmt->_stmt));
    } else {
        NanReturnValue(js_result_row);
    }
}

/*! todo: finish
 * Returns the number of field in the given statement
 *
 * @return {Integer}
 */
NAN_METHOD(MysqlStatement::FieldCountSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    NanReturnValue(Integer::New(mysql_stmt_field_count(stmt->_stmt)));
}

/*! todo: finish
 * Frees stored result memory for the given statement handle
 *
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::FreeResultSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    NanReturnValue(!mysql_stmt_free_result(stmt->_stmt) ? True() : False());
}

/*! todo: finish
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

/*! todo: finish
 * Helper for FetchAll(), FetchAllSync() methods. Converts raw data to JS type.
 */
Local<Value> MysqlStatement::GetFieldValue(void* ptr, unsigned long& length, MYSQL_FIELD& field) {
    unsigned int type = field.type;
    if (type == MYSQL_TYPE_TINY) {             // TINYINT
        int32_t val = *((signed char *) ptr);
        // handle as boolean
        if (length == 1) {
            DEBUG_PRINTF("TINYINT(1) %d", val);
            return NanNewLocal(Boolean::New(val));
        // handle as integer
        } else {
            DEBUG_PRINTF("TINYINT(>1) %d", val);
            return Integer::New(val);
        }
    } else if (
    type == MYSQL_TYPE_SHORT ||                // SMALLINT
    type == MYSQL_TYPE_SHORT) {                // YEAR
        if (field.flags & UNSIGNED_FLAG) {
            return Integer::NewFromUnsigned((uint32_t) *((unsigned short int *) ptr));
        } else {
            return Integer::New((int32_t) *((short int *) ptr));
        }
    } else if (
    type == MYSQL_TYPE_INT24 ||                // MEDIUMINT
    type == MYSQL_TYPE_LONG) {                 // INT
        if (field.flags & UNSIGNED_FLAG) {
            return Integer::NewFromUnsigned((uint32_t) *((unsigned int *) ptr));
        } else {
            return Integer::New((int32_t) *((int *) ptr));
        }
    } else if (type == MYSQL_TYPE_LONGLONG) {  // BIGINT
        return Number::New((double) *((long long int *) ptr));
    } else if (type == MYSQL_TYPE_FLOAT) {     // FLOAT
        return Number::New(*((float *) ptr));
    } else if (type == MYSQL_TYPE_DOUBLE) {    // DOUBLE, REAL
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
        char *data = (char *)ptr;

        if (field.flags & BINARY_FLAG) {
            DEBUG_PRINTF("Blob, length: (%lu)", length);

            Local<Object> local_js_buffer = NanNewBufferHandle(data, length);

            return local_js_buffer;
        } else {
            DEBUG_PRINTF("String, length: %lu/%lu", length, field.length);
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
            "Time: %04d-%02d-%02d %02d:%02d:%02d",
            ts.year, ts.month, ts.day,
            ts.hour, ts.minute, ts.second);

        char time_string[24];
        sprintf(
            time_string,
            "%04d-%02d-%02d %02d:%02d:%02d GMT",
            ts.year, ts.month, ts.day,
            ts.hour, ts.minute, ts.second);

        // First step is to get a handle to the global object:
        Local<Object> globalObj = Context::GetCurrent()->Global();

        // Now we need to grab the Date constructor function:
        Local<Function> dateConstructor = Local<Function>::Cast(globalObj->Get(V8STR("Date")));

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

/*! todo: finish
 * Get the ID generated from the previous INSERT operation
 *
 * @return {Integer}
 */
NAN_METHOD(MysqlStatement::LastInsertIdSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    NanReturnValue(Integer::New(mysql_stmt_insert_id(stmt->_stmt)));
}

/*! todo: finish
 * Return the number of rows in statements result set
 *
 * @return {Integer}
 */
NAN_METHOD(MysqlStatement::NextResultSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    NanReturnValue(Integer::New(mysql_stmt_next_result(stmt->_stmt)));
}

/*! todo: finish
 * Return the number of rows in statements result set
 *
 * @return {Integer}
 */
NAN_METHOD(MysqlStatement::NumRowsSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_STORED;  // TODO(Sannis): Or all result already fetched!

    NanReturnValue(Integer::New(mysql_stmt_num_rows(stmt->_stmt)));
}

/*! todo: finish
 * Prepare statement by given query
 *
 * @param {String} query
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::PrepareSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    REQ_STR_ARG(0, query)

    // TODO(Sannis): Smth else? close/reset

    unsigned long int query_len = args[0]->ToString()->Utf8Length();

    if (mysql_stmt_prepare(stmt->_stmt, *query, query_len)) {
        NanReturnValue(False());
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

    NanReturnValue(True());
}

/*! todo: finish
 * Resets a prepared statement
 *
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::ResetSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    if (mysql_stmt_reset(stmt->_stmt)) {
        NanReturnValue(False());
    }

    stmt->state = STMT_INITIALIZED;
    NanReturnValue(True());
}

/*! todo: finish
 * Returns result set metadata from a prepared statement
 *
 * @return {MysqlResult}
 */
NAN_METHOD(MysqlStatement::ResultMetadataSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    MYSQL_RES *my_result = mysql_stmt_result_metadata(stmt->_stmt);

    if (!my_result) {
        NanReturnValue(False());
    }

    Local<Object> local_js_result = MysqlResult::NewInstance(stmt->_stmt->mysql, my_result, mysql_stmt_field_count(stmt->_stmt));

    NanReturnValue(local_js_result);
}

/*! todo: finish
 * Send parameter data to the server in blocks (or "chunks")
 *
 * @param {Integer} parameter number, beginning with 0
 * @param {String} data
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::SendLongDataSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_PREPARED;

    REQ_INT_ARG(0, parameter_number);
    REQ_STR_ARG(1, data);

    if (mysql_stmt_send_long_data(stmt->_stmt,
                                  parameter_number, *data, data.length())) {
        NanReturnValue(False());
    }

    NanReturnValue(True());
}

/*! todo: finish
 * Returns SQLSTATE error from previous statement operation
 *
 * @return {String}
 */
NAN_METHOD(MysqlStatement::SqlStateSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_INITIALIZED;

    NanReturnValue(V8STR(mysql_stmt_sqlstate(stmt->_stmt)));
}

/*! todo: finish
 * After function for StoreResult() method
 */
void MysqlStatement::EIO_After_StoreResult(uv_work_t *req) {
    struct store_result_request* store_req = (struct store_result_request *) (req->data);
    MysqlStatement* stmt = store_req->stmt;

    const int argc = 1;
    Local<Value> argv[argc];

    if (!store_req->ok) {
        argv[0] = V8EXC(mysql_stmt_error(stmt->_stmt));
    } else {
        stmt->state = STMT_STORED_RESULT;
        argv[0] = NanNewLocal(Null());
    }

    store_req->nan_callback->Call(argc, argv);
    delete store_req->nan_callback;

    store_req->stmt->Unref();

    delete store_req;
    delete req;
}

/*! todo: finish
 * Thread function for StoreResult() method
 */
void MysqlStatement::EIO_StoreResult(uv_work_t *req) {
    struct store_result_request* store_req = (struct store_result_request *) (req->data);
    MysqlStatement* stmt = store_req->stmt;

    store_req->ok = !mysql_stmt_store_result(stmt->_stmt);
}

NAN_METHOD(MysqlStatement::StoreResult) {
    NanScope();

    REQ_FUN_ARG(0, callback);

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.This());

    MYSQLSTMT_MUSTBE_EXECUTED;

    store_result_request* store_req = new store_result_request;

    store_req->nan_callback = new NanCallback(callback.As<Function>());

    store_req->stmt = stmt;
    stmt->Ref();

    uv_work_t *_req = new uv_work_t;
    _req->data = store_req;
    uv_queue_work(uv_default_loop(), _req, EIO_StoreResult, (uv_after_work_cb)EIO_After_StoreResult);

    NanReturnUndefined();
}

/*! todo: finish
 * Transfers a result set from a prepared statement
 *
 * @return {Boolean}
 */
NAN_METHOD(MysqlStatement::StoreResultSync) {
    NanScope();

    MysqlStatement *stmt = OBJUNWRAP<MysqlStatement>(args.Holder());

    MYSQLSTMT_MUSTBE_EXECUTED;

    if (mysql_stmt_store_result(stmt->_stmt) != 0) {
        NanReturnValue(False());
    }

    stmt->state = STMT_STORED_RESULT;

    NanReturnValue(True());
}
