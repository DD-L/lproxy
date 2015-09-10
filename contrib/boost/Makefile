RM = rm -rf
PWD = $(shell pwd)

BOOST_PACKAGE = boost_1_57_0.7z
BOOST_SRC = ./boost_1_57_0


all: boost

boost:
	dos2unix ./boost_init.sh > /dev/null 2>&1
	sh ./boost_init.sh

boost.force:
	dos2unix ./boost_init.sh > /dev/null 2>&1
	sh ./boost_init.sh -force


clean: boost.clean

boost.clean:
	$(RM) $(BOOST_SRC)




