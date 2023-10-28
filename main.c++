#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <random>
#include <vector>

#include <jpeglib.h>

#define W 1920
#define H 1080

int screens = 0;
int width = 0;
int height = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> distribution(0, 1);

char stdout_buffer[128];

int get_screens() {
    FILE* pipe = popen("xrandr | grep \" connected \"", "r");
    if (!pipe) {
        exit(EXIT_FAILURE);
    }

    while (fgets(stdout_buffer, sizeof(stdout_buffer), pipe) != NULL) {
        // std::cout << stdout_buffer;
        screens += 1;
    }

    pclose(pipe);

    return screens;
}

std::vector<std::vector<int>> init_grid(std::vector<std::vector<int>> grid) {
    for (size_t i = 0; i < grid.size(); i++) {
        for (size_t j = 0; j < grid[i].size(); j++) {
            grid[i][j] = distribution(gen);
        }
    }
    return grid;
}

std::vector<std::vector<int>> new_generation(std::vector<std::vector<int>> grid) {
    std::vector<std::vector<int>> new_gen(width, std::vector<int>(height, 0));
    for (size_t i = 1; i < grid.size()-1; i++) {
        for (size_t j = 1; j < grid[i].size()-1; j++) {
            int neighbors = grid[i-1][j-1] + grid[i][j-1] + grid[i+1][j-1] +
            grid[i-1][j] + grid[i+1][j] +
            grid[i-1][j+1] + grid[i][j+1] + grid[i+1][j+1];

            if (grid[i][j]) {
                if (neighbors == 2 || neighbors == 3) {
                    new_gen[i][j] = 1;
                    continue;
                }
            }
            if (!grid[i][j]) {
                if (neighbors == 3) {
                    new_gen[i][j] = 1;
                    continue;
                }
            }
            new_gen[i][j] = 0;
        }
    }
    return new_gen;
}

void export_jpg(std::vector<std::vector<int>> grid) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);    

    FILE* outfile = fopen("swg_test.jpg", "wb");
    if (!outfile) {
        exit(1);
    }
    
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        JSAMPROW row_buffer = new JSAMPLE[cinfo.image_width * 3];

        for (int x = 0; x < width; x++) {
            row_buffer[x * 3] = 0;
            row_buffer[x * 3 + 2] = 0;
            row_buffer[x * 3 + 1] =  grid[cinfo.next_scanline][x] ? 255 : 0;

        }

        jpeg_write_scanlines(&cinfo, &row_buffer, 1);
        delete[] row_buffer;
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);



}


int main (int argc, char *argv[]) {
    screens = get_screens();

    width = W * screens;
    height = H;

    std::vector<std::vector<int>> grid(width, std::vector<int>(height, 1));
    std::vector<std::vector<int>> bufferGrid(width, std::vector<int>(height, 0));


    std::cout << grid[width-1][height-1] << std::endl;
    std::cout << bufferGrid[width-1][height-1] << std::endl;
    bufferGrid = grid;
    std::cout << bufferGrid[width-1][height-1] << std::endl;


    std::cout << grid[0].size() << std::endl;

    bufferGrid = init_grid(bufferGrid);


    while (1) {
        bufferGrid = new_generation(bufferGrid);

        export_jpg(bufferGrid);
        system("gsettings set org.gnome.desktop.background picture-uri 'file:///home/uomosucco/Desktop/dev/swg_bg/swg_test.jpg'");

        // system("sleep 5");

    }

    

    return 0;
}


