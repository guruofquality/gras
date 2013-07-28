// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_CALLABLE_HPP
#define INCLUDED_GRAS_DETAIL_CALLABLE_HPP

#include <PMC/Containers.hpp> //PMCList

namespace gras
{

/***********************************************************************
 * Registration entry base class
 **********************************************************************/
struct GRAS_API CallableRegistryEntry
{
    CallableRegistryEntry(void);
    virtual ~CallableRegistryEntry(void);
    virtual PMCC call(const PMCC &args) = 0;
    void arg_check(const PMCList &args, const size_t nargs);
};

#for $NARGS in range($MAX_ARGS)
/***********************************************************************
 * Registration for return with $NARGS args
 **********************************************************************/
template <typename ClassType, typename ReturnType, $expand('typename A%d', $NARGS)>
struct CallableRegistryEntryImpl$(NARGS) : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)($expand('const A%d &', $NARGS));
    CallableRegistryEntryImpl$(NARGS)(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, $NARGS);
        return PMC_M((_obj->*_fcn)($expand('a[%d].safe_as<A%d>()', $NARGS)));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, $expand('typename A%d', $NARGS)>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)($expand('const A%d &', $NARGS)))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl$(NARGS)<ClassType, ReturnType, $expand('A%d', $NARGS)>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, $expand('typename A%d', $NARGS)>
struct CallableRegistryEntryImplVoid$(NARGS) : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)($expand('const A%d &', $NARGS));
    CallableRegistryEntryImplVoid$(NARGS)(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, $NARGS);
        (_obj->*_fcn)($expand('a[%d].safe_as<A%d>()', $NARGS)); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, $expand('typename A%d', $NARGS)>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)($expand('const A%d &', $NARGS)))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid$(NARGS)<ClassType, $expand('A%d', $NARGS)>(obj, fcn);
    _register_call(name, fr);
}

#end for
#for $NARGS in range($MAX_ARGS)
/***********************************************************************
 * Call implementations with $NARGS args
 **********************************************************************/
template <typename ReturnType, $expand('typename A%d', $NARGS)>
ReturnType Callable::x(const std::string &name, $expand('const A%d &a%d', $NARGS))
{
    PMCList args($NARGS);
    #for $i in range($NARGS):
    args[$i] = PMC_M(a$i);
    #end for
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <$expand('typename A%d', $NARGS)>
void Callable::x(const std::string &name, $expand('const A%d &a%d', $NARGS))
{
    PMCList args($NARGS);
    #for $i in range($NARGS):
    args[$i] = PMC_M(a$i);
    #end for
    _handle_call(name, PMC_M(args));
}

#end for
} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_CALLABLE_HPP*/
