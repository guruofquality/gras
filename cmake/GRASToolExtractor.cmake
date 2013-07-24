
function(GRAS_TOOL_EXTRACT_GRC_INLINE_SOURCES source)

    file(READ ${source} contents)
    unset(grc_file_paths)

    #tokenizing constants
    set(BEGIN_TOKEN "BEGIN_GRC_INLINE_FILE")
    string(LENGTH "${BEGIN_TOKEN}" BEGIN_TOKEN_LENGTH)
    set(END_TOKEN "END_GRC_INLINE_FILE")
    string(LENGTH "${END_TOKEN}" END_TOKEN_LENGTH)

    while(TRUE)

        #search for the begin and end tokens
        string(FIND "${contents}" "${BEGIN_TOKEN}" begin)
        string(FIND "${contents}" "${END_TOKEN}" end)
        if (${begin} EQUAL -1 OR ${end} EQUAL -1)
            break()
        endif()

        #extract code inbetween the tokens
        math(EXPR begin "${begin} + ${BEGIN_TOKEN_LENGTH}")
        math(EXPR length "${end} - ${begin}")
        string(SUBSTRING "${contents}" ${begin} ${length} grc_code)

        #extract the file path after the token before newline
        string(FIND "${grc_code}" "\n" newline)
        string(SUBSTRING "${grc_code}" 0 ${newline} file_path)
        string(SUBSTRING "${grc_code}" ${newline} -1 grc_code)
        string(STRIP "${file_path}" file_path)
        list(APPEND grc_file_paths ${file_path})

        #take the next subset of contents for next iteration
        math(EXPR end "${end} + ${END_TOKEN_LENGTH}")
        string(SUBSTRING "${contents}" ${end} -1 contents)
        message(STATUS "next contents: ${contents}")

    endwhile()


endfunction(GRAS_TOOL_EXTRACT_GRC_INLINE_SOURCES)
