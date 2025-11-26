#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <cmath>
#include <optional>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>

const float MAJOR_RADIUS = 0.6; // radius of torus
const float MINOR_RADIUS = 0.2; // radius of inner tube
const float MAX_X_Y = MAJOR_RADIUS + MINOR_RADIUS;
const float THETA = 0.1; // radians rotation
const int NUM_POINTS = 100; // number of points in torus mesh
const std::string SYMBOLS[4] = {"█", "▓", "▒", "░"};

// rotation matrices, i think we only need the composition of all 3
const float S = std::sin(static_cast<float>(THETA));
const float C = std::cos(static_cast<float>(THETA));
const float   ROT[9] = {
  C*C,       -C*S,      S,
  S*C+S*S*C, C*C-S*S*S, -S*C,
  S*S-C*C*S, S*C+C*S*S, C*C,
};

inline void clear_screen() {
  std::cout << "\033[2J\033[H";
}

inline void print_at_pos(int row, int col, char c) {
  std::cout << "\033[" << row << ";" << col << 'H' << c;
}

// calculate the positive z given x and y, return none if not part of the torus
std::optional<float> calculate_pos_z(float x, float y) {
  float inner = std::pow(MINOR_RADIUS, 2.0) - std::pow(sqrt(x*x + y*y) - MAJOR_RADIUS, 2.0);
  if (inner < 0) {
    return std::nullopt;
  }
  return std::optional(sqrt(inner));
}

// convert i from range [a, b] into range [c, d]
inline float convert_range(float i, float a, float b, float c, float d) {
  return (i - a) / (b - a) * (d - c) + c;
}

inline float mesh_to_value(int i) {
  return convert_range(
    static_cast<float>(i),
    0.0,
    NUM_POINTS,
    -1.0 * MAX_X_Y,
    MAX_X_Y
  );
  //return static_cast<float>(i) / static_cast<float>(NUM_POINTS) * 2.0 * MAX_X_Y - MAX_X_Y;
}

struct Point {
  float x;
  float y;
  float z;
};

bool order_points(const Point& a, const Point& b) {
  if (a.z < b.z) {return true;} else {return false;}
}


inline Point rotate_point(Point &point, const float (&arr)[9]) {
  Point p;
  p.x = point.x * arr[0] + point.y * arr[1] + point.z * arr[2];
  p.y = point.x * arr[3] + point.y * arr[4] + point.z * arr[5];
  p.z = point.x * arr[6] + point.y * arr[7] + point.z * arr[8];
  return p;
}

void rotate_mesh(std::vector<Point> &points) {
  for (Point& p1 : points) {
    p1 = rotate_point(p1, ROT);
  }
}

std::vector<Point> init_mesh() {
  std::vector<Point> points;
  for (int i=0; i<NUM_POINTS; i++) {
    for (int j=0; j<NUM_POINTS; j++) {
      float x = mesh_to_value(i);
      float y = mesh_to_value(j);
      std::optional<float> z = calculate_pos_z(x, y);
      if (z.has_value()) {
        Point p;
        p.x = x;
        p.y = y;
        p.z = z.value();
        points.push_back(p);
        p.z = -1.0 * z.value(); // if we only get half a donut, this is why
        points.push_back(p);
      }
    }
  }
  return points;
}

void render_point(const Point &p){
}

void render_mesh(const std::vector<Point> &points) {

}

int main() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  std::vector<Point> points = init_mesh();
  while (true) {
    rotate_mesh(points);
    std::sort(points.begin(), points.end(), order_points);
  }
  return 0;
  
}
