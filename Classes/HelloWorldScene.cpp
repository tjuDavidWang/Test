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
    // ��ʼ��Physics
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    //getInstance������ģʽ�������
    auto winSize = Director::getInstance()->getVisibleSize();
    //��ÿ�������ĳ��������꣬�ڴ������λ��ʱ��ȷ���ڵ��ڲ�ͬ�ֱ����µ�λ��һ�¡�
    auto origin = Director::getInstance()->getVisibleOrigin();

    // set Background with grey colour
    auto background = DrawNode::create();

    background->drawSolidRect(origin, Vec2(winSize) + origin, cocos2d::Color4F(0.6, 0.6, 0.6, 1.0));
    this->addChild(background);

    // Add player
    _player = Sprite::create("player.png");
    _player->setPosition(Vec2(winSize.width * 0.1, winSize.height * 0.5));
    this->addChild(_player);

    // ��ʼ����������������������ִ����һ����ÿ�����г��򶼻����һ�����������
    srand((unsigned int)time(nullptr));
    // ÿ��1.5������һ������
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::addMonster), 1.5);

    // �������
    // ���崥���¼��ļ������������������֣�
    // 1.EventListenerTouchOneByOne�������Ͷ�ÿ�������¼�����һ�λص�������
    // 2.EventListenerTouchAllAtOnce�������Ͷ����еĴ����¼�����һ�λص�������
    auto eventListener = EventListenerTouchOneByOne::create();
    // ����ص�����onTouchBegan():��ָ��һ��������Ļʱ�����á�
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    /**/// ʹ��EventDispatcher��������ָ������¼����紥�������������¼�����������
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

    // ��ײ���
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}

void HelloWorld::addMonster(float dt) {
    auto monster = Sprite::create("monster.png");

    // 1 Add monster��getContentSize�õ�ͼƬ�Ĵ�С
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

    // �����ƶ���object
    // ��randomDuration���ʱ����(2-4����)���ù������Ļ�ұ��ƶ�����ߡ�(�����п�����)
    auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));
    // ����������Object�������Ƴ���Ļ���������ͷ���Դ��
    auto actionRemove = RemoveSelf::create();
    monster->runAction(Sequence::create(actionMove, actionRemove, nullptr));

}

//�ص���������ʽ
bool HelloWorld::onTouchBegan(Touch* touch, Event* unused_event) {
    // 1 - Just an example for how to get the player object
    // ˵��һ����Ϊ�ڶ����������ݸ�addEventListenerWithSceneGraphPriority(eventListener, _player)��_player���󱻷��ʵķ�ʽ��
    // auto node = unused_event->getcurrentTarget();

    // 2.��ȡ����������꣬����������������_player��ƫ������
    Vec2 touchLocation = touch->getLocation();
    Vec2 offset = touchLocation - _player->getPosition();
    // ���offset��xֵ�Ǹ�ֵ��������������ͼ����������ڱ���Ϸ�����ǲ�����ġ�
    if (offset.x < 0) {
        return true;
    }

    // 3.��������ڵ�λ�ô���һ�����ڣ�������ӵ������С�
    auto projectile = Sprite::create("Projectile.png");
    projectile->setPosition(_player->getPosition());

    //�����ÿһ������ʵ�嶼һ����
    // Add projectile's physicsBody
    auto physicsBody = PhysicsBody::createBox(projectile->getContentSize(), PhysicsMaterial(0.0f, 0.0f, 0.0f));
    physicsBody->setDynamic(false);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    projectile->setPhysicsBody(physicsBody);
    projectile->setTag(10);

    this->addChild(projectile);

    // 4.��ƫ����ת��Ϊ��λ������������Ϊ1��������
    offset.normalize();
    // �������1000����ͻ����һ��ָ���û���������ĳ���Ϊ1000��������Ϊʲô��1000�أ���Ϊ����Ӧ�����Գ�����ǰ�ֱ�������Ļ�ı߽硣
    auto shootAmount = offset * 1000;
    // ����������ӵ����ڵ�λ����ȥ�������������һ��Ŀ��λ�á�
    auto realDest = shootAmount + projectile->getPosition();

    // 5.����һ����������������2�����ƶ���Ŀ��λ�ã�Ȼ�����ӳ������Ƴ���
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