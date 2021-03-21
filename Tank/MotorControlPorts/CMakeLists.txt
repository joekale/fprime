####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MotorEnablePortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MotorSpeedControlPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SimpleMotorControlPortAi.xml"
)

register_fprime_module()
