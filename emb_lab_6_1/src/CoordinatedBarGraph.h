/*
 * CoordinatedBarGraph.h
 *
 *  Created on: 2.12.2016
 *      Author: Tony
 */

#ifndef COORDINATEDBARGRAPH_H_
#define COORDINATEDBARGRAPH_H_
#include "BarGraph.h"

class CoordinatedBarGraph: public BarGraph {
public:
	CoordinatedBarGraph(LiquidCrystal& lcd_, int length_, bool vertical_, int xcoord_, int ycoord_);
	virtual ~CoordinatedBarGraph();
	void draw(int value);
private:
	int xcoord, ycoord;
};

#endif /* COORDINATEDBARGRAPH_H_ */
