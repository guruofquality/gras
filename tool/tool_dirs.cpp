// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <cstdlib>

fs::path gras::get_gras_root_path(void)
{
    const char *gras_root_env = getenv("GRAS_ROOT");
    if (gras_root_env != NULL) return fs::path(gras_root_env);
    return fs::path("@CMAKE_INSTALL_PREFIX@");
}

fs::path gras::get_cmake_module_install_path(void)
{
    const fs::path root = gras::get_gras_root_path();
    return root / "share" / "gras" / "cmake";
}
