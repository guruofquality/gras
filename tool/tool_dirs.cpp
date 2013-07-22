// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <cstdlib>

static gras::CustomArgs args;

void gras::inject_args(const CustomArgs &a)
{
    args = a;
}

fs::path gras::get_gras_root_dir(void)
{
    const char *gras_root_env = getenv("GRAS_ROOT");
    if (gras_root_env != NULL) return fs::path(gras_root_env);
    return fs::path("@CMAKE_INSTALL_PREFIX@");
}

fs::path gras::get_cmake_module_install_dir(void)
{
    const fs::path root = gras::get_gras_root_dir();
    return root / "share" / "gras" / "cmake";
}

fs::path gras::get_source_dir(void)
{
    const fs::path path(args.source_dir);
    if (not path.empty()) return path;
    return fs::current_path();
}

fs::path gras::get_build_dir(void)
{
    const fs::path path(args.build_dir);
    if (not path.empty()) return path;
    return fs::current_path() / "build";
}

fs::path gras::get_cmake_executable_path(void)
{
    const fs::path path(args.cmake_path);
    if (not path.empty()) return path;
    return "cmake";
}

fs::path gras::get_make_executable_path(void)
{
    const fs::path path(args.make_path);
    if (not path.empty()) return path;
    return "make";
}

std::string gras::get_action(void)
{
    if (args.other.empty()) return "";
    return args.other.front();
}

std::string gras::get_project(void)
{
    if (not args.project.empty()) return args.project;
    return gras::get_source_dir().stem().string();
}

std::vector<std::string> gras::get_sources(void)
{
    std::vector<std::string> sources = args.other;
    if (not sources.empty()) sources.erase(sources.begin());
    return sources;
}
