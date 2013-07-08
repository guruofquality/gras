// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/gras.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdlib>

namespace fs = boost::filesystem;

#ifdef BOOST_WINDOWS_API
    #include <windows.h>
    static const char *SEP = ";";
    static bool load_module_in_path(const char *path)
    {
        return LoadLibrary(path) != NULL;
    }
#else
    #include <dlfcn.h>
    static const char *SEP = ":";
    static bool load_module_in_path(const char *path)
    {
        return dlopen(path, RTLD_LAZY) != NULL;
    }
#endif

static void load_all_modules_in_path(const fs::path &path)
{
    if (not fs::exists(fs::path(path))) return;
    if (not fs::is_directory(fs::path(path))) return;
    for(
        fs::directory_iterator dir_itr(path);
        dir_itr != fs::directory_iterator();
        ++dir_itr
    ){
        const std::string mod_path = dir_itr->path().string();
        if (not load_module_in_path(mod_path.c_str()))
        {
            std::cerr << "GRAS Module loader fail: " << mod_path << std::endl;
        }
    }
}

GRAS_STATIC_BLOCK(gras_module_loader)
{
    std::string search_paths = "@GRAS_MODULE_PATH@";
    const char *module_path_env = std::getenv("GRAS_MODULE_PATH");
    if (module_path_env != NULL)
    {
        search_paths += SEP;
        search_paths += module_path_env;
    }
    if (search_paths.empty()) return;
    BOOST_FOREACH(const std::string &path, boost::tokenizer<boost::char_separator<char> > (search_paths, boost::char_separator<char>(SEP)))
    {
        if (path.empty()) continue;
        load_all_modules_in_path(fs::path(path));
    }
}
