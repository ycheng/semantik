
SOURCES=Kadapplet.java Mapview.java TextCoord.java

.java.class:
	javac $<

all: Kadapplet.jar

Kadapplet.jar: $(SOURCES) $(SOURCES:.java=.class)
	jar cvf $@ $(SOURCES:.java=.class) pics/

clean:
	\rm -f *.jar *.class

view: Kadapplet.jar
	appletviewer main.html

.SUFFIXES:
.SUFFIXES: .java .class

