#include "hazardhandler.h"

#include <queue>
#include <prism/blitz.h>

#include "gamelogic.h"
#include "roadhandler.h"
#include "playerhandler.h"

class Hazard {
public:
	double t;
	double z;
	int mEntityID;
	Hazard(double t, double z) {
		this->t = t;
		this->z = z;
		mEntityID = addBlitzEntity(makePosition(0, 0, 1));
		addBlitzMugenAnimationComponent(mEntityID, getGameSprites(), getGameAnimations(), 7000);
		updateHazardAppearance();
	}

	~Hazard() {
		removeBlitzEntity(mEntityID);
	}

	void updateHazardAppearance() {
		double scale;
		Position tPos;
		getObjectPositionOnRoad(t, z, scale, tPos);
		setBlitzEntityPosition(mEntityID, tPos);
		setBlitzEntityScale2D(mEntityID, scale);
	}

	void updatePlayerCollision() {
		auto t = getPlayerT();
		if (t < -0.1 || t > 1.1) {
			setPlayerExplode();
		}
	}

	void update() {
		updateHazardAppearance();
		updatePlayerCollision();
	}

};

class HazardHandler {
public:
	int mCurrentZ;
	deque<unique_ptr<Hazard>> mHazards;
	HazardHandler() {
		mCurrentZ = 0;
		for (int i = 0; i < 100; i++) {
			addHazard();
			const auto scale = getBlitzEntityScale(mHazards.back()->mEntityID);
			if (!scale.x) break;
		}
	}

	void addHazard() {
		mCurrentZ += 5000;
		mHazards.push_back(make_unique<Hazard>(-0.1, mCurrentZ));
		mHazards.push_back(make_unique<Hazard>(1.1, mCurrentZ));
	}

	void removeHazard() {
		mHazards.pop_front();
		mHazards.pop_front();
	}

	void update() {
		for (auto& hazard : mHazards) {
			hazard->update();
		}

		for (int i = 0; i < 100; i++) {
			const auto scaleBack = getBlitzEntityScale(mHazards.back()->mEntityID);
			if (!scaleBack.x) {
				break;
			}
			addHazard();
		}

		for (int i = 0; i < 100; i++) {
			const auto scaleFront = getBlitzEntityScale(mHazards.front()->mEntityID);
			if (scaleFront.x) {
				break;
			}
			removeHazard();
		}

	}

};

EXPORT_ACTOR_CLASS(HazardHandler);