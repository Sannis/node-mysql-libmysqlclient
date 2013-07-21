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

/*!
 * Init V8 structures for MysqlResult class
 */
Persistent<FunctionTemplate> MysqlResult::constructor_template;

void MysqlResult::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(MysqlResult::New);

    // Constructor template
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->SetClassName(String::NewSymbol("MysqlResult"));

    // Instance template
    Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Properties
    instance_template->SetAccessor(V8STR("fieldCount"), FieldCountGetter);

    // Methods
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "dataSeekSync",         MysqlResult::DataSeekSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchAll",             MysqlResult::FetchAll);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchAllSync",         MysqlResult::FetchAllSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchFieldSync",       MysqlResult::FetchFieldSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchFieldDirectSync", MysqlResult::FetchFieldDirectSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchFieldsSync",      MysqlResult::FetchFieldsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchLengthsSync",     MysqlResult::FetchLengthsSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fetchRowSync",         MysqlResult::FetchRowSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fieldSeekSync",        MysqlResult::FieldSeekSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "fieldTellSync",        MysqlResult::FieldTellSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "freeSync",             MysqlResult::FreeSync);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "numRowsSync",          MysqlResult::NumRowsSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlResult"), constructor_template->GetFunction());
}

MysqlResult::MysqlResult(): ObjectWrap() {}

MysqlResult::~MysqlResult() {
    this->Free();
}

void MysqlResult::AddFieldProperties(Local<Object> &js_field_obj, MYSQL_FIELD *field) {
    js_field_obj->Set(V8STR("name"),
                      V8STR(field->name ? field->name : ""));
    js_field_obj->Set(V8STR("orgname"),
                      V8STR(field->org_name ? field->org_name : ""));
    js_field_obj->Set(V8STR("table"),
                      V8STR(field->table ? field->table : ""));
    js_field_obj->Set(V8STR("orgtable"),
                      V8STR(field->org_table ? field->org_table : ""));
    js_field_obj->Set(V8STR("def"),
                      V8STR(field->def ? field->def : ""));

    js_field_obj->Set(V8STR("max_length"),
                      Integer::NewFromUnsigned(field->max_length));
    js_field_obj->Set(V8STR("length"),
                      Integer::NewFromUnsigned(field->length));
    js_field_obj->Set(V8STR("charsetnr"),
                      Integer::NewFromUnsigned(field->charsetnr));
    js_field_obj->Set(V8STR("flags"),
                      Integer::NewFromUnsigned(field->flags));
    js_field_obj->Set(V8STR("type"),
                      Integer::New(field->type));
    js_field_obj->Set(V8STR("decimals"),
                      Integer::NewFromUnsigned(field->decimals));
}

Local<Value> MysqlResult::GetFieldValue(MYSQL_FIELD field, char* field_value, unsigned long field_length) {
    HandleScope scope;

    Local<Value> js_field = Local<Value>::New(Null());

    switch (field.type) {
        case MYSQL_TYPE_NULL:   // NULL-type field
            // Already null
            break;
        case MYSQL_TYPE_TINY:   // TINYINT field
        case MYSQL_TYPE_SHORT:  // SMALLINT field
        case MYSQL_TYPE_LONG:   // INTEGER field
        case MYSQL_TYPE_INT24:  // MEDIUMINT field
        case MYSQL_TYPE_YEAR:   // YEAR field
            if (field_value) {
              js_field = V8STR(field_value)->ToInteger();
            }
            break;
        case MYSQL_TYPE_BIT:       // BIT field (MySQL 5.0.3 and up)
        case MYSQL_TYPE_LONGLONG:  // BIGINT field
            // Return BIGINT as string, see #110
            if (field_value) {
              js_field = V8STR(field_value);
            }
            break;
        case MYSQL_TYPE_FLOAT:   // FLOAT field
        case MYSQL_TYPE_DOUBLE:  // DOUBLE or REAL field
            if (field_value) {
              js_field = V8STR(field_value)->ToNumber();
            }
            break;
        case MYSQL_TYPE_DECIMAL:     // DECIMAL or NUMERIC field
        case MYSQL_TYPE_NEWDECIMAL:  // Precision math DECIMAL or NUMERIC field
            // Return DECIMAL/NUMERIC as string, see #110
            if (field_value) {
              js_field = V8STR(field_value);
            }
            break;
        case MYSQL_TYPE_TIME:  // TIME field
            if (field_value) {
              int hours = 0, minutes = 0, seconds = 0;
              sscanf(field_value, "%d:%d:%d", &hours, &minutes, &seconds);
              time_t result = hours*60*60 + minutes*60 + seconds;
              js_field = Date::New(static_cast<double>(result)*1000);
            }
            break;
        case MYSQL_TYPE_TIMESTAMP:  // TIMESTAMP field
        case MYSQL_TYPE_DATETIME:   // DATETIME field
            if (field_value) {
                // First step is to get a handle to the global object:
                Local<v8::Object> globalObj = Context::GetCurrent()->Global();
                
                // Now we need to grab the Date constructor function:
                Local<v8::Function> dateConstructor = Local<Function>::Cast(globalObj->Get(V8STR("Date")));
                
                // Great. We can use this constructor function to allocate new Dates:
                const int argc = 1;
                Local<Value> argv[argc] = { String::Concat(V8STR(field_value), V8STR(" GMT")) };
                
                // Now we have our constructor, and our constructor args. Let's create the Date:
                js_field = dateConstructor->NewInstance(argc, argv);
            }
            break;
        case MYSQL_TYPE_DATE:     // DATE field
        case MYSQL_TYPE_NEWDATE:  // Newer const used in MySQL > 5.0
            if (field_value) {
                // First step is to get a handle to the global object:
                Local<v8::Object> globalObj = Context::GetCurrent()->Global();
                
                // Now we need to grab the Date constructor function:
                Local<v8::Function> dateConstructor = Local<Function>::Cast(globalObj->Get(V8STR("Date")));
                
                // Great. We can use this constructor function to allocate new Dates:
                const int argc = 1;
                Local<Value> argv[argc] = { String::Concat(V8STR(field_value), V8STR(" GMT")) };
                
                // Now we have our constructor, and our constructor args. Let's create the Date:
                js_field = dateConstructor->NewInstance(argc, argv);
            }
            break;
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
            if (field_value) {
                if (field.flags & BINARY_FLAG) {
                    js_field = Local<Value>::New(node::Buffer::New(V8STR2(field_value, field_length)));
                } else {
                    js_field = V8STR2(field_value, field_length);
                }
            }
            break;
        case MYSQL_TYPE_SET:  // SET field
            // TODO(Sannis): Maybe memory leaks here
            if (field_value) {
                char *pch, *last;
                int i = 0;
                Local<Array> js_field_array = Array::New();

                pch = strtok_r(field_value, ",", &last);
                while (pch != NULL) {
                    js_field_array->Set(Integer::New(i), V8STR(pch));
                    pch = strtok_r(NULL, ",", &last);
                    i++;
                }

                js_field = js_field_array;
            }
            break;
        case MYSQL_TYPE_ENUM:  // ENUM field
            if (field_value) {
                js_field = V8STR(field_value);
            }
            break;
        case MYSQL_TYPE_GEOMETRY:  // Spatial fields
            // See for information:
            // http://dev.mysql.com/doc/refman/5.1/en/spatial-extensions.html
            if (field_value) {
                js_field = V8STR(field_value);
            }
            break;
        default:
            if (field_value) {
                js_field = V8STR(field_value);
            }
    }

    // Proper MYSQL_TYPE_SET type handle, thanks for Mark Hechim
    // http://www.mirrorservice.org/sites/ftp.mysql.com/doc/refman/5.1/en/c-api-datatypes.html#c10485
    if (field_value && (field.flags & SET_FLAG)) {
        char *pch, *last;
        int i = 0;
        Local<Array> js_field_array = Array::New();

        pch = strtok_r(field_value, ",", &last);
        while (pch != NULL) {
            js_field_array->Set(Integer::New(i), V8STR(pch));
            pch = strtok_r(NULL, ",", &last);
            i++;
        }

        js_field = js_field_array;
    }

    return scope.Close(js_field);
}

MysqlResult::fetch_options MysqlResult::GetFetchOptions(Local<Object> options) {
    fetch_options fo = {false, false};

    // Inherit from options object
    if (options->Has(V8STR("asArray"))) {
        DEBUG_PRINTF("+asArray");
        fo.results_as_array = options->Get(V8STR("asArray"))->BooleanValue();
    }
    if (options->Has(V8STR("nestTables"))) {
        DEBUG_PRINTF("+nestTables");
        fo.results_nest_tables = options->ToObject()->Get(V8STR("nestTables"))->BooleanValue();
    }

    if (fo.results_as_array || fo.results_nest_tables) {
        DEBUG_PRINTF("\n");
    }

    return fo;
}

void MysqlResult::Free() {
    if (_res) {
        mysql_free_result(_res);
        _res = NULL;
    }
}

/** internal
 * new MysqlResult()
 *
 * Creates new MysqlResult object
 **/
Handle<Value> MysqlResult::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_connection);
    REQ_EXT_ARG(1, js_result);
    REQ_UINT_ARG(2, field_count);

    MYSQL *connection = static_cast<MYSQL*>(js_connection->Value());
    MYSQL_RES *result = static_cast<MYSQL_RES*>(js_result->Value());

    MysqlResult *my_res = new MysqlResult(connection, result, field_count);
    my_res->Wrap(args.Holder());

    return args.Holder();
}

/** read-only
 * MysqlResult#fieldCount -> Integer|Undefined
 *
 * Get the number of fields in a result
 **/
Handle<Value> MysqlResult::FieldCountGetter(Local<String> property, const AccessorInfo &info) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    if (res->field_count > 0) {
        return scope.Close(Integer::NewFromUnsigned(res->field_count));
    } else {
        return Undefined();
    }
}

/**
 * MysqlResult#dataSeekSync(offset)
 * - offset (Integer): The field offset
 *
 * Adjusts the result pointer to an arbitrary row in the result
 **/
Handle<Value> MysqlResult::DataSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, offset)

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    if (offset >= mysql_num_rows(res->_res)) {
        return THREXC("Invalid row offset");
    }

    mysql_data_seek(res->_res, offset);

    return Undefined();
}

/*!
 * EIO wrapper functions for MysqlResult::FetchAll
 */
void MysqlResult::EIO_After_FetchAll(uv_work_t *req) {
    HandleScope scope;

    struct fetchAll_request *fetchAll_req = (struct fetchAll_request *)(req->data);

    // We can't use const int argc here because argv is used
    // for both MysqlResult creation and callback call
    int argc = 1; // node.js convention, there is always at least one argument for callback
    Local<Value> argv[3];

    if (!fetchAll_req->ok) {
        argv[0] = V8EXC("Error on fetching fields");
    } else {
        MYSQL_FIELD *fields = fetchAll_req->fields;
        uint32_t num_fields = fetchAll_req->num_fields;
        MYSQL_ROW result_row;
        unsigned long *field_lengths;
        uint32_t i = 0, j = 0;

        // Get rows
        Local<Array> js_result = Array::New();
        Local<Object> js_result_row;
        Local<Value> js_field;

        i = 0;
        while ((result_row = mysql_fetch_row(fetchAll_req->res->_res))) {
            field_lengths = mysql_fetch_lengths(fetchAll_req->res->_res);

            if (fetchAll_req->fo.results_as_array) {
              js_result_row = Array::New();
            } else {
              js_result_row = Object::New();
            }

            for (j = 0; j < num_fields; j++) {
                js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);

                if (fetchAll_req->fo.results_as_array) {
                    js_result_row->Set(Integer::NewFromUnsigned(j), js_field);
                } else if (fetchAll_req->fo.results_nest_tables) {
                    if (!js_result_row->Has(V8STR(fields[j].table))) {
                        js_result_row->Set(V8STR(fields[j].table), Object::New());
                    }
                    js_result_row->Get(V8STR(fields[j].table))->ToObject()
                                 ->Set(V8STR(fields[j].name), js_field);
                } else {
                    js_result_row->Set(V8STR(fields[j].name), js_field);
                }
            }

            js_result->Set(Integer::NewFromUnsigned(i), js_result_row);

            i++;
        }

        if (i != mysql_num_rows(fetchAll_req->res->_res)) {
            unsigned int errno = mysql_errno(fetchAll_req->res->_conn);
            const char *error = mysql_error(fetchAll_req->res->_conn);
            unsigned long error_string_length = strlen(error) + 20;
            char* error_string = new char[error_string_length];
            snprintf(
                error_string, error_string_length,
                "Fetch error #%d: %s",
                errno, error
            );

            argv[0] = V8EXC(error_string);
            delete[] error_string;
        } else {
            // Get fields info
            Local<Array> js_fields = Array::New();

            for (i = 0; i < num_fields; i++) {
                js_result_row = Object::New();
                AddFieldProperties(js_result_row, &fields[i]);

                js_fields->Set(Integer::NewFromUnsigned(i), js_result_row);
            }

            argv[1] = js_result;
            argv[2] = js_fields;
            argv[0] = Local<Value>::New(Null());
            argc = 3;
        }
    }

    node::MakeCallback(Context::GetCurrent()->Global(), fetchAll_req->callback, argc, argv);

    fetchAll_req->callback.Dispose();
    
    fetchAll_req->res->Unref();

    // Free the result object after callback
    // All of the rows have been gotten at this point
    // Removed, see comment below
    //fetchAll_req->res->Free();

    // DO NOT do this. User must can manipulate result after fetchAll().
    // delete fetchAll_req->res;
    
    delete fetchAll_req;

    delete req;
}

void MysqlResult::EIO_FetchAll(uv_work_t *req) {
    struct fetchAll_request *fetchAll_req = (struct fetchAll_request *)(req->data);
    MysqlResult *res = fetchAll_req->res;

    // Errors: none
    fetchAll_req->fields = mysql_fetch_fields(res->_res);
    // Errors: none
    fetchAll_req->num_fields = mysql_num_fields(res->_res);

    fetchAll_req->ok = true;
}

/**
 * MysqlResult#fetchAll(callback)
 * MysqlResult#fetchAll(options, callback)
 * - options (Boolean|Object): Fetch style options (optional)
 * - callback (Function): Callback function, gets (error, rows)
 *
 * Fetches all result rows as an array
 **/
Handle<Value> MysqlResult::FetchAll(const Arguments& args) {
    HandleScope scope;

    int arg_pos = 0;
    fetch_options fo = {false, false};
    bool throw_wrong_arguments_exception = false;

    if (args.Length() > 0) {
        if (args[0]->IsObject()) { // Simple Object or Function
            if (!args[0]->IsFunction()) { // Simple Object - options hash
                fo = MysqlResult::GetFetchOptions(args[0]->ToObject());
                arg_pos++;
            }
        } else { // Not an options Object or a Function
            throw_wrong_arguments_exception = true;
            arg_pos++;
        }
        // NOT here: any function is object
        // arg_pos++;
    }

    REQ_FUN_ARG(arg_pos, callback)

    if (throw_wrong_arguments_exception) {
        int argc = 1;
        Local<Value> argv[1];
        argv[0] = V8EXC("fetchAllSync can handle only (options) or none arguments");
        node::MakeCallback(Context::GetCurrent()->Global(), callback, argc, argv);
        return Undefined();
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        // Cuz' this is not run-time error, just programmers mistake
        return THREXC("You can't mix 'asArray' and 'nestTables' options");
        //int argc = 1;
        //Local<Value> argv[1];
        //argv[0] = V8EXC("You can't mix 'asArray' and 'nestTables' options");
        //node::MakeCallback(Context::GetCurrent()->Global(), callback, argc, argv);
        //return Undefined();
    }

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder()); // NOLINT

    MYSQLRES_MUSTBE_VALID;

    fetchAll_request *fetchAll_req = new fetchAll_request;

    fetchAll_req->callback = Persistent<Function>::New(callback);
    fetchAll_req->res = res;
    res->Ref();
    
    fetchAll_req->fo = fo;

    uv_work_t *_req = new uv_work_t;
    _req->data = fetchAll_req;
    uv_queue_work(uv_default_loop(), _req, EIO_FetchAll, (uv_after_work_cb)EIO_After_FetchAll);

    return Undefined();
}

/**
 * MysqlResult#fetchAllSync([options]) -> Array
 * - options (Object): Fetch style options (optional)
 *
 * Fetches all result rows as an array
 **/
Handle<Value> MysqlResult::FetchAllSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    fetch_options fo = {false, false};

    if (args.Length() > 0) {
        if (!args[0]->IsObject()) {
            return THREXC("fetchAllSync can handle only (options) or none arguments");
        }
        fo = MysqlResult::GetFetchOptions(args[0]->ToObject());
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        return THREXC("You can't mix 'asArray' and 'nestTables' options");
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_ROW result_row;
    unsigned long *field_lengths;
    uint32_t i = 0, j = 0;

    Local<Array> js_result = Array::New();
    Local<Object> js_result_row;
    Local<Value> js_field;

    i = 0;
    while ( (result_row = mysql_fetch_row(res->_res)) ) {
        field_lengths = mysql_fetch_lengths(res->_res);

        if (fo.results_as_array) {
            js_result_row = Array::New();
        } else {
            js_result_row = Object::New();
        }

        for (j = 0; j < num_fields; j++) {
            js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);

            if (fo.results_as_array) {
                js_result_row->Set(Integer::NewFromUnsigned(j), js_field);
            } else if (fo.results_nest_tables) {
                if (!js_result_row->Has(V8STR(fields[j].table))) {
                    js_result_row->Set(V8STR(fields[j].table), Object::New());
                }
                js_result_row->Get(V8STR(fields[j].table))->ToObject()
                             ->Set(V8STR(fields[j].name), js_field);
            } else {
                js_result_row->Set(V8STR(fields[j].name), js_field);
            }
        }

        js_result->Set(Integer::NewFromUnsigned(i), js_result_row);

        i++;
    }

    return scope.Close(js_result);
}

/**
 * MysqlResult#fetchFieldSync() -> Object
 *
 * Returns metadata of the next field in the result set
 **/
Handle<Value> MysqlResult::FetchFieldSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    MYSQL_FIELD *field;

    Local<Object> js_result;

    field = mysql_fetch_field(res->_res);

    if (!field) {
        return scope.Close(False());
    }

    js_result = Object::New();
    AddFieldProperties(js_result, field);

    return scope.Close(js_result);
}

/**
 * MysqlResult#fetchFieldDirectSync(fieldNum) -> Object
 * - fieldNum (Integer): Field number (starts from 0)
 *
 * Returns metadata of the arbitrary field in the result set
 **/
Handle<Value> MysqlResult::FetchFieldDirectSync(const Arguments& args) { // NOLINT
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, field_num)

    MYSQL_FIELD *field;

    Local<Object> js_result;

    field = mysql_fetch_field_direct(res->_res, field_num);

    if (!field) {
        return scope.Close(False());
    }

    js_result = Object::New();
    AddFieldProperties(js_result, field);

    return scope.Close(js_result);
}

/**
 * MysqlResult#fetchFieldsSync() -> Array
 *
 * Returns an array of objects representing the fields in a result set
 **/
Handle<Value> MysqlResult::FetchFieldsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_FIELD *field;
    uint32_t i = 0;

    Local<Array> js_result = Array::New();
    Local<Object> js_result_obj;

    for (i = 0; i < num_fields; i++) {
        field = mysql_fetch_field_direct(res->_res, i);

        js_result_obj = Object::New();
        AddFieldProperties(js_result_obj, field);

        js_result->Set(Integer::NewFromUnsigned(i), js_result_obj);
    }

    return scope.Close(js_result);
}

/**
 * MysqlResult#fetchLengthsSync() -> Array
 *
 * Returns the lengths of the columns of the current row in the result set
 **/
Handle<Value> MysqlResult::FetchLengthsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    uint32_t num_fields = mysql_num_fields(res->_res);
    unsigned long int *lengths = mysql_fetch_lengths(res->_res); // NOLINT
    uint32_t i = 0;

    Local<Array> js_result = Array::New();

    if (!lengths) {
        return scope.Close(False());
    }

    for (i = 0; i < num_fields; i++) {
        js_result->Set(Integer::NewFromUnsigned(i),
                       Integer::NewFromUnsigned(lengths[i]));
    }

    return scope.Close(js_result);
}

/**
 * MysqlResult#fetchRowSync([options]) -> Array|Object
 * - options (Object): Fetch style options (optional)
 *
 * Fetch one row from result
 **/
Handle<Value> MysqlResult::FetchRowSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    fetch_options fo = {false, false};

    if (args.Length() > 0) {
        if (!args[0]->IsObject()) {
            return THREXC("fetchRowSync can handle only (options) or none arguments");
        }
        fo = MysqlResult::GetFetchOptions(args[0]->ToObject());
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        return THREXC("You can't mix 'asArray' and 'nestTables' options");
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    uint32_t j = 0;

    Local<Object> js_result_row;
    Local<Value> js_field;

    MYSQL_ROW result_row = mysql_fetch_row(res->_res);

    if (!result_row) {
        return scope.Close(False());
    }

    unsigned long *field_lengths = mysql_fetch_lengths(res->_res);

    if (fo.results_as_array) {
        js_result_row = Array::New();
    } else {
        js_result_row = Object::New();
    }

    for (j = 0; j < num_fields; j++) {
        js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);

        if (fo.results_as_array) {
            js_result_row->Set(Integer::NewFromUnsigned(j), js_field);
        } else if (fo.results_nest_tables) {
            if (!js_result_row->Has(V8STR(fields[j].table))) {
                js_result_row->Set(V8STR(fields[j].table), Object::New());
            }
            js_result_row->Get(V8STR(fields[j].table))->ToObject()
                         ->Set(V8STR(fields[j].name), js_field);
        } else {
            js_result_row->Set(V8STR(fields[j].name), js_field);
        }
    }

    return scope.Close(js_result_row);
}

/**
 * MysqlResult#fieldSeekSync(fieldNumber)
 * - fieldNumber (Integer): Field number (starts from 0)
 *
 * Set result pointer to a specified field offset
 **/
Handle<Value> MysqlResult::FieldSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, field_num)

    if (field_num >= res->field_count) {
        return THREXC("Invalid field offset");
    }

    mysql_field_seek(res->_res, field_num);

    return Undefined();
}

/**
 * MysqlResult#fieldTellSync() -> Integer
 *
 * Returns the position of the field cursor
 **/
Handle<Value> MysqlResult::FieldTellSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    return scope.Close(Integer::NewFromUnsigned(mysql_field_tell(res->_res)));
}

/**
 * MysqlResult#freeSync()
 *
 * Frees the memory associated with a result
 **/
Handle<Value> MysqlResult::FreeSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    res->Free();

    return Undefined();
}

/**
 * MysqlResult#numRowsSync() -> Integer
 *
 * Gets the number of rows in a result
 **/
Handle<Value> MysqlResult::NumRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.Holder());

    MYSQLRES_MUSTBE_VALID;

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    return scope.Close(Integer::New(mysql_num_rows(res->_res)));
}
