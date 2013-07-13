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
    #for $NARGS in range($MAX_ARGS)
    template <typename ClassType, typename ReturnType, $expand('typename A%d', $NARGS)>
    void register_call(const std::string &name, ReturnType(ClassType::*fcn)($expand('const A%d &', $NARGS)));

    template <typename ClassType, $expand('typename A%d', $NARGS)>
    void register_call(const std::string &name, void(ClassType::*fcn)($expand('const A%d &', $NARGS)));

    #end for
    /*******************************************************************
     * Call API - don't look here, template magic, not helpful
     ******************************************************************/
public:
    #for $NARGS in range($MAX_ARGS)
    template <typename ReturnType, $expand('typename A%d', $NARGS)>
    ReturnType x(const std::string &name, $expand('const A%d &', $NARGS));

    template <$expand('typename A%d', $NARGS)>
    void x(const std::string &name, $expand('const A%d &', $NARGS));

    #end for
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
