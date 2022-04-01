#include "Table.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::string;

extern "C" {
    #include "tables.h"
}

#define SAFE(msg, comm) if ((comm) != 0) {throw getErrorString(comm);}

Table::Table(string name_tab, FieldDef *fields, size_t len)
{
    name = name_tab;
    TableStruct tab;
    tab.fieldsDef = fields;
    tab.numOfFields = len;
    SAFE("createtable: ", createTable(name.c_str(), &tab));
}

Table::Table(string name_tab)
{
    name = name_tab;
}

string Table::get_name()
{
    return name;
}

void Table::delete_table()
{
    SAFE("delete_table: ", deleteTable(name.c_str()));
}

void Table::open()
{
    SAFE("opentable: ", openTable(name.c_str(), &table));
}

void Table::close()
{
    SAFE("closetable: ", closeTable(table));
}

void Table::move_first()
{
    SAFE("move_first: ", moveFirst(table));
}

void Table::move_last()
{
    SAFE("move_last: ", moveLast(table));
}

void Table::move_next()
{
    SAFE("move_next: ", moveNext(table));
}

void Table::move_previos()
{
    SAFE("move_previos: ", movePrevios(table));
}

bool Table::before_first()
{
    return beforeFirst(table);
}

bool Table::after_last()
{
    return afterLast(table);
}

string Table::get_text(const string fieldName)
{
    char *text;
    SAFE("get_text: ", getText(table, fieldName.c_str(), &text));
    string str = "";
    for (int i = 0; text[i] != '\0'; ++i)
        str += text[i];
    return str;
}

long Table::get_long(const string fieldName)
{
    long value;
    SAFE("get_long: ", getLong(table, fieldName.c_str(), &value));
    return value;
}

void Table::put_text(const string fieldName, const string text)
{
    SAFE("start_edit: ", startEdit(table));
    SAFE("put_text: ", putText(table, fieldName.c_str(), text.c_str()));
    SAFE("finish_edit: ", finishEdit(table));
}

void Table::put_long(const string fieldName, long value)
{
    SAFE("start_edit: ", startEdit(table));
    SAFE("put_long: ", putLong(table, fieldName.c_str(), value));
    SAFE("finish_edit: ", finishEdit(table));
}

void Table::create_new()
{
    SAFE("create_new: ", createNew(table));
}

void Table::put_text_new(const string fieldName, const string text)
{
    SAFE("put_text_new: ", putTextNew(table, fieldName.c_str(), text.c_str()));
}

void Table::put_long_new(const string fieldName, long value)
{
    SAFE("put_long_new: ", putLongNew(table, fieldName.c_str(), value));
}

void Table::insert_new()
{
    SAFE("insert_new: ", insertNew(table));
}

void Table::inserta_new()
{
    SAFE("inserta_new: ", insertaNew(table));
}

void Table::insertz_new()
{
    SAFE("insertz_new: ", insertzNew(table));
}

void Table::delete_rec()
{
    SAFE("delete_rec: ", deleteRec(table));
}

unsigned Table::get_field_len(const string fieldName)
{
    unsigned len;
    SAFE("get_field_len: ", getFieldLen(table, fieldName.c_str(), &len));
    return len;
}

FieldType Table::get_field_type(const string fieldName)
{
    FieldType type;
    SAFE("get_field_type: ", getFieldType(table, fieldName.c_str(), &type));
    return type;
}

unsigned Table::get_fields_num()
{
    unsigned num;
    SAFE("get_fields_num: ", getFieldsNum(table, &num));
    return num;
}

string Table::get_field_name(unsigned idx)
{
    const char *name_field;
    SAFE("get_field_name: ", getFieldName(table, idx, &name_field));
    string str = "";
    for (int i = 0; name_field[i] != '\0'; ++i)
        str += name_field[i];
    return str;
}



