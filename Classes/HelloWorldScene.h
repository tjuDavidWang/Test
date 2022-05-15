#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
private:
    //¾«Áé¶ÔÏó
    cocos2d::Sprite* _player;
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    void addMonster(float dt);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);

    bool onContactBegin(cocos2d::PhysicsContact& contact);
};

#endif // __HELLOWORLD_SCENE_H__
