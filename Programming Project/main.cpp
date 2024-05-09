#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <commdlg.h>

using namespace std;
using namespace cv;

// Function to open file dialog for selecting an image
string openFileDialog() {
    OPENFILENAMEA ofn;       // common dialog box structure for ANSI version
    char szFile[260];       // buffer for file name
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

// Function to save edited image using a file dialog
string saveFileDialog() {
    OPENFILENAMEA ofn;       // common dialog box structure for ANSI version
    char szFile[260];       // buffer for file name
    string fileName;

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "JPEG\0*.jpg\0PNG\0*.png\0BMP\0*.bmp\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    // Display the Save dialog box.
    if (GetSaveFileNameA(&ofn) == TRUE) {
        fileName = ofn.lpstrFile;
    }

    return fileName;
}

// Function to display a list of available editing tools beside the image
void displayToolsList(Mat& image) {
    string toolsList = "Available Tools:\n";
    toolsList += "   1. Convert to Grayscale\n";
    toolsList += "   2. Apply Gaussian Blur\n";
    toolsList += "   3. Perform Edge Detection\n";
    toolsList += "   4. Reset to Original Image\n";
    toolsList += "   5. Save Image\n";
    putText(image, toolsList, Point(image.cols + 20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
}

// Function to handle button clicks and perform corresponding actions
void onToolButtonClicked(int tool, Mat& image, const string& imagePath, Mat& edges, string& savePath) {
    switch (tool) {
    case 1: // Convert to grayscale
        cvtColor(image, image, COLOR_BGR2GRAY);
        imshow("Edited Image", image);
        break;
    case 2: // Apply Gaussian blur
        GaussianBlur(image, image, Size(5, 5), 0);
        imshow("Edited Image", image);
        break;
    case 3: // Perform edge detection using Canny
        Canny(image, edges, 50, 150);
        imshow("Edited Image", edges);
        break;
    case 4: // Reset to original image
        image = imread(imagePath); // Reload original image
        imshow("Edited Image", image);
        break;
    case 5: // Save the edited image
        savePath = saveFileDialog();
        if (!savePath.empty()) {
            imwrite(savePath, image);
            cout << "Edited image saved as '" << savePath << "'." << endl;
        }
        break;
    default:
        cout << "Invalid tool." << endl;
    }
}

int main() {
    string imagePath = openFileDialog();
    if (imagePath.empty()) {
        cout << "No image selected. Exiting program." << endl;
        return 0;
    }

    Mat image = imread(imagePath);
    if (image.empty()) {
        cout << "Error: Unable to load image from path: " << imagePath << endl;
        return -1;
    }

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
    displayToolsList(image);
    imshow("Edited Image", image);

    Mat edges;
    string savePath;

    namedWindow("Tools", WINDOW_NORMAL);

    createButton("Convert to Grayscale", [](int, void* userdata) {
        Mat* data = (Mat*)userdata;
        onToolButtonClicked(1, *data, *(string*)(data + 1), *(Mat*)(data + 2), *(string*)(data + 3));
        }, &image, QT_PUSH_BUTTON);

    createButton("Apply Gaussian Blur", [](int, void* userdata) {
        Mat* data = (Mat*)userdata;
        onToolButtonClicked(2, *data, *(string*)(data + 1), *(Mat*)(data + 2), *(string*)(data + 3));
        }, &image, QT_PUSH_BUTTON);

    createButton("Perform Edge Detection", [](int, void* userdata) {
        Mat* data = (Mat*)userdata;
        onToolButtonClicked(3, *data, *(string*)(data + 1), *(Mat*)(data + 2), *(string*)(data + 3));
        }, &image, QT_PUSH_BUTTON);

    createButton("Reset to Original Image", [](int, void* userdata) {
        Mat* data = (Mat*)userdata;
        onToolButtonClicked(4, *data, *(string*)(data + 1), *(Mat*)(data + 2), *(string*)(data + 3));
        }, &image, QT_PUSH_BUTTON);

    createButton("Save Image", [](int, void* userdata) {
        Mat* data = (Mat*)userdata;
        onToolButtonClicked(5, *data, *(string*)(data + 1), *(Mat*)(data + 2), *(string*)(data + 3));
        }, &image, QT_PUSH_BUTTON);


    waitKey(0);

    return 0;
}
