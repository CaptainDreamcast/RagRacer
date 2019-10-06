#pragma once

#include <prism/actorhandler.h>
#include <prism/geometry.h>

ActorBlueprint getPlayerHandler();

double getPlayerSpeedZ();
double getPlayerMaxSpeedZ();
Position getPlayerRoadPosition();
void setPlayerExplode();
void addPlayerPart();
void addPlayerImpulse(const Position& tAcc);
double getPlayerT();
int getPlayerStage();
int getPlayerParts();