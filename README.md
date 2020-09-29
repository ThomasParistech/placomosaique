# Placomosaic
The act of collecting the little metal plaques that sit on top of Champagne corks is called placomusophilie. Once the collection is large enough it's tempting to use it to cover a surface to form a mosaic. 
This application allows the user to:
- load capsules from pictures
- find the optimal arrangement of loaded capsules to represent a given photograph

## 1 - Installation

First, clone the repository.
```
git clone https://github.com/ThomasParistech/placomosaique.git
```
Then, go to the directory and compile it.
```
cd placomosaique
mkdir build
cd build
cmake ..
make -j6
```
Please note that CMakeLists.txt is configured in a such way that the executable is generated in the "bin" directory.

## 2 - Running

Go to the build directory and load capsules
```
bin/loading_capsules
```
![](./images/load_capsules.gif =250x)

Once the capsules have been loaded, run the solver
```
bin/capsules_solver
```

## 3 - Capsules Loading
First of all, we need to build a dataset of images of champagne capsules. Since each capsule will be used as "superpixels" to form an image, there will be a very large number of images. As an example, a 40x20 mosaic requires 3200 capsules, but if we want the colors to match correctly the input image we'll need an even bigger dataset!

I could have ask my aunt to take a picture for every single champagne capsule she had, but I'm kind of nice and decided to let her load her capsules by batches of size 30. By the way since capsules might cover a large spectrum of colors, I prefer to rely on an extraction based on the geometry of the whole batch, which is way more robust. The setup is really simple and allows me to automatically extract capsules with great robustness, without too much effort on her part.

#### Setup
- Put capsules on a dark capsule case, which has the shape of a grid
- Set it on top of a white sheet of paper (to maximize constrat)

#### Extraction
- Threshold the image based on grayscale intensity
- Find the largest contour
- Fit a quadrilateral to this contour
- Compute the homography with respect to the reference capsule case
- Map the case back to the perfectly aligned front view
- Extract cutouts using the grid geometry

![](./images/ths_board.png =300x)
![](./images/contour_board.png =300x)
![](./images/rectified_board.png =300x)

## 4 - Optimization
There are already a lot of photo-mosaics algorithms online, but their "cheating techniques" can't be applied here because we're dealing with real material ressources.
- We can't change the color of a capsule to make it look darker, brighter, more red, more blue...
- We can't change the size of capsule to have superpixels with adaptative size according to the local contrast of the input image
- We can't reuse twice the same capsule, because we can't put a material object at several places at the same time

#### Solution
- Compute coordinates of capsules centers in the grid using the input image
- Extract input cutouts
- Compute the similarity metric between the input cutouts and each element of the capsules dataset
- Find the optimal combination using the Gale Shapley Algorithm

![](./images/agathe.png =400x)