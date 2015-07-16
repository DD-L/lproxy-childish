CC = g++
RM = rm -rf

SRC = ./src
BIN = ./bin
TMP = ./tmp

#CFLAGS = -c -Wall -I. -D_DEBUG
CFLAGS = -c -Wall -I.
COMPILE = $(CC) $(CFLAGS) $^ -o $(TMP)/$@

lproxy: init proxy

proxy: main.o log.o server.o sock.o except.o \
    	thread.o http.o web.o predeal.o charcodecast.o
	cd $(TMP); $(CC) $^ -lpthread -liconv -o ../$(BIN)/$@ 
	$(CC) $(SRC)/sta.cpp -o $(BIN)/lproxy

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)

main.o: $(SRC)/main.cpp
	$(COMPILE)  	
log.o: $(SRC)/log.cpp
	$(COMPILE)
except.o: $(SRC)/except.cpp
	$(COMPILE)
server.o: $(SRC)/server.cpp
	$(COMPILE)
sock.o: $(SRC)/sock.cpp
	$(COMPILE)
thread.o: $(SRC)/thread.cpp
	$(COMPILE)
http.o: $(SRC)/http.cpp
	$(COMPILE)
web.o: $(SRC)/web.cpp
	$(COMPILE)
predeal.o: $(SRC)/predeal.cpp
	$(COMPILE)
charcodecast.o: $(SRC)/charcodecast.cpp
	$(COMPILE)
clean:
	$(RM) $(TMP)/*.o $(BIN)/* $(TMP) $(BIN)
