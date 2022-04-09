# OpenGL-3D_Bicycle-Crank_Slider

2022年OPENGL计算机图形学大作业

OPENGL computer graphics project in 2022

## Requirement

|     | Version |
| --- | --- |
| Ubuntu | 20.04 LTS |
| OpenGL | 1.4 (2.1 Mesa 10.5.4) |
| g++    | 9.4.1 |

## How to use

Execute the following code in the terminal to install the necessary libraries:

```bash
sudo apt-get install build-essential
sudo apt-get install libgl1-mesa-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install freeglut3-dev
```

Compile and run:

```bash
cd "<path>" && g++ "<name>.cpp" -lGL -lGLU -lglut -o "<name>" && "./<name>"
```

For Windows users, you can directly double-click the `<name>.exe` file to run.

## myBicycle

| Operate | Effect |
| --- | --- |
| Mouse Left Botton | Shift perspective |
| Mouse Right Botton | Toggle the filling mode and speed gear |
| w | Go ahead |
| a | Turn left |
| d | Turn right |
| s | Stop |
| r | Reset |
| q | Exit |

## myCrankSlider

| Operate | Effect |
| --- | --- |
| Mouse Left Botton | Rotate it clockwise at an angle |
| Mouse Right Botton | Shift perspective |
| Mouse Wheel | Zoom in or out |
| f | Zoom in |
| g | Zoom out |
| w | Toggle the filling mode |
| Key Up | Rotate it clockwise at an angle |
| Key Down | Rotate it counterclockwise at an angle |
| Key Space | Rotate continuously clockwise |
| Key Enter | Rotate continuously counterclockwise |
| s | Stop |
| r | Reset |
| q | Exit |

## References

[1] <http://www.ccbike.cc/bbs/?m=art&aid=2909>

[2] [KeerthanS1 / 3d-Bicycle-OpenGL](https://github.com/KeerthanS1/3d-Bicycle-OpenGL)

[3] [jpgallegos3 / Hierarchical-Object](https://github.com/jpgallegos3/Hierarchical-Object)

[4] [victordefoe / OpenGL_bicycle](https://github.com/victordefoe/OpenGL_bicycle)
