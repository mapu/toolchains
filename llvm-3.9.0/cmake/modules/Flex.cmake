
macro(flex project ofn)
  if (IS_ABSOLUTE ${LLVM_TARGET_LEXER})
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE ${LLVM_TARGET_LEXER})
  else()
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE 
      ${CMAKE_CURRENT_SOURCE_DIR}/${LLVM_TARGET_LEXER})
  endif()
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # Generate flex output in a temporary file.
    COMMAND flex -o ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp 
    ${LLVM_TARGET_DEFINITIONS_ABSOLUTE}
    # The file in LLVM_TARGET_DEFINITIONS may be not in the current
    # directory and local_tds may not contain it, so we must
    # explicitly list it here:
    DEPENDS ${LLVM_TARGET_DEFINITIONS_ABSOLUTE} ${BISON_OUTPUT}
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

  set(FLEX_OUTPUT ${FLEX_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/${ofn})
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${ofn} 
    PROPERTIES GENERATED 1)
endmacro(flex)

function(add_public_flex_target target)
  # Creates a target for publicly exporting tablegen dependencies.
  if( FLEX_OUTPUT )
    add_custom_target(${target}
      DEPENDS ${FLEX_OUTPUT})
    if (LLVM_COMMON_DEPENDS)
      add_dependencies(${target} ${LLVM_COMMON_DEPENDS})
    endif ()
    set_target_properties(${target} PROPERTIES FOLDER "Flexing")
  endif( FLEX_OUTPUT )
endfunction()
