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
    void arg_check(const PMCList &args, const size_t nargs);
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
        this->arg_check(a, 0);
        return _fcn();
    }
    Fcn _fcn;
};

template <typename ReturnType>
void register_factory(const std::string &path, ReturnType(*fcn)())
{
    void *r = new FactoryRegistryEntryImpl0<ReturnType>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 1 args
 **********************************************************************/
template <typename ReturnType, typename A0>
struct FactoryRegistryEntryImpl1 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &);
    FactoryRegistryEntryImpl1(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 1);
        return _fcn(a[0].safe_as<A0>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &))
{
    void *r = new FactoryRegistryEntryImpl1<ReturnType, A0>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 2 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1>
struct FactoryRegistryEntryImpl2 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &);
    FactoryRegistryEntryImpl2(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 2);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &))
{
    void *r = new FactoryRegistryEntryImpl2<ReturnType, A0, A1>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 3 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2>
struct FactoryRegistryEntryImpl3 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &);
    FactoryRegistryEntryImpl3(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 3);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &))
{
    void *r = new FactoryRegistryEntryImpl3<ReturnType, A0, A1, A2>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 4 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
struct FactoryRegistryEntryImpl4 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &);
    FactoryRegistryEntryImpl4(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 4);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &))
{
    void *r = new FactoryRegistryEntryImpl4<ReturnType, A0, A1, A2, A3>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 5 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
struct FactoryRegistryEntryImpl5 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);
    FactoryRegistryEntryImpl5(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 5);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &))
{
    void *r = new FactoryRegistryEntryImpl5<ReturnType, A0, A1, A2, A3, A4>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 6 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
struct FactoryRegistryEntryImpl6 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);
    FactoryRegistryEntryImpl6(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 6);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &))
{
    void *r = new FactoryRegistryEntryImpl6<ReturnType, A0, A1, A2, A3, A4, A5>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 7 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct FactoryRegistryEntryImpl7 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);
    FactoryRegistryEntryImpl7(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 7);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &))
{
    void *r = new FactoryRegistryEntryImpl7<ReturnType, A0, A1, A2, A3, A4, A5, A6>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 8 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct FactoryRegistryEntryImpl8 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);
    FactoryRegistryEntryImpl8(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 8);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &))
{
    void *r = new FactoryRegistryEntryImpl8<ReturnType, A0, A1, A2, A3, A4, A5, A6, A7>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 9 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct FactoryRegistryEntryImpl9 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);
    FactoryRegistryEntryImpl9(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 9);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &))
{
    void *r = new FactoryRegistryEntryImpl9<ReturnType, A0, A1, A2, A3, A4, A5, A6, A7, A8>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated registration - 10 args
 **********************************************************************/
template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct FactoryRegistryEntryImpl10 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);
    FactoryRegistryEntryImpl10(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        this->arg_check(a, 10);
        return _fcn(a[0].safe_as<A0>(), a[1].safe_as<A1>(), a[2].safe_as<A2>(), a[3].safe_as<A3>(), a[4].safe_as<A4>(), a[5].safe_as<A5>(), a[6].safe_as<A6>(), a[7].safe_as<A7>(), a[8].safe_as<A8>(), a[9].safe_as<A9>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &))
{
    void *r = new FactoryRegistryEntryImpl10<ReturnType, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>(fcn);
    _register_factory(path, r);
}

/***********************************************************************
 * Templated make implementations
 **********************************************************************/
inline
Element *make(const std::string &path)
{
    PMCList args(0);
    return _handle_make(path, PMC_M(args));
}

template <typename A0>
Element *make(const std::string &path, const A0 &a0)
{
    PMCList args(1);
    args[0] = PMC_M(a0);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1>
Element *make(const std::string &path, const A0 &a0, const A1 &a1)
{
    PMCList args(2);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2)
{
    PMCList args(3);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3)
{
    PMCList args(4);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    PMCList args(5);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{
    PMCList args(6);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{
    PMCList args(7);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    args[3] = PMC_M(a3);
    args[4] = PMC_M(a4);
    args[5] = PMC_M(a5);
    args[6] = PMC_M(a6);
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
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
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
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
    return _handle_make(path, PMC_M(args));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
Element *make(const std::string &path, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
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
    return _handle_make(path, PMC_M(args));
}

}

#endif /*INCLUDED_GRAS_DETAIL_FACTORY_HPP*/
