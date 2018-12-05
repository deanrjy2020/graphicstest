#include <string>
#include <iostream>
#include <iomanip>

#include "common.h"

// global variables
extern char *executableName;
extern bool interactive;
extern char *runSingleTestName;
extern char *outputDir;
extern bool callHelp;

static void printHelp(std::string cmd, std::string des) {
    std::cout << "  " << std::left << std::setw(20) << cmd << "-- " << des << std::endl;
}

static void printUsage() {
    printHelp("no arguements", "run all the supported tests.");
    printHelp("-i", "interactive mode.");
    printHelp("-o <dir>", "image output directory.");
    printHelp("-t <test>", "run test by name.");
}

void parseCmdLine(int argc, char** argv) {
    executableName = argv[0];
    for (int i = 1; i < argc; i++) {
        // sorted by lexicographically.
        if (!std::strcmp(argv[i], "-h")) {
            printUsage();
            callHelp = true;
            break; // ignore the para after -h.
        }
        if (!std::strcmp(argv[i], "-i")) {
            interactive = true;
            continue;
        }
        if (!std::strcmp(argv[i], "-o")) {
            // basically, need to check the next parameter is available.
            outputDir = argv[++i];
            continue;
        }
        if (!std::strcmp(argv[i], "-t")) {
            // basically, need to check the next parameter is available.
            runSingleTestName = argv[++i];
            continue;
        }

    }
}

// there is only one Common instance.
Common com;

int main(int argc, char **argv) {

    // Parse the parameters.
    parseCmdLine(argc, argv);

    // user calls "-h", skip everything.
    if (callHelp) {
        return 1;
    }

    com.runAllTests();

    return 1;
}


/* Dean todo:
 * 1) add -i loop
 * 2) run the test list and check the output.
 * 3, file name is the class name, one file = one class.
 * 4, class members start with m_xx
 * 5, int function {
 * }
 *  6, try to use all the std::
 * 7, impl the save to PNG???
 * 8, class name EGL and GL, use captial.
 * 9, run this program on Android and L4T (build locally? or in the l4t tree?)
*/
