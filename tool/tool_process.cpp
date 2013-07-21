// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>

static int system(const std::vector<std::string> &args)
{
    std::string cmd;
    BOOST_FOREACH(const std::string &arg, args)
    {
        cmd += arg;
        cmd += " ";
    }
    std::cout << "Executing " << cmd << std::endl;
    return std::system(cmd.c_str());
}

static int system(const std::string &a0 = "", const std::string &a1 = "", const std::string &a2 = "", const std::string &a3 = "")
{
    std::vector<std::string> args;
    args.push_back(a0);
    args.push_back(a1);
    args.push_back(a2);
    args.push_back(a3);
    return system(args);
}

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

static int write_cmake_file(const gras::ProcessArgs &args)
{
    const fs::path source_dir = args.get_source_dir();
    const fs::path cmakelists_path = source_dir / "CMakeLists.txt";
    std::cout << "Creating file " << cmakelists_path << "..." << std::endl;

    //ensure that the directory exists
    if (ensure_directory(source_dir) == EXIT_FAILURE) return EXIT_FAILURE;

    //open cmakelists for writing
    std::ofstream cmakelists(cmakelists_path.string().c_str());
    if (not cmakelists.is_open())
    {
        std::cerr << "Failed to open for writing " << cmakelists_path.string() << std::endl;
        return EXIT_FAILURE;
    }

    //write the cmakelists
    cmakelists
        << "cmake_minimum_required(VERSION 2.8)" << std::endl
        << "project(" << args.project << " CXX C)" << std::endl
        << "enable_testing()" << std::endl
        << std::endl
        << "list(APPEND CMAKE_MODULE_PATH " << gras::get_cmake_module_install_path().string() << ")" << std::endl
        << "include(GRASTool)" << std::endl
        << std::endl
        << "GRAS_TOOL(" << std::endl
        << "    SOURCES" << std::endl
    ;

    BOOST_FOREACH(const std::string &source, args.sources)
    {
        cmakelists << "    " << source << std::endl;
    }

    cmakelists
        << "    PROJECT " << args.project << std::endl
        << ")" << std::endl
        << std::endl
    ;
    cmakelists.close();

    return EXIT_SUCCESS;
}

int gras::process(const ProcessArgs &args)
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
    if (not args.sources.empty() and write_cmake_file(args) == EXIT_FAILURE)
    {
        std::cerr << "Failed to write the CMakeLists.txt!" << std::endl;
        return EXIT_FAILURE;
    }

    //ensure that build dir exists
    const fs::path build_dir = args.get_build_dir();
    if (ensure_directory(build_dir) == EXIT_FAILURE) return EXIT_FAILURE;

    if (args.action == "configure")
    {
        const fs::path source_dir = args.get_source_dir();
        fs::current_path(build_dir);
        return system("cmake", source_dir.string());
    }
    else if (args.action == "build")
    {
        fs::current_path(build_dir);
        return system("make");
    }
    else if (args.action == "clean")
    {
        fs::current_path(build_dir);
        return system("make", "clean");
    }
    else if (args.action == "install")
    {
        fs::current_path(build_dir);
        return system("make", "install");
    }
    else if (args.action == "uninstall")
    {
        fs::current_path(build_dir);
        return system("make", "uninstall");
    }

    return EXIT_SUCCESS;
}
