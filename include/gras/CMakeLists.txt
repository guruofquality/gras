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
    sbuffer.hpp
    sbuffer.i
    tags.hpp
    tags.i
    tag_iter.hpp
    tag_iter.i
    thread_pool.hpp
    top_block.hpp
    work_buffer.hpp
    buffer_queue.hpp

    DESTINATION include/gras
    COMPONENT ${GRAS_COMP_DEVEL}
)

install(FILES

    detail/sbuffer.hpp
    detail/work_buffer.hpp

    DESTINATION include/gras/detail
    COMPONENT ${GRAS_COMP_DEVEL}
)
