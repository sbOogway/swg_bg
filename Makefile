SRC = src/main.go
TARGET = build/swg_gl

$(TARGET): $(SRC)
	go build -o $(TARGET) $(SRC)

run:
	swg_tmr ./$(TARGET)
