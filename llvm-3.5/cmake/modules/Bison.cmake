
macro(bison project ofn)
  if (IS_ABSOLUTE ${LLVM_TARGET_PARSER})
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE ${LLVM_TARGET_PARSER})
  else()
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE 
      ${CMAKE_CURRENT_SOURCE_DIR}/${LLVM_TARGET_PARSER})
  endif()
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # Generate tablegen output in a temporary file.
    COMMAND /usr/bin/bison -o ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp 
    ${LLVM_TARGET_DEFINITIONS_ABSOLUTE}
    # The file in LLVM_TARGET_DEFINITIONS may be not in the current
    # directory and local_tds may not contain it, so we must
    # explicitly list it here:
    DEPENDS ${LLVM_TARGET_DEFINITIONS_ABSOLUTE}
    COMMENT "Building ${ofn}..."
    )
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    # Only update the real output file if there are any differences.
    # This prevents recompilation of all the files depending on it if there
    # aren't any.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    COMMENT ""
    )

  # `make clean' must remove all those generated files:
  set_property(DIRECTORY APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ofn}.tmp ${ofn})

  set(BISON_OUTPUT ${BISON_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/${ofn})
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${ofn} 
    PROPERTIES GENERATED 1)
endmacro(bison)

function(add_public_bison_target target)
  # Creates a target for publicly exporting tablegen dependencies.
  if( BISON_OUTPUT )
    add_custom_target(${target}
      DEPENDS ${BISON_OUTPUT})
    if (LLVM_COMMON_DEPENDS)
      add_dependencies(${target} ${LLVM_COMMON_DEPENDS})
    endif ()
    set_target_properties(${target} PROPERTIES FOLDER "Bisoning")
  endif( BISON_OUTPUT )
endfunction()
