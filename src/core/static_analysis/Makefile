ALL = except language store log
all: $(ALL)

RM = rm -rf


%:
	[[ " $(ALL) " =~ " $@ " ]] && sh ./cppcheck_report.sh $@

%.clean: 
	@if [[ " $(ALL) " =~ "$(subst .clean,, $@) " ]]; then echo " Deleting dir -$(subst .clean,, $@)"_report; $(RM) $(subst .clean,, $@)_report;  else echo "cannot find dir -$(subst .clean,, $@)"_report; fi

all.clean:
	$(foreach x, $(ALL), make $(x).clean;)
	$(RM) ./std.cfg

