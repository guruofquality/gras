//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_PMX_HELPER_HPP
#define INCLUDED_LIBGRAS_PMX_HELPER_HPP

#include <PMC/PMC.hpp>
#include <PMC/Containers.hpp>
#include <gruel/pmt.h>
#include <boost/foreach.hpp>

namespace pmt
{

inline pmt_t pmc_to_pmt(const PMCC &p)
{
    if (not p) return pmt::pmt_t();

    #define decl_pmc_to_pmt(type, conv) if (p.is<type >()) return conv(p.as<type >())

    decl_pmc_to_pmt(bool, pmt_from_bool);
    decl_pmc_to_pmt(std::string, pmt_string_to_symbol);

    //numeric types
    decl_pmc_to_pmt(int8_t, pmt_from_long);
    decl_pmc_to_pmt(int16_t, pmt_from_long);
    decl_pmc_to_pmt(int32_t, pmt_from_long);
    decl_pmc_to_pmt(uint8_t, pmt_from_long);
    decl_pmc_to_pmt(uint16_t, pmt_from_long);
    decl_pmc_to_pmt(uint32_t, pmt_from_long);
    decl_pmc_to_pmt(int64_t, pmt_from_uint64);
    decl_pmc_to_pmt(uint64_t, pmt_from_uint64);
    decl_pmc_to_pmt(float, pmt_from_double);
    decl_pmc_to_pmt(double, pmt_from_double);
    #define pmt_from_complex(x) pmt_make_rectangular((x).real(), (x).imag())
    decl_pmc_to_pmt(std::complex<float>, pmt_from_complex);
    decl_pmc_to_pmt(std::complex<double>, pmt_from_complex);

    //pair container
    if (p.is<PMCPair>())
    {
        const PMCPair &pr = p.as<PMCPair>();
        return pmt_cons(pmc_to_pmt(pr.first), pmc_to_pmt(pr.second));
    }

    //fucking tuples
/*
for i in range(11):
    args = list()
    for j in range(i):
        args.append('pmc_to_pmt(p.as<PMCTuple<%d> >()[%d])'%(i, j))
    print '    if (p.is<PMCTuple<%d> >())'%i
    print '        return pmt_make_tuple(%s);'%(', '.join(args),)
*/
    if (p.is<PMCTuple<0> >())
        return pmt_make_tuple();
    if (p.is<PMCTuple<1> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<1> >()[0]));
    if (p.is<PMCTuple<2> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<2> >()[0]), pmc_to_pmt(p.as<PMCTuple<2> >()[1]));
    if (p.is<PMCTuple<3> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<3> >()[0]), pmc_to_pmt(p.as<PMCTuple<3> >()[1]), pmc_to_pmt(p.as<PMCTuple<3> >()[2]));
    if (p.is<PMCTuple<4> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<4> >()[0]), pmc_to_pmt(p.as<PMCTuple<4> >()[1]), pmc_to_pmt(p.as<PMCTuple<4> >()[2]), pmc_to_pmt(p.as<PMCTuple<4> >()[3]));
    if (p.is<PMCTuple<5> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<5> >()[0]), pmc_to_pmt(p.as<PMCTuple<5> >()[1]), pmc_to_pmt(p.as<PMCTuple<5> >()[2]), pmc_to_pmt(p.as<PMCTuple<5> >()[3]), pmc_to_pmt(p.as<PMCTuple<5> >()[4]));
    if (p.is<PMCTuple<6> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<6> >()[0]), pmc_to_pmt(p.as<PMCTuple<6> >()[1]), pmc_to_pmt(p.as<PMCTuple<6> >()[2]), pmc_to_pmt(p.as<PMCTuple<6> >()[3]), pmc_to_pmt(p.as<PMCTuple<6> >()[4]), pmc_to_pmt(p.as<PMCTuple<6> >()[5]));
    if (p.is<PMCTuple<7> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<7> >()[0]), pmc_to_pmt(p.as<PMCTuple<7> >()[1]), pmc_to_pmt(p.as<PMCTuple<7> >()[2]), pmc_to_pmt(p.as<PMCTuple<7> >()[3]), pmc_to_pmt(p.as<PMCTuple<7> >()[4]), pmc_to_pmt(p.as<PMCTuple<7> >()[5]), pmc_to_pmt(p.as<PMCTuple<7> >()[6]));
    if (p.is<PMCTuple<8> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<8> >()[0]), pmc_to_pmt(p.as<PMCTuple<8> >()[1]), pmc_to_pmt(p.as<PMCTuple<8> >()[2]), pmc_to_pmt(p.as<PMCTuple<8> >()[3]), pmc_to_pmt(p.as<PMCTuple<8> >()[4]), pmc_to_pmt(p.as<PMCTuple<8> >()[5]), pmc_to_pmt(p.as<PMCTuple<8> >()[6]), pmc_to_pmt(p.as<PMCTuple<8> >()[7]));
    if (p.is<PMCTuple<9> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<9> >()[0]), pmc_to_pmt(p.as<PMCTuple<9> >()[1]), pmc_to_pmt(p.as<PMCTuple<9> >()[2]), pmc_to_pmt(p.as<PMCTuple<9> >()[3]), pmc_to_pmt(p.as<PMCTuple<9> >()[4]), pmc_to_pmt(p.as<PMCTuple<9> >()[5]), pmc_to_pmt(p.as<PMCTuple<9> >()[6]), pmc_to_pmt(p.as<PMCTuple<9> >()[7]), pmc_to_pmt(p.as<PMCTuple<9> >()[8]));
    if (p.is<PMCTuple<10> >())
        return pmt_make_tuple(pmc_to_pmt(p.as<PMCTuple<10> >()[0]), pmc_to_pmt(p.as<PMCTuple<10> >()[1]), pmc_to_pmt(p.as<PMCTuple<10> >()[2]), pmc_to_pmt(p.as<PMCTuple<10> >()[3]), pmc_to_pmt(p.as<PMCTuple<10> >()[4]), pmc_to_pmt(p.as<PMCTuple<10> >()[5]), pmc_to_pmt(p.as<PMCTuple<10> >()[6]), pmc_to_pmt(p.as<PMCTuple<10> >()[7]), pmc_to_pmt(p.as<PMCTuple<10> >()[8]), pmc_to_pmt(p.as<PMCTuple<10> >()[9]));

    //vectors
    if (p.is<PMCList>())
    {
        const PMCList &l = p.as<PMCList>();
        pmt_t v = pmt_make_vector(l.size(), pmt_t());
        for (size_t i = 0; i < l.size(); i++)
        {
            pmt_vector_set(v, i, pmc_to_pmt(l[i]));
        }
        return v;
    }

    //numeric arrays
    #define decl_pmc_to_pmt_numeric_array(type, suffix) \
    if (p.is<std::vector<type> >()) return pmt_init_ ## suffix ## vector(p.as<std::vector<type> >().size(), &p.as<std::vector<type> >()[0])
    decl_pmc_to_pmt_numeric_array(uint8_t, u8);
    decl_pmc_to_pmt_numeric_array(uint16_t, u16);
    decl_pmc_to_pmt_numeric_array(uint32_t, u32);
    decl_pmc_to_pmt_numeric_array(uint64_t, u64);
    decl_pmc_to_pmt_numeric_array(int8_t, s8);
    decl_pmc_to_pmt_numeric_array(int16_t, s16);
    decl_pmc_to_pmt_numeric_array(int32_t, s32);
    decl_pmc_to_pmt_numeric_array(int64_t, s64);
    decl_pmc_to_pmt_numeric_array(float, f32);
    decl_pmc_to_pmt_numeric_array(double, f64);
    decl_pmc_to_pmt_numeric_array(std::complex<float>, c32);
    decl_pmc_to_pmt_numeric_array(std::complex<double>, c64);

    //dictionary
    if (p.is<PMCDict>())
    {
        const PMCDict &m = p.as<PMCDict>();
        pmt_t d = pmt_make_dict();
        BOOST_FOREACH(const PMCPair &pr, m)
        {
            d = pmt_dict_add(d, pmc_to_pmt(pr.first), pmc_to_pmt(pr.second));
        }
        return d;
    }

    //set
    if (p.is<PMCSet>())
    {
        const PMCSet &s = p.as<PMCSet>();
        pmt_t l; //start w/ null - think this is ok
        BOOST_FOREACH(const PMCC &elem, s)
        {
            l = pmt_list_add(l, pmc_to_pmt(elem));
        }
        return l;
    }

    //is it already a pmt?
    if (p.is<pmt_t>()) return p.as<pmt_t>();

    //backup plan... boost::any
    return pmt_make_any(p);

}

inline PMC pmt_to_pmc(const pmt_t &p)
{
    
}

}

#endif /*INCLUDED_LIBGRAS_PMX_HELPER_HPP*/
