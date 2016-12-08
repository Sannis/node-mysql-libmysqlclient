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
 
Nan::Persistent<FunctionTemplate> MysqlResult::constructor_template;

void MysqlResult::Init(Handle<Object> target) {
    //Nan::HandleScope scope;
    // Constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New<String>("MysqlResult").ToLocalChecked());
    constructor_template.Reset(tpl) ;
    // Instance template
    v8::Local<v8::ObjectTemplate> instance_template = tpl->InstanceTemplate();
    instance_template->SetInternalFieldCount(1);

    // Instance properties
    Nan::SetAccessor(instance_template,Nan::New<String>("fieldCount").ToLocalChecked(), FieldCountGetter);

    // Prototype methods
    Nan::SetPrototypeMethod(tpl, "dataSeekSync",         DataSeekSync);
    Nan::SetPrototypeMethod(tpl, "fetchAll",             FetchAll);
    Nan::SetPrototypeMethod(tpl, "fetchAllSync",         FetchAllSync);
    Nan::SetPrototypeMethod(tpl, "fetchFieldSync",       FetchFieldSync);
    Nan::SetPrototypeMethod(tpl, "fetchFieldDirectSync", FetchFieldDirectSync);
    Nan::SetPrototypeMethod(tpl, "fetchFieldsSync",      FetchFieldsSync);
    Nan::SetPrototypeMethod(tpl, "fetchLengthsSync",     FetchLengthsSync);
    Nan::SetPrototypeMethod(tpl, "fetchRowSync",         FetchRowSync);
    Nan::SetPrototypeMethod(tpl, "fieldSeekSync",        FieldSeekSync);
    Nan::SetPrototypeMethod(tpl, "fieldTellSync",        FieldTellSync);
    Nan::SetPrototypeMethod(tpl, "freeSync",             FreeSync);
    Nan::SetPrototypeMethod(tpl, "numRowsSync",          NumRowsSync);

    // Make it visible in JavaScript land
    target->Set(Nan::New<String>("MysqlResult").ToLocalChecked(), tpl->GetFunction());
}

Local<Object> MysqlResult::NewInstance(MYSQL *my_conn, MYSQL_RES *my_result, uint32_t field_count) {
    //Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    //Local<FunctionTemplate> tpl = NanPersistentToLocal(constructor_template);
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(constructor_template);
    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(isolate,my_conn);
    argv[1] = External::New(isolate,my_result);
    argv[2] = Integer::NewFromUnsigned(isolate,field_count);

    Local<Object> instance = tpl->GetFunction()->NewInstance(argc, argv);

    return instance ;
}

MysqlResult::MysqlResult(): ObjectWrap() {}

MysqlResult::~MysqlResult() {
    this->Free();
}

void MysqlResult::AddFieldProperties(Local<Object> &js_field_obj, MYSQL_FIELD *field) {
    //Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;

    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"name"),
                      v8::String::NewFromUtf8(isolate,field->name ? field->name : ""));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"orgname"),
                      v8::String::NewFromUtf8(isolate,field->org_name ? field->org_name : ""));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"table"),
                      v8::String::NewFromUtf8(isolate,field->table ? field->table : ""));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"orgtable"),
                      v8::String::NewFromUtf8(isolate,field->org_table ? field->org_table : ""));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"def"),
                      v8::String::NewFromUtf8(isolate,field->def ? field->def : ""));

    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"max_length"),
                      Integer::NewFromUnsigned(isolate,field->max_length));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"length"),
                      Integer::NewFromUnsigned(isolate,field->length));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"charsetnr"),
                      Integer::NewFromUnsigned(isolate,field->charsetnr));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"flags"),
                      Integer::NewFromUnsigned(isolate,field->flags));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"type"),
                      Integer::New(isolate,field->type));
    js_field_obj->Set(v8::String::NewFromUtf8(isolate,"decimals"),
                      Integer::NewFromUnsigned(isolate,field->decimals));
}
char * MysqlResult::ToCString(v8::Local<Value> & value) 
{
    v8::String::Utf8Value str(value->ToString()) ;
    return *str ;
}

Local<Value> MysqlResult::GetFieldValue(MYSQL_FIELD field, char* field_value, unsigned long field_length) {
    //Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent();

    Local<Value> js_field = Nan::Null();

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
              js_field = v8::String::NewFromUtf8(isolate,field_value)->ToInteger();
            }
            break;
        case MYSQL_TYPE_BIT:       // BIT field (MySQL 5.0.3 and up)
        case MYSQL_TYPE_LONGLONG:  // BIGINT field
            // Return BIGINT as string, see #110
            if (field_value) {
              js_field = v8::String::NewFromUtf8(isolate,field_value);
            }
            break;
        case MYSQL_TYPE_FLOAT:   // FLOAT field
        case MYSQL_TYPE_DOUBLE:  // DOUBLE or REAL field
            if (field_value) {
              js_field = v8::String::NewFromUtf8(isolate,field_value)->ToNumber();
            }
            break;
        case MYSQL_TYPE_DECIMAL:     // DECIMAL or NUMERIC field
        case MYSQL_TYPE_NEWDECIMAL:  // Precision math DECIMAL or NUMERIC field
            // Return DECIMAL/NUMERIC as string, see #110
            if (field_value) {
              js_field = v8::String::NewFromUtf8(isolate,field_value);
            }
            break;
        case MYSQL_TYPE_TIME:  // TIME field
            if (field_value) {
              int hours = 0, minutes = 0, seconds = 0;
              sscanf(field_value, "%d:%d:%d", &hours, &minutes, &seconds);
              time_t result = hours*60*60 + minutes*60 + seconds;
              js_field = Date::New(isolate,static_cast<double>(result)*1000);
            }
            break;
        case MYSQL_TYPE_TIMESTAMP:  // TIMESTAMP field
        case MYSQL_TYPE_DATETIME:   // DATETIME field
            if (field_value) {
                // First step is to get a handle to the global object:
                Local<Object> globalObj = Nan::GetCurrentContext()->Global() ;
                
                // Now we need to grab the Date constructor function:
                Local<Function> dateConstructor = Local<Function>::Cast(globalObj->Get(v8::String::NewFromUtf8(isolate,"Date")));
                
                // Great. We can use this constructor function to allocate new Dates:
                const int argc = 1;
                Local<Value> argv[argc] = { String::Concat(v8::String::NewFromUtf8(isolate,field_value), v8::String::NewFromUtf8(isolate," GMT")) };
                
                // Now we have our constructor, and our constructor args. Let's create the Date:
                js_field = dateConstructor->NewInstance(argc, argv);
            }
            break;
        case MYSQL_TYPE_DATE:     // DATE field
        case MYSQL_TYPE_NEWDATE:  // Newer const used in MySQL > 5.0
            if (field_value) {
                // First step is to get a handle to the global object:
                Local<Object> globalObj = Nan::GetCurrentContext()->Global();
                
                // Now we need to grab the Date constructor function:
                Local<Function> dateConstructor = Local<Function>::Cast(globalObj->Get(v8::String::NewFromUtf8(isolate,"Date")));
                
                // Great. We can use this constructor function to allocate new Dates:
                const int argc = 1;
                Local<Value> argv[argc] = { String::Concat(v8::String::NewFromUtf8(isolate,field_value), v8::String::NewFromUtf8(isolate," GMT")) };
                
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
                    js_field =  Nan::CopyBuffer(field_value, field_length).ToLocalChecked() ;  //; v8::Local::New(isolate,node::Buffer::New(isolate,field_value, field_length));
                } else {
                    js_field = V8STR(field_value,isolate);
                }
            }
            break;
        case MYSQL_TYPE_SET:  // SET field
            // TODO(Sannis): Maybe memory leaks here
            if (field_value) {
                char *pch, *last;
                int i = 0;
                Local<Array> js_field_array = Array::New(isolate);

                pch = strtok_r(field_value, ",", &last);
                while (pch != NULL) {
                    js_field_array->Set(Integer::New(isolate,i), v8::String::NewFromUtf8(isolate,pch));
                    pch = strtok_r(NULL, ",", &last);
                    i++;
                }

                js_field = js_field_array;
            }
            break;
        case MYSQL_TYPE_ENUM:  // ENUM field
            if (field_value) {
                js_field = v8::String::NewFromUtf8(isolate,field_value);
            }
            break;
        case MYSQL_TYPE_GEOMETRY:  // Spatial fields
            // See for information:
            // http://dev.mysql.com/doc/refman/5.1/en/spatial-extensions.html
            if (field_value) {
                js_field = v8::String::NewFromUtf8(isolate,field_value);
            }
            break;
        default:
            if (field_value) {
                js_field = v8::String::NewFromUtf8(isolate,field_value);
            }
    }

    // Proper MYSQL_TYPE_SET type handle, thanks for Mark Hechim
    // http://www.mirrorservice.org/sites/ftp.mysql.com/doc/refman/5.1/en/c-api-datatypes.html#c10485
    if (field_value && (field.flags & SET_FLAG)) {
        char *pch, *last = 0x00;
        int i = 0;
        Local<Array> js_field_array = Array::New(isolate);

        pch = strtok_r(field_value, ",", &last);
        while (pch != NULL) {
            js_field_array->Set(Integer::New(isolate,i), v8::String::NewFromUtf8(isolate,pch));
            pch = strtok_r(NULL, ",", &last);
            i++;
        }

        js_field = js_field_array;
    }
    return js_field;
}

MysqlResult::fetch_options MysqlResult::GetFetchOptions(Local<Object> options) {
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;

    fetch_options fo = {false, false};

    // Inherit from options object
    if (options->Has(v8::String::NewFromUtf8(isolate,"asArray"))) {
        DEBUG_PRINTF("+asArray");
        fo.results_as_array = options->Get(v8::String::NewFromUtf8(isolate,"asArray"))->BooleanValue();
    }
    if (options->Has(v8::String::NewFromUtf8(isolate,"nestTables"))) {
        DEBUG_PRINTF("+nestTables");
        fo.results_nest_tables = options->ToObject()->Get(v8::String::NewFromUtf8(isolate,"nestTables"))->BooleanValue();
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
NAN_METHOD(MysqlResult::New) {
    Nan::HandleScope scope;

    REQ_EXT_ARG(0, js_connection);
    REQ_EXT_ARG(1, js_result);
    REQ_UINT_ARG(2, field_count);

    MYSQL *connection = static_cast<MYSQL*>(js_connection->Value());
    MYSQL_RES *result = static_cast<MYSQL_RES*>(js_result->Value());

    MysqlResult *my_res = new MysqlResult(connection, result, field_count);
    my_res->Wrap(info.Holder());

    return info.GetReturnValue().Set(info.Holder());
}

/** read-only
 * MysqlResult#fieldCount -> Integer|Undefined
 *
 * Get the number of fields in a result
 **/
NAN_GETTER(MysqlResult::FieldCountGetter) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    if (res->field_count > 0) {
        return info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate,res->field_count));
    } else {
        return info.GetReturnValue().Set(Nan::Undefined());
    }
}

/**
 * MysqlResult#dataSeekSync(offset)
 * - offset (Integer): The field offset
 *
 * Adjusts the result pointer to an arbitrary row in the result
 **/
NAN_METHOD(MysqlResult::DataSeekSync) {
    Nan::HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, offset)

    if (mysql_result_is_unbuffered(res->_res)) {
        return Nan::ThrowError("Function cannot be used with MYSQL_USE_RESULT");
    }

    if (offset >= mysql_num_rows(res->_res)) {
        return Nan::ThrowError("Invalid row offset");
    }

    mysql_data_seek(res->_res, offset);

    return info.GetReturnValue().Set(Nan::Undefined());
}

/*!
 * EIO wrapper functions for MysqlResult::FetchAll
 */
void MysqlResult::EIO_After_FetchAll(uv_work_t *req) {
    Nan::HandleScope scope;
    v8::Isolate * isolate;
    isolate = v8::Isolate::GetCurrent() ;
    struct fetchAll_request *fetchAll_req = (struct fetchAll_request *)(req->data);

    // We can't use const int argc here because argv is used
    // for both MysqlResult creation and callback call
    int argc = 1; // node.js convention, there is always at least one argument for callback
    Local<Value> argv[3];

    if (!fetchAll_req->ok) {
        argv[0] = V8EXC("Error on fetching fields",isolate);
    } else {
        MYSQL_FIELD *fields = fetchAll_req->fields;
        uint32_t num_fields = fetchAll_req->num_fields;
        MYSQL_ROW result_row;
        unsigned long *field_lengths;
        uint32_t i = 0, j = 0;

        // Get rows
        Local<Array> js_result = Array::New(isolate);
        Local<Object> js_result_row;
        Local<Value> js_field;

        i = 0;
        while ((result_row = mysql_fetch_row(fetchAll_req->res->_res))) {
            field_lengths = mysql_fetch_lengths(fetchAll_req->res->_res);

            if (fetchAll_req->fo.results_as_array) {
              js_result_row = Array::New(isolate);
            } else {
              js_result_row = Object::New(isolate);
            }

            for (j = 0; j < num_fields; j++) {
                js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);

                if (fetchAll_req->fo.results_as_array) {
                    js_result_row->Set(Integer::NewFromUnsigned(isolate,j), js_field);
                } else if (fetchAll_req->fo.results_nest_tables) {
                    if (!js_result_row->Has(v8::String::NewFromUtf8(isolate,fields[j].table))) {
                        js_result_row->Set(v8::String::NewFromUtf8(isolate,fields[j].table), Object::New(isolate));
                    }
                    js_result_row->Get(v8::String::NewFromUtf8(isolate,fields[j].table))->ToObject()
                                 ->Set(v8::String::NewFromUtf8(isolate,fields[j].name), js_field);
                } else {
                    js_result_row->Set(v8::String::NewFromUtf8(isolate,fields[j].name), js_field);
                }
            }

            js_result->Set(Integer::NewFromUnsigned(isolate,i), js_result_row);

            i++;
        }

        if (i != mysql_num_rows(fetchAll_req->res->_res)) {
            unsigned int my_errno = mysql_errno(fetchAll_req->res->_conn);
            const char *my_error = mysql_error(fetchAll_req->res->_conn);
            unsigned long error_string_length = strlen(my_error) + 20;
            char* error_string = new char[error_string_length];
            snprintf(
                error_string, error_string_length,
                "Fetch error #%d: %s",
                my_errno, my_error
            );

            argv[0] = V8EXC(error_string,isolate);
            delete[] error_string;
        } else {
            // Get fields info
            Local<Array> js_fields = Array::New(isolate);

            for (i = 0; i < num_fields; i++) {
                js_result_row = Object::New(isolate);
                AddFieldProperties(js_result_row, &fields[i]);

                js_fields->Set(Integer::NewFromUnsigned(isolate,i), js_result_row);
            }

            argv[1] = js_result;
            argv[2] = js_fields;
            argv[0] = Nan::Null();
            argc = 3;
        }
    }

    fetchAll_req->nan_callback->Call(argc, argv);
    delete fetchAll_req->nan_callback;

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
NAN_METHOD(MysqlResult::FetchAll) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    int arg_pos = 0;
    fetch_options fo = {false, false};
    bool throw_wrong_arguments_exception = false;

    if (info.Length() > 0) {
        if (info[0]->IsObject()) { // Simple Object or Function
            if (!info[0]->IsFunction()) { // Simple Object - options hash
                fo = MysqlResult::GetFetchOptions(info[0]->ToObject());
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
        argv[0] = V8EXC("fetchAllSync can handle only (options) or none arguments",isolate);
        //TODO(Sannis): Use NanCallback here
        node::MakeCallback(isolate, Nan::GetCurrentContext()->Global(), callback, argc, argv);
        return info.GetReturnValue().Set(Nan::Undefined());
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        // Cuz' this is not run-time error, just programmers mistake
        return Nan::ThrowError("You can't mix 'asArray' and 'nestTables' options");
    }

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder()); // NOLINT

    MYSQLRES_MUSTBE_VALID;

    fetchAll_request *fetchAll_req = new fetchAll_request;

    fetchAll_req->nan_callback = new Nan::Callback(callback.As<Function>());

    fetchAll_req->res = res;
    res->Ref();
    
    fetchAll_req->fo = fo;

    uv_work_t *_req = new uv_work_t;
    _req->data = fetchAll_req;
    uv_queue_work(uv_default_loop(), _req, EIO_FetchAll, (uv_after_work_cb)EIO_After_FetchAll);

    return info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlResult#fetchAllSync([options]) -> Array
 * - options (Object): Fetch style options (optional)
 *
 * Fetches all result rows as an array
 **/
NAN_METHOD(MysqlResult::FetchAllSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    fetch_options fo = {false, false};

    if (info.Length() > 0) {
        if (!info[0]->IsObject()) {
            return Nan::ThrowError("fetchAllSync can handle only (options) or none arguments");
        }
        fo = MysqlResult::GetFetchOptions(info[0]->ToObject());
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        return Nan::ThrowError("You can't mix 'asArray' and 'nestTables' options");
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_ROW result_row;
    unsigned long *field_lengths;
    uint32_t i = 0, j = 0;

    Local<Array> js_result = Array::New(isolate);
    Local<Object> js_result_row;
    Local<Value> js_field ;

    i = 0;
    while ( (result_row = mysql_fetch_row(res->_res)) ) {
        field_lengths = mysql_fetch_lengths(res->_res);

        if (fo.results_as_array) {
            js_result_row = Array::New(isolate);
        } else {
            js_result_row = Object::New(isolate);
        }

        for (j = 0; j < num_fields; j++) {
            js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);
            if (fo.results_as_array)  {
                js_result_row->Set(Integer::NewFromUnsigned(isolate,j), js_field);
            } else if (fo.results_nest_tables) {
                if (!js_result_row->Has(v8::String::NewFromOneByte(isolate,(unsigned char *)fields[j].table))) {
                    js_result_row->Set(v8::String::NewFromOneByte(isolate,(unsigned char *)fields[j].table), Object::New(isolate));
                }
    
                js_result_row->Get(v8::String::NewFromOneByte(isolate,(unsigned char *)fields[j].table))->ToObject()
                             ->Set(v8::String::NewFromOneByte(isolate,(unsigned char *)fields[j].name), js_field);
            } else {
                js_result_row->Set(v8::String::NewFromOneByte(isolate,(unsigned char *)fields[j].name), js_field);
            }
        }

        js_result->Set(Integer::NewFromUnsigned(isolate,i), js_result_row);

        i++;
    }
    return info.GetReturnValue().Set(js_result);
}

/**
 * MysqlResult#fetchFieldSync() -> Object
 *
 * Returns metadata of the next field in the result set
 **/
NAN_METHOD(MysqlResult::FetchFieldSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    MYSQL_FIELD *field;

    Local<Object> js_result;

    field = mysql_fetch_field(res->_res);

    if (!field) {
        return info.GetReturnValue().Set(False());
    }

    js_result = Object::New(isolate);
    AddFieldProperties(js_result, field);

    return info.GetReturnValue().Set(js_result);
}

/**
 * MysqlResult#fetchFieldDirectSync(fieldNum) -> Object
 * - fieldNum (Integer): Field number (starts from 0)
 *
 * Returns metadata of the arbitrary field in the result set
 **/
NAN_METHOD(MysqlResult::FetchFieldDirectSync) { // NOLINT
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, field_num)

    MYSQL_FIELD *field;

    Local<Object> js_result;

    field = mysql_fetch_field_direct(res->_res, field_num);

    if (!field) {
        return info.GetReturnValue().Set(False());
    }

    js_result = Object::New(isolate);
    AddFieldProperties(js_result, field);

    return info.GetReturnValue().Set(js_result);
}

/**
 * MysqlResult#fetchFieldsSync() -> Array
 *
 * Returns an array of objects representing the fields in a result set
 **/
NAN_METHOD(MysqlResult::FetchFieldsSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_FIELD *field;
    uint32_t i = 0;

    Local<Array> js_result = Array::New(isolate);
    Local<Object> js_result_obj;

    for (i = 0; i < num_fields; i++) {
        field = mysql_fetch_field_direct(res->_res, i);

        js_result_obj = Object::New(isolate);
        AddFieldProperties(js_result_obj, field);

        js_result->Set(Integer::NewFromUnsigned(isolate,i), js_result_obj);
    }

    return info.GetReturnValue().Set(js_result);
}

/**
 * MysqlResult#fetchLengthsSync() -> Array
 *
 * Returns the lengths of the columns of the current row in the result set
 **/
NAN_METHOD(MysqlResult::FetchLengthsSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    uint32_t num_fields = mysql_num_fields(res->_res);
    unsigned long int *lengths = mysql_fetch_lengths(res->_res); // NOLINT
    uint32_t i = 0;

    Local<Array> js_result = Array::New(isolate);

    if (!lengths) {
        return info.GetReturnValue().Set(False());
    }

    for (i = 0; i < num_fields; i++) {
        js_result->Set(Integer::NewFromUnsigned(isolate,i),
                       Integer::NewFromUnsigned(isolate,lengths[i]));
    }

    return info.GetReturnValue().Set(js_result);
}

/**
 * MysqlResult#fetchRowSync([options]) -> Array|Object
 * - options (Object): Fetch style options (optional)
 *
 * Fetch one row from result
 **/
NAN_METHOD(MysqlResult::FetchRowSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    fetch_options fo = {false, false};

    if (info.Length() > 0) {
        if (!info[0]->IsObject()) {
            return Nan::ThrowError("fetchRowSync can handle only (options) or none arguments");
        }
        fo = MysqlResult::GetFetchOptions(info[0]->ToObject());
    }

    if (fo.results_as_array && fo.results_nest_tables) {
        return Nan::ThrowError("You can't mix 'asArray' and 'nestTables' options");
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    uint32_t j = 0;

    Local<Object> js_result_row;
    Local<Value> js_field;

    MYSQL_ROW result_row = mysql_fetch_row(res->_res);

    if (!result_row) {
        return info.GetReturnValue().Set(False());
    }

    unsigned long *field_lengths = mysql_fetch_lengths(res->_res);

    if (fo.results_as_array) {
        js_result_row = Array::New(isolate);
    } else {
        js_result_row = Object::New(isolate);
    }

    for (j = 0; j < num_fields; j++) {
        js_field = GetFieldValue(fields[j], result_row[j], field_lengths[j]);

        if (fo.results_as_array) {
            js_result_row->Set(Integer::NewFromUnsigned(isolate,j), js_field);
        } else if (fo.results_nest_tables) {
            if (!js_result_row->Has(V8STR(fields[j].table,isolate))) {
                js_result_row->Set(V8STR(fields[j].table,isolate), Object::New(isolate));
            }
            js_result_row->Get(V8STR(fields[j].table,isolate))->ToObject()
                         ->Set(V8STR(fields[j].name,isolate), js_field);
        } else {
            js_result_row->Set(V8STR(fields[j].name,isolate), js_field);
        }
    }

    return info.GetReturnValue().Set(js_result_row);
}

/**
 * MysqlResult#fieldSeekSync(fieldNumber)
 * - fieldNumber (Integer): Field number (starts from 0)
 *
 * Set result pointer to a specified field offset
 **/
NAN_METHOD(MysqlResult::FieldSeekSync) {
    Nan::HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, field_num)

    if (field_num >= res->field_count) {
        return Nan::ThrowError("Invalid field offset");
    }

    mysql_field_seek(res->_res, field_num);

    return info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlResult#fieldTellSync() -> Integer
 *
 * Returns the position of the field cursor
 **/
NAN_METHOD(MysqlResult::FieldTellSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    return info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate,mysql_field_tell(res->_res)));
}

/**
 * MysqlResult#freeSync()
 *
 * Frees the memory associated with a result
 **/
NAN_METHOD(MysqlResult::FreeSync) {
    Nan::HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    res->Free();

    return info.GetReturnValue().Set(Nan::Undefined());
}

/**
 * MysqlResult#numRowsSync() -> Integer
 *
 * Gets the number of rows in a result
 **/
NAN_METHOD(MysqlResult::NumRowsSync) {
    Nan::HandleScope scope;
    v8::Isolate *isolate;
    isolate = v8::Isolate::GetCurrent() ;
    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    if (mysql_result_is_unbuffered(res->_res)) {
        return Nan::ThrowError("Function cannot be used with MYSQL_USE_RESULT");
    }

    return info.GetReturnValue().Set(Integer::New(isolate,mysql_num_rows(res->_res)));
}
