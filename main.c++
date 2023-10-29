#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <random>
#include <vector>
#include <chrono>

#include <jpeglib.h>

#define W 1920
#define H 1080

int scaling = 2;

int screens = 0;
int width = 0;
int height = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> distribution(0, 1);

struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
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
    for (size_t x = 0; x < grid.size(); x++) {
        for (size_t y = 0; y < grid[x].size(); y++) {
            grid[x][y] = distribution(gen);
        }
    }
    return grid;
}

std::vector<std::vector<int>> new_generation(std::vector<std::vector<int>> grid) {
    std::vector<std::vector<int>> new_gen(width, std::vector<int>(height, 0));
    for (size_t x = 1; x < grid.size()-1; x++) {
        for (size_t y = 1; y < grid[x].size()-1; y++) {
            int neighbors = 
            grid[x-1][y-1] + grid[x][y-1] + grid[x+1][y-1] + 
            grid[x-1][y]   +                grid[x+1][y]   +                    
            grid[x-1][y+1] + grid[x][y+1] + grid[x+1][y+1];
            switch (grid[x][y])
            {
            case 1:
                if (neighbors == 2 || neighbors == 3) new_gen[x][y] = 1;
                continue;
            case 0:
                if (neighbors == 3) new_gen[x][y] = 1;
                continue;
            }
            new_gen[x][y] = 0;
            
        }
    }
    return new_gen;
}

void export_jpg(std::vector<std::vector<int>>& grid) {   
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

    JSAMPROW row_buffer = new JSAMPLE[cinfo.image_width * 3]; 

    while (cinfo.next_scanline < cinfo.image_height) {
        for (int x = 0; x < width; x++) {
            row_buffer[x * 3] = row_buffer[x * 3 + 2] = 0;
            row_buffer[x * 3 + 1] = grid[x][cinfo.next_scanline] ? 255 : 0;
        }
        jpeg_write_scanlines(&cinfo, &row_buffer, 1);
    }
    jpeg_finish_compress(&cinfo);    
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    free(row_buffer);
    
}


int main (int argc, char *argv[]) {
    // screens = get_screens();

    width = W;
    height = H;

    // std::vector<std::vector<int>> grid(width, std::vector<int>(height, 1));
    std::vector<std::vector<int>> bufferGrid(width, std::vector<int>(height, 0));


    // std::cout << grid[width-1][height-1] << std::endl;
    // std::cout << bufferGrid[width-1][height-1] << std::endl;
    // bufferGrid = grid;
    // std::cout << bufferGrid[width-1][height-1] << std::endl;


    std::cout << bufferGrid.size() << "x" << bufferGrid[0].size() <<  std::endl;

    bufferGrid = init_grid(bufferGrid);  
    

    while (1) {
        auto start = std::chrono::high_resolution_clock::now();
        bufferGrid = new_generation(bufferGrid);
        auto new_gen = std::chrono::high_resolution_clock::now();
        auto new_gen_dur = std::chrono::duration_cast<std::chrono::microseconds>(new_gen-start);
        export_jpg(bufferGrid);
        auto exporting = std::chrono::high_resolution_clock::now();
        auto exporting_dur = std::chrono::duration_cast<std::chrono::microseconds>(exporting-new_gen);

        std::cout << "new gen duration \t->" <<  new_gen_dur.count() << " microseconds" << std::endl;
        std::cout << "exporting duration \t->" <<  exporting_dur.count() << " microseconds" << std::endl;

        system("gsettings set org.gnome.desktop.background picture-uri 'file:///home/uomosucco/Desktop/dev/swg_bg/swg_test.jpg'");

        system("sleep 5");

    }   

    return 0;
}


