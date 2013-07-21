// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TOOL_HPP
#define INCLUDED_GRAS_TOOL_HPP

#include <vector>
#include <string>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace gras
{

fs::path get_gras_root_path(void);
fs::path get_library_module_install_path(void);
fs::path get_grc_blocks_install_path(void);
fs::path get_cmake_module_install_path(void);

struct ProcessArgs
{
    std::string action;
    std::string project;
    std::vector<std::string> sources;
    fs::path get_source_dir(void) const
    {
        //TODO its CWD for now
        return fs::current_path();
    }
    fs::path get_build_dir(void) const
    {
        //TODO its CWD for now
        return fs::current_path() / "build";
    }
};

int process(const ProcessArgs &);

} //namespace gras

#endif /*INCLUDED_GRAS_TOOL_HPP*/
