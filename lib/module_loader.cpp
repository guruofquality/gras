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
    if (not fs::exists(path)) return;
    if (fs::is_regular_file(path))
    {
        const std::string mod_path = path.string();
        if (not load_module_in_path(mod_path.c_str()))
        {
            std::cerr << "GRAS Module loader fail: " << mod_path << std::endl;
        }
        return;
    }
    if (fs::is_directory(path)) for(
        fs::directory_iterator dir_itr(path);
        dir_itr != fs::directory_iterator();
        ++dir_itr
    ) load_all_modules_in_path(dir_itr->path());
}

static void load_modules_from_paths(const std::string &paths, const fs::path &suffix)
{
    if (paths.empty()) return;
    BOOST_FOREACH(const std::string &path, boost::tokenizer<boost::char_separator<char> > (paths, boost::char_separator<char>(SEP)))
    {
        if (path.empty()) continue;
        load_all_modules_in_path(fs::path(path) / suffix);
    }
}

static std::string my_get_env(const std::string &name, const std::string &defalt)
{
    const char *env_var = std::getenv(name.c_str());
    return (env_var != NULL)? env_var : defalt;
}

//used by jit factory, but defined here for lazyness
std::string get_gras_runtime_include_path(void)
{
    const std::string root = my_get_env("GRAS_ROOT", "@GRAS_ROOT@");
    const fs::path inc_path = fs::path(root) / "include";
    return inc_path.string();
}

GRAS_STATIC_BLOCK(gras_module_loader)
{
    //!search the GRAS_ROOT directory for this install
    load_modules_from_paths(my_get_env("GRAS_ROOT", "@GRAS_ROOT@"), fs::path("") / "lib@LIB_SUFFIX@" / "gras" / "modules");

    //!search the GRAS_PATH search directories for modules
    load_modules_from_paths(my_get_env("GRAS_PATH", ""), fs::path("") / "lib@LIB_SUFFIX@" / "gras" / "modules");

    //!search the explicit module paths
    load_modules_from_paths(my_get_env("GRAS_MODULE_PATH", ""), fs::path(""));
}
