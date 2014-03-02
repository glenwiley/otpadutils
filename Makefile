# Makefile for One Time Pad tools
# Glen Wiley <glen.wiley@gmail.com>
#

default:
	@echo "To build for your platform invoke make with the appropriate Makefil"
	@echo
	@echo "OSX:"
	@echo "  make -f Makefile.osx; make -f Makefile.osx install"
	@echo
	@echo "targets:"
	@echo "   default    builds binaries for all the utilities"
	@echo "   clean      remove .o files and other build artifacts (preserve eecutables)"
	@echo "   clobber    make clean and remove executables"
	@echo "   install    install binaries into system folders (probably needs root)"
	@echo "   uninstall  uninstall binaries from system folders (probably needs root)"
