# C++ Implementation of Sobel Edge Detection

### This program implements edge detection using the sobel operator  

#### If your image has 3 channels(r,b,g), the program will convert it to grayscale first. It will also give a ```grayscale.jpg``` file.
#### If you would like to visualize different parts of this algorithm, then you can use sobelEdgeDetectionExtra.cpp, which will create ```ver.jpg``` and ```hor.jpg``` which show the vertical and horizontal changes respectively. A ```sobel-orientation.jpg``` is also made which shows the orientations of the edges.

#### You might want to increase the memory a program can use using your bash since the programs are not too memory efficient, therefore high res images might not work. The extra program is even more so.. To increase stack size to 32MB (which is 8MB by default):

```bash
ulimit -s 32767
```

### To compile and then run:

```bash
g++ -O3 sobelEdgeDetection.cpp -o sobel -ljpeg -fpermissive
./sobel <input image file name> <output image file name>
```
##### My Example:

```bash
./sobel HK2.jpg hkborder.jpg
```

#### Requires jpeg library:
```bash
sudo yum install openjpeg-devel
```
#### or
```bash
sudo apt-get install openjpeg-devel
```
