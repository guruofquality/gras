// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_CALLABLE_HPP
#define INCLUDED_GRAS_DETAIL_CALLABLE_HPP

namespace gras
{

/***********************************************************************
 * Registration entry base class
 **********************************************************************/
struct GRAS_API CallableRegistryEntry
{
    CallableRegistryEntry(void);
    virtual ~CallableRegistryEntry(void);
    virtual PMCC call(const PMCC *args) = 0;
};

/***********************************************************************
 * Registration for return with 0 args
 **********************************************************************/
template <typename ClassType, typename ReturnType>
struct CallableRegistryEntryImpl0 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(void);
    CallableRegistryEntryImpl0(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        return PMC_M((_obj->*_fcn)());
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(void))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl0<ClassType, ReturnType>(obj, fcn);
    _register_call(key, fr);
}

template <typename ClassType>
struct CallableRegistryEntryImplVoid0 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(void);
    CallableRegistryEntryImplVoid0(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        (_obj->*_fcn)(); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType>
void Callable::register_call(const std::string &key, void(ClassType::*fcn)(void))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid0<ClassType>(obj, fcn);
    _register_call(key, fr);
}

/***********************************************************************
 * Registration for return with 1 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename Arg0>
struct CallableRegistryEntryImpl1 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const Arg0 &);
    CallableRegistryEntryImpl1(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        return PMC_M((_obj->*_fcn)(args[0].safe_as<Arg0>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename Arg0>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(const Arg0 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl1<ClassType, ReturnType, Arg0>(obj, fcn);
    _register_call(key, fr);
}

template <typename ClassType, typename Arg0>
struct CallableRegistryEntryImplVoid1 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const Arg0 &);
    CallableRegistryEntryImplVoid1(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        (_obj->*_fcn)(args[0].safe_as<Arg0>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename Arg0>
void Callable::register_call(const std::string &key, void(ClassType::*fcn)(const Arg0 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid1<ClassType, Arg0>(obj, fcn);
    _register_call(key, fr);
}

/***********************************************************************
 * Registration for return with 2 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1>
struct CallableRegistryEntryImpl2 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const Arg0 &, const Arg1 &);
    CallableRegistryEntryImpl2(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        return PMC_M((_obj->*_fcn)(args[0].safe_as<Arg0>(), args[1].safe_as<Arg1>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl2<ClassType, ReturnType, Arg0, Arg1>(obj, fcn);
    _register_call(key, fr);
}

template <typename ClassType, typename Arg0, typename Arg1>
struct CallableRegistryEntryImplVoid2 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const Arg0 &, const Arg1 &);
    CallableRegistryEntryImplVoid2(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        (_obj->*_fcn)(args[0].safe_as<Arg0>(), args[1].safe_as<Arg1>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename Arg0, typename Arg1>
void Callable::register_call(const std::string &key, void(ClassType::*fcn)(const Arg0 &, const Arg1 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid2<ClassType, Arg0, Arg1>(obj, fcn);
    _register_call(key, fr);
}

/***********************************************************************
 * Registration for return with 3 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
struct CallableRegistryEntryImpl3 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &);
    CallableRegistryEntryImpl3(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        return PMC_M((_obj->*_fcn)(args[0].safe_as<Arg0>(), args[1].safe_as<Arg1>(), args[2].safe_as<Arg2>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
void Callable::register_call(const std::string &key, ReturnType(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl3<ClassType, ReturnType, Arg0, Arg1, Arg2>(obj, fcn);
    _register_call(key, fr);
}

template <typename ClassType, typename Arg0, typename Arg1, typename Arg2>
struct CallableRegistryEntryImplVoid3 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &);
    CallableRegistryEntryImplVoid3(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC *args)
    {
        (_obj->*_fcn)(args[0].safe_as<Arg0>(), args[1].safe_as<Arg1>(), args[2].safe_as<Arg2>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename Arg0, typename Arg1, typename Arg2>
void Callable::register_call(const std::string &key, void(ClassType::*fcn)(const Arg0 &, const Arg1 &, const Arg2 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid3<ClassType, Arg0, Arg1, Arg2>(obj, fcn);
    _register_call(key, fr);
}

/***********************************************************************
 * Call implementations with 0 args
 **********************************************************************/
template <typename ReturnType>
ReturnType Callable::x(const std::string &key)
{
    PMCC args[0];
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

inline
void Callable::x(const std::string &key)
{
    PMCC args[0];
    _handle_call(key, args);
}

/***********************************************************************
 * Call implementations with 1 args
 **********************************************************************/
template <typename ReturnType, typename Arg0>
ReturnType Callable::x(const std::string &key, const Arg0 &a0)
{
    PMCC args[1];
    args[0] = PMC_M(a0);
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

template <typename Arg0>
void Callable::x(const std::string &key, const Arg0 &a0)
{
    PMCC args[1];
    args[0] = PMC_M(a0);
    _handle_call(key, args);
}

/***********************************************************************
 * Call implementations with 2 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1>
ReturnType Callable::x(const std::string &key, const Arg0 &a0, const Arg1 &a1)
{
    PMCC args[2];
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

template <typename Arg0, typename Arg1>
void Callable::x(const std::string &key, const Arg0 &a0, const Arg1 &a1)
{
    PMCC args[2];
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    _handle_call(key, args);
}

/***********************************************************************
 * Call implementations with 3 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
ReturnType Callable::x(const std::string &key, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2)
{
    PMCC args[3];
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    PMCC r = _handle_call(key, args);
    return r.safe_as<ReturnType>();
}

template <typename Arg0, typename Arg1, typename Arg2>
void Callable::x(const std::string &key, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2)
{
    PMCC args[3];
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    _handle_call(key, args);
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_CALLABLE_HPP*/
