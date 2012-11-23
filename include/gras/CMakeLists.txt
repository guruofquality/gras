########################################################################
# install include files
########################################################################
install(FILES

    block.hpp
    block.i
    element.hpp
    element.i
    gras.hpp
    hier_block.hpp
    hier_block.i
    io_signature.hpp
    io_signature.i
    sbuffer.hpp
    sbuffer.ipp
    sbuffer.i
    tags.hpp
    tags.i
    thread_pool.hpp
    top_block.hpp
    work_buffer.hpp
    work_buffer.ipp

    DESTINATION include/gras
    COMPONENT ${GRAS_COMP_DEVEL}
)
