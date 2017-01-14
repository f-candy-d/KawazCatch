#include "TitleScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* TitleScene::createScene()
{
	auto scene = Scene::create();
	auto layer = TitleScene::create();
	scene->addChild(layer);

	return scene;
}

TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{

}

bool TitleScene::init()
{
	if(!Layer::init()) {
		return false;
	}

	auto director = Director::getInstance();
	auto winSize = director->getWinSize();

	//add background
	auto background = Sprite::create("title_background.png");
	background->setPosition(Vec2(winSize.width / 2.0,winSize.height / 2.0));
	this->addChild(background);

	//add logo
	auto logo = Sprite::create("title_logo.png");
	logo->setPosition(Vec2(winSize.width / 2.0, winSize.height - 300));
	this->addChild(logo);

	//add 'touch to start' label
	auto touchToStart = Sprite::create("title_start.png");
	touchToStart->setPosition(Vec2(winSize.width / 2.0,180));
	//frash action
	auto blink = Sequence::create(
							FadeTo::create(0.5,127),
							FadeTo::create(0.5,255),
							NULL);

	touchToStart->runAction(RepeatForever::create(blink));
	this->addChild(touchToStart);
	//when screen is touched,go to the main scene
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch,Event* event) {

		//play effect for screen touched
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
		//once the player touch screen,disable the action to avoid that it touch screen many times
		this->getEventDispatcher()->removeAllEventListeners();
		//call CallFunc when 0.5s has past
		auto delay = DelayTime::create(0.5);
		//the action to start the game
		auto startGame = CallFunc::create([] {
			auto scene = MainScene::createScene();
			auto transition = TransitionPageTurn::create(0.5,scene,true);
			//replace scenes with transition
			Director::getInstance()->replaceScene(transition);
		});

		this->runAction(Sequence::create(delay,startGame,NULL));

		return true;
	};

	//register the listener
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);

	return true;
}

void TitleScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	//play BGM
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("title.mp3",true);
}

void TitleScene::onExit()
{
	Layer::onExit();
	//pause BGM
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}
