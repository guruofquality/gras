// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_FACTORY_HPP
#define INCLUDED_GRAS_DETAIL_FACTORY_HPP

#include <PMC/Containers.hpp> //PMCList

namespace gras
{

/***********************************************************************
 * Factory entry base class
 **********************************************************************/
struct GRAS_API FactoryRegistryEntry
{
    FactoryRegistryEntry(void);
    virtual ~FactoryRegistryEntry(void);
    virtual Element *make(const PMCC &args) = 0;
};

/***********************************************************************
 * Templated registration - 0 args
 **********************************************************************/
template <typename ReturnType>
struct FactoryRegistryEntryImpl0 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)();
    FactoryRegistryEntryImpl0(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 0) throw a;
        return _fcn();
    }
    Fcn _fcn;
};

template <typename ReturnType>
void Factory::register_make(const std::string &name, ReturnType(*fcn)())
{
    void *r = new FactoryRegistryEntryImpl0<ReturnType>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 1 args
 **********************************************************************/
template <typename ReturnType, typename Arg0>
struct FactoryRegistryEntryImpl1 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &);
    FactoryRegistryEntryImpl1(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 1) throw a;
        return _fcn(a[0].safe_as<Arg0>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &))
{
    void *r = new FactoryRegistryEntryImpl1<ReturnType, Arg0>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 2 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1>
struct FactoryRegistryEntryImpl2 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &);
    FactoryRegistryEntryImpl2(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 2) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &))
{
    void *r = new FactoryRegistryEntryImpl2<ReturnType, Arg0, Arg1>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 3 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
struct FactoryRegistryEntryImpl3 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &);
    FactoryRegistryEntryImpl3(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 3) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &))
{
    void *r = new FactoryRegistryEntryImpl3<ReturnType, Arg0, Arg1, Arg2>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 4 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
struct FactoryRegistryEntryImpl4 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &);
    FactoryRegistryEntryImpl4(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 4) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &))
{
    void *r = new FactoryRegistryEntryImpl4<ReturnType, Arg0, Arg1, Arg2, Arg3>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 5 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
struct FactoryRegistryEntryImpl5 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &);
    FactoryRegistryEntryImpl5(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 5) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &))
{
    void *r = new FactoryRegistryEntryImpl5<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 6 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
struct FactoryRegistryEntryImpl6 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &);
    FactoryRegistryEntryImpl6(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 6) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>(), a[5].safe_as<Arg5>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &))
{
    void *r = new FactoryRegistryEntryImpl6<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 7 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
struct FactoryRegistryEntryImpl7 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &);
    FactoryRegistryEntryImpl7(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 7) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>(), a[5].safe_as<Arg5>(), a[6].safe_as<Arg6>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &))
{
    void *r = new FactoryRegistryEntryImpl7<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 8 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
struct FactoryRegistryEntryImpl8 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &);
    FactoryRegistryEntryImpl8(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 8) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>(), a[5].safe_as<Arg5>(), a[6].safe_as<Arg6>(), a[7].safe_as<Arg7>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &))
{
    void *r = new FactoryRegistryEntryImpl8<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 9 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
struct FactoryRegistryEntryImpl9 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &);
    FactoryRegistryEntryImpl9(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 9) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>(), a[5].safe_as<Arg5>(), a[6].safe_as<Arg6>(), a[7].safe_as<Arg7>(), a[8].safe_as<Arg8>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &))
{
    void *r = new FactoryRegistryEntryImpl9<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 10 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
struct FactoryRegistryEntryImpl10 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &);
    FactoryRegistryEntryImpl10(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < 10) throw a;
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>(), a[3].safe_as<Arg3>(), a[4].safe_as<Arg4>(), a[5].safe_as<Arg5>(), a[6].safe_as<Arg6>(), a[7].safe_as<Arg7>(), a[8].safe_as<Arg8>(), a[9].safe_as<Arg9>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &))
{
    void *r = new FactoryRegistryEntryImpl10<ReturnType, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated make implementations
 **********************************************************************/
inline
Element *Factory::make(const std::string &name)
{
    PMCList args(0);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0>
Element *Factory::make(const std::string &name, const Arg0 &a0)
{
    PMCList args(1);
    args[0] = PMC_M(a0);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1)
{
    PMCList args(2);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2)
{
    PMCList args(3);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3)
{
    PMCList args(4);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4)
{
    PMCList args(5);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4, const Arg5 &a5)
{
    PMCList args(6);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6)
{
    PMCList args(7);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7)
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
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7, const Arg8 &a8)
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
    return Factory::_handle_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2, const Arg3 &a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7, const Arg8 &a8, const Arg9 &a9)
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
    return Factory::_handle_make(name, PMC_M(args));
}

}

#endif /*INCLUDED_GRAS_DETAIL_FACTORY_HPP*/
