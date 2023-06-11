#include <iostream>
#include <cmath>
#include <vector>
#include <windows.h>
#include <string>
#include <numbers>
#include <cassert>
#include <cmath>
#include <numbers>
#include <algorithm>

using std::cout, std::endl, std::vector, std::string;

struct Point {
    double x = {};
    double y = {};
    double z = {};
};
struct Normal {
    double Nx = {};
    double Ny = {};
    double Nz = {};
};


Point project_scene(const Point p, double K1, double screen_width, double screen_height) {

    double ooz = 1 / (p.z); // one over z
    double _x = std::floor(screen_width / 2 + K1 * ooz * p.x);
    double _y = std::floor(screen_height / 2 - K1 * ooz * p.y);
    return Point{
        _x, _y, K1
    };
}


Point make_donut_point(double R1, double R2, double phi, double theta, double A, double B, double K2) {

    double costheta = cos(theta), sintheta = sin(theta);
    double cosphi = cos(phi), sinphi = sin(phi);
    double circlex = R2 + R1 * costheta;
    double circley = R1 * sintheta;
    double x = circlex * (cos(B) * cosphi + sin(A) * sin(B) * sinphi) - circley * cos(A) * sin(B);
    double y = circlex * (sin(B) * cosphi - sin(A) * cos(B) * sinphi) + circley * cos(A) * cos(B);
    double z = K2 + cos(A) * circlex * sinphi + circley * sin(A);
    return Point{
        x, y, z
    };
}


Normal get_point_normal(double theta, double phi, double A, double B) {
    double s1 = cos(A) * sin(theta) - sin(A) * cos(theta) * sin(phi);
    double Nx = cos(B) * cos(phi) * cos(theta) - sin(B) * s1;
    double Ny = cos(B) * s1 + sin(B) * cos(phi) * cos(theta);
    double Nz = sin(A) * sin(theta) + cos(A) * cos(theta) * sin(phi);
    return Normal{
        Nx, Ny, Nz
    };
}


double luminance(const Point& light, const Normal& norm) {
    return light.x * norm.Nx + light.y * norm.Ny + light.z * norm.Nz;
}

/// get width of terminal (windows only)
int width() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
};

/// get height of terminal (windows only)
int height() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return rows;
};

const double PI = std::numbers::pi; // C++20

void render_frame(const double& A, const double& B) {

    // clear terminal output
    system("cls");    

    const double theta_spacing = 0.07;
    const double phi_spacing = 0.02;

    const char pixels[] = ".,-~:;=!*#$@";
    int screen_width = width();
    int screen_height = height();

    char* output = new char[screen_height * screen_width];
    double* zbuffer = new double[screen_height * screen_width];
    for (int i = 0; i < screen_height * screen_width; i++) {
        zbuffer[i] = 0;
        output[i] = ' ';
    }

    const double w = std::min(screen_width, screen_height);
    const double R2 = w / 2;
    const double R1 = w / 5;
    double K1 = w / 2;
    double K2 = 2 * K1;

    
    for (double theta = 0; theta < 2 * PI - theta_spacing; theta += theta_spacing) {
        for (double phi = 0; phi < 2 * PI - phi_spacing; phi += phi_spacing) {

            Point circle_point = make_donut_point(R1, R2, phi, theta, A, B, K2);
            Point project_point = project_scene(circle_point, K1, screen_width, screen_height);

            double ooz = double(1 / (circle_point.z + 1e-5));

            int xp = project_point.x;
            int yp = project_point.y;
            
            assert(xp + yp * screen_width < screen_height * screen_width && xp + yp * screen_width >= 0);

            Normal norm = get_point_normal(theta, phi, A, B);
            Point light = { 0, 1, -1 };
            double L = luminance(light, norm);

            if (L > 0) {

                if (ooz > zbuffer[xp + yp * screen_width]) {
                    zbuffer[xp + yp * screen_width] = ooz;
                    int luminance_index = L * 8;
                    assert(luminance_index < strlen(pixels));
                    output[xp + yp * screen_width] = pixels[luminance_index];
                }
            }
        }
    }

    // output to screen
    for (int j = 0; j < screen_height; j++) {
        for (int i = 0; i < screen_width; i++) {
            assert(i + j * screen_width < screen_height * screen_width);
            cout << output[i + j * screen_width];
        }
        cout << '\n';
    }

    delete[] zbuffer;
    delete[] output;
}

void render_frames() {
    const double A_spacing = PI / 16;
    const double B_spacing = PI / 16;
    for (double A = 0; A < 2 * PI - A_spacing; A += A_spacing) {
        for (double B = 0; B < 2 * PI - B_spacing; B += B_spacing) {
            render_frame(A, B);
        }
    }
}
int main() {

    // render_frames();
    render_frame(PI / 2, PI / 4);

    return 0;
}