// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TOOL_HPP
#define INCLUDED_GRAS_TOOL_HPP

#include <vector>
#include <string>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace gras
{

fs::path get_gras_root_dir(void);
fs::path get_cmake_module_install_dir(void);
fs::path get_source_dir(void);
fs::path get_build_dir(void);
fs::path get_cmake_executable_path(void);
fs::path get_make_executable_path(void);

struct CustomArgs
{
    std::string cmake_path;
    std::string make_path;
    std::string source_dir;
    std::string build_dir;
};

void customize(const CustomArgs &);

struct HandlerArgs
{
    std::string action;
    std::string project;
    std::vector<std::string> sources;
};

int handle(const HandlerArgs &);

int system(const std::vector<std::string> &args);
inline int system(const std::string &a0 = "", const std::string &a1 = "", const std::string &a2 = "", const std::string &a3 = "")
{
    std::vector<std::string> args;
    args.push_back(a0);
    args.push_back(a1);
    args.push_back(a2);
    args.push_back(a3);
    return system(args);
}

int write_cmake_file(const gras::HandlerArgs &args);

} //namespace gras

#endif /*INCLUDED_GRAS_TOOL_HPP*/
