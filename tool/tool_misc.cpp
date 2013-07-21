// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>

int gras::system(const std::vector<std::string> &args)
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

int gras::write_cmake_file(const gras::HandlerArgs &args)
{
    const fs::path source_dir = gras::get_source_dir();
    const fs::path cmakelists_path = source_dir / "CMakeLists.txt";
    std::cout << "Creating file " << cmakelists_path << "..." << std::endl;

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
        << "list(APPEND CMAKE_MODULE_PATH " << gras::get_cmake_module_install_dir().string() << ")" << std::endl
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
