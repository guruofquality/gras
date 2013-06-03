// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_impl/debug.hpp"
#include <PMC/PMC.hpp>
#include <PMC/Containers.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <complex>
#include <sstream>

using namespace boost::property_tree;

PMCC ptree_to_pmc(const ptree &value, const std::type_info &hint)
{
    //if the type is PMCC - educated guess and recursively call
    if (hint == typeid(PMCC) or hint == typeid(PMC))
    {
        //single child
        if (value.size() == 0)
        {
            //can we cast to number?
            try{return ptree_to_pmc(value, typeid(double));}
            catch(...){}

            //can we cast to complex?
            try{return ptree_to_pmc(value, typeid(std::complex<double>));}
            catch(...){}

            //then string
            return ptree_to_pmc(value, typeid(std::string));
        }
        //array
        else
        {
            //TODO more if statements - w/ define
            return ptree_to_pmc(value, typeid(std::vector<double>));
        }
    }

    #define ptree_to_pmc_try(type) \
    if (hint == typeid(type)) return PMC_M(value.get_value<type>());

    //determine number
    ptree_to_pmc_try(char);
    ptree_to_pmc_try(signed char);
    ptree_to_pmc_try(unsigned char);
    ptree_to_pmc_try(signed short);
    ptree_to_pmc_try(unsigned short);
    ptree_to_pmc_try(signed int);
    ptree_to_pmc_try(unsigned int);
    ptree_to_pmc_try(signed long);
    ptree_to_pmc_try(unsigned long);
    ptree_to_pmc_try(signed long long);
    ptree_to_pmc_try(unsigned long long);
    ptree_to_pmc_try(float);
    ptree_to_pmc_try(double);

    //complex number
    std::istringstream ss(value.get_value<std::string>());
    if (hint == typeid(std::complex<double>))
    {
        std::complex<double> c; ss >> c; return PMC_M(c);
    }
    if (hint == typeid(std::complex<float>))
    {
        std::complex<float> c; ss >> c; return PMC_M(c);
    }

    //string
    ptree_to_pmc_try(std::string);

    //determine number vector
    std::vector<long> vec;
    BOOST_FOREACH(const ptree::value_type &elem, value)
    {
        vec.push_back(elem.second.get_value<long>());
    }
    return PMC_M(vec);

    //otherwise null -- will crap out
    return PMC();
}

ptree pmc_to_ptree(const PMCC &value)
{
    ptree v;
    #define pmc_to_ptree_try(type) \
    if (value.is<type>()) {v.put_value(value.as<type>()); return v;}

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

    //derermine complex
    if (value.is<std::complex<double> >())
    {
        std::ostringstream oss;
        oss << value.as<std::complex<double> >();
        v.put_value(oss.str()); return v;
    }
    if (value.is<std::complex<float> >())
    {
        std::ostringstream oss;
        oss << value.as<std::complex<float> >();
        v.put_value(oss.str()); return v;
    }

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

    return v;
}
