if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/api/lib)
endif()

if(UNIX)
    link_directories(${CMAKE_SOURCE_DIR}/api/so)
endif()

add_executable(${PROJECT_NAME} main.cpp MarketData.cpp Trader.cpp record.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/api/include)

file(COPY ${CMAKE_SOURCE_DIR}/broker/ DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/broker)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE thosttraderapi_se thostmduserapi_se)
    file(COPY ${CMAKE_SOURCE_DIR}/api/dll/ DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
endif()

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE thostmduserapi_se thosttraderapi_se)
    file(COPY ${CMAKE_SOURCE_DIR}/api/so/ DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
endif()
