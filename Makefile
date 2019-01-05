CC=$(shell which g++-8 2>/dev/null||which g++-7 2>/dev/null)
CCNAME=$(CC)
DPROG::=efld
KR::=/dev/null 2>&1
KF::= 2>/dev/null
CFLAGS::=-c -Wall 
# -std=gnu++1z
CFLAGS::=$(CFLAGS) -g -I. -I$$HOME/cdk/include -I/usr/include/ncursesw 
LDFLAGS::=-lncursesw
# -lstdc++fs
EXEC::=$(DPROG)
ICH::=$(firstword $(MAKEFILE_LIST))
SRCS::=$(wildcard *.cpp)
WRZ::=$(SRCS:.cpp=)
OBJ::=$(SRCS:.cpp=.o)
BA::=&1
BFA::=2>>fehler.txt
ifdef ausvi
 BA::=/dev/null
endif
EXPFAD::=$(shell echo $(PATH) | tr -s ':' '\n' | grep /usr/ | awk '{ print length, $$0 }' | sort -n -s | cut -d" " -f2- | head -n1)
INSTEXEC::=$(EXPFAD)/$(EXEC)
LAUF::=0
GDAT::=$(DTN:inst.log=)
GDAT::=$(GDAT:uninstallinv=)
GDAT::=$(GDAT:Makefile=Makefile.roh)
DEPDIR ::= .d
$(shell mkdir -p $(DEPDIR)>$(KR))
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
rot::="\033[1;31m"
gruen::="\033[0;32m"
#blau="\033[0;34;1;47m"
blau::="\033[1;34m"
reset::="\033[0m"


.PHONY: all
all: anzeig weiter
.PHONY: neu
neu: loesche all
.PHONY: loesche
loesche:
	rm $(EXEC) *.o
.PHONY: anzeig
anzeig:
# 'echo -e' geht nicht z.B. in ubuntu
	@[ "$(DPROG)" ]||{ printf "Datei/File %b'vars'%b fehlerhaft/faulty, bitte vorher/please call %b'./install.sh'%b aufrufen/before!\n" \
	       $(blau) $(reset) $(blau) $(reset);let 0;}
	@printf " %bGNU Make%b, Target file/Zieldatei: %b%s%b, before/vorher:                        \n" $(gruen) $(reset) $(rot) "$(EXEC)" $(reset) >$(BA)
	@printf " '%b%s%b'\n" $(blau) "$$(ls -l --time-style=+' %d.%m.%Y %H:%M:%S' --color=always $(EXEC) $(KF))" $(reset) >$(BA)
	@printf " Source files/Quelldateien: %b%s%b\n" $(blau) "$(SRCS)" $(reset) >$(BA)
	@printf " Compiler: %b%s%b, installed as/installiert als: %b%s%b; Compiler Parameter: %b%s%b\n"\
	        $(blau) "$(CCName)" $(reset) $(blau) "$(CCInst)" $(reset) $(blau) "$(CFLAGS)" $(reset)
	@printf " Target path for/Zielpfad fuer '%bmake install%b': %b%s%b\n" $(blau) $(reset) $(blau) "'$(EXPFAD)'" $(reset) >$(BA)
	-@rm -f fehler.txt $(KF)

.PHONY: weiter
weiter: $(EXEC) $(GZS) 

$(EXEC): $(OBJ)
	-@printf " linking/verlinke %s to/zu %b%s%b ..." "$(OBJ)" $(blau) "$@" $(reset) >$(BA)
	-@printf " (Version: %b%s%s%b\n " $(blau) "$$(cat versdt)" ")" $(reset) >$(BA)
	$(CC) $^ $(DEBUG)-o $@ $(LDFLAGS)
	-@ls .d/*.Td >$(KR) &&{ for datei in .d/*.Td; do mv -f $${datei} $${datei%.Td}.d; done;};:
	$(shell touch *.o $${EXEC})
	-@printf " Fertig mit/Finished with %b$(ICH)%b, Target: %b$@%b:, nachher/afterwords:\n" $(blau) $(reset) $(blau) $(reset)
	-@printf " '%b%s%b'\n" $(blau) "$$(ls -l --time-style=+' %d.%m.%Y %H:%M:%S' --color=always $(EXEC))" $(reset)

define machvers	 
	VD="versdt";[ -f $$VD ]||echo 0.1>$$VD;:
endef

%.o: %.cpp
%.o: %.cpp $(DEPDIR)/%.d
	-@$(eval LAUF=$(shell echo $$(($(LAUF)+1))))
	-@[ $(LAUF) = 1 ]&& rm -f fehler.txt;:
	@[ $@ = $(DPROG).o ]&&{ $(call machvers);if test -f entwickeln; then awk "BEGIN {print `cat versdt`+0.00001}">versdt;\
	else printf " %bFile '%bentwickeln%b' missing, keeping the version number stable/ Datei '%bentwickeln%b' fehlt, lasse die Version gleich%b\n" \
	$(schwarz) $(grau) $(schwarz) $(grau) $(schwarz) $(reset) >$(BA); fi;}; :;
	@printf " kompiliere %b%s%b: " $(blau) "$<" $(reset) >$(BA);
#	-@if ! test -f instvz; then printf \"$$(getent passwd $$(logname)|cut -d: -f6)\">instvz; fi;
	-@if ! test -f instvz; then printf \"$$(pwd)\">instvz; fi; # wird in kons.cpp verwendet
	-$(CC) $(DEBUG)$(DEPFLAGS) $(CFLAGS) $< $(BFA);
	-@sed -i 's/versdt //g;s/gitvdt //g' $(DEPDIR)/*.Td
	-@if test -s fehler.txt; then vi +0/error fehler.txt; else rm -f fehler.txt; fi;
#	-@$(shell $(POSTCOMPILE))
	@if test -s fehler.txt; then echo Fehler!!!!!!; fi;
	@if test -s fehler.txt; then false; fi;

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

git:
	git add -u
	git commit -m"aus Makefile"
	git push

.PHONY: comp
comp:
	@echo CC=\'$(CC)\'

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))
