#ifndef _PROGRAM_OPTIONS_H_1
#define _PROGRAM_OPTIONS_H_1
/*************************************************************************
	> File Name:    program_options.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/29 9:06:26
 ************************************************************************/

#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
//#include <set>
#include <cstddef>
#include <sstream>
#include <assert.h>
#include <exception>

class program_options {
public:
    class parameter_error : public std::exception {
    public:
        parameter_error(const std::string& option) noexcept 
            : m_msg("Exception: program_options::parameter_error: " + option) {}
        virtual ~parameter_error (void) noexcept {}
        virtual const char* what() const noexcept {
            return m_msg.c_str();
        }
    private:
        std::string m_msg;
    };
public:
    explicit program_options(const std::string& usage) : m_usage(usage) {}
    void add_option(const std::string& options, const std::string& message) {
        m_max_options_str_length = std::max(m_max_options_str_length, 
                options.length());
        m_helpmessage.push_back({options, message});

        // 分割 options  (", "),  并存放到 m_opts_set 中去, 
        // 以便验证 经store 的参数是否符合要求.
        // TODO
    }


    void example(const std::string& message) {
        m_examples.push_back(message); 
    }

    const std::string show_help(const std::string& title = "") const {
        std::ostringstream message;
        // show Title
        if (title != "") {
            message << title << std::endl << std::endl;
        }
        // show usage
        message << "Usage: " << m_usage << std::endl << std::endl;

        // show options
        message << "Options: " << std::endl;

        for (auto& v : m_helpmessage) {
            // 分割 v.help, 分割符'\n'
            std::vector<std::string> helplines;
            std::istringstream iss(v.help);
            std::string line;
            while (getline(iss, line, '\n')) {
                helplines.push_back(line);
            }
            assert(helplines.size() >= 1);
            message << "  " << std::left << std::setw(m_max_options_str_length)
                <<  v.options << "  "  << helplines[0] << std::endl;
            for (auto cit = helplines.cbegin() +1; 
                    cit != helplines.cend(); ++cit) {
                message << "  " //<< std::left 
                    << std::setw(m_max_options_str_length) << ' ' << "  "
                    << *cit << std::endl;
            }
        }

        // show example
        if (m_examples.size()) {
            message << std::endl << "Examples:" << std::endl;
            for (auto& v : m_examples) {
                message << "  " << v << std::endl;
            }
        }
        return message.str();
    }

    void store(int argc, char** argv) {
        m_argc = argc; 
        for (int i = 0; i < argc; ++i) {
            m_argv.push_back(argv[i]);
        }
    }

    bool count(const std::string& option) const {
        auto ret = std::find(m_argv.begin(), m_argv.end(), option);
        return ret != m_argv.end();
    }

    bool empty(void) const {
        // *unix
        // TODO
        return m_argc == 1;
    }

    std::string get(const std::string& option) const {
        // 在 argv 中找 option
        for (auto cit = m_argv.cbegin(); cit != m_argv.cend(); ++cit) {
            if (option == *cit) {
                auto ret = cit + 1;
                if (ret != m_argv.cend()) {
                    return *(cit+1); 
                }
                else {
                    // 参数错误
                    throw parameter_error(option); 
                }
            }
        }
        // 没有找到
        return "";
    }

private:
    struct opts_help_t {
        std::string options;
        std::string help;
    };
    std::string    m_usage;
    std::size_t    m_max_options_str_length = 0;
    std::vector<opts_help_t> m_helpmessage;
    std::vector<std::string> m_examples;
    int                      m_argc;
    std::vector<std::string> m_argv;
};

#endif // _PROGRAM_OPTIONS_H_1
