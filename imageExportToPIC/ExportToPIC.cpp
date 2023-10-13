// Rewrite of pic2tga.cpp so that the functionality is reversed - converting a common image format to a proprietary one

#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include "app_exit.hpp" // messages before appplication closes
#include "tinyfiledialogs.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef uint32_t INT;
typedef uint16_t SHORT;
typedef uint8_t BYTE;

typedef struct PICHeader {
    INT file_size;
    SHORT magic; //Always 38144 (or 0, 149 in separate bytes)
    SHORT width;
    SHORT height;
    SHORT unknown1;
    SHORT unknown2;
    BYTE unknown3[50];
    SHORT palette_size; //I thought this was palette size - but it's not, it's always 776
    BYTE unknown4[6]; //Always zeros, probably reserved
    BYTE palette[256 * 3];
    SHORT unknown5; //In one file this was the number of bytes after and including this one, but not in others (??)
    BYTE unknown6[4]; //Seen (0, 0, 1, 0), (4, 0, 1, 0) - probably orientation (e.g flipped image)
} PICHeader;

void fillPalette(BYTE palette[], int size) {  // Test function: Each Component gradually decreases from 255 to 0
	for (int i = 0; i < size; i += 3) {
		palette[i] = 255 - i / 3; 
		palette[i + 1] = i / 3;
		palette[i + 2] = 255;
	}
}

int main(int argc, char* argv[]) {

	char const* lTheOpenFileName;
	char const* lTheSaveFileName;
	FILE* lIn;
	char const* lopenFilterPatterns[2] = { "*.png", "*.tga" };
	char const* lsaveFilterPatterns[1] = { "*.pic"};

	lTheOpenFileName = tinyfd_openFileDialog(
		"open image file",
		"",
		2,
		lopenFilterPatterns,
		"texture files",
		0);

	if (!lTheOpenFileName)
	{
		tinyfd_messageBox(
			"Error",
			"Open file name is NULL",
			"ok",
			"error",
			0);
		return 1;
	}

	int img_width, img_height, img_channels;
	unsigned char* img = stbi_load(lTheOpenFileName, &img_width, &img_height, &img_channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");

		APP_EXIT_readFAIL();
	}
		
	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img_width, img_height, img_channels);

	PICHeader header;
	//header.file_size; //unused
	header.height = img_height;
	header.width = img_width;
	header.palette_size = 776;
	
	header.magic = 38144; // magic number

	fillPalette(header.palette, 256 * 3);

	//..   header.uknownX magic might be required

	
	lTheSaveFileName = tinyfd_saveFileDialog(
		"let us save this password",
		"",
		1,
		lsaveFilterPatterns,
		NULL);

	if (!lTheSaveFileName)
	{
		tinyfd_messageBox(
			"Error",
			"Save file name is NULL",
			"ok",
			"error",
			1);
		return 1;
	}

	// Open a binary file for writing
	std::ofstream file(lTheSaveFileName, std::ios::binary);
	// Check if the file is opened successfully
	if (!file.is_open()) {
		// Failed to open the file
		std::cerr << "Error: Unable to open the file." << std::endl;
	}

	file.write(reinterpret_cast<const char*>(&header), sizeof(PICHeader));

	for (int i = 0; i < img_width * img_height; ++i) {
		// Convert RGB pixel to palette index
		BYTE palette_index = 0; // Set appropriate palette index based on the RGB color from the image data

		// Write palette index to the output file
		file.write(reinterpret_cast<const char*>(&palette_index), sizeof(BYTE));
	}

	file.close();

    printf("Exported %s to %s successfully!\n", lTheOpenFileName, lTheSaveFileName); // placeholder for now
    waitForAnyKey();
    return 0;
}