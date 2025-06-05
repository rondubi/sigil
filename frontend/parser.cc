#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr int PRINT_DEBUG = 0;

#define prs_printf(...) \
        if constexpr (PRINT_DEBUG) \
                printf(__VA_ARGS__);

enum class LineKind
{
        ModuleStart,
        ComponentDecl,
        ModuleInst,
        Assignment,
        BlockEnd,
        SystemTask,
        Unknown
};

struct Match
{
        LineKind kind;
        std::smatch groups;
};

// rdubi's note: completely fucking unreadable but yolo
// poor man's Bison + Flex
std::optional<Match> classify(const std::string & line)
{
        // NOTE (dub): major problem arose from identifiers in argument list
        // being unable to include numbers This whole classification function is
        // held together by snot and the hopes and dreams of small children
        static const std::regex module_start(
                R"(module[ ]+([a-zA-Z_]+)[ ]*\(([a-zA-Z][a-zA-Z_0-9]*([ ]*,[ ]*[a-zA-Z][a-zA-Z_0-9]*)*)\)[ ]*->[ ]*([a-zA-Z][a-zA-Z_0-9]*([ ]*,[ ]*[a-zA-Z][a-zA-Z_0-9]*)*)[ ]*\{)",
                std::regex::extended);

        static const std::regex component_decl(
                "^[[:space:]]*(dust|torch|repeater<[1-4]>|clock<[0-9]+[ ]*,[ ]*[0-9]+>|subtract_comparator|compare_comparator)[[:space:]]"
                "+([[:alnum:]_]+)[[:space:]]*;[[:space:]]*$",
                std::regex::extended);

        static const std::regex module_inst(
                R"(^[[:space:]]*([a-zA-Z_][a-zA-Z_0-9]*)[[:space:]]+([a-zA-Z_][a-zA-Z_0-9]*)[[:space:]]*\(([[:space:]]*[a-zA-Z_][a-zA-Z_0-9]*(\.[a-zA-Z_][a-zA-Z_0-9]*)?[[:space:]]*(,[[:space:]]*[a-zA-Z_][a-zA-Z_0-9]*(\.[a-zA-Z_][a-zA-Z_0-9]*)?[[:space:]]*)*)?\)[[:space:]]*;[[:space:]]*$)",
                std::regex::extended);

        static const std::regex assignment(
                "^[[:space:]]*([[:alnum:]_]+)[[:space:]]*=[[:space:]]*([[:"
                "alnum:]_\\.]+)[[:space:]]*;[[:space:]]*$",
                std::regex::extended);
        static const std::regex block_end(
                "^[[:space:]]*\\}[[:space:]]*$", std::regex::extended);
        static const std::regex system_task(
                R"(\$([a-zA-Z_][a-zA-Z_0-9]*)\([ ]*([0-9]+)[ ]*,[ ]*([a-zA-Z_][a-zA-Z_0-9]*)\(([0-9]+([ ]*,[ ]*[0-9]+)*)\)\.([a-zA-Z_][a-zA-Z_0-9]*)[ ]*,[ ]*([0-9]+)\);)",
                std::regex::extended);


        std::smatch m;
        if (std::regex_match(line, m, module_start))
                return Match{LineKind::ModuleStart, m};
        if (std::regex_match(line, m, component_decl))
                return Match{LineKind::ComponentDecl, m};
        if (std::regex_match(line, m, module_inst))
                return Match{LineKind::ModuleInst, m};
        if (std::regex_match(line, m, assignment))
                return Match{LineKind::Assignment, m};
        if (std::regex_match(line, m, block_end))
                return Match{LineKind::BlockEnd, m};
        if (std::regex_match(line, m, system_task))
                return Match{LineKind::SystemTask, m};

        prs_printf("Failed to match %s\n", line.c_str());

        return std::nullopt;
}

// dub note: stupid state machine
enum class State
{
        OutsideModuleDecl,
        InsideModuleDecl,
};

State cur_state = State::OutsideModuleDecl;

#define ASSERT_OUT_MODULE \
        assert(cur_state == State::OutsideModuleDecl \
               && "Must be outside module declaration");
#define ASSERT_IN_MODULE \
        assert(cur_state == State::InsideModuleDecl \
               && "Must be inside module declaration");

struct Component
{
        std::string name;
        std::string type;
        std::vector<std::string> prerequisites;
};

struct ModuleState
{
        std::string module_name;
        std::vector<std::string> named_inputs;
        std::vector<Component> components;
        std::unordered_map<std::string, std::vector<std::string>>
                named_outputs_and_sources;
};

ModuleState module_state;

struct SysTask
{
        enum class Task
        {
                Print,
                Assert,
        };

        Task task;
        int timestamp;
        int expect;
        std::vector<int> args;
        std::string module_name;
        std::string field_name;
};

std::vector<SysTask> tasks = {};

// TODO: save system tasks

std::optional<std::reference_wrapper<Component>>
find_component(const std::string & name)
{
        auto res = std::find_if(
                module_state.components.begin(),
                module_state.components.end(),
                [name](const Component & c) { return c.name == name; });
        if (res != module_state.components.cend())
                return {*res};

        return std::nullopt;
};

std::vector<std::string> parse_list(const std::string & input)
{
        std::vector<std::string> result;
        std::stringstream ss(input);
        std::string item;

        while (std::getline(ss, item, ','))
        {
                // Trim leading/trailing spaces
                item.erase(
                        item.begin(),
                        std::find_if(
                                item.begin(),
                                item.end(),
                                [](char ch) { return !std::isspace(ch); }));
                item.erase(
                        std::find_if(
                                item.rbegin(),
                                item.rend(),
                                [](char ch) { return !std::isspace(ch); })
                                .base(),
                        item.end());

                if (!item.empty())
                        result.push_back(item);
        }

        return result;
}

std::vector<int> split_to_ints(const std::string & input)
{
        std::vector<int> result;
        std::stringstream ss(input);
        std::string token;

        while (std::getline(ss, token, ','))
        {
                // Trim whitespace
                token.erase(
                        token.begin(),
                        std::find_if(
                                token.begin(),
                                token.end(),
                                [](char c) { return !std::isspace(c); }));
                token.erase(
                        std::find_if(
                                token.rbegin(),
                                token.rend(),
                                [](char c) { return !std::isspace(c); })
                                .base(),
                        token.end());

                if (!token.empty())
                        result.push_back(std::stoi(token));
        }

        return result;
}

std::unordered_map<std::string, std::vector<std::string>>
make_map(std::vector<std::string> v)
{
        std::unordered_map<std::string, std::vector<std::string>> res;
        for (auto & s : v)
                res[s] = {};
        return res;
}

void process_module_start(std::smatch s)
{
        ASSERT_OUT_MODULE;

        module_state.module_name = s[1];
        module_state.named_inputs = parse_list(s[2]);
        module_state.named_outputs_and_sources = make_map(parse_list(s[4]));

        prs_printf("Got module named %s\n", module_state.module_name.c_str());

        cur_state = State::InsideModuleDecl;
        ASSERT_IN_MODULE;
}

void process_component_decl(std::smatch s)
{
        ASSERT_IN_MODULE;

        module_state.components.push_back({
                .name = s[2],
                .type = s[1],
                .prerequisites = {},
        });

        prs_printf(
                "Got component of type %s with name %s\n",
                module_state.components.back().type.c_str(),
                module_state.components.back().name.c_str());
}

void process_module_instantiation(std::smatch s)
{
        ASSERT_IN_MODULE;

        module_state.components.push_back({
                .name = s[2],
                .type = s[1],
                .prerequisites = parse_list(s[3]),
        });
}

void process_assignment(std::smatch s)
{
        ASSERT_IN_MODULE;

        std::optional<std::reference_wrapper<Component>> maybe_c
                = find_component(s[1]);
        if (maybe_c.has_value())
        {
                Component & c = *maybe_c;
                c.prerequisites.push_back(s[2]);
                prs_printf(
                        "Component %s has prerequisite %s\n",
                        c.name.c_str(),
                        c.prerequisites.back().c_str());
        }
        else
        {
                if (!module_state.named_outputs_and_sources.contains(s[1]))
                {
                        prs_printf(
                                "Can't find component or result %s\n",
                                s[1].str().c_str());
                        assert(false);
                }

                module_state.named_outputs_and_sources[s[1].str()].push_back(
                        s[2].str());
                prs_printf(
                        "Return value %s has prerequisite %s\n",
                        s[1].str().c_str(),
                        module_state.named_outputs_and_sources[s[1]]
                                .back()
                                .c_str());
        }
}

SysTask::Task get_task_type(const std::string & s)
{
        if (s == "print")
                return SysTask::Task::Print;
        if (s == "expect")
                return SysTask::Task::Assert;

        assert(false);
}

void process_system_task(std::smatch s)
{
        ASSERT_OUT_MODULE;

        tasks.push_back((SysTask){
                .task = get_task_type(s[1]),
                .timestamp = std::stoi(s[2]),
                .expect
                = std::stoi(s[7]), // will need to be used for prints for now
                .args = split_to_ints(s[4]),
                .module_name = s[3].str(),
                .field_name = s[6],

        });
        prs_printf(
                "Have task called %s at time %d with expect %d on module %s, "
                "field %s\n",
                s[1].str().c_str(),
                tasks.back().timestamp,
                tasks.back().expect,
                tasks.back().module_name.c_str(),
                tasks.back().field_name.c_str());
}

void clear_state()
{
        module_state.module_name.clear();
        module_state.named_inputs.clear();
        module_state.components.clear();
        module_state.named_outputs_and_sources.clear();
}

void emit_res_t()
{
        std::cout << "struct module_" << module_state.module_name << "_res_t\n";
        std::cout << "{\n";
        for (const auto & [res, _] : module_state.named_outputs_and_sources)
                std::cout << "\tbool " << res << ";\n";
        std::cout << "};\n";
}

void emit_type()
{
        std::cout << "using module_" << module_state.module_name
                  << " = std::function<module_" << module_state.module_name
                  << "_res_t(int)>;\n";
}

void emit_signature()
{
        std::cout << "std::shared_ptr<module_" << module_state.module_name
                  << "> make_" << module_state.module_name << "(";

        bool comma = false;
        for (const std::string & s : module_state.named_inputs)
        {
                if (comma)
                        std::cout << ", ";
                std::cout << "sigil::Queryable " << s;
                comma = true;
        }
        std::cout << ") {\n";
}

bool is_sigil_type(const std::string & s)
{
        if (s.starts_with("clock"))
                return true;
        return s == "torch" || s == "dust" || s == "repeater<1>"
                || s == "repeater<2>" || s == "repeater<3>"
                || s == "repeater<4>" || s == "subtract_comparator"
                || s == "compare_comparator";
}

struct split_name
{
        std::string name;
        std::optional<std::string> field;
};

split_name splitQualifiedName(const std::string & input)
{
        size_t dotPos = input.find('.');
        if (dotPos == std::string::npos)
        {
                return {
                        .name = input,
                        .field = std::nullopt,
                };
        }
        return {.name = input.substr(0, dotPos),
                .field = input.substr(dotPos + 1)};
}

void emit_prereq_smart(const std::string & prereq)
{
        split_name split = splitQualifiedName(prereq);
        if (!split.field)
        {
                std::cout << prereq;
                return;
        }

        std::cout << "std::make_shared<sigil::QueryableFn>(";
        std::cout << "[" << split.name << "](int timestamp) { return (*"
                  << split.name;
        std::cout << ")(timestamp)." << split.field.value() << "; })";
}

void emit_components()
{
        for (Component & c : module_state.components)
        {
                std::cout << "\tauto " << c.name << " = ";
                // TODO: clock support
                if (is_sigil_type(c.type))
                        std::cout << "sigil::";
                std::cout << "make_" << c.type << "(";
                if (c.type == "torch" || c.type == "dust")
                        std::cout << "{";

                if (c.type == "subtract_comparator" || c.type == "compare_comparator")
                {
                        assert(c.prerequisites.size() <= 3 && "Comparators must have no more than three prerequisites");
                        while (c.prerequisites.size() < 3)
                                c.prerequisites.push_back("sigil::make_pulse_set({})");
                }

                bool comma = false;
                for (const std::string & prereq : c.prerequisites)
                {
                        if (comma)
                                std::cout << ", ";
                        emit_prereq_smart(prereq);
                        comma = true;
                }
                if (c.type == "torch" || c.type == "dust")
                        std::cout << "}";
                std::cout << ");\n";
        }
}

void emit_return()
{
        std::unordered_set<std::string> return_prereqs;
        std::unordered_map<std::string, std::string> prereq_fields;

        for (const auto & [_, ret_p] : module_state.named_outputs_and_sources)
                for (const std::string & s : ret_p)
                {
                        split_name split = splitQualifiedName(s);
                        if (!split.field)
                                return_prereqs.insert(s);
                        else
                        {
                                return_prereqs.insert(split.name);
                                prereq_fields[split.name] = split.field.value();
                        }
                }

        std::cout << "\treturn std::make_shared<module_"
                  << module_state.module_name << ">(\n"
                  << "\t\t[";

        bool comma = false;
        for (const std::string & s : return_prereqs)
        {
                if (comma)
                        std::cout << ", ";
                std::cout << s;
                comma = true;
        }

        std::cout << "] (int timestamp) -> module_";
        std::cout << module_state.module_name << "_res_t {\n";
        std::cout << "\t\treturn {\n";

        for (const auto & [res_field, sources] :
             module_state.named_outputs_and_sources)
        {
                std::cout << "\t\t\t." << res_field << " = ";

                bool write_or = false;
                for (const std::string & src : sources)
                {
                        if (write_or)
                                std::cout << " || ";

                        split_name split = splitQualifiedName(src);
                        std::cout << "(*" << split.name << ")(timestamp)";
                        if (split.field)
                                std::cout << "." << split.field.value();
                        write_or = true;
                }
                std::cout << ",\n";
        }

        std::cout << "\t\t};\n";
        std::cout << "\t}\n";
        std::cout << "\t);\n";
}

void emit_body()
{
        emit_components();
        emit_return();
}

void emit_mk_fn()
{
        emit_signature();
        emit_body();

        std::cout << "}\n";
}

void emit_state()
{
        emit_res_t();
        emit_type();
        emit_mk_fn();
}

void process_block_end()
{
        ASSERT_IN_MODULE;

        emit_state();
        clear_state();

        cur_state = State::OutsideModuleDecl;
        ASSERT_OUT_MODULE;
}

int temp_name_idx = 0;

void emit_task(const SysTask & task)
{
        std::cout << "\tauto t" << temp_name_idx << " = make_"
                  << task.module_name << "(";
        bool comma = false;
        for (int arg : task.args)
        {
                if (comma)
                        std::cout << ", ";
                std::cout << "sigil::make_pulse_set({";
                // NOTE: we allow a pulse of something at 0 for now
                if (arg)
                        std::cout << "0";
                std::cout << "})";
                comma = true;
        }

        std::cout << ");\n";

        switch (task.task)
        {
                case SysTask::Task::Print:
                        std::cout << "\tstd::cout << (*t" << temp_name_idx
                                  << ")(" << task.timestamp << ")."
                                  << task.field_name << " << std::endl;;\n";
                        break;
                case SysTask::Task::Assert:
                        std::cout << "\tassert((*t" << temp_name_idx << ")("
                                  << task.timestamp << ")." << task.field_name
                                  << " == " << task.expect << ");\n";
                        break;
        }
        ++temp_name_idx;
}

void emit_main()
{
        std::cout << "int main() {\n";
        for (const SysTask & task : tasks)
                emit_task(task);

        std::cout << "\treturn 0;\n";
        std::cout << "}\n";
}

void process_line(const std::string & line)
{
        auto result = classify(line);
        if (!result)
        {
                // std::cerr << "[unrecognized line]: " << line << "\n";
                return;
        }

        const auto & [kind, m] = *result;

        switch (kind)
        {
                case LineKind::ModuleStart:
                        // std::cout << "Module: " << m[1]
                        //           << " args: " << m[2]
                        //           << " results: " << m[4] << "\n";
                        process_module_start(m);
                        return;
                case LineKind::ComponentDecl:
                        // std::cout << "Component: type=" << m[1]
                        //           << " name=" << m[2] << "\n";
                        process_component_decl(m);
                        return;
                case LineKind::ModuleInst:
                        // std::cout << "Module inst: type=" << m[1]
                        //           << " name=" << m[2] << " args=" << m[3]
                        //           << "\n";
                        process_module_instantiation(m);
                        return;
                case LineKind::Assignment:
                        // std::cout << "Assignment: " << m[1] << " = "
                        //           << m[2] << "\n";
                        process_assignment(m);
                        return;
                case LineKind::SystemTask:
                        // std::cout << "System task: $" << m[1] << "(" << m[2]
                        //           << ")\n";
                        process_system_task(m);
                        return;
                case LineKind::BlockEnd:
                        // std::cout << "End of block\n";
                        process_block_end();
                        return;
                default:
                        // std::cout << "Quietly ignore failure\n";
                        return;
        }
}

void emit_includes()
{
        std::cout << "#include \"sigil.hh\"\n";
        std::cout << "#include <cassert>\n";
        std::cout << "#include <memory>\n";
        std::cout << "#include <iostream>\n";
        std::cout << "#include <functional>\n";
}

void process_file(const std::string & file_name)
{
        std::ifstream f(file_name);
        std::string line;
        while (std::getline(f, line))
                process_line(line);
}

int main(int argc, char ** argv)
{
        assert(argc == 2);

        emit_includes();

        process_file(std::string(argv[1]));

        emit_main();

        // TODO: includes

        return 0;
}
