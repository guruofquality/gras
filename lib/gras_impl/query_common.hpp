// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_QUERY_COMMON_HPP
#define INCLUDED_LIBGRAS_IMPL_QUERY_COMMON_HPP

#include <PMC/PMC.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

// misc functions used in the implementation cpp files

namespace gras
{
    PMCC ptree_to_pmc(const boost::property_tree::ptree &value);
    boost::property_tree::ptree pmc_to_ptree(const PMCC &value);

    boost::property_tree::ptree json_to_ptree(const std::string &s);
    std::string ptree_to_json(const boost::property_tree::ptree &p);
}

#endif /*INCLUDED_LIBGRAS_IMPL_QUERY_COMMON_HPP*/
