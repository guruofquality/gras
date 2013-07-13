// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_CALLABLE_HPP
#define INCLUDED_GRAS_CALLABLE_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:4251)  // needs to have dll interface
#endif //_MSC_VER

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

    template <typename ClassType, typename ReturnType, typename A0>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &));

    template <typename ClassType, typename A0>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &));

    template <typename ClassType, typename A0, typename A1>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &));

    template <typename ClassType, typename A0, typename A1, typename A2>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &));

    template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &));

    template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    void register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &));

    /*******************************************************************
     * Call API - don't look here, template magic, not helpful
     ******************************************************************/
public:
    template <typename ReturnType>
    ReturnType x(const std::string &name);

    inline
    void x(const std::string &name);

    template <typename ReturnType, typename A0>
    ReturnType x(const std::string &name, const A0 &);

    template <typename A0>
    void x(const std::string &name, const A0 &);

    template <typename ReturnType, typename A0, typename A1>
    ReturnType x(const std::string &name, const A0 &, const A1 &);

    template <typename A0, typename A1>
    void x(const std::string &name, const A0 &, const A1 &);

    template <typename ReturnType, typename A0, typename A1, typename A2>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &);

    template <typename A0, typename A1, typename A2>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &);

    template <typename A0, typename A1, typename A2, typename A3>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    ReturnType x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    void x(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);

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

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_CALLABLE_HPP*/
