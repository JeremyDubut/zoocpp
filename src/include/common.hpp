#pragma once
#include <vector>
#include <unordered_map>
#include <string>


struct value_t;
struct term_t;
struct raw_t;
struct context_t;
struct inferrance_t;
struct closure_t;
struct renaming_t;
typedef std::shared_ptr<value_t> value_ptr;
typedef std::shared_ptr<term_t> term_ptr;
typedef std::shared_ptr<raw_t> raw_ptr;
typedef std::vector<value_ptr> environment_t;
// typedef std::unordered_map<std::string,std::pair<value_ptr,std::size_t>> types_t;
typedef std::unordered_map<std::string,std::vector<std::pair<value_ptr,std::size_t>>> types_t;
typedef std::string name_t;
typedef std::vector<name_t> names_t;
typedef std::optional<value_ptr> metaentry_t;
typedef std::vector<metaentry_t> metadata_t;
typedef std::vector<bool> flags_t;
typedef std::vector<value_ptr> spine_t;
typedef std::unordered_map<std::size_t,std::size_t> renamingFun_t;


#define FRESHMETA std::make_shared<imeta_t>(cont.flags)

