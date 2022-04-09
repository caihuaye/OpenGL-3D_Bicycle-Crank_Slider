# OpenGL-3D_Bicycle-Crank_Slider

2022年OPENGL计算机图形学大作业

## Requirement

|     | Version |
| --- | --- |
| Ubuntu | 20.04 LTS |
| OpenGL | 1.4 (2.1 Mesa 10.5.4) |
| g++    | 9.4.1 |

### How to use

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
