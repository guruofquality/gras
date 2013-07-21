// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    //variables to be set by po
    gras::HandlerArgs process_args;
    gras::CustomArgs custom_args;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("action", po::value(&process_args.action), "build, clean, install, uninstall the sources")
        ("project", po::value(&process_args.project), "unique project and install target name")
        ("sources", po::value(&process_args.sources), "list of grc, python, c++ sources")
        ("source-dir", po::value(&custom_args.source_dir), "set the source directory [default is cwd]")
        ("build-dir", po::value(&custom_args.build_dir), "set the build directory [default is cwd/build]")
        ("cmake-path", po::value(&custom_args.cmake_path), "override path to the cmake exectuable")
        ("make-path", po::value(&custom_args.make_path), "override path to the make exectuable")
        ("print-root-dir", "print the GRAS root install directory")
        ("print-cmake-dir", "print the GRAS CMake module directory")
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

    //inject custom args if provided
    gras::customize(custom_args);

    //print directories if queried for by the args
    if (vm.count("print-root-dir")) {std::cout << gras::get_gras_root_dir().string() << std::endl; return EXIT_SUCCESS;}
    if (vm.count("print-cmake-dir")) {std::cout << gras::get_cmake_module_install_dir().string() << std::endl; return EXIT_SUCCESS;}

    //call into the action processor
    if (vm.count("action")) return gras::handle(process_args);

    return EXIT_SUCCESS;
}
