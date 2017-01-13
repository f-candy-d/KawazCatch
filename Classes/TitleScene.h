#ifndef __TITLE_SCENE__
#define __TITLE_SCENE__

#include "cocos2d.h"

class TitleScene :public cocos2d::Layer
{
protected:
	TitleScene();
	virtual ~TitleScene();
	bool init() override;

private:

public:
	static cocos2d::Scene* createScene();
	void onEnterTransitionDidFinish() override;
	void onExit() override;
	CREATE_FUNC(TitleScene);
};

#endif /* defined(__TITLE_SCENE__) */
