# Seam Carving Image Resizer

## Results
<iframe width="560" height="315" src="https://www.youtube.com/embed/A9uiIOozk2M" frameborder="0" allowfullscreen></iframe>


## Description
This project implements seam carving, a content-aware image resizing technique. It adjusts the dimensions of an image by intelligently removing seams (paths of least importance) either vertically or horizontally. The goal is to resize the image without distorting or losing important visual content.

## Key Features
- **Seam Detection**: Calculates energy at each pixel to identify the least important seams.
- **Vertical and Horizontal Resizing**: Supports resizing images by removing vertical and horizontal seams.
- **Animation of Seam Removal**: Displays an animation of the seam being removed in real-time.
- **User-Defined Target Size**: Allows the user to specify the desired width and height for the output image.
- **Image Saving**: Saves the resized image to a file.

## Compilation and Execution
### Dependencies
- **OpenCV**: Ensure OpenCV is installed on your system. You can install it using `sudo apt-get install libopencv-dev` on Ubuntu or by following the [OpenCV installation guide](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html) for other systems.

### Compilation
To compile the code, use the following command:
```sh
g++ -o seam_carving main.cpp `pkg-config --cflags --libs opencv4`
```

### Execution
To run the program, execute the following command:
```sh
./seam_carving
```

The program will prompt you to enter the file path of the image and the target width and height for resizing.

## Input Format
- **Image Path**: Provide the path to the image file you want to resize.
- **Target Dimensions**: Enter the desired width and height for the output image.

### Example Usage
```sh
Enter the image file path: example.jpg
Enter the target width: 300
Enter the target height: 200
```

## Project Structure
- **seam_carving.cpp**: Contains the main implementation of the seam carving algorithm.
- **output_resized_image.jpg**: The resized image file saved after processing.

## How It Works
1. **Image Loading**: The program loads the specified image using OpenCV.
2. **Energy Calculation**: Computes the energy at each pixel, which is a measure of the pixel's importance.
3. **Seam Calculation**: Identifies and removes the vertical and horizontal seams with the least energy.
4. **Seam Removal Animation**: Animates the seam removal process, showing which seams are removed.
5. **Image Resizing**: Continues seam removal until the image reaches the user-defined dimensions.
6. **Image Saving**: The final resized image is saved as `output_resized_image.jpg`.

