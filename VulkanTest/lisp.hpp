//
//  lisp.hpp
//  VulkanTest
//
//  Created by Jeremy Reimer on 2024-09-16.
//

#ifndef lisp_hpp
#define lisp_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

////////////////////// cell

enum cell_type { Symbol, Number, List, Proc, Lambda, String };

struct environment; // forward declaration; cell and environment reference each other

// a variant that can hold any kind of lisp value
struct cell {
    typedef cell (*proc_type)(const std::vector<cell> &);
    typedef std::vector<cell>::const_iterator iter;
    typedef std::map<std::string, cell> map;
    cell_type type; std::string val; std::vector<cell> list; proc_type proc; environment * env;
    cell(cell_type type = Symbol) : type(type), env(0) {}
    cell(cell_type type, const std::string & val) : type(type), val(val), env(0) {}
    cell(proc_type proc) : type(Proc), proc(proc), env(0) {}
};

typedef std::vector<cell> cells;
typedef cells::const_iterator cellit;

const cell false_sym(Symbol, "#f");
const cell true_sym(Symbol, "#t"); // anything that isn't false_sym is true
const cell nil(Symbol, "nil");


////////////////////// environment

// a dictionary that (a) associates symbols with cells, and
// (b) can chain to an "outer" dictionary
struct environment {
    environment(environment * outer = 0) : outer_(outer) {}

    environment(const cells & parms, const cells & args, environment * outer)
    : outer_(outer)
    {
        cellit a = args.begin();
        for (cellit p = parms.begin(); p != parms.end(); ++p)
            env_[p->val] = *a++;
    }

    // map a variable name onto a cell
    typedef std::map<std::string, cell> map;

    // return a reference to the innermost environment where 'var' appears
    map & find(const std::string & var)
    {
        if (env_.find(var) != env_.end())
            return env_; // the symbol exists in this environment
        if (outer_)
            return outer_->find(var); // attempt to find the symbol in some "outer" env
        std::cout << "unbound symbol '" << var << "'\n";
        // exit(1); // We aren't exiting on errors for the moment
        return env_;
    }

    // return a reference to the cell associated with the given symbol 'var'
    cell & operator[] (const std::string & var)
    {
        return env_[var];
    }
    
private:
    map env_; // inner symbol->cell mapping
    environment * outer_; // next adjacent outer env, or 0 if there are no further environments
};

void add_globals(environment & env);
void StartLisp();
void StartREPL();
std::string RunLISPexpression(std::string expression_to_run, environment * env);
std::string to_string(const cell & exp);

#endif /* lisp_hpp */
