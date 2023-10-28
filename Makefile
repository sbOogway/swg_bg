SRC = main.c++
TARGET = life

$(TARGET): $(SRC)
	g++ $(SRC) -o $(TARGET) -ljpeg

run:
	swg_tmr ./$(TARGET)
