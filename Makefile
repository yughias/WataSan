SRC = $(wildcard src/*.c)

all: 
	gcc -O3 $(SRC) -flto -Iinclude -Llib -lmingw32 -lSDL2 -lopengl32 -lshlwapi -lcomdlg32 -lole32 -o watasan.exe

emcc:
	emcc $(SRC) -Iinclude -O3 -flto -sUSE_SDL=2 -sNO_INVOKE_RUN -o website/emulator.js