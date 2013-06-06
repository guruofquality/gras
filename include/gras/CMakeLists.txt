########################################################################
# install include files
########################################################################
install(FILES

    exception.i
    chrono.hpp
    block.hpp
    block_config.hpp
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
    weak_container.hpp

    DESTINATION include/gras
    COMPONENT ${GRAS_COMP_DEVEL}
)

install(FILES

    detail/block.hpp
    detail/chrono.hpp
    detail/element.hpp
    detail/sbuffer.hpp
    detail/work_buffer.hpp

    DESTINATION include/gras/detail
    COMPONENT ${GRAS_COMP_DEVEL}
)
