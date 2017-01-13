#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include <random>


class MainScene :public cocos2d::Layer
{
    protected:
        MainScene();
        virtual ~MainScene();
        bool init() override;


    private:
        //the types of fruits
        enum class FruitType
        {
            APPLE,GRAPE,ORANGE,BANANA,CHERRY,
            GOLDEN,BOMB,
            COUNT //max value
        };

        //game status
        enum class GameState
        {
            READY,   //before playing game
            PLAYING,    //playing game
            ENDING, //showing finish effect
            RESULT  //showing result
        };

        /**create new fruit and add scene it,then return it.
         *@return new fruits
         */
        cocos2d::Sprite* addFruit();

        /**remove a fruit from the scene.
         *@param fruit : fruit be removed
         *@return : if a fruit is removed,return true
         */
        bool removeFruit(cocos2d::Sprite* fruit);

        /**cathch a fruit.this is called when the player catched a fruit.
         *@param fruit : fruit catched
         */
        void catchFruit(cocos2d::Sprite* fruit);

        /**this will be called when player catch a bomb
         */
        void onCatchBomb();

        /**show result screen.this method is called when the game finish.
         */
        void onResult();

        /**add ready-start label on main scene.
         */
        void addReadyLabel();

        /**add 'FINISH' label on main scene with animation.
         */
        void addFinishLabel();

        /**@return : high score of all times
         */
        int  getHighScore();

        /**update high score if new score is bigger than current high score
         */
        void updateHighScore();

    public:
        static cocos2d::Scene* createScene();

        /**this method will be called every frame.
         * update any data in this method.
         *@param dt : delta time
         */
        void update(float dt);

        /**this is called when scene's enter transition does finished
         */
        void onEnterTransitionDidFinish() override;

        /**
         */
        void onExit() override;

        /**generate random value among min and max.
         *@param min : minimum value
         *@param max : maximum value
         *@return : random value
         */
        float generateRandom(float min,float max);

        CREATE_FUNC(MainScene);

        //(private member)_player getPlayer() setPlayer(Sprite*) will be generated automatically
        CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*,_player,Player);
        //socre label
        CC_SYNTHESIZE_RETAIN(cocos2d::Label*,_scoreLabel,ScoreLabel);
        //time limit label
        CC_SYNTHESIZE_RETAIN(cocos2d::Label*,_secondLabel,SecondLabel);
        //batch
        CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode*,_fruitsBatchNode,FruitsBatchNode);
        //baske of fruits
        CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite*>,_fruits,Fruits);
        //score
        CC_SYNTHESIZE(int,_score,Score);
        //time limit
        CC_SYNTHESIZE(float,_second,Second);
        //player state
        CC_SYNTHESIZE(bool,_isCrash,IsCrash);
        //game status
        CC_SYNTHESIZE(GameState,_state,State);
        //random device
        CC_SYNTHESIZE(std::mt19937,_engine,Engine);
};

#endif /*defined(__MAIN_SCENE_H__) */
