#include "Poliz.hpp"
#include "Table.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <set>
#include <regex>

using std::stack;
using std::set;
 
void Poliz::set(Lexema lex)
{
    pol.push_back(lex);
}

void Poliz::set_wh(Lexema lex)
{
    where_poliz.push_back(lex);
}

size_t Poliz::size()
{
    return pol.size();
}

vector<string> Poliz::calculate()
{
    vector<string> res;
    switch (pol[0].type) {
        case LEX_SELECT:
            res = select();
            break;
        case LEX_INSERT:
            res = insert();
            break;
        case LEX_UPDATE:
            res = update();
            break;
        case LEX_DELETE:
            res = delete_rec();
            break;
        case LEX_CREATE:
            res = create();
            break;
        case LEX_DROP:
            res = drop();
        break;
        default:
            break;
    }
    return res;
}

vector<string> Poliz::create() {
    string tab_name = pol[1].lex;
    size_t size = (pol.size() - 2) / 3;
    FieldDef arr[size];
    size_t j = 0;
    for (size_t i = 2; i < pol.size(); i += 3) {
        strcpy(arr[j].name, pol[i].lex.c_str());
        if (pol[i+1].type == LEX_TEXT)
            arr[j].type = Text;
        else
            arr[j].type = Long;
        arr[j].len = stol(pol[i+2].lex);
        ++j;
    }
    Table tab(tab_name.c_str(), arr, size);
    return {"OK"};
}

vector<string> Poliz::select()
{
    string name = pol[pol.size() - 1].lex;
    Table tab(name);
    tab.open();
    tab.move_first();
    vector<string> name_fields;
    if (pol[1].type == LEX_MUL)
        for (size_t i = 0; i < tab.get_fields_num(); ++i)
            name_fields.push_back(tab.get_field_name(i));
    else
        for (size_t i = 1; i < pol.size() - 1; ++i)
            name_fields.push_back(pol[i].lex);
    vector<bool> fields;
    where(fields, tab);
    
    tab.move_first();
    size_t j = 0;
    vector<string> res;
    string str;
    for (size_t i = 0; i < name_fields.size(); ++i) {
        str += name_fields[i];
        for (size_t idx = 0; idx < tab.get_field_len(name_fields[i]) - name_fields[i].size() + 1; ++idx)
            str += " ";
    }
    res.push_back(str);
    while (!tab.after_last()) {
        str.clear();
        if (fields.empty() || fields[j]) {
            for (size_t i = 0; i < name_fields.size(); ++i)
                if (tab.get_field_type(name_fields[i]) == Text) {
                    string text = tab.get_text(name_fields[i]);
                    str += text;
                    for (size_t idx = 0; idx < tab.get_field_len(name_fields[i]) + 1 - text.size(); ++idx)
                        str += " ";
                }
                else {
                    std::stringstream ss;
                    ss << tab.get_long(name_fields[i]);
                    str += ss.str();
                    for (size_t idx = 0; idx < 1 + tab.get_field_len(name_fields[i]) - ss.str().size(); ++idx)
                        str += " ";
                }
            res.push_back(str);
        }
        ++j;
        tab.move_next();
    }
    tab.close();
    return res;
}

vector<string> Poliz::insert()
{
    string name = pol[1].lex;
    Table tab(name);
    tab.open();
    tab.create_new();
    for (size_t i = 2; i < pol.size(); ++i) {
        string fld_name = tab.get_field_name(i - 2);
        if (pol[i].type == LEX_NUM)
            tab.put_long_new(fld_name, stol(pol[i].lex));
        else
            tab.put_text_new(fld_name, pol[i].lex);
        
    }
    tab.insertz_new();
    tab.close();
    return {"OK"};
}

vector<string> Poliz::drop()
{
    string tab_name = pol[1].lex;
    deleteTable(tab_name.c_str());
    return {"OK"};
}

vector<string> Poliz::update()
{
    string name = pol[1].lex;
    Table tab(name);
    tab.open();
    tab.move_first();
    string fld_name = pol[2].lex;
    
    vector<bool> fields;
    where(fields, tab);
    
    tab.move_first();
    size_t j = 0;
    while (!tab.after_last()) {
        if (fields.empty() || fields[j]) {
            size_t i = 3;
            Lexema res = calc_expr(pol, tab, i);
            if (check_num(res, tab))
                tab.put_long(fld_name, get_num(res, tab));
            else
                tab.put_text(fld_name, get_str(res, tab));
        }
        ++j;
        tab.move_next();
    }
    tab.close();
    return {"OK"};
}

vector<string> Poliz::delete_rec()
{
    string name = pol[1].lex;
    Table tab(name);
    tab.open();
    tab.move_first();
    
    vector<bool> fields;
    where(fields, tab);
    
    tab.move_first();
    size_t j = 0;
    while (!tab.after_last()) {
        if (fields.empty() || fields[j])
            tab.delete_rec();
        ++j;
        tab.move_next();
    }
    tab.close();
    return {"OK"};
}

void Poliz::where(vector<bool> & fields, Table & tab)
{
    if (where_poliz[0].type == LEX_ALL)
        return;
    size_t i = 0;
    Lexema res = calc_expr(where_poliz, tab, i);
    FieldType type;
    if (check_num(res, tab))
        type = Long;
    else
        type = Text;
    if (i == where_poliz.size())
        logic(tab, fields);
    else if (where_poliz[i].type == LEX_IN)
        in(type, fields, tab, i + 1);
    else
        like(fields, tab);
        
}

void Poliz::like(vector<bool> & fields, Table & tab)
{
    if (where_poliz[0].type != LEX_ID) {
        tab.close();
        throw "Not right type";
    }
    string field_name = where_poliz[0].lex;
    if (tab.get_field_type(field_name) != Text) {
        tab.close();
        throw "Not right type";
    }
    string str_smpl = where_poliz[2].lex;
    bool not_ = false;
    if (where_poliz.size() > 3 && where_poliz[3].type == LEX_NOT)
        not_ = true;
    
    size_t pos = 0;
    while ((pos = str_smpl.find("%", pos)) != string::npos) {
        str_smpl.replace(pos, 1, ".*");
        pos += 2;
    }
    pos = 0;
    while ((pos = str_smpl.find("_", pos)) != string::npos) {
        str_smpl.replace(pos, 1, ".");
        pos += 1;
    }
    std::regex reg(str_smpl);
    while (!tab.after_last()) {
        string val = get_str(where_poliz[0], tab);
        if (regex_match(val, reg))
            if (not_)
                fields.push_back(false);
            else 
                fields.push_back(true);
        else
            if (not_)
                fields.push_back(true);
            else 
                fields.push_back(false);
        tab.move_next();
    }
}

void Poliz::logic(Table & tab, vector<bool> & fields)
{
    while (!tab.after_last()) {
        size_t i = 0;
        Lexema res = calc_expr(where_poliz, tab, i);
        fields.push_back(get_bool(res, tab));
        tab.move_next();
    }
}

void Poliz::in(FieldType type, vector<bool> & fields, Table & tab, size_t start)
{
    std::set<long> set_num;
    std::set<string> set_str;
    FieldType type_const;
    if (where_poliz[start].type == LEX_NUM)
        type_const = Long;
    else
        type_const = Text;
    if (type != type_const) {
        tab.close();
        throw "Not right type";
    }
    bool not_ = false;
    for (size_t i = start; i < where_poliz.size(); ++i) {
        if (where_poliz[i].type == LEX_NOT) {
            not_ = true;
            break;
        }
        if (where_poliz[i].type == LEX_NUM)
            set_num.insert(stol(where_poliz[i].lex));
        else
            set_str.insert(where_poliz[i].lex);
    }
    while (!tab.after_last()) {
        size_t i = 0;
        Lexema res = calc_expr(where_poliz, tab, i);
        if (type == Long) {
            long val = get_num(res, tab);
            if (not_)
                fields.push_back(set_num.find(val) == set_num.end());
            else
                fields.push_back(set_num.find(val) != set_num.end());
        }
        else {
            string val = get_str(res, tab);
            if (not_)
                fields.push_back(set_str.find(val) == set_str.end());
            else
                fields.push_back(set_str.find(val) != set_str.end());
        }
        tab.move_next();
    }
}

bool Poliz::check_num(Lexema lex, Table & tab)
{
    if (lex.type == LEX_NUM)
        return true;
    else if (lex.type == LEX_ID && (tab.get_field_type(lex.lex) == Long))
        return true;
    else
        return false;
}

bool Poliz::check_str(Lexema lex, Table & tab)
{
    if (lex.type == LEX_STR)
        return true;
    else if (lex.type == LEX_ID && (tab.get_field_type(lex.lex) == Text))
        return true;
    else
        return false;
}

bool Poliz::diff_types(Lexema lex1, Lexema lex2, Table & tab)
{
    if (check_num(lex1, tab) && !check_num(lex2, tab))
        return true;
    if (check_str(lex1, tab) && !check_str(lex2, tab))
        return true;
    if (lex1.type == BOOL && lex2.type != BOOL)
        return true;
    else
        return false;
}

long Poliz::get_num(Lexema lex, Table & tab)
{
    if (check_num(lex, tab))
        if (lex.type == LEX_NUM)
            return stol(lex.lex);
        else
            return tab.get_long(lex.lex);
    else {
        tab.close();
        throw;
    }
}

string Poliz::get_str(Lexema lex, Table & tab)
{
    if (check_str(lex, tab))
        if (lex.type == LEX_STR)
            return lex.lex;
        else
            return tab.get_text(lex.lex);
    else {
        tab.close();
        throw;
    }
}

bool Poliz::get_bool(Lexema lex, Table & tab)
{
    if (lex.type == BOOL)
        if (lex.lex == "true")
            return true;
        else
            return false;
    else {
        tab.close();
        throw;
    }
}

Lexema Poliz::calc_expr(vector<Lexema> & poliz, Table & tab, size_t & i)
{
    stack<Lexema> st;
    while(i < poliz.size() && poliz[i].type != LEX_IN && poliz[i].type != LEX_LIKE && poliz[i].type != LEX_ALL) {
        if (poliz[i].type == LEX_ID || poliz[i].type == LEX_NUM || poliz[i].type == LEX_STR)
            st.push(poliz[i]);
        else {
            if (poliz[i].type != LEX_NOT) {
                Lexema second = st.top();
                st.pop();
                Lexema first = st.top();
                st.pop();
                if (diff_types(first, second, tab)) {
                    tab.close();
                    throw "Not right types";
                }
                if (check_num(first, tab)) {
                    long val1 = get_num(first, tab), val2 = get_num(second, tab);
                    if (poliz[i].type == LEX_ADD || poliz[i].type == LEX_DEC || 
                        poliz[i].type == LEX_DIV || poliz[i].type == LEX_PERCENT
                        ) {
                        if (poliz[i].type == LEX_ADD)
                            val1 += val2;
                        else if (poliz[i].type == LEX_DEC)
                            val1 -= val2;
                        else if (poliz[i].type == LEX_MUL)
                            val1 *= val2;
                        else if (poliz[i].type == LEX_DIV)
                            val1 = val1 / val2;
                        else if (poliz[i].type == LEX_PERCENT)
                            val1 = val1 % val2;
                        std::stringstream ss;
                        ss << val1;
                        string str = ss.str();
                        st.push({LEX_NUM, str});
                    } else {
                        if (poliz[i].type == LEX_EQ)
                            val1 = (val1 == val2);
                        else if (poliz[i].type == LEX_NQ)
                            val1 = (val1 != val2);
                        else if (poliz[i].type == LEX_BQ)
                            val1 = (val1 >= val2);
                        else if (poliz[i].type == LEX_LQ)
                            val1 = (val1 <= val2);
                        else if (poliz[i].type == LEX_BIGGER)
                            val1 = (val1 > val2);
                        else if (poliz[i].type == LEX_LESS)
                            val1 = (val1 < val2);
                        if (val1)
                            st.push({BOOL, "true"});
                        else
                            st.push({BOOL, "false"});
                    }
                }
                else if (check_str(first, tab)){
                    string val1 = get_str(first, tab), val2 = get_str(second, tab);
                    bool res;
                    if (poliz[i].type == LEX_EQ)
                        res = (val1 == val2);
                    else if (poliz[i].type == LEX_NQ)
                        res = (val1 != val2);
                    else if (poliz[i].type == LEX_BQ)
                        res = (val1 >= val2);
                    else if (poliz[i].type == LEX_LQ)
                        res = (val1 <= val2);
                    else if (poliz[i].type == LEX_BIGGER)
                        res = (val1 > val2);
                    else if (poliz[i].type == LEX_LESS)
                        res = (val1 < val2);
                    if (res)
                        st.push({BOOL, "true"});
                    else
                        st.push({BOOL, "false"});
                }
                else {
                    bool val1 = get_bool(first, tab), val2 = get_bool(second, tab);
                    if (poliz[i].type == LEX_AND)
                        val1 = (val1 && val2);
                    else if (poliz[i].type == LEX_OR)
                        val1 = (val1 || val2);
                    
                    if (val1)
                        st.push({BOOL, "true"});
                    else
                        st.push({BOOL, "false"});
                }
            } else {
                Lexema first = st.top();
                st.pop();
                if (first.type != BOOL) {
                    tab.close();
                    throw "Not right types";
                }
                if (first.lex == "false")
                    st.push({BOOL, "true"});
                else
                    st.push({BOOL, "false"});
            }
        }
        ++i;
    }
    Lexema res = st.top();
    st.pop();
    return res;
}


