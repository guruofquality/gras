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

int gras::handle(const HandlerArgs &args)
{
    //check that sources exist
    BOOST_FOREACH(const std::string &source, args.sources)
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
    if (args.project.empty())
    {
        std::cerr << "The user must specify a project name with --project" << std::endl;
        return EXIT_FAILURE;
    }

    //write a new cmakelists if sources are specified
    if (not args.sources.empty() and gras::write_cmake_file(args) == EXIT_FAILURE)
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
    if (args.action == "build")
    {
        if (ensure_directory(build_dir) == EXIT_FAILURE) return EXIT_FAILURE;
        fs::current_path(build_dir);
        std::cout << "Configuring " << args.project  << "..." << std::endl;
        if (gras::system(cmake_path.string(), source_dir.string()) == EXIT_FAILURE) return EXIT_FAILURE;
        std::cout << "Building " << args.project  << "..." << std::endl;
        return gras::system(make_path.string());
    }

    //remove the build directory
    else if (args.action == "clean")
    {
        std::cout << "Cleaning " << args.project  << "..." << std::endl;
        const size_t num_removed = fs::remove_all(build_dir);
        if (num_removed == 0)
        {
            std::cerr << "Failed to clean " << build_dir << std::endl;
            return EXIT_FAILURE;
        }
    }

    //install build products
    else if (args.action == "install")
    {
        std::cout << "Installing " << args.project  << "..." << std::endl;
        fs::current_path(build_dir);
        return gras::system(make_path.string(), "install");
    }

    //uninstall build products
    else if (args.action == "uninstall")
    {
        std::cout << "Uninstalling " << args.project  << "..." << std::endl;
        fs::current_path(build_dir);
        return gras::system(make_path.string(), "uninstall_"+args.project);
    }

    else
    {
        std::cerr << "Unknown action " << args.action << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "sucess!" << std::endl;

    return EXIT_SUCCESS;
}
