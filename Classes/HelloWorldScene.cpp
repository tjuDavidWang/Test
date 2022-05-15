#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }
    // 初始化Physics
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    //getInstance单例化模式处理对象
    auto winSize = Director::getInstance()->getVisibleSize();
    //获得可视区域的出发点坐标，在处理相对位置时，确保节点在不同分辨率下的位置一致。
    auto origin = Director::getInstance()->getVisibleOrigin();

    // set Background with grey colour
    auto background = DrawNode::create();

    background->drawSolidRect(origin, Vec2(winSize) + origin, cocos2d::Color4F(0.6, 0.6, 0.6, 1.0));
    this->addChild(background);

    // Add player
    _player = Sprite::create("player.png");
    _player->setPosition(Vec2(winSize.width * 0.1, winSize.height * 0.5));
    this->addChild(_player);

    // 初始化了随机数生成器。如果不执行这一步，每次运行程序都会产生一样的随机数。
    srand((unsigned int)time(nullptr));
    // 每隔1.5秒生成一个怪物
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::addMonster), 1.5);

    // 发射飞镖
    // 定义触摸事件的监听器。监听器有两种：
    // 1.EventListenerTouchOneByOne：此类型对每个触摸事件调用一次回调方法。
    // 2.EventListenerTouchAllAtOnce：此类型对所有的触摸事件调用一次回调方法。
    auto eventListener = EventListenerTouchOneByOne::create();
    // 定义回调函数onTouchBegan():手指第一次碰到屏幕时被调用。
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    /**/// 使用EventDispatcher来处理各种各样的事件，如触摸和其他键盘事件。！！！！
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

    // 碰撞检测
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}

void HelloWorld::addMonster(float dt) {
    auto monster = Sprite::create("monster.png");

    // 1 Add monster，getContentSize得到图片的大小
    auto monsterContentSize = monster->getContentSize();
    auto selfContentSize = this->getContentSize();
    int minY = monsterContentSize.height / 2;
    int maxY = selfContentSize.height - minY;
    int rangeY = maxY - minY;
    int randomY = (rand() % rangeY) + minY;
    monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));

    // Add monster's physicsBody
    auto physicsBody = PhysicsBody::createBox(monster->getContentSize(), PhysicsMaterial(0.0f, 0.0f, 0.0f));
    physicsBody->setDynamic(false);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    monster->setPhysicsBody(physicsBody);

    this->addChild(monster);

    // 2 Let monster run
    int minDuration = 2.0;
    int maxDuration = 4.0;
    int rangeDuration = maxDuration - minDuration;
    int randomDuration = (rand() % rangeDuration) + minDuration;

    // 定义移动的object
    // 在randomDuration这个时间内(2-4秒内)，让怪物从屏幕右边移动到左边。(怪物有快有慢)
    auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));
    // 定义消除的Object。怪物移出屏幕后被消除，释放资源。
    auto actionRemove = RemoveSelf::create();
    monster->runAction(Sequence::create(actionMove, actionRemove, nullptr));

}

//回调函数的形式
bool HelloWorld::onTouchBegan(Touch* touch, Event* unused_event) {
    // 1 - Just an example for how to get the player object
    // 说明一下作为第二个参数传递给addEventListenerWithSceneGraphPriority(eventListener, _player)的_player对象被访问的方式。
    // auto node = unused_event->getcurrentTarget();

    // 2.获取触摸点的坐标，并计算这个点相对于_player的偏移量。
    Vec2 touchLocation = touch->getLocation();
    Vec2 offset = touchLocation - _player->getPosition();
    // 如果offset的x值是负值，这表明玩家正试图朝后射击。在本游戏中这是不允许的。
    if (offset.x < 0) {
        return true;
    }

    // 3.在玩家所在的位置创建一个飞镖，将其添加到场景中。
    auto projectile = Sprite::create("Projectile.png");
    projectile->setPosition(_player->getPosition());

    //这个是每一个物理实体都一样的
    // Add projectile's physicsBody
    auto physicsBody = PhysicsBody::createBox(projectile->getContentSize(), PhysicsMaterial(0.0f, 0.0f, 0.0f));
    physicsBody->setDynamic(false);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    projectile->setPhysicsBody(physicsBody);
    projectile->setTag(10);

    this->addChild(projectile);

    // 4.将偏移量转化为单位向量，即长度为1的向量。
    offset.normalize();
    // 将其乘以1000，你就获得了一个指向用户触屏方向的长度为1000的向量。为什么是1000呢？因为长度应当足以超过当前分辨率下屏幕的边界。
    auto shootAmount = offset * 1000;
    // 将此向量添加到飞镖的位置上去，这样你就有了一个目标位置。
    auto realDest = shootAmount + projectile->getPosition();

    // 5.创建一个动作，将飞镖在2秒内移动到目标位置，然后将它从场景中移除。
    auto actionMove = MoveTo::create(2.0f, realDest);
    auto actionRemove = RemoveSelf::create();
    projectile->runAction(Sequence::create(actionMove, actionRemove, nullptr));

    return true;
}
bool HelloWorld::onContactBegin(cocos2d::PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    if (nodeA && nodeB)
    {
        if (nodeA->getTag() == 10)
        {
            nodeB->removeFromParentAndCleanup(true);
        }
        else if (nodeB->getTag() == 10)
        {
            nodeA->removeFromParentAndCleanup(true);
        }
    }

    return true;
}