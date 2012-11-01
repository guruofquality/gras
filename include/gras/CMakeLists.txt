########################################################################
# install include files
########################################################################
install(FILES

    block.hpp
    element.hpp
    gras.hpp
    hier_block.hpp
    io_signature.hpp
    sbuffer.hpp
    sbuffer.ipp
    tags.hpp
    thread_pool.hpp
    top_block.hpp

    DESTINATION include/gras
    COMPONENT ${GRAS_COMP_DEVEL}
)
