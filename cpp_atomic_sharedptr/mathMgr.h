#pragma once
#include <vector>
#include <chrono>
#include <mutex>

struct pt
{
	float x, y;
};

class mathMgr
{
private:
	
	int funcsel;
	float xmin, xmax, ymin, ymax;
	float a, f;
	float func(float x);
	std::vector<pt> chartstate;
	int xl;
	float xstep;
	float lastTs;
	bool redispFlag;
	void removeFirst();
	void appendValue(pt val);
	void findExtremes();
public:
	bool canDraw;
	mathMgr();
	void setA(float val) { a = val; }
	void setF(float val) { f = val; }
	void setXI(float val) { xstep = val; }
	void setRedispFlag(bool val) { redispFlag = val; }

	float getXMin() { return xmin; }
	float getXMax() { return xmax; }
	float getYMin() { return ymin; }
	float getYMax() { return ymax; }

	float getChartX(int i);
	float getChartY(int i);

	float getMaxVSize();

	bool getRedispFlag() { return redispFlag; }	

	void pushForw(float ts);
	int getVSize() { return chartstate.size(); }
};

