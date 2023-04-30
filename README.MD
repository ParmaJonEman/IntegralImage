# II
### Setup

- [Install OpenCV](https://opencv.org/releases/), and replace the path in the CMAKE file on the find_package line with your new path
- Build the project with the standard CMAKE build commands

### Run

- Run the application via the command line with the following usage patten:
    - ii.exe \<image\>
- Example:
    - ii.exe cat.jpg
- Key commands:
    - Left click and release to select a region of interest.
    - Marvel as the pixel values in the ROI all change to the average value of the pixels in the ROI
    - Press any key to exit
