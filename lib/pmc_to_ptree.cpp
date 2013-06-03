// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_impl/debug.hpp"
#include <PMC/PMC.hpp>
#include <PMC/Containers.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

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
            try{return ptree_to_pmc(value, typeid(long));}
            catch(...){}
            //TODO complex
            //then string
            return ptree_to_pmc(value, typeid(std::string));
        }
        //array
        else
        {
            //TODO more if statements - w/ define
            return ptree_to_pmc(value, typeid(std::vector<long>));
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
    //complex number TODO

    //string
    ptree_to_pmc_try(std::string);

    //determine number vector
    std::vector<long> vec;
    BOOST_FOREACH(const ptree::value_type &elem, value)
    {
        vec.push_back(elem.second.get_value<long>());
        VAR(vec.back());
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

    //determine string
    pmc_to_ptree_try(std::string);

    //try numeric vector
            //TODO more if statements - w/ define
    if (value.is<std::vector<long> >())
    {
        BOOST_FOREACH(const long &elem, value.as<std::vector<long> >())
        {
            
        }
    }

    return v;
}
