// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

//--------- begin bullshit --------------//
#include <boost/version.hpp>
//why the fuck does no OS ever patch boost when there is a bug
//https://svn.boost.org/trac/boost/ticket/6785
#if BOOST_VERSION == 104900
#include "boost_fail/json_parser_read_104900.hpp"
#endif
//--------- end bullshit --------------//

#include "gras_impl/query_common.hpp"
#include "gras_impl/debug.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/regex.hpp>
#include <sstream>
#include <string>

boost::property_tree::ptree gras::json_to_ptree(const std::string &s)
{
    std::stringstream ss(s);
    boost::property_tree::ptree pt;
    boost::property_tree::json_parser::read_json(ss, pt);
    return pt;
}

//http://stackoverflow.com/questions/13464383/boost-property-write-json-incorrect-behaviour
std::string gras::ptree_to_json(const boost::property_tree::ptree &p)
{
    boost::regex exp("\"(null|true|false|[0-9]+(\\.[0-9]+)?)\"");
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, p);
    std::string rv = boost::regex_replace(ss.str(), exp, "$1");

    return rv;
}
