#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "TitleScene.h"

USING_NS_CC;

//margin of fruits from top of the screen(px)
const int FRUIT_TOP_MARGIN = 40;
//incidence of a fruit
const int FRUIT_SPAWN_RATE = 20;
//max time limit
const float TIME_LIMIT_SECOND = 60.0;
//when player catch a golden fruit,add this score
const int GOLDEN_FRUIT_SCORE = 5;
//when player catch a bomb,it get this penalty score
const int BOMB_PENALTY_SCORE = 4;
//base probability of a golden fruit appearing
const float GOLDEN_FRUIT_PROBABILITY_BASE = 0.02;
//base probability of a bomb appearing
const float BOMB_PROBABILITY_BASE = 0.05;
//rate of probability of a golden furit appearing
const float GOLDEN_FRUIT_PROBABILITY_RATE = 0.001;
//rate of probability of a bomb appearing
const float BOMB_PROBABILITY_RATE = 0.003;
//base spawn frequency of a fruit
const float FRUIT_SPAWN_FREQUENCY_BASE = 0.02;
//rate of spawn frequency of a fruit
const float FRUIT_SPAWN_FREQUENCY_RATE = 1.05f;
//maximum value of spawn frequency of a fruit
const float MAXIMUM_SPAWN_FREQUENCY = 0.5;
//the number of normal fruits
const int NOMAL_FRUIT_COUNT = 5;
//font size of labels
const int LABEL_FONT_SIZE = 20;
//the key for UserDefault
const char* HIGH_SCORE_KEY = "highscorekey";

MainScene::MainScene()
: _player(NULL)
,_scoreLabel(NULL)
,_secondLabel(NULL)
,_fruitsBatchNode(NULL)
,_score(0)
,_second(TIME_LIMIT_SECOND)
,_isCrash(false)
,_state(GameState::READY)
{
    //initialize seed
    std::random_device rdev;
    _engine.seed(rdev());
}

//destracta
MainScene::~MainScene()
{
    //update high score
    updateHighScore();
    //release objects
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
    CC_SAFE_RELEASE_NULL(_fruitsBatchNode);
}

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();

    scene->addChild(layer);

    return scene;
}

bool MainScene::init()
{
    if(!Layer::init()) {
        return false;
    }

    //initialization...

    //get Director
    auto director = Director::getInstance();
    //get screen size
    auto size = director->getWinSize();
    //create background sprite
    auto background = Sprite::create("background.png");
    //set sprite's position where it is displayed
    background->setPosition(Vec2(size.width / 2.0,size.height / 2.0));
    //add the sprite to the parent
    this->addChild(background);

    //create a sprite of player
    this->setPlayer(Sprite::create("player_static.png"));
    //set position
    _player->setPosition(Vec2(size.width / 2.0,size.height - 890));
    this->addChild(_player);

    //setting for handling user input
    //touch event
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch,Event* event) {
        //on touch...
        log("random->%f",this->generateRandom(0,10));
        return true;
    };

    listener->onTouchMoved = [this](Touch* touch,Event* event) {
        //on touch moved...
        if(!this->getIsCrash() && _state == GameState::PLAYING) {
            //get difference of current and previous position as a vector
            Vec2 delta = touch->getDelta();
            //get currrent player position
            Vec2 position = _player->getPosition();
            //new position is (current position) + (delta)
            Vec2 newposition = position + delta;
            //get window size
            auto winSize = Director::getInstance()->getWinSize();
            //newposition must be in the screen
            newposition = newposition.getClampPoint(Vec2(0,position.y),Vec2(winSize.width,position.y));
            //the player does not move y-direction
            newposition.y = winSize.height - 890;

            //set position
            _player->setPosition(newposition);
        }
    };
    //register the listener to the EventDispatcher
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);

    //initialization of Batch Node
    auto fruitsBatchNode = SpriteBatchNode::create("fruits.png");
    this->addChild(fruitsBatchNode);
    this->setFruitsBatchNode(fruitsBatchNode);

    //this line is for calling update() method every frame
    this->scheduleUpdate();

    //make score header
    auto scoreLabelHeader = Label::createWithSystemFont("SCORE","Marker Felt",LABEL_FONT_SIZE);
    scoreLabelHeader->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    scoreLabelHeader->enableOutline(Color4B::BLACK,1.5);
    scoreLabelHeader->setPosition(Vec2(size.width / 2.0 * 1.5,size.height - FRUIT_TOP_MARGIN / 2.0));
    this->addChild(scoreLabelHeader);

    //make score label
    auto scoreLabel = Label::createWithSystemFont(StringUtils::toString(_score),"Marker Felt",LABEL_FONT_SIZE);
    this->setScoreLabel(scoreLabel);
    scoreLabel->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    scoreLabel->enableOutline(Color4B::BLACK,1.5);
    scoreLabel->setPosition(Vec2(size.width / 2.0 * 1.5,size.height - FRUIT_TOP_MARGIN));

    this->addChild(scoreLabel);

    //make second label that display time limit
    int second = static_cast<int>(_second);
    auto secondLabel = Label::createWithSystemFont(StringUtils::toString(second),"Marker Felt",LABEL_FONT_SIZE);
    this->setSecondLabel(secondLabel);
    secondLabel->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    secondLabel->enableOutline(Color4B::BLACK,1.5);
    secondLabel->setPosition(Vec2(size.width / 2.0,size.height - FRUIT_TOP_MARGIN));
    this->addChild(secondLabel);

    //make header second label
    auto secondLabelHeader = Label::createWithSystemFont("TIME","Marker Felt",LABEL_FONT_SIZE);
    secondLabelHeader->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    secondLabelHeader->enableOutline(Color4B::BLACK,1.5);
    secondLabelHeader->setPosition(Vec2(size.width / 2.0,size.height - FRUIT_TOP_MARGIN / 2.0));
    this->addChild(secondLabelHeader);

    //make a header label for displaying high score
    auto hsLabellHeader = Label::createWithSystemFont("HIGH SCORE","Marker Felt",LABEL_FONT_SIZE);
    hsLabellHeader->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    hsLabellHeader->enableOutline(Color4B::BLACK,1.5);
    hsLabellHeader->setPosition(Vec2(size.width / 2.0 * 0.5,size.height - FRUIT_TOP_MARGIN / 2.0));
    this->addChild(hsLabellHeader);

    //make a header label that show high score
    int highSc = getHighScore();
    auto highScoreLabel = Label::createWithSystemFont(StringUtils::toString(highSc),"Marker Felt",LABEL_FONT_SIZE);
    highScoreLabel->enableShadow(Color4B::BLACK,Size(0.5,0.5),3);
    highScoreLabel->enableOutline(Color4B::BLACK,1.5);
    highScoreLabel->setPosition(Vec2(size.width / 2.0 * 0.5,size.height - FRUIT_TOP_MARGIN));
    this->addChild(highScoreLabel);

    return true;
}

Sprite* MainScene::addFruit()
{
    //get screen size
    auto winSize = Director::getInstance()->getWinSize();

    //select a type of a fruit
    // int fruitType = rand() % static_cast<int>(FruitType::COUNT);
    int fruitType = 0;
    float r = this->generateRandom(0,1);
    int pastSecond = TIME_LIMIT_SECOND - _second;
    float goldenFruitProbability = GOLDEN_FRUIT_PROBABILITY_BASE + GOLDEN_FRUIT_PROBABILITY_RATE * pastSecond;
    float bombProbability = BOMB_PROBABILITY_BASE + BOMB_PROBABILITY_RATE * pastSecond;

    if (r <= goldenFruitProbability) {
        fruitType = static_cast<int>(FruitType::GOLDEN);
    } else if (r <= goldenFruitProbability + bombProbability) {
        fruitType = static_cast<int>(FruitType::BOMB);
    } else {
        //nomal fruit
        fruitType = round(generateRandom(0,NOMAL_FRUIT_COUNT - 1));
    }

    //make fruit
    // std::string filename = StringUtils::format("fruit%d.png",fruitType);
    // auto fruit = Sprite::create(filename);
    //make fruit using batch node
    //get texture size
    auto textureSize = _fruitsBatchNode->getTextureAtlas()->getTexture()->getContentSize();
    //calclate texture width of one sprite
    auto fruitWidth = textureSize.width / static_cast<int>(FruitType::COUNT);
    auto fruit = Sprite::create("fruits.png",Rect(fruitWidth * fruitType,
                                                    0,
                                                    fruitWidth,
                                                    textureSize.height));
    //set type of the fruit as a tag
    fruit->setTag(fruitType);

    //get size of the fruit
    auto fruitSize = fruit->getContentSize();
    //make x position in random
    float fruitXpos = rand() % static_cast<int>(winSize.width);

    fruit->setPosition(Vec2(fruitXpos,
        winSize.height - FRUIT_TOP_MARGIN - fruitSize.height / 2.0));
    //add fruit to the scene using batch node
    _fruitsBatchNode->addChild(fruit);
    //add fruit object to the _fruits Vector
    _fruits.pushBack(fruit);

    //setting for falling animation
    //goal point
    auto ground = Vec2(fruitXpos,0);

    //make appear animation
    auto swing = Repeat::create(
            Sequence::create(
            RotateTo::create(0.25,-30),
            RotateTo::create(0.25,30),
            NULL),2);

    //make falling animation(3s)
    // auto expoFall = EaseExponentialIn::create(MoveTo::create(3,ground));
    auto fall = MoveTo::create(3,ground);
    //action that call removeFruit() immediately
    auto remove = CallFuncN::create([this](Node* node) {
        //down cast node to sprite
        auto sprite = dynamic_cast<Sprite*>(node);
        //call removeFruit()
        this->removeFruit(sprite);
    });

    //action that run fall action and remove action continuously
    auto sequence = Sequence::create(
        ScaleTo::create(0.25,1),
        swing,
        RotateTo::create(0.125,0),
        fall,
        remove,
        NULL);

    fruit->runAction(sequence);

    //return the new fruit
    return fruit;
}

bool MainScene::removeFruit(Sprite* fruit)
{
    //check the Vector _fruits does contain any fruit or not
    if(_fruits.contains(fruit)) {
        //remove it from parent node
        fruit->removeFromParent();
        //also remove it from _fruits
        _fruits.eraseObject(fruit);

        return true;
    }

    return false;
}

void MainScene::catchFruit(cocos2d::Sprite* fruit)
{
    //only when the player is not crash
    if(!this->getIsCrash()) {
        auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();
        //get fruit type
        FruitType fruitType = static_cast<FruitType>(fruit->getTag());
        switch (fruitType) {

            case MainScene::FruitType::GOLDEN:
                _score += GOLDEN_FRUIT_SCORE;
                audioEngine->playEffect("catch_golden.mp3");
                break;

            case MainScene::FruitType::BOMB:
                this->onCatchBomb();
                audioEngine->playEffect("crash.mp3");
                break;

            default:
                //other furits
                _score += 1;
                audioEngine->playEffect("catch_fruit.mp3");
                break;
        }

        //remove the catched fruit
        this->removeFruit(fruit);
        //update score label
        _scoreLabel->setString(StringUtils::toString(_score));
    }
}

void MainScene::update(float dt)
{
    //this method is called every frame...

    //check the game state
    if (_state == GameState::PLAYING) {
        //create fruit in random time
        // int random = rand() % FRUIT_SPAWN_RATE;
        // if(random == 0) {
        // this->addFruit();
        // }
        float pastSecond = TIME_LIMIT_SECOND - _second;
        float p = FRUIT_SPAWN_FREQUENCY_BASE * (1 + powf(FRUIT_SPAWN_FREQUENCY_RATE,pastSecond));
        p = MIN(p,MAXIMUM_SPAWN_FREQUENCY);
        float random = this->generateRandom(0,1);
        if (random < p) {
            this->addFruit();
        }

        //check that does the player catch a fruit or not
        for(auto& fruit : _fruits) {
            Vec2 busketPosition = _player->getPosition() - Vec2(0,10);
            //get a rectangle from a fruit
            Rect boundingBox = fruit->getBoundingBox();
            //check that boundingBox contains busketPosition or not.
            //if isHit is true,it seems to  that the player catched a fruit.
            bool isHit = boundingBox.containsPoint(busketPosition);
            if(isHit) {
                this->catchFruit(fruit);
            }
        }
        //update time limit
        _second -= dt;
        int second = static_cast<int>(_second);
        //update second label
        _secondLabel->setString(StringUtils::toString(second));
        //check is there time still left or not
        if (_second < 0) {
            //change game state
            _state = GameState::ENDING;
            //show ending effect
            addFinishLabel();
        }
    }
}

void MainScene::onResult()
{
    //change game state
    _state = GameState::RESULT;
    auto winSize = Director::getInstance()->getWinSize();

    //replay button
    auto replayButton = MenuItemImage::create(
        "replay_button.png",
        "replay_button_pressed.png",
        [](Ref* ref) {
            //on button pressed...
            //play effect for button is pressed
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
            //create new main scene and replace it
            auto scene = MainScene::createScene();
            Director::getInstance()->replaceScene(scene);
        });

    //back to the title button
    auto titleButton = MenuItemImage::create(
        "title_button.png",
        "title_button_pressed.png",
        [](Ref* ref) {
            //on button pressed...
            //play effect for button is pressed
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
            //create titile scene and replace it and main scene
            auto scene = TitleScene::createScene();
            auto transition = TransitionCrossFade::create(1.0,scene);
            Director::getInstance()->replaceScene(transition);
        });

    //create menu using two buttons above
    auto menu = Menu::create(replayButton,titleButton,NULL);
    menu->alignItemsVerticallyWithPadding(15);
    menu->setPosition(Vec2(winSize.width / 2.0,winSize.height / 2.0));
    this->addChild(menu);
}

void MainScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    //play BGM
    //method params::playeBackgroundMusic("[filename]",[loop or not]);
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3",true);

    //show ready-start animation
    addReadyLabel();
}

//This function will be called when the player catch a bomb.
void MainScene::onCatchBomb()
{
    //change the state
    _isCrash = true;

    //crash animation
    Vector<SpriteFrame*> frames;
    auto playerSize = _player->getContentSize();
    //the number of frames of animation
    const int animationFrameCount = 3;
    //make animation frames
    for (int i = 0; i < animationFrameCount; i++) {
        auto rect = Rect(playerSize.width * i,0,playerSize.width,playerSize.height);
        auto frame = SpriteFrame::create("player_crash.png",rect);
        frames.pushBack(frame);
    }

    //make crash animation(1 frame is 1/6 seconds)
    auto animation = Animation::createWithSpriteFrames(frames,10.0 / 60.0);
    //loop 3 times
    animation->setLoops(3);
    animation->setRestoreOriginalFrame(true);
    _player->runAction(Sequence::create(Animate::create(animation),
                                        CallFunc::create([this] {
                                            _isCrash = false;
                                        }),
                                        NULL));
    //add penalty score
    _score = MAX(0,_score - BOMB_PENALTY_SCORE);
}

//this function will be called before ~MainScene() will be called
void MainScene::onExit()
{
    Layer::onExit();
    //pause BGM
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

//this function will be called before the game start.
//add ready-start label on main screen with action.
void MainScene::addReadyLabel()
{
    auto winSize = Director::getInstance()->getWinSize();
    auto center = Vec2(winSize.width / 2.0,winSize.height / 2.0);

    //a string 'READY'
    auto ready = Sprite::create("ready.png");
    //set default size 0%
    ready->setScale(0);
    ready->setPosition(center);
    this->addChild(ready);

    //animation for 'READY'
    //1.scale up 0% -> 100% in 0.25s
    //2.wait 1.0s
    //3.add 'START' and play effect for start game
    //4.remove itself
    ready->runAction(Sequence::create(
            ScaleTo::create(0.25,1),
            DelayTime::create(1.0),
            CallFunc::create([this,center] {
                //a string 'START'
                auto start = Sprite::create("start.png");
                //run scale up and fade out action same time in 0.5s
                //then,remove itself
                start->runAction(Sequence::create(
                        CCSpawn::create(
                            EaseIn::create(ScaleTo::create(0.5,5.0),0.5),
                            FadeOut::create(0.5),
                            NULL),
                        RemoveSelf::create(),
                        NULL));
                start->setPosition(center);

                //add 'START' and animation begin
                this->addChild(start);
                //change state
                _state = GameState::PLAYING;
                //play effect for game start
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("start.mp3");
            }),
            RemoveSelf::create(),
            NULL));
}

void MainScene::addFinishLabel()
{
    //change game state
    _state = GameState::ENDING;
    //make 'FINISH' label
    auto finish = Sprite::create("finish.png");
    auto winSize = Director::getInstance()->getWinSize();
    finish->setPosition(Vec2(winSize.width / 2.0,winSize.height / 2.0));
    finish->setScale(0);
    //play effect for game finish
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("finish.mp3");

    //make entering nad exiting action
    auto appear = EaseExponentialIn::create(ScaleTo::create(0.25,1.0));
    auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25,0));

    finish->runAction(Sequence::create(
        appear,
        DelayTime::create(2.0),
        disappear,
        DelayTime::create(1.0),
        CallFunc::create([this] {
            //show result screen
            this->onResult();
        }),
        NULL));

    this->addChild(finish);
}

//when you want to get high score,call this method
int MainScene::getHighScore()
{
    //get high score from UserDefault
    return UserDefault::getInstance()->getIntegerForKey(HIGH_SCORE_KEY);
}

void MainScene::updateHighScore()
{
    //if new score is bigger than current high score,update it
    if(_score > getHighScore())
        UserDefault::getInstance()->setIntegerForKey(HIGH_SCORE_KEY,_score);
}

float MainScene::generateRandom(float min,float max)
{
    std::uniform_real_distribution<float> dest(min,max);
    return dest(_engine);
}
