# ARDrone and Optical Character Recognition

I had some fun playing with a ARDrone 2.0 for my Robotic (Computer) Vision class.

My final project consisted of using the drone's built-in camera to get live feed and do OCR on text it was put in front of it. Such text consisted of words instructing the drone how to move. The next table shows the list of words and the respective action to be performed:

| Word detected    | Drone action                             |
| ---------------- | ---------------------------------------- |
| Ascend, Rise     | The drone moves up for 0.5 seconds       |
| Descend, Fall    | The drone moves down for 0.5 seconds     |
| Advance, Forward | The drone moves straight for 0.5 seconds |
| Backward         | The drone moves backward for 0.5 seconds |
| Left             | The drone moves left for 0.5 seconds     |
| Right            | The drone moves right for 0.5 seconds    |
| Die              | The drone powers off                     |

To do OCR, the OpenCV extensions were built. The provided samples in the library were used for most of the critical functions involved. 

The repository contains a project built in Visual Studio 2013 Community Version, OpenCV 3.0.0 and OpenCV 3.0.0 extra modules. In case anyone is eager to try out our project, I shall warn it’s going to be a tad frustrating getting the thing to run. If this warning is not enough, then go ahead and follow these steps. For more guidance, you can always use your friend Google to find out more about a specific problem CMake, OpenCV or MSVC++ might throw at you.

1. Download the OpenCV 3.0.0 release from https://github.com/opencv/opencv and extract it in any folder F
2. Download the OpenCv 3.0.0 extra modules’ release from https://github.com/opencv/opencv_contrib and extract it in previously mentioned folder F as well
3. If you are using Visual Studio 2013, download a build of Tesseract + Leptonica for OpenCV 3.0.0 from https://github.com/cpierceworld/tesseract-3.02.02-vc2013. Follow the instructions in the repository’s README. Otherwise, download Tesseract from Github and good luck building it.
4. Download CMake and install it
5. Open CMake
6. For the first “source code folder” option, find the folder OpenCV 3.0.0 was extracted to (F/opencv)
7. For the second “build folder” option, indicate any folder you would like the build to be poured into (e.g. F/build)
8. Click Configure, select native compilers next, and pick Visual Studio 2013 (make sure it’s the x86 version)
9. Find the VS 2013 .exe, called cl.exe inside its /bin folder
10. Find OPENCV_EXTRA_MODULES in the list of options that is now shown. Add to its field the folder where the extensions’ modules lie (e.g. F/opencvcontrib/modules)
11. Disable anything related to Python, tests, or perf_tests in the list of options
12. Click Configure
13. Click Generate, and if no errors were displayed, that means a Visual Studio solution for OpenCV 3.0.0 is now available to compile it.
14. Go to F/build, and open OpenCV.sln
15. Select Release, if Debug is active, in the upper panel of the solution. 
16. Unload projects that contain the word “bioinspired” anywhere in the solution (these are unstable and cause compilation errors).
17. Right click on INSTALL, then left click on build. This will take between 30 to 60 minutes.
18. Add F/build/install/x86/vc12/bin to the Windows system PATH
19. Open the .sln file provided in the project archive AS ADMINISTRATOR.
20. You must update the “additional libraries” under x86 Release to point to their current location. (Relative paths do not seem to work in VS 2013, so use absolute paths)
21. Make sure Release and x86 are selected in the upper panel (they should be)
22. Make sure the drone is on, and your computer is connected to it through WiFi
23. Run the program and have fun

