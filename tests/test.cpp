#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using ::testing::HasSubstr;

// Helper function to run the program and capture output
std::string runProgramWithInput(const std::string &inputFile) {
    // Command to run the executable, redirecting input and capturing output
    std::string command = "./../bin/ElaborationZoo -f ../../tests/" + inputFile;
    std::ostringstream output;

    // Open a pipe to read the program's output
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("Failed to run the program");

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output << buffer;
    }
    pclose(pipe);

    return output.str();
}

// Helper function to read the expected output file
std::string readFile(const std::string &filename) {
    std::ifstream file("../../tests/"+filename);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + filename);

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

// Helper function to generate file pairs
std::vector<std::pair<std::string, std::string>> generateTestFilePairs(int numTests) {
    std::vector<std::pair<std::string, std::string>> filePairs;
    for (int i = 0; i < numTests; ++i) {
        filePairs.emplace_back("pos" + std::to_string(i) + ".txt", "ans" + std::to_string(i) + ".txt");
    }
    return filePairs;
}

// Helper function to generate list of negative files
std::vector<std::string> generateNegTestFiles(int numTests) {
    std::vector<std::string> files;
    for (int i = 0; i < numTests; ++i) {
        files.emplace_back("neg" + std::to_string(i) + ".txt");
    }
    return files;
}

// Parameterized Test Fixture
class PositiveTests : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(PositiveTests, CompareProgramOutput) {
    auto [inputFile, expectedFile] = GetParam();

    // Run the program with input and capture the output
    std::string actualOutput = runProgramWithInput(inputFile);

    // Read the expected output
    std::string expectedOutput = readFile(expectedFile);

    // Compare actual and expected output
    ASSERT_EQ(actualOutput, expectedOutput) << "Mismatch for input file: " << inputFile;
}

// Define test cases with input and expected output files
INSTANTIATE_TEST_SUITE_P(
    ElaborationTests,
    PositiveTests,
    ::testing::ValuesIn(generateTestFilePairs(10))
);


// Parameterized Test Fixture
class NegativeTests : public ::testing::TestWithParam<std::string> {};

TEST_P(NegativeTests, CheckForErrors) {
    auto inputFile = GetParam();

    // Run the program with input and capture the output
    std::string actualOutput = runProgramWithInput(inputFile);

    // Compare actual and expected output
    EXPECT_THAT(actualOutput, HasSubstr("Error:")) << "Error not found in input file: " << inputFile;
}

// Define test cases with negative input files
INSTANTIATE_TEST_SUITE_P(
    ElaborationTests,
    NegativeTests,
    ::testing::ValuesIn(generateNegTestFiles(6))
);



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}