#include <iostream>
#include <string>
#include <cstring>
#include <stack>
#include <vector>
#include "Scanner.hpp"
#include "Parser.hpp"
#include "Table.hpp"
#include "Poliz.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::stack;
using std::string;
using std::vector;

Parser::Parser(string str)
{
    input = str;
    Scanner s(str);
    scan = s;
}

void Parser::get_lex()
{
    curr_lex = scan.get_lex();
    curr_type = curr_lex.type;
}

bool Parser::analyze(Poliz & pol) 
{
    get_lex();
    start(pol);
    if (curr_type != LEX_END)
        throw curr_type;
    return true;
}

void Parser::start(Poliz & pol)
{
    pol.set(curr_lex);
    switch (curr_type) {
        case LEX_SELECT:
            get_lex();
            select(pol);
            break;
        case LEX_INSERT:
            get_lex();
            insert(pol);
            break;
        case LEX_UPDATE:
            get_lex();
            update(pol);
            break;
        case LEX_DELETE:
            get_lex();
            delete_tab(pol);
            break;
        case LEX_CREATE:
            get_lex();
            create(pol);
            break;
        case LEX_DROP:
            get_lex();
            drop(pol);
            break;
        default:
            throw curr_type;
    }
}

void Parser::name_table(Poliz & pol)
{
    if (curr_type != LEX_ID)
        throw curr_type;
    pol.set(curr_lex);
    get_lex();
}

void Parser::field_name(Poliz & pol)
{
    if (curr_type != LEX_ID)
        throw curr_type;
    pol.set(curr_lex);
    get_lex();
}

void Parser::list_fields(Poliz & pol)
{
    if (curr_type == LEX_MUL) {
        pol.set(curr_lex);
        get_lex();
        return;
    }
    if (curr_type != LEX_ID)
        throw curr_type;
    pol.set(curr_lex);
    get_lex();
    while (curr_type == LEX_COMMA) {
        get_lex();
        if (curr_type != LEX_ID)
            throw curr_type;
        pol.set(curr_lex);
        get_lex();
    }
}

void Parser::field_value(Poliz & pol)
{
    if (curr_type != LEX_OPBR)
        throw curr_type;
    get_lex();
    
    //check str/num/-
    if (curr_type != LEX_STR && curr_type != LEX_NUM && curr_type != LEX_DEC)
        throw curr_type;
    string lex = curr_lex.lex;
    if (curr_type == LEX_DEC) {
        get_lex();
        if (curr_type != LEX_NUM)
            throw curr_type;
        lex += curr_lex.lex;
    }
    pol.set({curr_type, lex});
    get_lex();
    while (curr_type == LEX_COMMA) {
        get_lex();
        if (curr_type != LEX_STR && curr_type != LEX_NUM && curr_type != LEX_DEC)
            throw curr_type;
        lex = curr_lex.lex;
        if (curr_type == LEX_DEC) {
            get_lex();
            if (curr_type != LEX_NUM)
                throw curr_type;
            lex += curr_lex.lex;
        }
        pol.set({curr_type, lex});
        get_lex();
    }
    if (curr_type != LEX_CLBR)
        throw curr_type;
    get_lex();
}

void Parser::select(Poliz & pol)
{
    list_fields(pol);
    if (curr_type != LEX_FROM)
        throw curr_type;
    get_lex();
    name_table(pol);
    where(pol);
}

void Parser::insert(Poliz & pol)
{
    if (curr_type != LEX_INTO)
        throw curr_type;
    get_lex();
    name_table(pol);
    field_value(pol);
}

void Parser::update(Poliz & pol)
{
    name_table(pol);
    if (curr_type != LEX_SET)
        throw curr_type;
    get_lex();
    field_name(pol);
    if (curr_type != LEX_EQ)
        throw curr_type;
    get_lex();
    expression(pol, false);
    LexemaType type = types.top();
    types.pop();
    if (type == BOOL)
        throw type;
    types.push(BOOL);
    where(pol);
}

void Parser::delete_tab(Poliz & pol)
{
    if (curr_type != LEX_FROM)
        throw curr_type;
    get_lex();
    name_table(pol);
    where(pol);
}

void Parser::create(Poliz & pol)
{
    if (curr_type != LEX_TABLE)
        throw curr_type;
    get_lex();
    name_table(pol);
    if (curr_type != LEX_OPBR)
        throw curr_type;
    get_lex();
    list_descriptions(pol);
    if (curr_type != LEX_CLBR)
        throw curr_type;
    get_lex();
}

void Parser::list_descriptions(Poliz & pol)
{
    field_name(pol);
    field_type(pol);
    while (curr_type == LEX_COMMA) {
        get_lex();
        field_name(pol);
        field_type(pol);
    }
}

void Parser::field_type(Poliz & pol)
{
    if (curr_type != LEX_TEXT && curr_type != LEX_LONG)
        throw curr_type;
    pol.set(curr_lex);
    if (curr_type == LEX_TEXT) {
        get_lex();
        if (curr_type != LEX_OPBR)
            throw curr_type;
        get_lex();
        if (curr_type != LEX_NUM)
            throw curr_type;
        
        pol.set(curr_lex);
        get_lex();
        if (curr_type != LEX_CLBR)
            throw curr_type;
    } else
        pol.set({LEX_NUM, "8"});
    get_lex();
}

void Parser::drop(Poliz & pol)
{
    if (curr_type != LEX_TABLE)
        throw curr_type;
    get_lex();
    name_table(pol);
}

void Parser::where(Poliz & pol)
{
    if (curr_type != LEX_WHERE)
        throw curr_type;
    get_lex();
    if (curr_type == LEX_ALL) {
        pol.set_wh(curr_lex);
        get_lex();
        return;
    }
    expression(pol, true);
    conditions(pol);
}

void Parser::conditions(Poliz & pol)
{
    int not_ = 0;
    if (curr_type == LEX_NOT) {
        not_ = 1;
        get_lex();
    }
    if (curr_type == LEX_LIKE) {
        LexemaType type = types.top();
        types.pop();
        if (type != LEX_ID)
            throw type;
        types.push(type);
        pol.set_wh(curr_lex);
        get_lex();
        string_sample(pol);
    } else if (curr_type == LEX_IN) {
        LexemaType type = types.top();
        types.pop();
        if (type == BOOL)
            throw type;
        types.push(type);
        pol.set_wh(curr_lex);
        get_lex();
        if (curr_type != LEX_OPBR)
            throw curr_type;
        get_lex();
        list_const(pol);
        if (curr_type != LEX_CLBR)
            throw curr_type;
        get_lex();
    } else {
        LexemaType type = types.top();
        types.pop();
        if (type != BOOL)
            throw type;
        types.push(type);
    }
    if (not_)
        pol.set_wh({LEX_NOT, "NOT"});
}

void Parser::string_sample(Poliz & pol)
{
    if (curr_type != LEX_STR)
        throw curr_type;
    pol.set_wh(curr_lex);
    get_lex();
}

void Parser::list_const(Poliz & pol)
{
    if (curr_type == LEX_STR) {
        pol.set_wh(curr_lex);
        get_lex();
        while (curr_type == LEX_COMMA) {
            get_lex();
            if (curr_type != LEX_STR)
                throw curr_type;
            pol.set_wh(curr_lex);
            get_lex();
        }
    } else if (curr_type == LEX_NUM || curr_type == LEX_DEC) {
        string lex = curr_lex.lex;
        if (curr_type == LEX_DEC) {
            get_lex();
            if (curr_type != LEX_NUM)
                throw curr_type;
            lex += curr_lex.lex;
        }
        pol.set_wh({LEX_NUM, lex});
        get_lex();
        while (curr_type == LEX_COMMA) {
            get_lex();
            if (curr_type != LEX_NUM && curr_type != LEX_DEC)
                throw curr_type;
            lex = curr_lex.lex;
            if (curr_type == LEX_DEC) {
                get_lex();
                if (curr_type != LEX_NUM)
                    throw curr_type;
                lex += curr_lex.lex;
            }
            pol.set_wh({LEX_NUM, lex});
            get_lex();
        }
    } else
        throw curr_type;
}

void Parser::expression(Poliz & pol, bool fl)
{
    term(pol, fl);
    while (curr_type == LEX_ADD || curr_type == LEX_DEC || curr_type == LEX_OR) {
        Lexema lex = curr_lex;
        get_lex();
        term(pol, fl);
        if (fl)
            pol.set_wh(lex);
        else
            pol.set(lex);
        
        //types
        LexemaType second_type = types.top();
        types.pop();
        LexemaType first_type = types.top();
        types.pop();
        if (lex.type == LEX_OR) {
            if (first_type != BOOL || second_type != BOOL)
                throw first_type;
            types.push(BOOL);
        }
        else {
            if ((first_type != LEX_NUM && first_type != LEX_ID ) ||
                (second_type != LEX_NUM && second_type != LEX_ID ))
                throw first_type;
            types.push(LEX_NUM);
        }
    }
}

void Parser::term(Poliz & pol, bool fl)
{
    factor(pol, fl);
    while (curr_type == LEX_MUL || curr_type == LEX_DIV ||
                curr_type == LEX_PERCENT || curr_type == LEX_AND
        ) {
        Lexema lex = curr_lex;
        get_lex();
        factor(pol, fl);
        if (fl)
            pol.set_wh(lex);
        else
            pol.set(lex);
        
        //types
        LexemaType second_type = types.top();
        types.pop();
        LexemaType first_type = types.top();
        types.pop();
        if (lex.type == LEX_AND) {
            if (first_type != BOOL || second_type != BOOL)
                throw first_type;
            types.push(BOOL);
        }
        else {
            if ((first_type != LEX_NUM && first_type != LEX_ID ) ||
                (second_type != LEX_NUM && second_type != LEX_ID ))
                throw first_type;
            types.push(LEX_NUM);
        }
    }
}

void Parser::factor(Poliz & pol, bool fl)
{
    if (curr_type == LEX_NOT) {
        get_lex();
        factor(pol, fl);
        if (fl)
            pol.set_wh({LEX_NOT, "NOT"});
        else
            pol.set({LEX_NOT, "NOT"});
        
        //types
        LexemaType first_type = types.top();
        types.pop();
        if (first_type != BOOL)
            throw first_type;
        types.push(BOOL);
    } else if (curr_type == LEX_OPBR) {
        get_lex();
        expression(pol, fl);
        attitude(pol, fl);
    } else
        value(pol, fl);
}

void Parser::attitude(Poliz & pol, bool fl)
{
    if (curr_type == LEX_CLBR) {
        get_lex();
        return;
    }
    if (curr_type == LEX_EQ || curr_type == LEX_NQ || curr_type == LEX_BQ ||
        curr_type == LEX_LQ || curr_type == LEX_BIGGER || curr_type == LEX_LESS
        ) {
        Lexema lex = curr_lex;
        get_lex();
        expression(pol, fl);
        if (fl)
            pol.set_wh(lex);
        else
            pol.set(lex);
        if (curr_type != LEX_CLBR)
            throw curr_type;
        get_lex();
        
        //types
        LexemaType second_type = types.top();
        types.pop();
        LexemaType first_type = types.top();
        types.pop();
        
        if ((first_type == LEX_NUM && second_type == LEX_STR) ||
            (first_type == LEX_STR && second_type == LEX_NUM)
            )
            throw first_type;
        types.push(BOOL);
    }
    else
        throw curr_type;
}

void Parser::value(Poliz & pol, bool fl)
{
    string lex = curr_lex.lex;
    if (curr_type == LEX_DEC) {
        get_lex();
        if (curr_type != LEX_NUM)
            throw curr_type;
        lex += curr_lex.lex;
    }
    if (curr_type == LEX_ID || curr_type == LEX_NUM || curr_type == LEX_STR) {
        types.push(curr_type);
        if (fl)
            pol.set_wh({curr_type, lex});
        else
            pol.set({curr_type, lex});
        get_lex();
    }
    else
        throw curr_type;
}

