# 
# make init
#
# make
# make all
# make core/language
# make core
#
# make all.clean
# make core/language.clean
# make core.clean
#

PWD=$(shell pwd)
ROOT=$(PWD)/../../../
SRCROOT=$(ROOT)/src/

RM = rm -rf
#RMDIR = rmdir --ignore-fail-on-non-empty
RM_EMPTY_DIR = rmdir

CPPCHECK_OPT_EX = --report-progress -j 4 --max-configs=12
#CPPCHECK_OPT_EX = --report-progress -j 4 --force
ALL = core/except core/language core/store core/log

init:
	@dos2unix ./cppcheck_report.sh > /dev/null 2>&1
	sh ./cppcheck_report.sh $@
	

all: $(ALL) msg

msg:
	@echo -e "\ninfo: static analysis completed (by cppcheck)."

%:
	@dos2unix ./cppcheck_report.sh > /dev/null 2>&1
	@echo -e "\n[ $@ ] ..."
	@if [[ " $(ALL) " =~ " $@ " ]]; then sh ./cppcheck_report.sh $@ $(CPPCHECK_OPT_EX); fi

core:
	@dos2unix ./cppcheck_report.sh > /dev/null 2>&1
	@echo -e "\n[ core ] ..."
#	检查$(ROOT)/src/core, 但忽略(-i) $(ROOT)/src/core/boost_built 
#	-i 参数必须为相对路径（相对当前文件目录）, 这应该是 cppcheck的一个bug
	sh ./cppcheck_report.sh core $(CPPCHECK_OPT_EX) -i ../../core/boost_built 
gui:
	@dos2unix ./cppcheck_report.sh > /dev/null 2>&1
	@echo -e "\n[ $@ ] ..."
	sh ./cppcheck_report.sh $@ $(CPPCHECK_OPT_EX)

%.clean: 
	@if [[ " $(ALL) " =~ "$(subst .clean,, $@) " ]]; then echo " Deleting dir -$(subst /,_report/, $(subst .clean,, $@))"_report; $(RM) $(subst /,_report/, $(subst .clean,, $@))_report;  else echo "cannot find dir -$(subst /,_report/, $(subst .clean,, $@))"_report; fi

all.clean: core.clean gui.clean
	$(foreach x, $(ALL), make $(x).clean;)
	$(RM) ./std.cfg ./gcc_include_cpp_path

core.clean:
	echo " Deleting dir - ./core_report..."
	$(RM) ./core_report

gui.clean:
	echo " Deleting dir - ./gui_report..."
	$(RM) ./gui_report

