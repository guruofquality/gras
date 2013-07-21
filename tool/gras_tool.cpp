// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    //variables to be set by po
    gras::ProcessArgs process_args;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("print-gras-root-dir", "print the GRAS root install directory")
        ("print-cmake-dir", "print the GRAS CMake module directory")
        ("action", po::value(&process_args.action), "build, clean, install, uninstall the sources")
        ("project", po::value(&process_args.project), "unique project name")
        ("sources", po::value(&process_args.sources), "list of grc, python, c++ sources")
    ;

    //setup positional program options
    po::positional_options_description p;
    p.add("sources", -1);

    //setup the variables map and parse
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(p).run(), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help"))
    {
        std::cout << boost::format("GRAS Tool %s") % desc << std::endl;
        return EXIT_FAILURE;
    }

    //print directories if queried for by the args
    if (vm.count("print-gras-root-dir")) {std::cout << gras::get_gras_root_path().string() << std::endl; return EXIT_SUCCESS;}
    if (vm.count("print-cmake-dir")) {std::cout << gras::get_cmake_module_install_path().string() << std::endl; return EXIT_SUCCESS;}

    //call into the action processor
    if (vm.count("action")) return gras::handle(process_args);

    return EXIT_SUCCESS;
}
