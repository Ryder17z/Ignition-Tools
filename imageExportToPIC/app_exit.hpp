#include <iostream>
#include <stdio.h>

// Pause methods
#ifdef _WIN32
// For windows
void waitForAnyKey() {
	system("pause");
}
#elif __linux__
// For linux
void waitForAnyKey() {
	std::cout << "Press any key to continue...";
	system("read -s -N 1"); // Continues when pressed a key like windows
}
#endif

#define PERMISSIONS_STRING "Please verify file/folder R/W permissions\n"
#define APP_EXIT_STRING "The application will now exit.\n\n"

void APP_IO_Error(const char *str) {
	printf(str);
	printf(PERMISSIONS_STRING);
	printf(APP_EXIT_STRING);
}

void APP_EXIT_writeFAIL() {
	APP_IO_Error("Error writing to output.file\n");
	waitForAnyKey();
	exit(0); // RW failure
}

void APP_EXIT_readFAIL() {
	APP_IO_Error("Error reading input.file\n");
	printf(APP_EXIT_STRING);
	waitForAnyKey();
	exit(0); // RW failure
}