// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_SBUFFER_I
#define INCLUDED_GRAS_SBUFFER_I

%{
#include <gras/gras.hpp>
#include <gras/sbuffer.hpp>
%}

////////////////////////////////////////////////////////////////////////
// comprehend base class
////////////////////////////////////////////////////////////////////////
#define SWIG_INTRUSIVE_PTR_NAMESPACE boost
%include <intrusive_ptr.i>
%template (boost_intrusive_ptr_gras_sbuffer_impl) boost::intrusive_ptr<gras::SBufferImpl>;

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <gras/gras.hpp>
%include <gras/sbuffer.hpp>

////////////////////////////////////////////////////////////////////////
// Replace the get method
////////////////////////////////////////////////////////////////////////
%extend gras::SBuffer
{
    %insert("python")
    %{
        def get(self):
            from gras.GRAS_Utils import pointer_to_ndarray
            addr = long(self.get_actual_memory())
            readonly = hasattr(self, 'readonly') and getattr(self, 'readonly')
            return pointer_to_ndarray(
                addr=addr + self.offset,
                nitems=self.length,
                readonly=readonly
            )

        def __len__(self): return self.length

    %}
}

#endif /*INCLUDED_GRAS_SBUFFER_I*/
