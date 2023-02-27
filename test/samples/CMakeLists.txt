cmake_minimum_required(VERSION 3.13)


macro(add_executable_with_links exe_name cpp_list)
add_executable(${exe_name} ${cpp_list})
target_link_libraries(${exe_name} agge agge.text samples.common)
endmacro(add_executable_with_links)

if (WIN32 OR APPLE OR ANDROID)
	add_custom_command(OUTPUT arial-1000.fnt COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/text/assets/arial-1000.fnt "$<TARGET_FILE_DIR:text>")

	if (WIN32 OR APPLE)
		add_executable_with_links(balls "balls/balls.cpp;balls/balls_data.cpp")
		add_executable_with_links(figures figures/figures.cpp)
		add_executable_with_links(lines lines/lines.cpp)

		add_executable_with_links(piechart piechart/piechart.cpp)
		add_executable_with_links(rasterizer rasterizer/rasterizer.cpp)
		add_executable_with_links(sandbox sandbox/sandbox.cpp)
		add_executable_with_links(text "text/text.cpp;arial-1000.fnt")
		add_executable_with_links(balls-async "balls-async/balls-async.cpp;balls-async/balls_data.cpp")
        target_link_libraries(balls-async polyq)


		if (WIN32)
			add_executable_with_links(text-gdi text-gdi/text-gdi.cpp)
		endif()

		if (0)
			set_target_properties(balls balls-async figures lines piechart rasterizer sandbox text PROPERTIES
				MACOSX_BUNDLE YES
				MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/common/src/platform/apple/info.plist
				XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.agge.sample"
				XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
				XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
				XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
			)
		endif()
	elseif (ANDROID)
		add_library(balls SHARED balls/balls.cpp balls/balls_data.cpp)
		add_library(figures SHARED figures/figures.cpp)
		add_library(lines SHARED lines/lines.cpp)
		add_library(piechart SHARED piechart/piechart.cpp)
		add_library(rasterizer SHARED rasterizer/rasterizer.cpp)
		add_library(sandbox SHARED sandbox/sandbox.cpp)
		add_library(text SHARED text/text.cpp arial-1000.fnt)
		add_library(balls-async SHARED balls-async/balls-async.cpp balls-async/balls_data.cpp)
	endif()


endif()

