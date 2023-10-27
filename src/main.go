package main

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	"math/rand"
	"os"
	"os/exec"
	"strings"
)

var screens int
var height = 1080
var widthBase = 1920
var width int
var grid [][]int
var bufferImg *image.NRGBA
var green = color.RGBA{R: 0, G: 255, B: 0, A: 255}
var black = color.RGBA{R: 0, G: 0, B: 0, A: 255}

func updateScreens() {
	out, err := exec.Command("bash", "-c", "xrandr | grep \" connected \"").Output()
	if err != nil {
		fmt.Println("failed to look how many screens there are.")
		os.Exit(1)
	}
	screens = len(strings.Split(string(out), "\n")) - 1
	width = screens * widthBase

}

func initGrid() {
	for i := 0; i < height; i++ {
		row := make([]int, width)
		for j := 0; j < width; j++ {
			row[j] = rand.Intn(2)
		}
		grid = append(grid, row)
	}
}

func updateBufferImg() {
	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			if grid[y][x] == 1 {
				bufferImg.Set(x, y, green)
				continue
			}
			bufferImg.Set(x, y, black)
		}
	}
}

func updateBackground() {
	exec.Command("bash", "-c", "gsettings set org.gnome.desktop.background enable-animations false picture-uri 'file:///home/uomosucco/Desktop/dev/swg_bg/test.png'").Start()
}

func main() {
	updateScreens()
	initGrid()

	bufferImg = image.NewNRGBA(image.Rect(0, 0, width, height))

	outFile, err := os.Create("test.png")
	if err != nil {
		panic(err)
	}
	defer outFile.Close()

	updateBufferImg()

	err = png.Encode(outFile, bufferImg)
	if err != nil {
		panic(err)
	}

	// fmt.Println(grid)
	// fmt.Println(screens, width, height)

	updateBackground()

}
