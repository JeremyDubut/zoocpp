#pragma once

#ifdef DEBUG
    #define LOG(x) std::cout << x << std::endl
#else
    #define LOG(x)
#endif


// Some common types and macros

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <iostream>
#include <sstream>




struct value_t;
struct term_t;
struct raw_t;
struct context_t;
struct inferrance_t;
struct closure_t;
struct renaming_t;
struct type_t;
struct pibinder_t;
struct icit;
struct arg_t;
struct locals_t;

// We decided to use share pointers because terms are shared during 
// evaluation and inferrance through spines and environments
typedef std::shared_ptr<value_t> value_ptr;
typedef std::shared_ptr<term_t> term_ptr;
typedef std::shared_ptr<raw_t> raw_ptr;
typedef std::unique_ptr<locals_t> locals_ptr;

// We keep track of types as a map from variable names to a vector
// keeping the type of the occurences of this variable
// we need a vector because a variable could be binded several times
// except for named arguments, we consider that the last bind is the 
// current occurence in the scope
typedef std::unordered_map<std::string,std::vector<type_t>> types_t;

typedef std::vector<value_ptr> environment_t;
typedef std::string name_t;
typedef std::vector<name_t> names_t;
typedef std::optional<value_ptr> metaentry_t;
typedef std::vector<metaentry_t> metadata_t;
typedef std::vector<bool> flags_t;
typedef std::vector<std::pair<value_ptr,bool>> spine_t;
typedef std::unordered_map<std::size_t,std::size_t> renamingFun_t;
enum pruning_t { Implicit, Explicit, None };
typedef std::vector<pruning_t> prunings_t;

// some common macros
#define FRESHMETA std::make_shared<imeta_t>(cont.flags)
#define VU std::make_shared<vu_t>()
#define CAPP(v,body,val) \
    body.environment.push_back(v); \
    value_ptr val = body.term->eval(body.environment); \
    body.environment.pop_back(); 

