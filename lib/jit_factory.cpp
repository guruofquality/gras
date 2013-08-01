// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/factory.hpp>
#include <stdexcept>

#ifdef CLANG_EXECUTABLE

#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>
#include <llvm/Module.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <map>

/***********************************************************************
 * Helper function to call a clang compliation
 **********************************************************************/
static std::string call_clang(const std::string &source, const std::vector<std::string> &flags)
{
    //make up bitcode file path
    char bitcode_file[L_tmpnam];
    std::tmpnam(bitcode_file);

    //write source to tmp file
    char source_file[L_tmpnam];
    std::tmpnam(source_file);
    std::ofstream source_fstream(source_file);
    source_fstream << source;
    source_fstream.close();

    //begin command setup
    std::vector<std::string> cmd;
    cmd.push_back(BOOST_STRINGIZE(CLANG_EXECUTABLE));
    cmd.push_back("-emit-llvm");

    //inject source
    cmd.push_back("-c");
    cmd.push_back("-x");
    cmd.push_back("c++");
    cmd.push_back(source_file);

    //inject output
    cmd.push_back("-o");
    cmd.push_back(bitcode_file);

    //inject args...
    BOOST_FOREACH(const std::string &flag, flags)
    {
        cmd.push_back(flag.c_str());
    }

    //format command string
    std::string command;
    BOOST_FOREACH(const std::string &c, cmd)
    {
        command += c + " ";
    }
    std::cout << "  " << command << std::endl;
    const int ret = system(command.c_str());
    if (ret != 0)
    {
        throw std::runtime_error("GRAS compiler: error system exec clang");
    }

    //readback bitcode for result
    std::ifstream bitcode_fstream(bitcode_file);
    return std::string((std::istreambuf_iterator<char>(bitcode_fstream)), std::istreambuf_iterator<char>());
}

/***********************************************************************
 * Storage for execution engines
 **********************************************************************/
struct ExecutionEngineMonitor
{
    ExecutionEngineMonitor(void)
    {
        boost::mutex::scoped_lock l(mutex);
    }

    ~ExecutionEngineMonitor(void)
    {
        boost::mutex::scoped_lock l(mutex);
        for (size_t i = 0; i < ees.size(); i++)
        {
            ees[i]->runStaticConstructorsDestructors(true);
            ees[i].reset();
        }
        ees.clear();
    }

    void add(boost::shared_ptr<llvm::ExecutionEngine> ee)
    {
        boost::mutex::scoped_lock l(mutex);
        ee->runStaticConstructorsDestructors(false);
        ees.push_back(ee);
    }

    llvm::LLVMContext &get_context(void)
    {
        return context;
    }

    boost::mutex mutex;
    llvm::LLVMContext context;
    std::vector<boost::shared_ptr<llvm::ExecutionEngine> > ees;
};

static ExecutionEngineMonitor &get_eemon(void)
{
    static ExecutionEngineMonitor eemon;
    return eemon;
}

/***********************************************************************
 * factory compile implementation
 **********************************************************************/
void gras::jit_factory(const std::string &source, const std::vector<std::string> &flags)
{
    std::cout << "GRAS compiler: compile source into bitcode..." << std::endl;
    const std::string bitcode = call_clang(source, flags);

    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();
    std::string error;

    //create a memory buffer from the bitcode
    boost::shared_ptr<llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));

    //parse the bitcode into a module
    llvm::Module *module = llvm::ParseBitcodeFile(buffer.get(), get_eemon().get_context(), &error);
    if (not error.empty()) throw std::runtime_error("GRAS compiler: ParseBitcodeFile " + error);

    //create execution engine
    boost::shared_ptr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(module, false, &error));
    if (not error.empty()) throw std::runtime_error("GRAS compiler: ExecutionEngine " + error);
    std::cout << "GRAS compiler: execute static constructors..." << std::endl;
    get_eemon().add(ee);
}

#else //CLANG_EXECUTABLE

void Factory::compile(const std::string &, const std::vector<std::string> &)
{
    throw std::runtime_error("GRAS compiler not built with Clang support!");
}

#endif //CLANG_EXECUTABLE
