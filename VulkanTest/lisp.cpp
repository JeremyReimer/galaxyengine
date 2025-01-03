//
//  lisp.cpp
//  VulkanTest
//
//  Created by Jeremy Reimer on 2024-09-16.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lisp.hpp"

// NOTE: The following LISP interpreter is borrowed from Anthony Hay, but it's a placeholder for now.
// I may rewrite it later or I may start from scratch. :)

// Scheme Interpreter in 90 lines of C++ (not counting lines after the first 90).
// Inspired by Peter Norvig's Lis.py.
// Copyright (c) 2010 Anthony C. Hay. This program leaks memory.

bool quit = false; // for exiting REPL

// return given mumber as a string
std::string str(float n) { std::ostringstream os; os << n; return os.str(); }

// return true iff given character is '0'..'9'
bool isdig(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }


////////////////////// built-in primitive procedures

cell proc_add(const cells & c)
{
    float n(atof(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i) n += atof(i->val.c_str());
    return cell(Number, str(n));
}

cell proc_sub(const cells & c)
{
    float n(atof(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i) n -= atof(i->val.c_str());
    return cell(Number, str(n));
}

cell proc_mul(const cells & c)
{
    float n(1);
    for (cellit i = c.begin(); i != c.end(); ++i) n *= atof(i->val.c_str());
    return cell(Number, str(n));
}

cell proc_div(const cells & c)
{
    float n(atof(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i) n /= atof(i->val.c_str());
    return cell(Number, str(n));
}

cell proc_greater(const cells & c)
{
    float n(atof(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n <= atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_less(const cells & c)
{
    float n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n >= atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_less_equal(const cells & c)
{
    float n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n > atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_length(const cells & c) { return cell(Number, str(c[0].list.size())); }
cell proc_nullp(const cells & c)  { return c[0].list.empty() ? true_sym : false_sym; }
cell proc_car(const cells & c)    { return c[0].list[0]; }

cell proc_cdr(const cells & c)
{
    if (c[0].list.size() < 2)
        return nil;
    cell result(c[0]);
    result.list.erase(result.list.begin());
    return result;
}

cell proc_append(const cells & c)
{
    cell result(List);
    result.list = c[0].list;
    for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) result.list.push_back(*i);
    return result;
}

cell proc_cons(const cells & c)
{
    cell result(List);
    result.list.push_back(c[0]);
    for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) result.list.push_back(*i);
    return result;
}

cell proc_list(const cells & c)
{
    cell result(List); result.list = c;
    return result;
}

cell proc_quit(const cells & c)
{
    quit = true;
    std::printf("LISP REPL unloaded.\n");
    return nil;
}

cell proc_print(const cells & c)
{
    cell result(c[0]);
    std::printf("Printing expression...\n");
    std::cout << to_string(result) << '\n';
    //std::printf(result); // We need to convert this to const char * somehow
    return result;
}

// define the bare minimum set of primintives necessary to pass the unit tests
void add_globals(environment & env)
{
    env["nil"] = nil;   env["#f"] = false_sym;  env["#t"] = true_sym;
    env["append"] = cell(&proc_append);   env["car"]  = cell(&proc_car);
    env["cdr"]    = cell(&proc_cdr);      env["cons"] = cell(&proc_cons);
    env["length"] = cell(&proc_length);   env["list"] = cell(&proc_list);
    env["null?"]  = cell(&proc_nullp);    env["+"]    = cell(&proc_add);
    env["-"]      = cell(&proc_sub);      env["*"]    = cell(&proc_mul);
    env["/"]      = cell(&proc_div);      env[">"]    = cell(&proc_greater);
    env["<"]      = cell(&proc_less);     env["<="]   = cell(&proc_less_equal);
    env["quit"]   = cell(&proc_quit);     env["print"]= cell(&proc_print);
}


////////////////////// eval

cell eval(cell x, environment * env)
{
    if (x.type == Symbol)
        return env->find(x.val)[x.val];
    if (x.type == Number)
        return x;
    if (x.type == String)
        return x;
    if (x.list.empty())
        return nil;
    if (x.list[0].type == Symbol) {
        if (x.list[0].val == "quote")       // (quote exp)
            return x.list[1];
        if (x.list[0].val == "if")          // (if test conseq [alt])
            return eval(eval(x.list[1], env).val == "#f" ? (x.list.size() < 4 ? nil : x.list[3]) : x.list[2], env);
        if (x.list[0].val == "set")        // (set! var exp)
            return env->find(x.list[1].val)[x.list[1].val] = eval(x.list[2], env);
        if (x.list[0].val == "define")      // (define var exp)
            return (*env)[x.list[1].val] = eval(x.list[2], env);
        if (x.list[0].val == "lambda") {    // (lambda (var*) exp)
            x.type = Lambda;
            // keep a reference to the environment that exists now (when the
            // lambda is being defined) because that's the outer environment
            // we'll need to use when the lambda is executed
            x.env = env;
            return x;
        }
        if (x.list[0].val == "begin") {     // (begin exp*)
            for (size_t i = 1; i < x.list.size() - 1; ++i)
                eval(x.list[i], env);
            return eval(x.list[x.list.size() - 1], env);
        }
    }
                                            // (proc exp*)
    cell proc(eval(x.list[0], env));
    cells exps;
    for (cell::iter exp = x.list.begin() + 1; exp != x.list.end(); ++exp)
        exps.push_back(eval(*exp, env));
    if (proc.type == Lambda) {
        // Create an environment for the execution of this lambda function
        // where the outer environment is the one that existed* at the time
        // the lambda was defined and the new inner associations are the
        // parameter names with the given arguments.
        // *Although the environmet existed at the time the lambda was defined
        // it wasn't necessarily complete - it may have subsequently had
        // more symbols defined in that environment.
        return eval(/*body*/proc.list[2], new environment(/*parms*/proc.list[1].list, /*args*/exps, proc.env));
    }
    else if (proc.type == Proc)
        return proc.proc(exps);

    std::cout << "not a function\n";
    //exit(1); // We aren't exiting on errors for the moment
    return nil;
}


////////////////////// parse, read and user interaction

// convert given string to list of tokens
std::list<std::string> tokenize(const std::string & str)
{
    std::list<std::string> tokens;
    std::list<std::string> noresult;
    noresult.push_back("(");
    noresult.push_back(")");
    int leftparencount = 0;
    int rightparencount = 0;
    const char * s = str.c_str();
    while (*s) {
        while (*s == ' ')
            ++s;
        if (*s == '(' || *s == ')')
            tokens.push_back(*s++ == '(' ? "(" : ")");
        else {
            const char * t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')') {
                ++t;
            }
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    
    // Check to see if parentheses match
    for (auto const& i : tokens) {
        if (i == "(") { leftparencount++; }
        if (i == ")") { rightparencount++; }
    }
    
    //std::printf("Left parens: %i Right parens: %i", leftparencount, rightparencount);
    if (leftparencount != rightparencount) {
        std::printf("Mismatched parentheses! LISP expression not evaluated.\n");
        return noresult;
    }
    else if (leftparencount == 0 && rightparencount == 0 && tokens.size() == 0) {
        std::printf("\n");
        return noresult;
    }
    else
    {
        return tokens;
    }
}

// numbers become Numbers; stuff in quotes become Strings, every other token is a Symbol
cell atom(const std::string & token)
{
    if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
        return cell(Number, token);
    else if (token[0] == '\"')
        return cell(String, token);
    return cell(Symbol, token);
}

// return the Lisp expression in the given tokens
cell read_from(std::list<std::string> & tokens)
{
    const std::string token(tokens.front());
    tokens.pop_front();
    if (token == "(") {
        cell c(List);
        while (tokens.front() != ")")
            c.list.push_back(read_from(tokens));
        tokens.pop_front();
        return c;
    }
    else
        return atom(token);
}

// return the Lisp expression represented by the given string
cell read(const std::string & s)
{
    std::list<std::string> tokens(tokenize(s));
    return read_from(tokens);
}

// convert given cell to a Lisp-readable string
std::string to_string(const cell & exp)
{
    if (exp.type == List) {
        std::string s("(");
        for (cell::iter e = exp.list.begin(); e != exp.list.end(); ++e)
            s += to_string(*e) + ' ';
        if (s[s.size() - 1] == ' ')
            s.erase(s.size() - 1);
        return s + ')';
    }
    else if (exp.type == Lambda)
        return "<Lambda>";
    else if (exp.type == Proc)
        return "<Proc>";
    return exp.val;
}

// the default read-eval-print-loop
void repl(const std::string & prompt, environment * env)
{
    std::string line = "";
    while (!quit) {
        std::cout << prompt;
        std::string line; std::getline(std::cin, line);
        std::cout << to_string(eval(read(line), env)) << '\n';
        //std::printf("%s", line.c_str()); // Debugging
    }
}

void StartREPL()
{
    environment global_env; add_globals(global_env);
    repl("galaxyOS> ", &global_env);
}

// Execute a single LISP expression
std::string RunLISPexpression(std::string expression_to_run, environment * env) {
    cell run_cell = read(expression_to_run);
    std::string result = to_string(eval(run_cell, env)).c_str();
    std::cout << result; // Debugging
    return result;
}

// Run once at startup

void StartLisp() {
    printf("LISP Engine loaded.\n");
    //StartREPL(); // We can start the REPL if we want before the triangle is run, but we won't right now
    std::string opening_lisp_expression = "(print 123)";
    environment startup_environment; add_globals(startup_environment);
    cell start_cell = read(opening_lisp_expression);
    std::printf("%s", to_string(eval(start_cell, &startup_environment)).c_str());
}
