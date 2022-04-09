#include <GL/glut.h>
#include <math.h>

#include <iostream>
using namespace std;

/* 无参宏定义 */
#define WIN_WIDTH 800  // 窗口大小
#define WIN_HEIGHT 800
#define PI 3.1415926
GLfloat scale;  // 鼠标滚轮控制窗口缩放

/* 全局变量 */
// 相机视角
GLfloat camx, camy, camz;
GLfloat anglex, angley, anglez;
// 鼠标
GLenum Mouse;
int prevx, prevy;
bool is_outline_back;  // 填充模式切换
bool is_move;          // 是否运动
bool is_clockwise;     // 转动方向
// 运动参数
GLfloat crank = 20.0f;                  // 曲柄长度
GLfloat radius = 5.0f;                  // 圆盘半径
GLfloat leftx, lefty, rightx;           // 曲柄左右两端的位置
GLfloat theta;                          // 圆盘旋转角度
GLfloat beta;                           // 曲柄旋转对应值
GLfloat rad_to_deg = 45.0 / atan(1.0);  // 弧度到角度的比率
GLfloat step = 0.1f;                    // 旋转步进值

// 重置窗口中的场景
void reset() {
    camx = 40.0f;
    camy = 0.0f;
    camz = 15.0f;

    anglex = 0.0f;
    angley = 0.0f;
    anglez = 0.0f;

    Mouse = GLUT_UP;
    is_move = false;
    is_clockwise = true;

    scale = 1.0f;
    theta = 0.0f;
    beta = 0.0f;

    leftx = radius;
    lefty = 0.0f;
    rightx = radius + crank;

    glLoadIdentity();
    gluLookAt(camx, camy, camz,
              15.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
    // cout << "Reset the windows" << endl;
}

// 初始化窗口内的场景
void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 设置清屏颜色，白色背景

    GLfloat light_position[] = {-1.0, 1.0, 1.0, 0.0};
    GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0};

    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {100.0};

    reset();                  // 场景重置
    glShadeModel(GL_SMOOTH);  // 图元颜色平滑过渡

    // 设置 0 号光源
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 位置属性
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);    // 环境光属性
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);    // 散射光属性

    // 设置材质的环境颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);    // 镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  // 镜面反射指数
    // 指定哪些材料参数跟踪当前颜色
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glEnable(GL_LIGHTING);        // 启用光源
    glEnable(GL_LIGHT0);          // 打开 0 号光源
    glEnable(GL_COLOR_MATERIAL);  // 启用颜色追踪
    glEnable(GL_DEPTH_TEST);      // 打开深度测试，实现消影
    // glEnable(GL_CULL_FACE);       // 面剔除
    // glFrontFace(GL_CCW);          // 逆时针是正面
}

// 沿 Z 轴方向的圆柱面 (指向屏幕外)，绘制后回到上一个位置
void ZCylinder(GLfloat radius, GLfloat length) {
    // 创建二次方程状态对象，保存绘图模式、法线模式、法线朝向、纹理等信息
    GLUquadricObj* cylinder;
    cylinder = gluNewQuadric();  // 初始化二次方程状态

    glPushMatrix();
    int n = 3600;  // 多边形边数
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        glVertex2f(radius * cos(2 * PI * i / n), radius * sin(2 * PI * i / n));  //定义顶点
    }
    glEnd();

    /**
     * @brief 绘制圆柱体
     * gluCylinder(二次方程对象，底部半径，顶部半径，高度，...)
     * 最后两个参数决定精度（光滑程度）
     * 即对象沿着 y 轴分割 slices 次，围绕 y 轴分割 stacks 次
     */
    gluCylinder(cylinder, radius, radius, length, 32, 1);

    glTranslatef(0.0f, 0.0f, length);
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        glVertex2f(radius * cos(2 * PI * i / n), radius * sin(2 * PI * i / n));  //定义顶点
    }
    glEnd();

    glPopMatrix();
}

// 沿 X 轴方向的圆柱面 (指向屏幕右边)，绘制后回到上一个位置
void XCylinder(GLfloat radius, GLfloat length) {
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);  // 沿 y 轴转 90°
    ZCylinder(radius, length);
    glPopMatrix();
}

// 圆盘
void draw_disk(void) {
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f);
    glRotatef(-theta * rad_to_deg, 0.0f, 0.0f, 1.0f);
    glutSolidTorus(2.0f, radius, 30, 6);
    glPopMatrix();
}

// 基座
void draw_base(void) {
    glColor3f(0.8f, 0.8f, 0.8f);

    glPushMatrix();
    glTranslatef(2.0f, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(25.0f, 0.0f, 0.0f);
    glScalef(2.0f, 6.0f, 2.0f);
    glutSolidCube(1.0f);  // 右端
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17.5f, 2.0f, 0.0f);
    glScalef(15.0f, 2.0f, 2.0f);
    glutSolidCube(1.0f);  // 上部
    glPopMatrix();

    glPushMatrix();
    glTranslatef(17.5f, -2.0f, 0.0f);
    glScalef(15.0f, 2.0f, 2.0f);
    glutSolidCube(1.0f);  // 下部
    glPopMatrix();

    glPopMatrix();
}

// 滑块
void draw_slider(void) {
    glPushMatrix();
    glColor3f(240 / 255.0f, 70 / 255.0f, 70 / 255.0f);
    glTranslatef(rightx - 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// 曲柄
void draw_crank(void) {
    glPushMatrix();
    glColor3f(160 / 255.0f, 216 / 255.0f, 232 / 255.0f);
    glTranslatef(leftx - 1.0f, lefty, 4.0f);
    glRotatef(-beta, 0.0f, 0.0f, 1.0f);
    XCylinder(1.0f, crank + 1.0f);  // 曲柄主体

    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    XCylinder(0.75f, 3.0f);  // 左关节
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightx - 1.0f, 0.0f, 4.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    XCylinder(0.5f, 3.0f);  // 右关节
    glPopMatrix();
}

// 绘制图元
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 用当前值清除缓冲区
    glEnable(GL_NORMALIZE);

    if (is_outline_back)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // 线框图
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 填充图

    // 使用鼠标左键旋转场景，并开始拍摄剩下的场景
    glPushMatrix();
    glScalef(scale, scale, scale);
    glRotatef(angley, 1.0f, 0.0f, 0.0f);
    glRotatef(anglex, 0.0f, 1.0f, 0.0f);
    glRotatef(anglez, 0.0f, 0.0f, 1.0f);

    draw_disk();
    draw_base();
    draw_slider();
    draw_crank();
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);  // 对模型视图矩阵堆栈应用随后的矩阵操作
    glLoadIdentity();            // 重置当前指定的矩阵为单位矩阵

    /**
     * @brief 定义相机状态
     * void gluLookAt (
            GLdouble eyex,    GLdouble eyey,     GLdouble eyez,
            GLdouble centerx, GLdouble centery,  GLdouble centerz,
            GLdouble upx,     GLdouble upy,      GLdouble upz);
     * 第一组 eyex, eyey, eyez: 相机在世界坐标的位置
     * 第二组 centerx, centery, centerz: 相机镜头对准的物体在世界坐标的位置，成像后这一点会位于画板的中心位置
     * 第三组 upx, upy, upz: 相机向上的方向在世界坐标中的方向
     */
    gluLookAt(camx, camy, camz,
              15.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
    glutSwapBuffers();  // 交换前后两个缓冲区的指针，通过双缓冲技术来避免画面出现闪烁
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':  // 重置
            reset();
            break;
        case 'w':  // 切换填充模式
            is_outline_back = !is_outline_back;
            break;
        case 'f':  // 滚轮向前滚动 scroll forward
            scale += 0.05f;
            break;
        case 'g':  // 滚轮向后滚动 scroll backward
            scale -= 0.05f;
            break;
        case 32:  // 空格，顺时针连续运动
            is_move = true;
            is_clockwise = true;
            break;
        case 13:  // 回车，逆时针连续运动
            is_move = true;
            is_clockwise = false;
            break;
        case 's':  // 停止运动
            is_move = false;
            break;
        case 'q':  // 退出程序
            exit(1);
    }

    glutPostRedisplay();  // 刷新窗口
}

void special_key(GLint key, GLint x, GLint y) {
    if (key == GLUT_KEY_UP) {  // 上键，顺时针转一个位置
        theta += step;
    }
    if (key == GLUT_KEY_LEFT) {
        // nothing
    }
    if (key == GLUT_KEY_DOWN) {  // 下键，逆时针转一个位置
        theta -= step;
    }
    if (key == GLUT_KEY_RIGHT) {
        // nothing
    }

    glutPostRedisplay();  // 刷新窗口
}

void on_mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_RIGHT_BUTTON:        // 鼠标右键移动画面
            if (state == GLUT_DOWN) {  // 按下
                Mouse = GLUT_DOWN;
                prevx = x;
                prevy = y;
            }
            if (state == GLUT_UP) {  // 放开
                Mouse = GLUT_UP;
            }
            break;
        case GLUT_LEFT_BUTTON:  // 鼠标左键控制运动
            if (state == GLUT_DOWN) {
                theta += step;
            }
            break;
        case 3:  // 滚轮向前滚动 scroll forward
            if (state == GLUT_UP) return;
            scale += 0.05f;
            break;
        case 4:  // 滚轮向后滚动 scroll backward
            scale -= 0.05f;
            break;
    }

    if (scale < 0.5f)
        scale = 0.5f;
    if (scale > 3.0f)
        scale = 3.0f;

    glutPostRedisplay();  // 刷新窗口
}

void motion(int x, int y) {
    if (Mouse == GLUT_DOWN) {
        int deltax, deltay;

        deltax = prevx - x;      // 获取移动距离
        anglex -= 0.1 * deltax;  // 系数控制移动速度，正负号切换移动方向
        // 控制转动视角，以防眩晕
        if (anglex < 0.0)  // 水平方向无限制
            anglex += 360.0;
        if (anglex > 360.0)
            anglex -= 360.0;

        deltay = prevy - y;
        anglez += 0.1 * deltay;
        if (anglez < 0.0)  // 上下方向无限制
            anglez += 360.0;
        if (anglez > 360.0)
            anglez -= 360.0;
    }
    prevx = x;
    prevy = y;

    glutPostRedisplay();  // 刷新窗口
}

// 窗口变化时更新窗口
void change_size(GLint w, GLint h) {
    if ((w == 0) || (h == 0))
        return;  // 窗口宽高为零直接返回

    // 设置视口和窗口大小一致
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);  // 对投影矩阵应用随后的矩阵操作
    glLoadIdentity();             // 重置当前指定的矩阵为单位矩阵

    /**
     * @brief 定义相机的内在镜头参数
     * gluPerspective (
            GLdouble fovy,      // 定义可视角的大小
            GLdouble aspect,    // 定义物体显示在画板上的 x 和 y 方向上的比例
            GLdouble zNear,     // 定义距离相机最近处物体截面相距的距离
            GLdouble zFar);     // 定义可观测物体的最远处截面相距相机的距离
        */
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.001, 100.0);
    glMatrixMode(GL_MODELVIEW);  // 对模型视图矩阵堆栈应用随后的矩阵操作
    glLoadIdentity();            // 重置当前指定的矩阵为单位矩阵

    gluLookAt(camx, camy, camz,
              15.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
}

void update_scene(void) {
    if (is_move) {         // 是否连续运动
        if (is_clockwise)  // 是否顺时针旋转
            theta += step;
        else
            theta -= step;
    }

    leftx = sin(theta) * radius;  // 曲柄左端点位置更新
    lefty = cos(theta) * radius;

    beta = asin(lefty / crank);
    beta = beta * rad_to_deg;  // 曲柄相对于左端点的转动角度更新

    // 曲柄右端点，同时也是滑块的位置更新
    rightx = sqrt(pow(crank, 2) - pow(lefty, 2)) + leftx;

    glutPostRedisplay();  // 刷新窗口
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // 设置显示模式（双缓冲、彩色、消影）
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);           // 设置窗口在屏幕中的位置
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);  // 设置窗口的尺寸

    glutCreateWindow("my Crank Slider");  // 设置窗口标题
    init();                               // 初始化场景

    // 设置渲染状态
    glutDisplayFunc(display);      // 显示
    glutReshapeFunc(change_size);  // 窗口大小变化时的回调函数
    glutKeyboardFunc(keyboard);    // 键盘回调函数
    glutSpecialFunc(special_key);  // 方向键回调函数
    glutMouseFunc(on_mouse);       // 鼠标回调函数
    glutMotionFunc(motion);        // 处理鼠标移动（当鼠标被点击且移动时）的回调函数

    glutIdleFunc(update_scene);  // 更新场景
    glutMainLoop();              // 事件主循环
    return 0;
}