#pragma once
#include "GSingleton.h"
#include "Horloge.h"

//singleton de la classe du game manager
class GameManager : public Incopiable {
	static GameManager instance;
	GameManager() = default;

public:
	static GameManager& get() noexcept { return instance; }
	void setPauseMenu(bool) noexcept;
	bool getIsPauseStatus() noexcept;
	bool hasBeenEnoughTimeSinceLastPause();

private:
	PM3D::Horloge horloge;
	int64_t lastPaused = 0;
	bool isPause = false;

};



