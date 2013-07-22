// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    //variables to be set by po
    gras::CustomArgs args;

    //setup the program options
    po::options_description visible("");
    visible.add_options()
        ("help", "help message")
        ("project", po::value(&args.project), "unique project and install target name")
        ("source-dir", po::value(&args.source_dir), "set the source directory [default is cwd]")
        ("build-dir", po::value(&args.build_dir), "set the build directory [default is cwd/build]")
        ("cmake-path", po::value(&args.cmake_path), "override path to the cmake exectuable")
        ("make-path", po::value(&args.make_path), "override path to the make exectuable")
        ("print-root-dir", "print the GRAS root install directory")
        ("print-cmake-dir", "print the GRAS CMake module directory")
    ;

    //hidden and position options
    po::options_description hidden("Hidden options");
    hidden.add_options()("other", po::value(&args.other));
    po::positional_options_description p;
    p.add("other", -1);

    //for real description has hidden and visible
    po::options_description desc;
    desc.add(visible).add(hidden);

    //setup the variables map and parse
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(p).run(), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help"))
    {
        std::cout <<
            "Usage: gras_tool [options] [action] sources...\n"
            "\n"
            "GRAS Tool allows users to painlessly compile\n"
            "and install their sources into the block factory.\n"
            "The tool should be used for the most common use case.\n"
            "Users who need custom libraries, type registrations,\n"
            "or external dependencies should create a cmake project,\n"
            "and directly use the GRAS_TOOL cmake utility.\n"
            "\n"
            "Typical usage:\n"
            "   gras_tool configure *.cpp *.py\n"
            "   gras_tool build\n"
            "   sudo gras_tool install\n"
            "\n"
            << visible << std::endl;
        return EXIT_FAILURE;
    }

    //inject custom args if provided
    gras::inject_args(args);

    //print directories if queried for by the args
    if (vm.count("print-root-dir")) {std::cout << gras::get_gras_root_dir().string() << std::endl; return EXIT_SUCCESS;}
    if (vm.count("print-cmake-dir")) {std::cout << gras::get_cmake_module_install_dir().string() << std::endl; return EXIT_SUCCESS;}

    //call into the action processor
    return gras::handle_action();
}
