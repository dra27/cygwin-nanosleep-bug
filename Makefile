all: signal.exe
	while bash ./signal.run ; do true; done

%.exe: %.o
	$(CC) -o $@ $^
