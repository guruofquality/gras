// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_CALLABLE_HPP
#define INCLUDED_GRAS_DETAIL_CALLABLE_HPP

#include <typeinfo>

namespace gras
{

struct GRAS_API CallableRegistryEntry
{
    CallableRegistryEntry(void);
    virtual ~CallableRegistryEntry(void);
    virtual PMCC call(const std::vector<PMCC> &args) = 0;
};

template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1>
class CallableRegistryEntryImpl : public CallableRegistryEntry
{
public:

    typedef ReturnType(ClassType::*Fcn0)(void);
    typedef ReturnType(ClassType::*Fcn1)(const Arg0 &);
    typedef ReturnType(ClassType::*Fcn2)(const Arg0 &, const Arg1 &);

    CallableRegistryEntryImpl(ClassType *obj, Fcn0 fcn0, Fcn1 fcn1 = NULL, Fcn2 fcn2 = NULL):
        _obj(obj), _fcn0(fcn0), _fcn1(fcn1), _fcn2(fcn2)
    {}
    virtual ~CallableRegistryEntryImpl(void){}

    PMCC call(const std::vector<PMCC> &args)
    {
        if (_fcn0) return PMC_M((_obj->*_fcn0)());
        if (_fcn1) return PMC_M((_obj->*_fcn1)(args[0].safe_as<Arg0>()));
        if (_fcn2) return PMC_M((_obj->*_fcn2)(args[0].safe_as<Arg0>(), args[1].safe_as<Arg1>()));
        return PMCC();//should not get here
    }

private:
    ClassType *_obj;
    Fcn0 _fcn0; Fcn1 _fcn1; Fcn2 _fcn2;
};

template <typename ClassType, typename ReturnType>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(void))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl<ClassType, ReturnType, int, int>(obj, fcn);
    _register_call(key, fr);
}

template <typename ClassType, typename ReturnType, typename Arg0>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(const Arg0 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl<ClassType, ReturnType, Arg0, int>(obj, NULL, fcn);
    _register_call(key, fr);
}

template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl<ClassType, ReturnType, Arg0, Arg1>(obj, NULL, NULL, fcn);
    _register_call(key, fr);
}

template <typename ReturnType>
ReturnType Callable::call(const std::string &key)
{
    std::vector<PMCC> args;
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

template <typename ReturnType, typename Arg0>
ReturnType Callable::call(const std::string &key, const Arg0 &a0)
{
    std::vector<PMCC> args;
    args.push_back(PMC_M(a0));
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

template <typename ReturnType, typename Arg0, typename Arg1>
ReturnType Callable::call(const std::string &key, const Arg0 &a0, const Arg1 &a1)
{
    std::vector<PMCC> args;
    args.push_back(PMC_M(a0));
    args.push_back(PMC_M(a1));
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

inline
void Callable::call(const std::string &key)
{
    std::vector<PMCC> args;
    _handle_call(key, args);
}

template <typename Arg0>
void Callable::call(const std::string &key, const Arg0 &a0)
{
    std::vector<PMCC> args;
    args.push_back(PMC_M(a0));
    _handle_call(key, args);
}

template <typename Arg0, typename Arg1>
void Callable::call(const std::string &key, const Arg0 &a0, const Arg1 &a1)
{
    std::vector<PMCC> args;
    args.push_back(PMC_M(a0));
    args.push_back(PMC_M(a1));
    _handle_call(key, args);
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_CALLABLE_HPP*/
