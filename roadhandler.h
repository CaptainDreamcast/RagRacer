#pragma once

#include <prism/actorhandler.h>
#include <prism/geometry.h>

ActorBlueprint getRoadHandler();

void getObjectPositionOnRoad(double t, double z, double& oScale, Position& oPos);
double getRoadZ();
double getCurveStrength(double z);