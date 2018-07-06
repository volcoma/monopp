find_program(
	CLANG_TIDY_EXE
	NAMES "clang-tidy"
	DOC "Path to clang-tidy executable"
)
set(CLANG_TIDY_SNAKE_CASE_CONF "\
-config={ \
Checks: '-*,readability-identifier-naming', \
HeaderFilterRegex: '', \
CheckOptions: \
  - { key: readability-identifier-naming.ClassCase, value: lower_case }\
  - { key: readability-identifier-naming.StructCase, value: lower_case }\
  - { key: readability-identifier-naming.ParameterCase, value: lower_case }\
  - { key: readability-identifier-naming.FunctionCase, value: lower_case }\
  - { key: readability-identifier-naming.VariableCase, value: lower_case }\
  - { key: readability-identifier-naming.MemberCase, value: lower_case }\
  - { key: readability-identifier-naming.PrivateMemberCase, value: lower_case }\
  - { key: readability-identifier-naming.ProtectedMemberCase, value: lower_case }\
  - { key: readability-identifier-naming.PublicMemberCase, value: lower_case }\
  - { key: readability-identifier-naming.PrivateMemberSuffix, value: _ }\
  - { key: readability-identifier-naming.ProtectedMemberSuffix, value: _ }\
}")

macro(set_target_clang_tidy_args target ARGS_LIST)
	if(CLANG_TIDY_EXE)
		#message(STATUS "==========================")
		#message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
		#message(STATUS "clang-tidy command line: ${ARGS_LIST}")
		#message(STATUS "==========================")

		set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
				"${ARGS_LIST}"
			)
		set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

		set_target_properties(${target} PROPERTIES
			CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
		)
	endif()
endmacro()

macro(set_code_style)
set(target ${ARGV0})
set(preset ${ARGV1})

if(preset MATCHES "snake_case")
	set(CLANG_TIDY_CHECKS_CONF ${CLANG_TIDY_SNAKE_CASE_CONF})
else()
	set(CLANG_TIDY_CHECKS_CONF ${preset})
endif()

if(BUILD_WITH_CODE_STYLE_CHECKS)
	message(STATUS "========================")
	message(STATUS "target : ${target}")
	message(STATUS "requested style preset : ${preset}")
	message(STATUS "========================")
	set_target_clang_tidy_args(${target} "${CLANG_TIDY_CHECKS_CONF}")
endif()
endmacro()
