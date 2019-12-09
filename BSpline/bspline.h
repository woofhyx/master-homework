#pragma once
#include <vector>
#include <Eigen/Dense>
#include "filehelper.h"
using namespace std;
using namespace Eigen;

vector<Vector3d> points;
vector<float> param_t;
vector <float> knot;
int n, degree, order;
MatrixXd P, D;
enum en_t {
	UNIFORM_SPACED,
	CHORD_LENGTH
};
enum en_knot {
	k_SPACED,
	AVERAGE,
	UNIVER
};

bool debug = 1;

const double Nikt(int i, int k, double t) 
{
	using namespace std;
	auto& x = knot;
	if (k == 1)
	{
		if (t >= x[i] && t < x[i + 1]) return 1;
		else return 0;
	}
	double d1 = 0, d2 = 0;
	if (i + k - 1 < x.size() && i < x.size())
		d1 = x[i + k - 1] - x[i];
	if (i + k < x.size() && i + 1 < x.size())
		d2 = x[i + k] - x[i + 1];
	double a, b;
	if (abs(d1) < 1e-6) a = 0;
	else a = (t - x[i]) * Nikt(i, k - 1, t) / d1;
	if (abs(d2) < 1e-6 || i + k >= x.size()) b = 0;
	else b = (x[i + k] - t) * Nikt(i + 1, k - 1, t) / d2;
	return a + b;
}

float cal_N(int i, int k, float t) {
	if (k ==1) {
		if (t >= knot[i] && t < knot[i + 1])
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	}
	float lfrac = (knot[i + k - 1] - knot[i]);
	float rfrac = (knot[i + k] - knot[i + 1]);
	float a = 0.0f, b = 0.0f;
	if (abs(lfrac) < 1e-6 && abs(rfrac) < 1e-6) {
		return 0.0;
	}
	else if (abs(lfrac ) < 1e-6) {
		a = (knot[i + k] - t) / rfrac * cal_N(i + 1, k - 1, t);
	}
	else if (abs(rfrac) < 1e-6 ) {
		b = (t - knot[i]) / lfrac * cal_N(i, k - 1, t);
	}
	else {
		a = (knot[i + k] - t) / rfrac * cal_N(i + 1, k - 1, t);
		b = (t - knot[i]) / lfrac * cal_N(i, k - 1, t);
	}
	if (i == 0 && k == 3 && t == 0.01f) debug = 0; 
	return a + b;
}

double distance(Vector3d& v1, Vector3d& v2) {
	return sqrt((v1(0) - v2(0)) * (v1(0) - v2(0)) + (v1(1) - v2(1)) * (v1(1) - v2(1)) + (v1(2) - v2(2)) * (v1(2) - v2(2)));
}
/*
get parm t
*/
void generatePram(en_t type, int a, int b) {
	param_t.clear();
	double length = 0.0;
	double ct = 0;
	switch (type)
	{
	case UNIFORM_SPACED:
		for (int i = 0; i <= n;++i) {
			param_t.push_back((float)a + (float)i / (n) * (b - a));
		}
		break;
	case CHORD_LENGTH:
		for (int i = 1; i <= n;++i) {
			length += distance(points[i], points[i - 1]);
		}
		for (int i = 0; i <= n;++i) {
			if (i == 0) param_t.push_back(0);
			else {
				ct += distance(points[i], points[i - 1]);
				param_t.push_back((float)ct / length);
			}
			
		}
		break;
	default:
		break;
	}
	if (debug) {
		cout << "T :";
		for (int i = 0; i < param_t.size(); ++i) {
			cout <<  param_t[i] << " ";
		}
		cout << endl;
	}
	

}

/*
	generate knot
*/
void generateKnot(en_knot type) {
	knot.clear();
	if (type == UNIVER) {
		for (int i = 1; i <= n+1+order;++i) {
			knot.push_back((float)i / (n+order+1));
		}
	}
	else if (type == k_SPACED) {
		for (int i = 0; i <order;++i) {
			knot.push_back(0);
		}
		for (int i = 1;i <= n - degree;++i) {
			knot.push_back((float)i / (n - degree + 1));
		}
		for (int i = 0; i < order;++i) {
			knot.push_back(1);
		}
	}
	else if (type == AVERAGE) {
		for (int i = 0; i < order;++i) {
			knot.push_back(0);
		}
		for (int j = 1; j <= n - degree;++j) {
			double avg = 0.0;
			for (int i = j; i <= j + degree - 1;++i) {
				avg += param_t[i];
			}
			knot.push_back(avg / degree);
		}
		for (int i = 0; i < order;++i) {
			knot.push_back(1);
		}
	}
	if(debug){
		cout << "KNOT: " << knot.size();
		for (int i = 0; i < knot.size(); ++i) {
			cout << knot[i] << " ";
		}
		cout << endl;
	}
}

void drawPoly(int i, int k, float t) {
	float step = 0.1f;
	fprintf(stderr, "N(%d,%d): ", i, k);
	for (float c = 0; c < 1;c += step) {
		fprintf(stderr, "%f ", cal_N(i, k, c));
	}
	fprintf(stderr, "%f ", cal_N(i, k, t));
	fprintf(stderr, "\n");
}
void evaluate() {
	MatrixXd N(n + 1, n + 1);
	for (int i = 0; i <= n; ++i) {
		for (int j = 0; j <= n; ++j) {
			N(i, j) = Nikt(j, order, param_t[i]);
		}
	}
	N(n, n) = 1;
	cout << "N: " << endl;
	cout << N << endl;
	D.resize(n + 1, 3);
	for (int i = 0; i <= n; ++i) {
		for (int j = 0; j < 3; ++j) {
			D(i, j) = points[i](j);
		}
	}
	cout << "D: " << endl;
	cout << D << endl;
	P.resize(n + 1, 3);
	P =N.inverse() * D;
	cout << "P: " << endl;
	cout << P << endl;
}
void testcase() {
	readdata("D:\\working\\B-spline\\BSpline\\Release\\data.in");
	n = points.size() -1;
	degree = 2;
	order = degree + 1;
	if (n < degree) {
		fprintf(stderr, "need degree +1 control points\n");
		exit(1);
	}
	generatePram(en_t::UNIFORM_SPACED, 0, 1);
	cout << "n+1: "<<n + 1 << " degree "<<degree<<endl;
	generateKnot(en_knot::k_SPACED);
	evaluate();
}
void drawBspline() {
	glPointSize(4);
	glColor3f(0, 0, 1);
	glBegin(GL_POINTS);
	float step = 0.01f;
	
	for (float t = 0.0; t <= 1.0; t += step) {
		Vector3d v(0, 0, 0);
		for (int i = 0; i < n+1; ++i) {
			v += Vector3d(P(i,0), P(i,1), P(i,2)) * cal_N(i, order, t);
		}
		glVertex3f(v(0), v(1), v(2));
	}
	glEnd();

	glPointSize(10);
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	for (float t = 0; t <= n; t++) {
		glVertex3f(D(t, 0), D(t, 1), D(t, 2));
	}
	glEnd();
	glPointSize(10);
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (float t = 0; t <= n; t++) {
		glVertex3f(P(t, 0), P(t, 1), P(t, 2));
	}
	glEnd();
}

void displayB() {
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(100, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 100, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 100);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_LINES);
	for (int i = 1; i < points.size(); ++i) {
		glVertex3d(points[i - 1](0), points[i - 1](1), points[i - 1](2));
		glVertex3d(points[i](0), points[i](1), points[i](2));
	}
	glEnd();
	drawBspline();
	glFlush();
}