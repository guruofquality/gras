// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_impl/query_common.hpp"
#include "gras_impl/debug.hpp"
#include <PMC/PMC.hpp>
#include <PMC/Containers.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <complex>

using namespace boost::property_tree;

PMCC gras::ptree_to_pmc(const ptree &value)
{

    #define ptree_to_pmc_try(type) \
    try{return PMC_M(value.get_value<type >());} \
    catch(...){}

    #define ptree_to_pmc_tryv(type) \
    try{ \
        std::vector<type > vec; \
        BOOST_FOREACH(const ptree::value_type &elem, value) \
        { \
            vec.push_back(elem.second.get_value<type >()); \
        } \
        return PMC_M(vec); \
    }catch(...){}

    //single child
    if (value.size() == 0)
    {
        //can we cast to integer?
        ptree_to_pmc_try(boost::int64_t)

        //can we cast to float?
        ptree_to_pmc_try(double)

        //can we cast to complex?
        ptree_to_pmc_try(std::complex<double>)

        //then string
        ptree_to_pmc_try(std::string);
    }
    //array
    else
    {
        ptree_to_pmc_tryv(boost::int64_t);
        ptree_to_pmc_tryv(double);
        ptree_to_pmc_tryv(std::complex<double>);
    }

    //otherwise null -- will crap out
    return PMC();
}

ptree gras::pmc_to_ptree(const PMCC &value)
{
    ptree v;
    #define pmc_to_ptree_try(type) \
    if (value.is<type >()) {v.put_value(value.as<type >()); return v;}

    //determine number
    pmc_to_ptree_try(char);
    pmc_to_ptree_try(signed char);
    pmc_to_ptree_try(unsigned char);
    pmc_to_ptree_try(signed short);
    pmc_to_ptree_try(unsigned short);
    pmc_to_ptree_try(signed int);
    pmc_to_ptree_try(unsigned int);
    pmc_to_ptree_try(signed long);
    pmc_to_ptree_try(unsigned long);
    pmc_to_ptree_try(signed long long);
    pmc_to_ptree_try(unsigned long long);
    pmc_to_ptree_try(float);
    pmc_to_ptree_try(double);
    pmc_to_ptree_try(std::complex<float>);
    pmc_to_ptree_try(std::complex<double>);

    //determine string
    pmc_to_ptree_try(std::string);

    //try numeric vector
    #define pmc_to_ptree_tryv(type) \
    if (value.is<std::vector<type> >()) \
    { \
        BOOST_FOREACH(const type &elem, value.as<std::vector<type> >()) \
        { \
            ptree t; t.put_value(elem); \
            v.push_back(std::make_pair("", t)); \
        } \
    }
    pmc_to_ptree_tryv(char);
    pmc_to_ptree_tryv(signed char);
    pmc_to_ptree_tryv(unsigned char);
    pmc_to_ptree_tryv(signed short);
    pmc_to_ptree_tryv(unsigned short);
    pmc_to_ptree_tryv(signed int);
    pmc_to_ptree_tryv(unsigned int);
    pmc_to_ptree_tryv(signed long);
    pmc_to_ptree_tryv(unsigned long);
    pmc_to_ptree_tryv(signed long long);
    pmc_to_ptree_tryv(unsigned long long);
    pmc_to_ptree_tryv(float);
    pmc_to_ptree_tryv(double);
    pmc_to_ptree_tryv(std::complex<float>);
    pmc_to_ptree_tryv(std::complex<double>);

    return v;
}
