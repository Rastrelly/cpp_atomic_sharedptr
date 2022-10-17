#include "mathMgr.h"


mathMgr::mathMgr()
{
	chartstate = {};
	xmin = 0; xmax = 0;
	ymin = 0; ymax = 0;
	funcsel = 0;
	a = 1; f = 1;
	xl = 100;
	xstep = 0.1;
	lastTs = 0;
	canDraw = true;
	redispFlag = true;
}

float mathMgr::func(float x)
{
	return a * sin(f * x * (3.14f / 180.0f));
}

void mathMgr::removeFirst()
{
	int l = getVSize();
	for (int i = 0; i < l - 1; i++)
	{
		chartstate[i] = chartstate[i + 1];
	}
	chartstate.resize(xl);
}

void mathMgr::appendValue(pt val)
{	
	chartstate.push_back(val);
	if (chartstate.size() > xl)
	{
		removeFirst();
	}
}

void mathMgr::pushForw(float ts)
{
		if ((ts - lastTs) >= xstep)
		{
			lastTs = ts;
			printf("TS=%f; VL=%d\n",ts,getVSize());
			appendValue({ ts, func(ts) });
			findExtremes();
			setRedispFlag(true);
		}
}

void mathMgr::findExtremes()
{
	
	if (getVSize() > 0)
	{
		int l = getMaxVSize();
		xmin = chartstate[0].x;
		xmax = chartstate[l - 1].x;
		ymin = chartstate[0].y;
		ymax = chartstate[0].y;
		for (int i = 0; i < l; i++)
		{
			float cnd = chartstate[i].y;
			if (cnd < ymin) ymin = cnd;
			if (cnd > ymax) ymax = cnd;
		}
	
	}
}

float mathMgr::getChartX(int i) {
	if (canDraw)
		if (i < getVSize())
			return chartstate[i].x;
		else return 0;
	else
		return 0;
}

float mathMgr::getChartY(int i) {
	if (canDraw)
		if (i < getVSize())
			return chartstate[i].y;
		else return 0;
	else
		return 0;
}

float mathMgr::getMaxVSize()
{
	if (getVSize() <= xl) return getVSize();
	else return xl;
}