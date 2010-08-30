#
# create by: jianjun jiang <jerryjianjun@gmail.com>.
#

JAVAC		:= javac
JAR			:= jar
RM			:= -rm -fr

.PHONY: all clean

all:
	$(JAVAC) -encoding ascii DumpPublicKey.java
	$(JAR) -cvfm DumpPublicKey.jar DumpPublicKey.mf DumpPublicKey.class

clean:
	$(RM) *.class *.jar *~
