set(c_SOURCE_FILES
    "BoardDev34.cpp"
    "HTTPC2Server.cpp"
)

set(c_PRIVATE_REQUIREMENTS
    BoardManager
    Debouncer
)

idf_component_register(
    SRCS ${c_SOURCE_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${c_PRIVATE_REQUIREMENTS}
)
