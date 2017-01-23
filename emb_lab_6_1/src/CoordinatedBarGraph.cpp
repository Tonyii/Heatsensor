/*
 * CoordinatedBarGraph.cpp
 *
 *  Created on: 2.12.2016
 *      Author: Tony
 */
#include "BarGraph.h"
#include "CoordinatedBarGraph.h"

CoordinatedBarGraph::CoordinatedBarGraph(LiquidCrystal& lcd_, int length_, bool vertical_, int xcoord_, int ycoord_): BarGraph(lcd_, length_, vertical_), ycoord(ycoord_), xcoord(xcoord_) {
	// TODO Auto-generated constructor stub
}

void CoordinatedBarGraph::draw(int value){
	BarGraph::lcd.setCursor(xcoord, ycoord);
	BarGraph::draw(value);
}

CoordinatedBarGraph::~CoordinatedBarGraph() {
	// TODO Auto-generated destructor stub
}

