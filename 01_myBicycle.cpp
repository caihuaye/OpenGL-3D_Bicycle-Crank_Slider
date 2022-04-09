#include <GL/glut.h>
#include <math.h>

#include <iostream>
using namespace std;

/* 无参宏定义 */
#define WIN_WIDTH 800  // 窗口大小
#define WIN_HEIGHT 800
#define PI 3.1415926
// 步进值
#define INC_STEERING 2.0f  // 旋转把手的递进角度大小
// 旋转角度
#define ANGLE_TOP 23.0f
#define ANGLE_LEFT 38.0f
#define ANGLE_RIGHT 47.0f
#define ANGLE_MIDDLE 110.0f
#define ANGLE_FRONT 83.0f
#define ANGLE_LIMIT 70.0f  // 车把手旋转角度限制
#define RADIUS_VIEW 5.0f   // 画面旋转半径
// 框架各部件尺寸
#define GEAR_WIDTH 0.03f     // 齿轮宽度
#define GEAR_OFFSET 0.06f    // 齿轮的偏移距离
#define WHEEL_OFFSET 0.13f   // 自行车框架与轮胎之间的偏移距离
#define RADIUS_WHEEL 1.025f  // 车轮半径
#define RADIUS_TUBE 0.05f    // 管半径
#define HEAD_TUBE 0.7f       // 头管
#define HEAD_TOP 0.4f        // 头管上端与上管之间的距离
#define STEM 0.2f            // 竖管
#define HANDLE_BAR 1.2f      // 车把手
#define FRONT_FORK 1.15f     // 前叉
#define TOP_TUBE 1.45f       // 上管长度
#define DOWN_TUBE 1.6f       // 下管长度
#define SEAT_TUBE 1.0f       // 立管长度
#define SEAT_POST 0.7f       // 鞍管长度
#define SEAT_STAY 1.2f       // 后上叉长度
#define CHAIN_STAY 1.2f      // 后下叉长度
#define BACK_CONNECTOR 0.35f
#define NUM_SPOKES 30  // 辐条个数
// 颜色
#define color_frame (255.0f / 255), (210.0f / 255), (63.0f / 255)  // 自行车整体框架 - 橘黄色
#define color_seat (66.0f / 255), (60.0f / 255), (64.0f / 255)     // 车座 - 深灰色
#define color_gear 0.0f, 0.0f, 0.0f                                // 链轮 - 黑色

/* 全局变量 */
// 相机视角
GLfloat camx, camy, camz;
GLfloat anglex, angley, anglez;
// 自行车
GLfloat pedal_angle;  // 脚踏板旋转角度
GLfloat speed;        // 自行车速度
GLfloat steering;     // 车把手旋转角度
GLfloat xpos, zpos, direction;
bool is_move;  // 自行车是否前进的标志位
// 鼠标
GLenum Mouse;
int prevx, prevy;
bool is_outline_back = false;

// 重置窗口中的场景
void reset() {
    anglex = -0.815f;
    angley = 0.6f;
    anglez = 0.0f;

    camx = RADIUS_VIEW * sin(anglex);
    camy = RADIUS_VIEW * sin(angley);
    camz = RADIUS_VIEW * cos(anglex);

    pedal_angle = steering = 0.0f;
    speed = 0.10f;
    xpos = zpos = 0.0f;
    direction = 0.0f;

    is_move = false;
    Mouse = GLUT_UP;

    glLoadIdentity();  // 重置当前指定的矩阵为单位矩阵
    gluLookAt(camx + xpos, camy, camz + zpos,
              xpos, 0.0, zpos,
              0.0, 1.0, 0.0);
    // cout << "Reset the windows" << endl;
}

// 初始化窗口内的场景
void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 设置清屏颜色，白色背景

    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
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

// 渲染背景
void landmarks(void) {
    glColor3f(0.5f, 0.5f, 0.5f);  // 浅灰色地板
    GLfloat bias = 0.005f;

    // 为自行车绘制地板
    glBegin(GL_LINES);
    for (GLfloat i = -1000.0f; i < 1000.0f; i += 1.0f) {
        glVertex3f(-1000.0f, -(RADIUS_WHEEL + bias), i);
        glVertex3f(1000.0f, -(RADIUS_WHEEL + bias), i);
        glVertex3f(i, -(RADIUS_WHEEL + bias), -1000.0f);
        glVertex3f(i, -(RADIUS_WHEEL + bias), 1000.0f);
    }
    glEnd();
}

// 绘制链轮 (Portions of this code have been borrowed from Brian Paul's Mesa distribution.)
void gear(GLfloat inner_radius,   // 中心孔的半径
          GLfloat outer_radius,   // 齿心半径
          GLfloat width,          // 齿宽
          GLint teeth,            // 齿数
          GLfloat tooth_depth) {  // depth of tooth

    GLint i;
    GLfloat r0, r1, r2;
    GLfloat angle, da;
    GLfloat u, v, len;
    const double pi = 3.14159264;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0;
    r2 = outer_radius + tooth_depth / 2.0;

    da = 2.0 * pi / teeth / 4.0;

    glShadeModel(GL_FLAT);

    glNormal3f(0.0, 0.0, 1.0);

    /* draw front face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * pi / teeth;
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
    glEnd();

    /* draw front sides of teeth */
    glBegin(GL_QUADS);
    da = 2.0 * pi / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * pi / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    /* draw back face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * pi / teeth;
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
    glEnd();

    /* draw back sides of teeth */
    glBegin(GL_QUADS);
    da = 2.0 * pi / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * pi / teeth;

        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    }
    glEnd();

    /* draw outward faces of teeth */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * pi / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        u = r2 * cos(angle + da) - r1 * cos(angle);
        v = r2 * sin(angle + da) - r1 * sin(angle);
        len = sqrt(u * u + v * v);
        u /= len;
        v /= len;
        glNormal3f(v, -u, 0.0);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
        v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
        glNormal3f(v, -u, 0.0);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
    }

    glVertex3f(r1 * cos(0.0), r1 * sin(0.0), width * 0.5);
    glVertex3f(r1 * cos(0.0), r1 * sin(0.0), -width * 0.5);

    glEnd();

    glShadeModel(GL_SMOOTH);

    /* draw inside radius cylinder */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * pi / teeth;
        glNormal3f(-cos(angle), -sin(angle), 0.0);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    }
    glEnd();
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

// 绘制车座
void drawSeat() {
    // 绘制顶部
    glBegin(GL_POLYGON);
    glColor3f(color_frame);
    glVertex3f(-0.1f, 1.0f, -0.5f);
    glVertex3f(1.0f, 1.0f, -0.3f);
    glVertex3f(1.0f, 1.0f, 0.3f);
    glVertex3f(-0.1f, 1.0f, 0.5f);
    glVertex3f(-0.5f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-0.5f, 1.0f, -1.0f);
    glEnd();

    // 绘制底部
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.1f, -1.0f, -0.5f);
    glVertex3f(1.0f, -1.0f, -0.3f);
    glVertex3f(1.0f, -1.0f, 0.3f);
    glVertex3f(-0.1f, -1.0f, 0.5f);
    glVertex3f(-0.5f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-0.5f, -1.0f, -1.0f);
    glEnd();

    // 绘制侧边缘
    glBegin(GL_QUADS);
    glColor3f(color_seat);
    glVertex3f(1.0f, 1.0f, -0.3f);
    glVertex3f(1.0f, 1.0f, 0.3f);
    glVertex3f(1.0f, -1.0f, 0.3f);
    glVertex3f(1.0f, -1.0f, -0.3f);

    glVertex3f(1.0f, 1.0f, 0.3f);
    glVertex3f(-0.1f, 1.0f, 0.5f);
    glVertex3f(-0.1f, -1.0f, 0.5f);
    glVertex3f(1.0f, -1.0f, 0.3f);

    glVertex3f(1.0f, 1.0f, -0.3f);
    glVertex3f(-0.1f, 1.0f, -0.5f);
    glVertex3f(-0.1f, -1.0f, -0.5f);
    glVertex3f(1.0f, -1.0f, -0.3f);

    glVertex3f(-0.1f, 1.0f, 0.5f);
    glVertex3f(-0.5f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -1.0f, 1.0f);
    glVertex3f(-0.1f, -1.0f, 0.5f);

    glVertex3f(-0.1f, 1.0f, -0.5f);
    glVertex3f(-0.5f, 1.0f, -1.0f);
    glVertex3f(-0.5f, -1.0f, -1.0f);
    glVertex3f(-0.1f, -1.0f, -0.5f);

    glVertex3f(-0.5f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-0.5f, -1.0f, 1.0f);

    glVertex3f(-0.5f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-0.5f, -1.0f, -1.0f);

    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}

// 绘制轮胎
void drawTyre(void) {
    // 绘制轮廓 (轮圈)
    glColor3f(1.0f, 1.0f, 1.0f);  // 白色
    /**
     * @brief 绘制环面（甜甜圈）
     * void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings)
     * innerRadius: 圆管的半径
     * outerRadius: 环面中心到圆管中心的距离
     */
    glutSolidTorus(0.03f, RADIUS_WHEEL - 0.05f, 10, 100);

    // 绘制中心连杆
    glColor3f(1.0f, 1.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.08f);
    ZCylinder(0.025f, 0.16f);
    glPopMatrix();

    // 绘制辐条
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < NUM_SPOKES; ++i) {
        glPushMatrix();
        glRotatef(i * (360 / NUM_SPOKES), 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.02f, 0.0f);
        glVertex3f(0.0f, RADIUS_WHEEL - 0.05f, 0.0f);
        glEnd();
        glPopMatrix();
    }

    // 绘制轮胎
    glColor3f(0.0f, 0.0f, 0.0f);  // 黑色
    glutSolidTorus(0.05f, RADIUS_WHEEL, 10, 100);
    glColor3f(1.0f, 0.0f, 0.0f);
}

// 绘制自行车整体框架
void draw_frame(void) {
    glPushMatrix();  // 起始位置 1
    glPushMatrix();  // 起始位置 2
    glPushMatrix();  // 起始位置 3
    glColor3f(color_gear);
    glTranslatef(0.0f, 0.0f, 0.06f);
    glRotatef(-2 * pedal_angle, 0.0f, 0.0f, 1.0f);
    gear(0.08f, 0.25f, GEAR_WIDTH, 30, 0.03f);  // 绘制链轮
    glPopMatrix();                              // 回到起始位置 3

    // 还原自行车框架颜色，绘制连接脚蹬曲柄的中间连杆
    glColor3f(color_frame);
    glTranslatef(0.0f, 0.0f, -0.2f);
    ZCylinder(0.08f, 0.4f);

    glPopMatrix();  // 回到起始位置 2

    // 绘制下管
    glRotatef(ANGLE_RIGHT, 0.0f, 0.0f, 1.0f);  // 沿 z 轴旋转
    XCylinder(RADIUS_TUBE, DOWN_TUBE);

    // 绘制立管
    glRotatef(ANGLE_MIDDLE - ANGLE_RIGHT, 0.0f, 0.0f, 1.0f);  // 再沿 z 轴旋转
    XCylinder(RADIUS_TUBE, SEAT_TUBE);

    // 绘制车座支柱
    glColor3f(0.0f, 0.0f, 0.0f);  // 黑色
    glTranslatef(SEAT_TUBE, 0.0f, 0.0f);
    XCylinder(RADIUS_TUBE, SEAT_POST);

    // 绘制车座
    glTranslatef(SEAT_POST, 0.0f, 0.0f);
    glRotatef(-ANGLE_MIDDLE, 0.0f, 0.0f, 1.0f);
    glScalef(0.3f, RADIUS_TUBE, 0.25f);
    drawSeat();

    glPopMatrix();  // 回到起始位置 1

    // 绘制后下叉
    glPushMatrix();                          // 记录起始位置 1
    glColor3f(color_frame);                  // 还原自行车框架颜色
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);    // 顺时针
    XCylinder(RADIUS_TUBE, BACK_CONNECTOR);  // 中后下叉

    glPushMatrix();
    glTranslatef(BACK_CONNECTOR, 0.0f, WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, CHAIN_STAY);  // 右后下叉
    glPopMatrix();

    glPushMatrix();
    glTranslatef(BACK_CONNECTOR, 0.0f, -WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, CHAIN_STAY);  // 左后下叉
    glPopMatrix();

    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-(WHEEL_OFFSET + RADIUS_TUBE), 0.0f, BACK_CONNECTOR);
    XCylinder(RADIUS_TUBE, 2 * (WHEEL_OFFSET + RADIUS_TUBE));  // 中间连杆
    glPopMatrix();                                             // 回到起始位置 1

    glPushMatrix();  // 记录起始位置 1
    glTranslatef(-(BACK_CONNECTOR + CHAIN_STAY), 0.0f, 0.0f);

    glPushMatrix();  // 原点在后车轮中心
    glRotatef(-2 * pedal_angle, 0.0f, 0.0f, 1.0f);
    drawTyre();  // 绘制后轮胎
    glColor3f(color_gear);
    glTranslatef(0.0f, 0.0f, 0.06f);
    gear(0.03f, 0.15f, GEAR_WIDTH, 20, 0.03f);  // 绘制飞轮
    glPopMatrix();

    // 绘制后上叉
    glRotatef(ANGLE_LEFT, 0.0f, 0.0f, 1.0f);
    glColor3f(color_frame);

    glPushMatrix();  // 原点在后车轮中心
    glTranslatef(0.0f, 0.0f, -WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, SEAT_STAY);  // 左后上叉
    glPopMatrix();

    glPushMatrix();  // 原点在后车轮中心
    glTranslatef(0.0f, 0.0f, WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, SEAT_STAY);  // 右后上叉
    glPopMatrix();

    glPushMatrix();  // 原点在后车轮中心
    glTranslatef(SEAT_STAY, 0.0f, 0.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-(WHEEL_OFFSET + RADIUS_TUBE), 0.0f, 0.0f);
    XCylinder(RADIUS_TUBE, 2 * (WHEEL_OFFSET + RADIUS_TUBE));  // 中间连杆
    glPopMatrix();

    glTranslatef(SEAT_STAY, 0.0f, 0.0f);
    XCylinder(RADIUS_TUBE, 0.3f);  // 中后上叉

    // 绘制上管
    glTranslatef(0.3f, 0.0f, 0.0f);
    glRotatef(-ANGLE_TOP, 0.0f, 0.0f, 1.0f);
    XCylinder(RADIUS_TUBE, TOP_TUBE);

    // 绘制头管
    glTranslatef(TOP_TUBE, 0.0f, 0.0f);
    glRotatef(-ANGLE_FRONT, 0.0f, 0.0f, 1.0f);  // 此时 z 轴指向屏幕外，x 轴沿着头管指向下方

    glPushMatrix();
    glTranslatef(-HEAD_TOP, 0.0f, 0.0f);  // 原点移动到头管上端
    XCylinder(RADIUS_TUBE, HEAD_TUBE);
    glPopMatrix();

    // 绘制把手
    // 此时 z 轴指向屏幕外，x 轴沿着头管指向下方，原点在上管的右端
    glPushMatrix();
    glRotatef(-steering, 1.0f, 0.0f, 0.0f);  // 记录车头转向

    glTranslatef(-(HEAD_TOP - RADIUS_TUBE), 0.0f, 0.0f);  // 原点移动到头管的上端

    glPushMatrix();
    glRotatef(ANGLE_FRONT, 0.0f, 0.0f, 1.0f);
    XCylinder(RADIUS_TUBE, STEM);    // 竖管
    glTranslatef(STEM, 0.0f, 0.0f);  // 原点移动到竖管的右端

    glPushMatrix();                             // 记录车头转向后的世界坐标
    glTranslatef(0.0f, 0.0f, -HANDLE_BAR / 2);  // 原点移动到车把手左端
    ZCylinder(RADIUS_TUBE, HANDLE_BAR);         // 车把手
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);  // 黑色
    glTranslatef(0.0f, 0.0f, -HANDLE_BAR / 2);
    ZCylinder(0.07f, HANDLE_BAR / 4);  // 左握把
    glTranslatef(0.0f, 0.0f, HANDLE_BAR * 3 / 4);
    ZCylinder(0.07f, HANDLE_BAR / 4);  // 右握把
    glPopMatrix();
    glPopMatrix();

    // 此时 z 轴指向屏幕外，x 轴沿着头管指向下方，原点在上管的右端
    glPushMatrix();
    glColor3f(color_frame);
    glTranslatef(HEAD_TUBE, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -(WHEEL_OFFSET + RADIUS_TUBE));
    ZCylinder(RADIUS_TUBE, 2 * (WHEEL_OFFSET + RADIUS_TUBE));  // 中间连接件
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, FRONT_FORK);  // 右前叉
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -WHEEL_OFFSET);
    XCylinder(RADIUS_TUBE, FRONT_FORK);  // 左前叉
    glPopMatrix();

    glTranslatef(FRONT_FORK, 0.0f, 0.0f);
    glRotatef(-2 * pedal_angle, 0.0f, 0.0f, 1.0f);
    drawTyre();     // 绘制前轮胎
    glPopMatrix();  // 此时 z 轴指向屏幕外，x 轴沿着头管指向下方，原点在上管的右端
    glPopMatrix();  // 回到起始位置 1
    glPopMatrix();  // 旋转前的位置
}

// 绘制图元
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 用当前值清除缓冲区
    glEnable(GL_NORMALIZE);

    if (is_outline_back)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // 线框图
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 填充图

    // 开始拍摄场景
    glPushMatrix();

    // 渲染背景
    landmarks();

    // 移动自行车
    glPushMatrix();
    glTranslatef(xpos, 0.0f, zpos);
    glRotatef(direction, 0.0f, 1.0f, 0.0f);

    // 绘制自行车整体框架
    draw_frame();

    //=== 绘制链条 ===//
    glBegin(GL_QUADS);  // 绘制四边形
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.25f, GEAR_OFFSET - GEAR_WIDTH / 2);
    glVertex3f(0.0f, 0.25f, GEAR_OFFSET + GEAR_WIDTH / 2);
    glVertex3f(-(BACK_CONNECTOR + CHAIN_STAY), 0.15f, GEAR_OFFSET + GEAR_WIDTH / 2);
    glVertex3f(-(BACK_CONNECTOR + CHAIN_STAY), 0.15f, GEAR_OFFSET - GEAR_WIDTH / 2);

    glVertex3f(0.0f, -0.25f, GEAR_OFFSET - GEAR_WIDTH / 2);
    glVertex3f(0.0f, -0.25f, GEAR_OFFSET + GEAR_WIDTH / 2);
    glVertex3f(-(BACK_CONNECTOR + CHAIN_STAY), -0.15f, GEAR_OFFSET + GEAR_WIDTH / 2);
    glVertex3f(-(BACK_CONNECTOR + CHAIN_STAY), -0.15f, GEAR_OFFSET - GEAR_WIDTH / 2);
    glEnd();
    //=== 绘制链条 ===//

    //=== 绘制脚踏板 ===//
    glColor3f(color_frame);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.25f);
    glRotatef(-pedal_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.2f, 0.0f, 0.0f);

    glPushMatrix();
    glScalef(0.5f, 0.1f, 0.1f);
    glutSolidCube(1.0f);  // 右曲柄
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, 0.0f, 0.2f);
    glRotatef(pedal_angle, 0.0f, 0.0f, 1.0f);
    glScalef(0.2f, 0.02f, 0.3f);
    glutSolidCube(1.0f);  // 右踏板
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.25f);
    glRotatef(180.0f - pedal_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.2f, 0.0f, 0.0f);

    glPushMatrix();
    glScalef(0.5f, 0.1f, 0.1f);
    glutSolidCube(1.0f);  // 左曲柄
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, 0.0f, -0.2f);
    glRotatef(pedal_angle - 180.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.2f, 0.02f, 0.3f);
    glutSolidCube(1.0f);  // 左踏板
    glPopMatrix();
    glPopMatrix();
    //=== 绘制脚踏板 ===//

    glPopMatrix();
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
    gluLookAt(camx + xpos, camy, camz + zpos,
              xpos, 0.0, zpos,
              0.0, 1.0, 0.0);
    glutSwapBuffers();  // 交换前后两个缓冲区的指针，通过双缓冲技术来避免画面出现闪烁
}

void main_menu_func(int option) {
    switch (option) {
        case 1:
            is_outline_back = !is_outline_back;
            break;
    }

    glutPostRedisplay();  // 刷新窗口
}

void sub_menu_func(int option) {
    switch (option) {
        case 1:
            speed = 0.15f;
            break;
        case 2:
            speed = 0.10f;
            break;
        case 3:
            speed = 0.05f;
            break;
    }

    pedal_angle = speed;
    if (pedal_angle < 0.0f)
        pedal_angle = 0.0f;
    if (pedal_angle > 360.0f)
        pedal_angle -= 360.0f;

    glutPostRedisplay();  // 刷新窗口
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':  // 重置
            reset();
            break;
        case 'w':  // 前进
            is_move = true;
            break;
        case 's':  // 停车
            is_move = false;
            break;
        case 'a':  // 左转
            if (steering < ANGLE_LIMIT)
                steering += INC_STEERING;
            break;
        case 'd':  // 右转
            if (steering > -ANGLE_LIMIT)
                steering -= INC_STEERING;
            break;
        case 'q':  // 退出程序
            exit(1);
    }

    glutPostRedisplay();  // 刷新窗口
}

void on_mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:         // 鼠标左键
            if (state == GLUT_DOWN) {  // 按下
                Mouse = GLUT_DOWN;
                prevx = x;
                prevy = y;
            }
            if (state == GLUT_UP) {  // 放开
                Mouse = GLUT_UP;
            }
            break;
    }

    glutPostRedisplay();  // 刷新窗口
}

void motion(int x, int y) {
    if (Mouse == GLUT_DOWN) {
        int deltax, deltay;

        deltax = prevx - x;                // 获取移动距离
        anglex += 0.005 * deltax;          // 系数控制移动速度，正负号切换移动方向
        camx = RADIUS_VIEW * sin(anglex);  // 相机位置更新
        camz = RADIUS_VIEW * cos(anglex);

        deltay = prevy - y;
        angley -= 0.005 * deltay;
        camy = RADIUS_VIEW * sin(angley);
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
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.001, 30.0);
    glMatrixMode(GL_MODELVIEW);  // 对模型视图矩阵堆栈应用随后的矩阵操作
    glLoadIdentity();            // 重置当前指定的矩阵为单位矩阵

    gluLookAt(camx + xpos, camy, camz + zpos,
              xpos, 0.0, zpos,
              0.0, 1.0, 0.0);
}

GLfloat degrees(GLfloat a) {
    return a * 180.0f / PI;
}

GLfloat radians(GLfloat a) {
    return a * PI / 180.0f;
}

GLfloat angleSum(GLfloat a, GLfloat b) {
    a += b;
    if (a < 0)
        return a + 2 * PI;
    else if (a > 2 * PI)
        return a - 2 * PI;
    else
        return a;
}

void update_scene() {
    GLfloat xDelta, zDelta;
    GLfloat rotation;
    GLfloat sin_steering, cos_steering;

    // if the bicycle is not moving then do nothing
    if (!is_move) return;

    if (speed < 0.0f)
        pedal_angle = speed = 0.0f;

    xDelta = speed * cos(radians(direction + steering));
    zDelta = speed * sin(radians(direction + steering));
    xpos += xDelta;
    zpos -= zDelta;
    pedal_angle = degrees(angleSum(radians(pedal_angle), speed / RADIUS_WHEEL));

    sin_steering = sin(radians(steering));
    cos_steering = cos(radians(steering));

    rotation = atan2(speed * sin_steering, 3.3f + speed * cos_steering);
    direction = degrees(angleSum(radians(direction), rotation));

    glutPostRedisplay();  // 刷新窗口
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // 设置显示模式（双缓冲、彩色、消影）
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);           // 设置窗口在屏幕中的位置
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);  // 设置窗口的尺寸

    glutCreateWindow("myBicycle");  // 设置窗口标题
    init();                         // 初始化场景

    // 创建菜单
    int sub_menu = glutCreateMenu(sub_menu_func);  // 车速的子菜单
    glutAddMenuEntry("High Speed", 1);
    glutAddMenuEntry("Medium Speed", 2);
    glutAddMenuEntry("Low Speed", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    int menu = glutCreateMenu(main_menu_func);   // 主菜单
    glutAddMenuEntry("Toggle Filling Mode", 1);  // 填充显示方式
    glutAddSubMenu("Toggle Speed Gear", sub_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);  // 关联菜单与鼠标右键

    // 设置渲染状态
    glutDisplayFunc(display);      // 显示
    glutReshapeFunc(change_size);  // 窗口大小变化时的回调函数
    glutKeyboardFunc(keyboard);    // 键盘回调函数
    glutMouseFunc(on_mouse);       // 鼠标回调函数
    glutMotionFunc(motion);        // 处理鼠标移动（当鼠标被点击且移动时）的回调函数

    glutIdleFunc(update_scene);  // 更新场景
    glutMainLoop();              // 事件主循环
    return 0;
}
