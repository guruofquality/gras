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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#include <gnuradio/sbuffer.hpp>
#include <numanuma.hpp>
#include <boost/bind.hpp>

using namespace gnuradio;

void gnuradio::sbuffer_handle_deref(SBufferImpl *impl)
{
    //call the deleter if possible
    boost::shared_ptr<SBufferDeleter> token_deleter = impl->config.token.lock();
    if (token_deleter)
    {
        SBuffer buff;
        buff.reset(impl);
        (*token_deleter)(buff);
    }
    else if (impl->config.deleter)
    {
        SBuffer buff;
        buff.reset(impl);
        impl->config.deleter(buff);
        impl->config.deleter = SBufferDeleter(); //reset deleter, so we dont double delete
    }
    else
    {
        delete impl; //its really dead now
    }
}

SBufferConfig::SBufferConfig(void)
{
    memory = NULL;
    length = 0;
}

static void numanuma_mem_deleter(SBuffer &, numanuma::mem *m)
{
    delete m;
}

static void default_allocator_deleter(SBuffer &, char *m)
{
    delete m;
}

static void default_allocator(SBufferConfig &config)
{
    if (config.affinity == Affinity())
    {
        char *m = new char[config.length + GRAS_MAX_ALIGNMENT - 1];
        size_t x = size_t(m) + GRAS_MAX_ALIGNMENT - 1;
        x -= x % GRAS_MAX_ALIGNMENT;
        config.memory = (void *)x;
        config.deleter = boost::bind(&default_allocator_deleter, _1, m);
    }
    else
    {
        numanuma::mem *m = numanuma::mem::make(config.affinity, config.length);
        config.memory = m->get();
        config.length = m->len();
        config.deleter = boost::bind(&numanuma_mem_deleter, _1, m);
    }
}

SBuffer::SBuffer(const SBufferConfig &config):
    offset(0),
    length(0)
{
    this->reset(new SBufferImpl(config));
    if (config.memory == NULL)
    {
        default_allocator((*this)->config);
    }
    this->length = this->get_actual_length();
}
