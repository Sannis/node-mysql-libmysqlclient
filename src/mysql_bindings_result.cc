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

/**
 * Init V8 structures for MysqlResult class
 *
 * @ignore
 */
Persistent<FunctionTemplate> MysqlResult::constructor_template;

void MysqlResult::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    // Constructor
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlResult"));

    Local<ObjectTemplate> instance_template =
        constructor_template->InstanceTemplate();

    // Properties
    instance_template->SetAccessor(V8STR("fieldCount"), FieldCountGetter);

    // Methods
    ADD_PROTOTYPE_METHOD(result, dataSeekSync, DataSeekSync);
    ADD_PROTOTYPE_METHOD(result, fetchAll, FetchAll);
    ADD_PROTOTYPE_METHOD(result, fetchAllSync, FetchAllSync);
    ADD_PROTOTYPE_METHOD(result, fetchArraySync, FetchArraySync);
    ADD_PROTOTYPE_METHOD(result, fetchFieldSync, FetchFieldSync);
    ADD_PROTOTYPE_METHOD(result, fetchFieldDirectSync, FetchFieldDirectSync);
    ADD_PROTOTYPE_METHOD(result, fetchFieldsSync, FetchFieldsSync);
    ADD_PROTOTYPE_METHOD(result, fetchLengthsSync, FetchLengthsSync);
    ADD_PROTOTYPE_METHOD(result, fetchObjectSync, FetchObjectSync);
    ADD_PROTOTYPE_METHOD(result, fieldSeekSync, FieldSeekSync);
    ADD_PROTOTYPE_METHOD(result, fieldTellSync, FieldTellSync);
    ADD_PROTOTYPE_METHOD(result, freeSync, FreeSync);
    ADD_PROTOTYPE_METHOD(result, numRowsSync, NumRowsSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlResult"),
                constructor_template->GetFunction());
}

MysqlResult::MysqlResult(): EventEmitter() {}

MysqlResult::~MysqlResult() {
    this->Free();
}

void MysqlResult::AddFieldProperties(
                                        Local<Object> &js_field_obj,
                                        MYSQL_FIELD *field) {
    js_field_obj->Set(V8STR("name"), V8STR(field->name ? field->name : ""));
    js_field_obj->Set(V8STR("orgname"),
                      V8STR(field->org_name ? field->org_name : ""));
    js_field_obj->Set(V8STR("table"), V8STR(field->table ? field->table : ""));
    js_field_obj->Set(V8STR("orgtable"),
                      V8STR(field->org_table ? field->org_table : ""));
    js_field_obj->Set(V8STR("def"), V8STR(field->def ? field->def : ""));

    js_field_obj->Set(V8STR("max_length"), Integer::New(field->max_length));
    js_field_obj->Set(V8STR("length"), Integer::New(field->length));
    js_field_obj->Set(V8STR("charsetnr"), Integer::New(field->charsetnr));
    js_field_obj->Set(V8STR("flags"), Integer::New(field->flags));
    js_field_obj->Set(V8STR("type"), Integer::New(field->type));
    js_field_obj->Set(V8STR("decimals"), Integer::New(field->decimals));
}

Local<Value> MysqlResult::GetFieldValue(MYSQL_FIELD field, char* field_value) {
    HandleScope scope;

    Local<Value> js_field = Local<Value>::New(Null());

    switch (field.type) {
        case MYSQL_TYPE_NULL:  // NULL-type field
            // Already null
            break;
        case MYSQL_TYPE_TINY:  // TINYINT field
        case MYSQL_TYPE_BIT:  // BIT field (MySQL 5.0.3 and up)
        case MYSQL_TYPE_SHORT:  // SMALLINT field
        case MYSQL_TYPE_LONG:  // INTEGER field
        case MYSQL_TYPE_INT24:  // MEDIUMINT field
        case MYSQL_TYPE_LONGLONG:  // BIGINT field
        case MYSQL_TYPE_YEAR:  // YEAR field
            if (field_value) {
              js_field = V8STR(field_value)->ToInteger();
            }
            break;
        case MYSQL_TYPE_DECIMAL:  // DECIMAL or NUMERIC field
        case MYSQL_TYPE_NEWDECIMAL:  // Precision math DECIMAL or NUMERIC field
        case MYSQL_TYPE_FLOAT:  // FLOAT field
        case MYSQL_TYPE_DOUBLE:  // DOUBLE or REAL field
            if (field_value) {
              js_field = V8STR(field_value)->ToNumber();
            }
            break;
        case MYSQL_TYPE_TIME:  // TIME field
            if (field_value) {
              int hours = 0, minutes = 0, seconds = 0;
              sscanf(field_value, "%d:%d:%d",
                                  &hours, &minutes, &seconds);
              time_t result = hours*60*60 + minutes*60 + seconds;
              js_field = Date::New(static_cast<double>(result)*1000);
            }
            break;
        case MYSQL_TYPE_TIMESTAMP:  // TIMESTAMP field
        case MYSQL_TYPE_DATETIME:  // DATETIME field
            if (field_value) {
              int year = 0, month = 0, day = 0;
              int hour = 0, min = 0, sec = 0;
              int h1 = 0, h2 = 0, m1 = 0, m2 = 0;
              sscanf(field_value, "%d-%d-%d %d:%d:%d",
                                  &year, &month, &day, &hour, &min, &sec);
              time_t rawtime, gmt_delta;
              struct tm timeinfo;
              time(&rawtime);
              if (!localtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  break;
              }
              h1 = timeinfo.tm_hour - (timeinfo.tm_isdst > 0 ? 1 : 0);
              m1 = timeinfo.tm_min;
              if (!gmtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  break;
              }
              h2 = timeinfo.tm_hour;
              m2 = timeinfo.tm_min;
              gmt_delta = (((h1 - h2 + 24)%24)*60 + (m1-m2))*60;
              timeinfo.tm_year = year - 1900;
              timeinfo.tm_mon = month - 1;
              timeinfo.tm_mday = day;
              timeinfo.tm_hour = hour;
              timeinfo.tm_min = min;
              timeinfo.tm_sec = sec;
              rawtime = mktime(&timeinfo);
              js_field = Date::New(
                             static_cast<double>(rawtime + gmt_delta)*1000);
            }
            break;
        case MYSQL_TYPE_DATE:  // DATE field
        case MYSQL_TYPE_NEWDATE:  // Newer const used > 5.0
            if (field_value) {
              int year = 0, month = 0, day = 0;
              int h1 = 0, h2 = 0, m1 = 0, m2 = 0;
              sscanf(field_value, "%d-%d-%d", &year, &month, &day);
              time_t rawtime, gmt_delta;
              struct tm timeinfo;
              time(&rawtime);
              if (!localtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  break;
              }
              h1 = timeinfo.tm_hour - (timeinfo.tm_isdst > 0 ? 1 : 0);
              m1 = timeinfo.tm_min;
              if (!gmtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  break;
              }
              h2 = timeinfo.tm_hour;
              m2 = timeinfo.tm_min;
              gmt_delta = (((h1 - h2 + 24)%24)*60 + (m1-m2))*60;
              timeinfo.tm_year = year - 1900;
              timeinfo.tm_mon = month - 1;
              timeinfo.tm_mday = day;
              timeinfo.tm_hour = 0;
              timeinfo.tm_min = 0;
              timeinfo.tm_sec = 0;
              rawtime = mktime(&timeinfo);
              js_field = Date::New(
                             static_cast<double>(rawtime + gmt_delta)*1000);
            }
            break;
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_VARCHAR:
            if (field_value) {
                js_field = V8STR(field_value);
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
        case MYSQL_TYPE_GEOMETRY:  // Spatial fielda
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

void MysqlResult::Free() {
    if (_res) {
        mysql_free_result(_res);
        _res = NULL;
    }
}

/**
 * Creates new MysqlResult object
 *
 * @constructor
 */
Handle<Value> MysqlResult::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_res);
    uint32_t field_count = args[1]->IntegerValue();
    MYSQL_RES *res = static_cast<MYSQL_RES*>(js_res->Value());
    MysqlResult *my_res = new MysqlResult(res, field_count);
    my_res->Wrap(args.This());

    return args.This();
}

/**
 * Get the number of fields in a result
 *
 * @getter
 * @return {Integer|Undefined}
 */
Handle<Value> MysqlResult::FieldCountGetter(Local<String> property,
                                                   const AccessorInfo &info) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    MYSQLRES_MUSTBE_VALID;

    if (res->field_count > 0) {
        return scope.Close(Integer::New(res->field_count));
    } else {
        return Undefined();
    }
}

/**
 * Adjusts the result pointer to an arbitary row in the result
 *
 * @param {Integer} The field offset
 */
Handle<Value> MysqlResult::DataSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, offset)

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    if (offset < 0 || offset >= mysql_num_rows(res->_res)) {
        return THREXC("Invalid row offset");
    }

    mysql_data_seek(res->_res, offset);

    return Undefined();
}

/**
 * EIO wrapper functions for MysqlResult::FetchAll
 */
#ifndef MYSQL_NON_THREADSAFE
int MysqlResult::EIO_After_FetchAll(eio_req *req) {
    HandleScope scope;

    ev_unref(EV_DEFAULT_UC);
    struct fetchAll_request *fetchAll_req =
        reinterpret_cast<struct fetchAll_request *>(req->data);

    int argc = 1; /* node.js convention, there is always one argument */
    Local<Value> argv[2];

    if (req->result) {
        argv[0] = V8EXC("Error on fetching fields");
    } else {
        MYSQL_FIELD *fields = fetchAll_req->fields;
        uint32_t num_fields = fetchAll_req->num_fields;
        MYSQL_ROW result_row;
        uint32_t i = 0, j = 0;

        Local<Array> js_result = Array::New();
        Local<Object> js_result_row;
        Local<Value> js_field;

        i = 0;
        while ( (result_row = mysql_fetch_row(fetchAll_req->res->_res)) ) {
            if(fetchAll_req->results_array) {
              js_result_row = Array::New();
            } else {
              js_result_row = Object::New();
            }

            for (j = 0; j < num_fields; j++) {
                js_field = GetFieldValue(fields[j], result_row[j]);
                if (fetchAll_req->results_array) {
                    js_result_row->Set(Integer::New(j), js_field);
                } else {
                    if (fetchAll_req->results_structured) {
                        if (!js_result_row->Has(V8STR(fields[j].table))) {
                            js_result_row->Set(V8STR(fields[j].table),
                                               Object::New());
                        }
                        js_result_row->Get(V8STR(fields[j].table))->ToObject()
                                     ->Set(V8STR(fields[j].name), js_field);
                    } else {
                        js_result_row->Set(V8STR(fields[j].name), js_field);
                    }
                }
            }

            js_result->Set(Integer::New(i), js_result_row);

            i++;
        }

        // TODO(Sannis): Make some error check here

        argv[1] = js_result;
        argv[0] = Local<Value>::New(Null());
        argc = 2;
    }

    TryCatch try_catch;

    fetchAll_req->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    fetchAll_req->callback.Dispose();
    fetchAll_req->res->Unref();
    // TODO(Sannis): should I free this?
    //free(fetchAll_req->fields);
    free(fetchAll_req);

    return 0;
}

int MysqlResult::EIO_FetchAll(eio_req *req) {
    struct fetchAll_request *fetchAll_req =
        reinterpret_cast<struct fetchAll_request *>(req->data);
    MysqlResult *res = fetchAll_req->res;

    fetchAll_req->fields = mysql_fetch_fields(res->_res);
    fetchAll_req->num_fields = mysql_num_fields(res->_res);

    // TODO(Sannis): Make some error check here

    req->result = 0;

    return 0;
}
#endif

/**
 * Fetches all result rows as an array
 *
 * @param {Boolean|Object} options (optional)
 * @param {Function(error, rows)} callback
 */
Handle<Value> MysqlResult::FetchAll(const Arguments& args) {
    HandleScope scope;
#ifdef MYSQL_NON_THREADSAFE
    return THREXC(MYSQL_NON_THREADSAFE_ERRORSTRING);
#else
    int arg_pos = 0;
    bool results_array = false;
    bool results_structured = false;

    if (args.Length() > 0) {
        if (args[0]->IsBoolean()) {
            results_array = args[0]->BooleanValue();
            arg_pos++;
        } else if (args[0]->IsObject() && !args[0]->IsFunction()) {
            if (args[0]->ToObject()->Has(V8STR("array"))) {
                results_array = args[0]->ToObject()
                                ->Get(V8STR("array"))->BooleanValue();
            }
            if (args[0]->ToObject()->Has(V8STR("structured"))) {
                results_structured = args[0]->ToObject()
                                     ->Get(V8STR("structured"))->BooleanValue();
            }
            arg_pos++;
        }
        // NOT here: any function is object
        // arg_pos++;
    }

    if (results_array && results_structured) {
        return THREXC("You can't mix 'array' and 'structured' parameters");
    }

    REQ_FUN_ARG(arg_pos, callback)

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This()); // NOLINT

    MYSQLRES_MUSTBE_VALID;

    struct fetchAll_request *fetchAll_req = (struct fetchAll_request *)
        calloc(1, sizeof(struct fetchAll_request));

    if (!fetchAll_req) {
        V8::LowMemoryNotification();
        return THREXC("Could not allocate enough memory");
    }

    fetchAll_req->callback = Persistent<Function>::New(callback);
    fetchAll_req->res = res;
    fetchAll_req->results_array = results_array;
    fetchAll_req->results_structured = results_structured;

    eio_custom(EIO_FetchAll, EIO_PRI_DEFAULT, EIO_After_FetchAll, fetchAll_req);

    ev_ref(EV_DEFAULT_UC);
    res->Ref();

    return Undefined();
#endif
}

/**
 * Fetches all result rows as an array
 *
 * @param {Boolean|Object} options (optional)
 * @return {Array}
 */
Handle<Value> MysqlResult::FetchAllSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    bool results_array = false;
    bool results_structured = false;

    if (args.Length() > 0) {
        if (args[0]->IsBoolean()) {
            results_array = args[0]->BooleanValue();
        } else if (args[0]->IsObject()) {
            if (args[0]->ToObject()->Has(V8STR("array"))) {
                results_array = args[0]->ToObject()
                                ->Get(V8STR("array"))->BooleanValue();
            }
            if (args[0]->ToObject()->Has(V8STR("structured"))) {
                results_structured = args[0]->ToObject()
                                     ->Get(V8STR("structured"))->BooleanValue();
            }
        }
    }

    if (results_array && results_structured) {
        return THREXC("You can't mix 'array' and 'structured' parameters");
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_ROW result_row;
    uint32_t i = 0, j = 0;

    Local<Array> js_result = Array::New();
    Local<Object> js_result_row;
    Local<Value> js_field;

    i = 0;
    while ( (result_row = mysql_fetch_row(res->_res)) ) {
        if (results_array) {
            js_result_row = Array::New();
        } else {
            js_result_row = Object::New();
        }

        for (j = 0; j < num_fields; j++) {
            js_field = GetFieldValue(fields[j], result_row[j]);
            if (results_array) {
                js_result_row->Set(Integer::New(j), js_field);
            } else {
                if (results_structured) {
                    if (!js_result_row->Has(V8STR(fields[j].table))) {
                        js_result_row->Set(V8STR(fields[j].table), Object::New());
                    }
                    js_result_row->Get(V8STR(fields[j].table))->ToObject()
                                 ->Set(V8STR(fields[j].name), js_field);
                } else {
                    js_result_row->Set(V8STR(fields[j].name), js_field);
                }
            }
        }

        js_result->Set(Integer::New(i), js_result_row);

        i++;
    }

    return scope.Close(js_result);
}

/**
 * Fetch a result row as an array
 *
 * @return {Array}
 */
Handle<Value> MysqlResult::FetchArraySync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    uint32_t j = 0;

    Local<Array> js_result_row;
    Local<Value> js_field;

    MYSQL_ROW result_row = mysql_fetch_row(res->_res);

    if (!result_row) {
        return scope.Close(False());
    }

    js_result_row = Array::New();

    for ( j = 0; j < num_fields; j++ ) {
        js_field = GetFieldValue(fields[j], result_row[j]);

        js_result_row->Set(Integer::New(j), js_field);
    }

    return scope.Close(js_result_row);
}

/**
 * Returns meta-data of the next field in the result set
 *
 * @return {Object}
 */
Handle<Value> MysqlResult::FetchFieldSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

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
 * Fetch meta-data for a single field
 *
 * @param {Integer} field number
 * @return {Array}
 */
Handle<Value> MysqlResult::FetchFieldDirectSync(const Arguments& args) { // NOLINT
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    REQ_INT_ARG(0, field_num)

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
 * Returns an array of objects representing the fields in a result set
 *
 * @return {Array}
 */
Handle<Value> MysqlResult::FetchFieldsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

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

        js_result->Set(Integer::New(i), js_result_obj);
    }

    return scope.Close(js_result);
}

/**
 * Returns the lengths of the columns of the current row in the result set
 *
 * @return {Array}
 */
Handle<Value> MysqlResult::FetchLengthsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    uint32_t num_fields = mysql_num_fields(res->_res);
    unsigned long int *lengths = mysql_fetch_lengths(res->_res); // NOLINT (unsigned long required by API)
    uint32_t i = 0;

    Local<Array> js_result = Array::New();

    if (!lengths) {
        return scope.Close(False());
    }

    for (i = 0; i < num_fields; i++) {
        js_result->Set(Integer::New(i), Integer::New(lengths[i]));
    }

    return scope.Close(js_result);
}

/**
 * Fetch a result row as an object
 *
 * @return {Object}
 */
Handle<Value> MysqlResult::FetchObjectSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_ROW result_row;
    uint32_t j = 0;

    Local<Object> js_result_row;
    Local<Value> js_field;

    result_row = mysql_fetch_row(res->_res);

    if (!result_row) {
        return scope.Close(False());
    }

    js_result_row = Object::New();

    for ( j = 0; j < num_fields; j++ ) {
        js_field = GetFieldValue(fields[j], result_row[j]);

        js_result_row->Set(V8STR(fields[j].name), js_field);
    }

    return scope.Close(js_result_row);
}

/**
 * Set result pointer to a specified field offset
 *
 * @param {Integer} field number
 */
Handle<Value> MysqlResult::FieldSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    REQ_UINT_ARG(0, field_num)

    if (field_num < 0 || field_num >= res->field_count) {
        return THREXC("Invalid field offset");
    }

    mysql_field_seek(res->_res, field_num);

    return Undefined();
}

/**
 * Returns the position of the field cursor
 *
 * @return {Integer}
 */
Handle<Value> MysqlResult::FieldTellSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    return scope.Close(Integer::New(mysql_field_tell(res->_res)));
}

/**
 * Frees the memory associated with a result
 */
Handle<Value> MysqlResult::FreeSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    res->Free();

    return Undefined();
}

/**
 * Gets the number of rows in a result
 *
 * @return {Integer}
 */
Handle<Value> MysqlResult::NumRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    MYSQLRES_MUSTBE_VALID;

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    return scope.Close(Integer::New(mysql_num_rows(res->_res)));
}

