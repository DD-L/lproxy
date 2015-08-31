#
# make
# make all
# make core/language
#
# make all.clean
# make core/language.clean
#

ALL = core/except core/language core/store core/log
RM = rm -rf
#RMDIR = rmdir --ignore-fail-on-non-empty
RM_EMPTY_DIR = rmdir

all: $(ALL) msg

msg:
	@echo -e "\ninfo: static analysis completed (by cppcheck)."

%:
	@dos2unix ./cppcheck_report.sh > /dev/null 2>&1
	@echo -e "\n[ $@ ] ..."
	@if [[ " $(ALL) " =~ " $@ " ]]; then sh ./cppcheck_report.sh $@; fi


%.clean: 
	@if [[ " $(ALL) " =~ "$(subst .clean,, $@) " ]]; then echo " Deleting dir -$(subst .clean,, $@)"_report; $(RM) $(subst .clean,, $@)_report;  else echo "cannot find dir -$(subst .clean,, $@)"_report; fi

all.clean:
	$(foreach x, $(ALL), make $(x).clean;)
	$(RM) ./std.cfg
	if [ -d ./core ];then if [ x"`ls -A ./core`" = x"" ];then $(RM_EMPTY_DIR) ./core;fi;fi
	if [ -d ./gui ];then if [ x"`ls -A ./gui`" = x"" ];then $(RM_EMPTY_DIR) ./gui;fi;fi

