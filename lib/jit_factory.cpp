// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/factory.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <map>

//defined in module loader
std::string get_gras_runtime_include_path(void);

#ifdef HAVE_CLANG
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/ArgList.h>
#endif //HAVE_CLANG

#ifdef HAVE_LLVM
#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Program.h>
#include <llvm/Module.h>
#endif //HAVE_LLVM

/***********************************************************************
 * Storage for execution engines
 **********************************************************************/
#ifdef HAVE_LLVM
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
#endif //HAVE_LLVM

/***********************************************************************
 * Helper function to call a clang compliation -- execs clang
 **********************************************************************/
#ifdef HAVE_LLVM
static llvm::Module *call_clang_exe(const std::string &source_file, const std::vector<std::string> &flags)
{
    //make up bitcode file path
    char bitcode_file[L_tmpnam];
    if (std::tmpnam(bitcode_file) == NULL) throw std::runtime_error("GRAS compiler: tmp bitcode file path failed");

    //begin command setup
    std::vector<std::string> cmd;
    llvm::sys::Path clangPath = llvm::sys::Program::FindProgramByName("clang");
    cmd.push_back(clangPath.str());
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
    const std::string bitcode((std::istreambuf_iterator<char>(bitcode_fstream)), std::istreambuf_iterator<char>());

    //create a memory buffer from the bitcode
    boost::shared_ptr<llvm::MemoryBuffer> buffer(llvm::MemoryBuffer::getMemBuffer(bitcode));

    //parse the bitcode into a module
    std::string error;
    llvm::Module *module = llvm::ParseBitcodeFile(buffer.get(), get_eemon().get_context(), &error);
    if (not error.empty()) throw std::runtime_error("GRAS compiler: ParseBitcodeFile " + error);
    return module;
}
#endif //HAVE_LLVM

/***********************************************************************
 * Helper function to call a clang compliation -- uses clang API
 **********************************************************************/
/*
#ifdef HAVE_CLANG
static llvm::Module *call_clang_api(const std::string &source_file, const std::vector<std::string> &flags)
{
    //begin command setup
    std::vector<const char *> args;

    //inject source
    args.push_back("-c");
    args.push_back("-x");
    args.push_back("c++");
    args.push_back(source_file.c_str());

    //inject args...
    BOOST_FOREACH(const std::string &flag, flags)
    {
        args.push_back(flag.c_str());
    }

    //http://fdiv.net/2012/08/15/compiling-code-clang-api

    // The compiler invocation needs a DiagnosticsEngine so it can report problems
    clang::DiagnosticOptions *DiagOpts = new clang::DiagnosticOptions();
    clang::TextDiagnosticPrinter *DiagClient = new clang::TextDiagnosticPrinter(llvm::errs(), DiagOpts);
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(new clang::DiagnosticIDs());
    clang::DiagnosticsEngine Diags(DiagID, DiagOpts, DiagClient);

    // Create the compiler invocation
    clang::LangOptions LangOpts;
    llvm::OwningPtr<clang::CompilerInvocation> CI(new clang::CompilerInvocation());
    //CI->setLangDefaults(LangOpts, clang::IK_CXX, clang::LangStandard::lang_gnucxx98);
    clang::CompilerInvocation::CreateFromArgs(*CI, &args[0], &args[0] + args.size(), Diags);

    // Print the argument list
    std::cout << "clang ";
    BOOST_FOREACH(const char *arg, args)
    {
        std::cout << arg << " ";
    }
    std::cout << std::endl;

    // Create the compiler instance
    clang::CompilerInstance Clang;
    Clang.setInvocation(CI.take());

    // Get ready to report problems
    Clang.createDiagnostics(args.size(), &args[0]);
    if (not Clang.hasDiagnostics()) throw std::runtime_error("ClangBlock::createDiagnostics");

    // Create an action and make the compiler instance carry it out
    clang::CodeGenAction *Act = new clang::EmitLLVMOnlyAction();
    if (not Clang.ExecuteAction(*Act)) throw std::runtime_error("ClangBlock::EmitLLVMOnlyAction");

    return Act->takeModule();
}
#endif //HAVE_CLANG
*/

/***********************************************************************
 * factory compile implementation
 **********************************************************************/
#ifdef HAVE_LLVM
void gras::jit_factory(const std::string &source, const std::vector<std::string> &flags_)
{
    //write source to tmp file
    char source_file[L_tmpnam];
    if (std::tmpnam(source_file) == NULL) throw std::runtime_error("GRAS compiler: tmp source file path failed");
    std::ofstream source_fstream(source_file);
    source_fstream << source;
    source_fstream.close();

    llvm::InitializeNativeTarget();
    llvm::llvm_start_multithreaded();

    //use clang to compile source into bitcode
    std::cout << "GRAS compiler: compile source into bitcode..." << std::endl;
    std::vector<std::string> flags = flags_;
    flags.push_back("-I"+get_gras_runtime_include_path()); //add root include path
    llvm::Module *module = call_clang_exe(source_file, flags);

    //create execution engine
    std::string error;
    boost::shared_ptr<llvm::ExecutionEngine> ee(llvm::ExecutionEngine::create(module, false, &error));
    if (not error.empty()) throw std::runtime_error("GRAS compiler: ExecutionEngine " + error);
    std::cout << "GRAS compiler: execute static constructors..." << std::endl;
    get_eemon().add(ee);
}

#else //HAVE_LLVM

void gras::jit_factory(const std::string &, const std::vector<std::string> &)
{
    throw std::runtime_error("GRAS compiler not built with Clang support!");
}

#endif //HAVE_LLVM
