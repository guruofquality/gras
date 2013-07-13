// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_CALLABLE_HPP
#define INCLUDED_GRAS_CALLABLE_HPP

#include <gras/gras.hpp>
#include <PMC/PMC.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

namespace gras
{

/*!
 * The callable interface allows subclasses to export public methods,
 * but without actually exporting traditional library symbols.
 *
 * Callable handles the template magic so you don't have to:
 *  - registering subclass methods is simple and easy on the user.
 *  - users call registered methods with natural code aesthetics.
 *
 * Register a method (in the constructor of MyClass):
 * this->register_call("set_foo", &MyClass::set_foo);
 *
 * Call a method on a instance of MyClass:
 * my_class->x("set_foo", new_foo_val);
 *
 * Why x for the call method?
 *  - The "x" is short, one character of screen width.
 *  - The "x" looks like "*", which is commonly used.
 *  - The overloaded () operator sucks with pointers.
 *  - The overloaded () operator has template issues.
 */
class GRAS_API Callable
{
public:

    //! Default constructor
    Callable(void);

    //! Destructor (virtual for subclasses)
    virtual ~Callable(void);

    //! Get a list of names for registered calls
    std::vector<std::string> get_registered_names(void) const;

protected:
    /*!
     * Unregister a previously registered call.
     * Throws if the name is not found in the registry.
     */
    void unregister_call(const std::string &name);

    /*******************************************************************
     * Register API - don't look here, template magic, not helpful
     ******************************************************************/
protected:
    template <typename ClassType, typename ReturnType>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)());

    template <typename ClassType>
    void register_call(const std::string &name, void(ClassType::*fcn)());

    template <typename ClassType, typename ReturnType, typename Arg0>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &));

    template <typename ClassType, typename Arg0>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &));

    template <typename ClassType, typename Arg0, typename Arg1>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &));

    template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &));

    template <typename ClassType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    void register_call(const std::string &name, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &));

    /*******************************************************************
     * Call API - don't look here, template magic, not helpful
     ******************************************************************/
public:
    template <typename ReturnType>
    ReturnType x(const std::string &name);

    inline
    void x(const std::string &name);

    template <typename ReturnType, typename Arg0>
    ReturnType x(const std::string &name, const Arg0 &);

    template <typename Arg0>
    void x(const std::string &name, const Arg0 &);

    template <typename ReturnType, typename Arg0, typename Arg1>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &);

    template <typename Arg0, typename Arg1>
    void x(const std::string &name, const Arg0 &, const Arg1 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &);

    template <typename Arg0, typename Arg1, typename Arg2>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &);

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    ReturnType x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    void x(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &);

    /*******************************************************************
     * Private registration hooks
     ******************************************************************/
protected:
    void _register_call(const std::string &, void *);
public:
    virtual PMCC _handle_call(const std::string &, const PMCC &);
private:
    boost::shared_ptr<void> _call_registry;
};

} //namespace gras

#include <gras/detail/callable.hpp>

#endif /*INCLUDED_GRAS_CALLABLE_HPP*/
