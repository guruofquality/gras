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

/***********************************************************************
 * Registration for return with 0 args
 **********************************************************************/
template <typename ClassType, typename ReturnType>
struct CallableRegistryEntryImpl0 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)();
    CallableRegistryEntryImpl0(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 0);
        return PMC_M((_obj->*_fcn)());
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)())
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl0<ClassType, ReturnType>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType>
struct CallableRegistryEntryImplVoid0 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)();
    CallableRegistryEntryImplVoid0(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 0);
        (_obj->*_fcn)(); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)())
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid0<ClassType>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 1 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0>
struct CallableRegistryEntryImpl1 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &);
    CallableRegistryEntryImpl1(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 1);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl1<ClassType, ReturnType, A0>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0>
struct CallableRegistryEntryImplVoid1 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &);
    CallableRegistryEntryImplVoid1(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 1);
        (_obj->*_fcn)(a[0].safe_as<A0>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid1<ClassType, A0>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 2 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1>
struct CallableRegistryEntryImpl2 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &);
    CallableRegistryEntryImpl2(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 2);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl2<ClassType, ReturnType, A0, A1>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1>
struct CallableRegistryEntryImplVoid2 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &);
    CallableRegistryEntryImplVoid2(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 2);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid2<ClassType, A0, A1>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 3 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2>
struct CallableRegistryEntryImpl3 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &);
    CallableRegistryEntryImpl3(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 3);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl3<ClassType, ReturnType, A0, A1, A2>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2>
struct CallableRegistryEntryImplVoid3 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &);
    CallableRegistryEntryImplVoid3(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 3);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid3<ClassType, A0, A1, A2>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 4 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3>
struct CallableRegistryEntryImpl4 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &);
    CallableRegistryEntryImpl4(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 4);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl4<ClassType, ReturnType, A0, A1, A2, A3>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3>
struct CallableRegistryEntryImplVoid4 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &);
    CallableRegistryEntryImplVoid4(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 4);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid4<ClassType, A0, A1, A2, A3>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 5 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
struct CallableRegistryEntryImpl5 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);
    CallableRegistryEntryImpl5(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 5);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl5<ClassType, ReturnType, A0, A1, A2, A3, A4>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4>
struct CallableRegistryEntryImplVoid5 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);
    CallableRegistryEntryImplVoid5(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 5);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid5<ClassType, A0, A1, A2, A3, A4>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 6 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
struct CallableRegistryEntryImpl6 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);
    CallableRegistryEntryImpl6(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 6);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl6<ClassType, ReturnType, A0, A1, A2, A3, A4, A5>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
struct CallableRegistryEntryImplVoid6 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);
    CallableRegistryEntryImplVoid6(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 6);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid6<ClassType, A0, A1, A2, A3, A4, A5>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 7 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct CallableRegistryEntryImpl7 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);
    CallableRegistryEntryImpl7(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 7);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl7<ClassType, ReturnType, A0, A1, A2, A3, A4, A5, A6>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct CallableRegistryEntryImplVoid7 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);
    CallableRegistryEntryImplVoid7(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 7);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid7<ClassType, A0, A1, A2, A3, A4, A5, A6>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 8 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct CallableRegistryEntryImpl8 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);
    CallableRegistryEntryImpl8(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 8);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl8<ClassType, ReturnType, A0, A1, A2, A3, A4, A5, A6, A7>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct CallableRegistryEntryImplVoid8 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);
    CallableRegistryEntryImplVoid8(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 8);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid8<ClassType, A0, A1, A2, A3, A4, A5, A6, A7>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 9 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct CallableRegistryEntryImpl9 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);
    CallableRegistryEntryImpl9(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 9);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl9<ClassType, ReturnType, A0, A1, A2, A3, A4, A5, A6, A7, A8>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct CallableRegistryEntryImplVoid9 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);
    CallableRegistryEntryImplVoid9(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 9);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid9<ClassType, A0, A1, A2, A3, A4, A5, A6, A7, A8>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Registration for return with 10 args
 **********************************************************************/
template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct CallableRegistryEntryImpl10 : CallableRegistryEntry
{
    typedef ReturnType(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);
    CallableRegistryEntryImpl10(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 10);
        return PMC_M((_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>(), a[9].safe_as<A9>()));
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void Callable::register_call(const std::string &name, ReturnType(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImpl10<ClassType, ReturnType, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>(obj, fcn);
    _register_call(name, fr);
}

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct CallableRegistryEntryImplVoid10 : CallableRegistryEntry
{
    typedef void(ClassType::*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);
    CallableRegistryEntryImplVoid10(ClassType *obj, Fcn fcn):
    _obj(obj), _fcn(fcn){}
    PMCC call(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 10);
        (_obj->*_fcn)(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>(), a[9].safe_as<A9>()); return PMCC();
    }
    ClassType *_obj; Fcn _fcn;
};

template <typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void Callable::register_call(const std::string &name, void(ClassType::*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &))
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *fr = new CallableRegistryEntryImplVoid10<ClassType, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>(obj, fcn);
    _register_call(name, fr);
}

/***********************************************************************
 * Call implementations with 0 args
 **********************************************************************/
template <typename ReturnType>
ReturnType Callable::x(const std::string &name)
{
    PMCList args(0);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

inline
void Callable::x(const std::string &name)
{
    PMCList args(0);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 1 args
 **********************************************************************/
template <typename ReturnType, typename A0>
ReturnType Callable::x(const std::string &name, const A0 &a0)
{
    PMCList args(1);
    args[0] = PMC_M(a0);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0>
void Callable::x(const std::string &name, const A0 &a0)
{
    PMCList args(1);
    args[0] = PMC_M(a0);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 2 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1)
{
    PMCList args(2);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1)
{
    PMCList args(2);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 3 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2)
{
    PMCList args(3);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2)
{
    PMCList args(3);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 4 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3)
{
    PMCList args(4);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3)
{
    PMCList args(4);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 5 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    PMCList args(5);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    PMCList args(5);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 6 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{
    PMCList args(6);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{
    PMCList args(6);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 7 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{
    PMCList args(7);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{
    PMCList args(7);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 8 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{
    PMCList args(8);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{
    PMCList args(8);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 9 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{
    PMCList args(9);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    args[8] = PMC_M(a8);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{
    PMCList args(9);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    args[8] = PMC_M(a8);
    _handle_call(name, PMC_M(args));
}

/***********************************************************************
 * Call implementations with 10 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
ReturnType Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{
    PMCList args(10);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    args[8] = PMC_M(a8);
    args[9] = PMC_M(a9);
    PMCC r = _handle_call(name, PMC_M(args));
    return r.safe_as<ReturnType>();
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void Callable::x(const std::string &name, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{
    PMCList args(10);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    args[7] = PMC_M(a7);
    args[8] = PMC_M(a8);
    args[9] = PMC_M(a9);
    _handle_call(name, PMC_M(args));
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_CALLABLE_HPP*/
