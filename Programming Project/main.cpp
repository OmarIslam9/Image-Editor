#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <commdlg.h>

using namespace std;
using namespace cv;

// Function to open file dialog for selecting an image
string openFileDialog() {
    OPENFILENAMEA ofn;       // common dialog box structure for ANSI version
    char szFile[260];        // buffer for file name
    string fileName;

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;

    // Display the Open dialog box.
    if (GetOpenFileNameA(&ofn) == TRUE) {
        fileName = ofn.lpstrFile;
    }

    return fileName;
}

// Function to save edited image with a default file name and path
string saveEditedImage(const Mat& image) {
    string defaultSavePath = "C:/Users/Omar Islam/Pictures/edited_image.jpg"; // Default save path and filename
    if (imwrite(defaultSavePath, image)) {
        cout << "Edited image saved as '" << defaultSavePath << "'." << endl;
        return defaultSavePath;
    }
    else {
        cout << "Error: Failed to save edited image to '" << defaultSavePath << "'." << endl;
        return ""; // Return empty string to indicate failure
    }
}

// Function to display instructions on the tools window
void displayToolInstructions() {
    Mat toolsWindow = Mat::zeros(200, 300, CV_8UC3);
    putText(toolsWindow, "Available Tools:", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 1, LINE_AA);
    putText(toolsWindow, "1. Convert to Grayscale", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
    putText(toolsWindow, "2. Increase Brightness", Point(10, 90), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
    putText(toolsWindow, "3. Decrease Brightness", Point(10, 120), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
    putText(toolsWindow, "4. Reset to Original Image", Point(10, 150), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
    putText(toolsWindow, "5. Save Image", Point(10, 180), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);

    imshow("Tools", toolsWindow);
}

int main() {
    string imagePath = openFileDialog();
    if (imagePath.empty()) {
        cout << "No image selected. Exiting program." << endl;
        return 0;
    }

    Mat originalImage = imread(imagePath);
    if (originalImage.empty()) {
        cout << "Error: Unable to load image from path: " << imagePath << endl;
        return -1;
    }

    Mat image = originalImage.clone(); // Clone the original image for editing

    const int defaultWindowWidth = 800;
    const int defaultWindowHeight = 600;
    int windowHeight, windowWidth;
    double aspectRatio = (double)image.cols / image.rows;
    if (aspectRatio > 1.0) {
        windowWidth = defaultWindowWidth;
        windowHeight = cvRound(defaultWindowWidth / aspectRatio);
    }
    else {
        windowHeight = defaultWindowHeight;
        windowWidth = cvRound(defaultWindowHeight * aspectRatio);
    }

    namedWindow("Edited Image", WINDOW_NORMAL);
    resizeWindow("Edited Image", windowWidth, windowHeight);
    imshow("Edited Image", image);

    displayToolInstructions(); // Display the tools window

    string savePath;

    bool isEdited = false;
    char lastKey = 0;

    double brightnessLevel = 0.0; // Initial brightness level

    while (true) {
        char key = waitKey(0);
        if (key == lastKey) {
            // Toggle effect
            if (isEdited) {
                image = originalImage.clone();
                brightnessLevel = 0.0;
            }
            else {
                switch (key) {
                case '1':
                    cvtColor(image, image, COLOR_BGR2GRAY);
                    break;
                case '2':
                    // Increase brightness
                    brightnessLevel += 40.0;
                    image.convertTo(image, -1, 1.0, brightnessLevel);
                    break;
                case '3':
                    // Decrease brightness
                    brightnessLevel -= 40.0;
                    image.convertTo(image, -1, 1.0, brightnessLevel);
                    break;
                case '4':
                    image = originalImage.clone();
                    brightnessLevel = 0.0;
                    break;
                }
            }
            isEdited = !isEdited;
        }
        else if (key == 27) { // Escape key
            break; // Exit the loop
        }
        else if (key == '5') {
            savePath = saveEditedImage(image);
            if (savePath.empty()) {
                cout << "Error: Failed to save edited image." << endl;
            }
        }

        imshow("Edited Image", image);
        lastKey = key;
    }

    destroyAllWindows(); // Close all windows before exiting

    return 0;
}
