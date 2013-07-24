// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <boost/foreach.hpp>
#include <iostream>
#include <cstdlib>

static int ensure_directory(const fs::path &path)
{
    if (fs::exists(path) and not fs::is_directory(path))
    {
        std::cerr << "The specified directory is a file " << path.string() << std::endl;
        return EXIT_FAILURE;
    }
    else if (not fs::exists(path) and not fs::create_directory(path))
    {
        std::cerr << "Failed to create the directory " << path.string() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int gras::handle_action(void)
{
    if (gras::get_action().empty()) return EXIT_SUCCESS;

    //check that sources exist
    BOOST_FOREACH(const std::string &source, gras::get_sources())
    {
        const fs::path path(source);
        if (not fs::exists(path))
        {
            std::cerr << "Source " << source << " does not exist!" << std::endl;
            return EXIT_FAILURE;
        }
        if (not fs::is_regular_file(path))
        {
            std::cerr << "Source " << source << " is not a file!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    //validate input
    if (gras::get_project().empty())
    {
        std::cerr << "The user must specify a project name with --project" << std::endl;
        return EXIT_FAILURE;
    }

    //write a new cmakelists if sources are specified
    if (not gras::get_sources().empty() and gras::write_cmake_file() == EXIT_FAILURE)
    {
        std::cerr << "Failed to write the CMakeLists.txt!" << std::endl;
        return EXIT_FAILURE;
    }

    //source and build directories
    const fs::path source_dir = gras::get_source_dir();
    const fs::path build_dir = gras::get_build_dir();
    const fs::path cmake_path = gras::get_cmake_executable_path();
    const fs::path make_path = gras::get_make_executable_path();

    //build the cmake project and build directory
    if (gras::get_action() == "configure")
    {
        if (ensure_directory(build_dir) == EXIT_FAILURE) return EXIT_FAILURE;
        fs::current_path(build_dir);
        std::cout << "Configuring " << gras::get_project()  << "..." << std::endl;
        if (gras::system(cmake_path.string(), source_dir.string(),
            "-DCMAKE_INSTALL_PREFIX="+gras::get_gras_root_dir().string()
        ) == EXIT_FAILURE) return EXIT_FAILURE;
    }

    //compile into the build directory
    else if (gras::get_action() == "build")
    {
        if (not fs::exists(build_dir / "CMakeCache.txt"))
        {
            std::cerr << "No project has been configured!" << std::endl;
            return EXIT_FAILURE;
        }
        fs::current_path(build_dir);
        std::cout << "Building " << gras::get_project()  << "..." << std::endl;
        return gras::system(make_path.string());
    }

    //remove the build directory
    else if (gras::get_action() == "clean")
    {
        std::cout << "Cleaning " << gras::get_project()  << "..." << std::endl;
        const size_t num_removed = fs::remove_all(build_dir);
        if (num_removed == 0)
        {
            std::cerr << "Failed to clean " << build_dir << std::endl;
            return EXIT_FAILURE;
        }
    }

    //install build products
    else if (gras::get_action() == "install")
    {
        std::cout << "Installing " << gras::get_project()  << "..." << std::endl;
        fs::current_path(build_dir);
        return gras::system(make_path.string(), "install");
    }

    //uninstall build products
    else if (gras::get_action() == "uninstall")
    {
        std::cout << "Uninstalling " << gras::get_project()  << "..." << std::endl;
        fs::current_path(build_dir);
        return gras::system(make_path.string(), "uninstall_"+gras::get_project());
    }

    else
    {
        std::cerr << "Unknown action " << gras::get_action() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "sucess!" << std::endl;

    return EXIT_SUCCESS;
}
