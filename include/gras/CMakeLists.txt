########################################################################
# install include files
########################################################################
install(FILES

    exception.i
    callable.hpp
    chrono.hpp
    block.hpp
    block_config.hpp
    block.i
    element.hpp
    element.i
    factory.hpp
    gras.hpp
    hier_block.hpp
    hier_block.i
    sbuffer.hpp
    sbuffer.i
    tags.hpp
    tags.i
    time_tag.hpp
    time_tag.i
    tag_iter.hpp
    tag_iter.i
    thread_pool.hpp
    top_block.hpp
    top_block.i
    work_buffer.hpp
    buffer_queue.hpp
    weak_container.hpp

    DESTINATION include/gras
    COMPONENT ${GRAS_COMP_DEVEL}
)

install(FILES

    detail/callable.hpp
    detail/block.hpp
    detail/chrono.hpp
    detail/element.hpp
    detail/factory.hpp
    detail/sbuffer.hpp
    detail/work_buffer.hpp

    DESTINATION include/gras/detail
    COMPONENT ${GRAS_COMP_DEVEL}
)
