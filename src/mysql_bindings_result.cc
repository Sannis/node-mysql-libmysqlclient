/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"

Persistent<FunctionTemplate> MysqlConn::MysqlResult::constructor_template;

void MysqlConn::MysqlResult::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    // Constructor
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->Inherit(EventEmitter::constructor_template);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlResult"));

    Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate(); // NOLINT

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
}

MysqlConn::MysqlResult::MysqlResult(): EventEmitter() {}

MysqlConn::MysqlResult::~MysqlResult() {
    this->Free();
}

void MysqlConn::MysqlResult::AddFieldProperties(
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

void MysqlConn::MysqlResult::SetFieldValue(
                                        Handle<Value> &js_field,
                                        MYSQL_FIELD field,
                                        char* field_value) {
    js_field = Null();
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
                  return;
              }
              h1 = timeinfo.tm_hour - (timeinfo.tm_isdst > 0 ? 1 : 0);
              m1 = timeinfo.tm_min;
              if (!gmtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  return;
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
                  return;
              }
              h1 = timeinfo.tm_hour - (timeinfo.tm_isdst > 0 ? 1 : 0);
              m1 = timeinfo.tm_min;
              if (!gmtime_r(&rawtime, &timeinfo)) {
                  js_field = V8STR(field_value);
                  return;
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
}

void MysqlConn::MysqlResult::Free() {
    if (_res) {
        mysql_free_result(_res);
        _res = NULL;
    }
}

Handle<Value> MysqlConn::MysqlResult::New(const Arguments& args) {
    HandleScope scope;

    REQ_EXT_ARG(0, js_res);
    uint32_t field_count = args[1]->IntegerValue();
    MYSQL_RES *res = static_cast<MYSQL_RES*>(js_res->Value());
    MysqlResult *my_res = new MysqlResult(res, field_count);
    my_res->Wrap(args.This());

    return args.This();
}

Handle<Value> MysqlConn::MysqlResult::FieldCountGetter(Local<String> property,
                                                   const AccessorInfo &info) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(info.Holder());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    if (res->field_count > 0) {
        return scope.Close(Integer::New(res->field_count));
    } else {
        return Undefined();
    }
}

Handle<Value> MysqlConn::MysqlResult::DataSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    REQ_UINT_ARG(0, row_num)

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    if (row_num < 0 || row_num >= mysql_num_rows(res->_res)) {
        return THREXC("Invalid row offset");
    }

    mysql_data_seek(res->_res, row_num);

    return Undefined();
}

#ifndef MYSQL_NON_THREADSAFE
int MysqlConn::MysqlResult::EIO_After_FetchAll(eio_req *req) {
    HandleScope scope;

    ev_unref(EV_DEFAULT_UC);
    struct fetchAll_request *fetchAll_req =
        reinterpret_cast<struct fetchAll_request *>(req->data);

    int argc = 1; /* node.js convention, there is always one argument */
    Local<Value> argv[2];

    if (req->result) {
        argv[0] = V8EXC("Error on fetching");
    } else {
        argv[1] = Local<Value>::New(fetchAll_req->js_result);
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
    free(fetchAll_req);

    return 0;
}

int MysqlConn::MysqlResult::EIO_FetchAll(eio_req *req) {
    HandleScope scope;

    struct fetchAll_request *fetchAll_req =
        reinterpret_cast<struct fetchAll_request *>(req->data);
    MysqlConn::MysqlResult *res = fetchAll_req->res;

    MYSQL_FIELD *fields = mysql_fetch_fields(res->_res);
    uint32_t num_fields = mysql_num_fields(res->_res);
    MYSQL_ROW result_row;
    uint32_t i = 0, j = 0;

    Local<Array> js_result = Array::New();
    Local<Object> js_result_row;
    Local<Value> js_field;

    i = 0;
    while ( (result_row = mysql_fetch_row(res->_res)) ) {
        js_result_row = Object::New();

        for ( j = 0; j < num_fields; j++ ) {
            SetFieldValue(js_field, fields[j], result_row[j]);

            js_result_row->Set(V8STR(fields[j].name), js_field);
        }

        js_result->Set(Integer::New(i), js_result_row);

        i++;
    }

    fetchAll_req->js_result = scope.Close(js_result);
    req->result = 0;

    return 0;
}
#endif

Handle<Value> MysqlConn::MysqlResult::FetchAll(const Arguments& args) {
    HandleScope scope;
#ifdef MYSQL_NON_THREADSAFE
    return THREXC(MYSQL_NON_THREADSAFE_ERRORSTRING);
#else
    REQ_FUN_ARG(0, callback);

    MysqlConn::MysqlResult *res = OBJUNWRAP<MysqlConn::MysqlResult>(args.This()); // NOLINT

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    struct fetchAll_request *fetchAll_req = (struct fetchAll_request *)
        calloc(1, sizeof(struct fetchAll_request));

    if (!fetchAll_req) {
        V8::LowMemoryNotification();
        return THREXC("Could not allocate enough memory");
    }

    fetchAll_req->callback = Persistent<Function>::New(callback);
    fetchAll_req->res = res;

    eio_custom(EIO_FetchAll, EIO_PRI_DEFAULT, EIO_After_FetchAll, fetchAll_req);

    ev_ref(EV_DEFAULT_UC);
    res->Ref();

    return Undefined();
#endif
}

Handle<Value> MysqlConn::MysqlResult::FetchAllSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
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
        js_result_row = Object::New();

        for ( j = 0; j < num_fields; j++ ) {
            SetFieldValue(js_field, fields[j], result_row[j]);

            js_result_row->Set(V8STR(fields[j].name), js_field);
        }

        js_result->Set(Integer::New(i), js_result_row);

        i++;
    }

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::MysqlResult::FetchArraySync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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
        SetFieldValue(js_field, fields[j], result_row[j]);

        js_result_row->Set(Integer::New(j), js_field);
    }

    return scope.Close(js_result_row);
}

Handle<Value> MysqlConn::MysqlResult::FetchFieldSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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

Handle<Value> MysqlConn::MysqlResult::FetchFieldDirectSync(const Arguments& args) { // NOLINT
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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

Handle<Value> MysqlConn::MysqlResult::FetchFieldsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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

Handle<Value> MysqlConn::MysqlResult::FetchLengthsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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

Handle<Value> MysqlConn::MysqlResult::FetchObjectSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

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
        SetFieldValue(js_field, fields[j], result_row[j]);

        js_result_row->Set(V8STR(fields[j].name), js_field);
    }

    return scope.Close(js_result_row);
}

Handle<Value> MysqlConn::MysqlResult::FieldSeekSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    REQ_UINT_ARG(0, field_num)

    if (field_num < 0 || field_num >= res->field_count) {
        return THREXC("Invalid field offset");
    }

    mysql_field_seek(res->_res, field_num);

    return Undefined();
}

Handle<Value> MysqlConn::MysqlResult::FieldTellSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    Local<Value> js_result = Integer::New(mysql_field_tell(res->_res));

    return scope.Close(js_result);
}

Handle<Value> MysqlConn::MysqlResult::FreeSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    res->Free();

    return scope.Close(True());
}

Handle<Value> MysqlConn::MysqlResult::NumRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlResult *res = OBJUNWRAP<MysqlResult>(args.This());

    if (!res->_res) {
        return THREXC("Result has been freed.");
    }

    if (mysql_result_is_unbuffered(res->_res)) {
        return THREXC("Function cannot be used with MYSQL_USE_RESULT");
    }

    Local<Value> js_result = Integer::New(mysql_num_rows(res->_res));

    return scope.Close(js_result);
}

