function(BuildConfigFile infile outfile contentfile var)

    file(READ ${contentfile} ${var})
    string(STRIP ${${var}} ${var})

    configure_file(${infile} ${outfile} @ONLY)

endfunction()

if (RUN_BUILD_CONFIG_FILE)
    BuildConfigFile(${FILE_IN} ${FILE_OUT} ${FILE_CONTENT} ${VAR_NAME})
endif ()