####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####

set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/LinuxPWMDriverComponentAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/LinuxPWMDriverComponentImpl.cpp"
)

register_fprime_module()
