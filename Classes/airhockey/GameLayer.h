#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#define GOAL_WIDTH 400

#include "cocos2d.h"
#include "GameSprite.h"
#include "CocosNetworkLogic.h"

enum {
    kPlayer1Tag,
    kPlayer2Tag
};

using namespace cocos2d;

class GameLayer : public cocos2d::CCLayer, public CocosNetworkDelegate
{
	
    GameSprite * _player1;
	GameSprite * _player2;
	GameSprite * _ball;
    
	CCArray * _players;
    CCLabelTTF * _player1ScoreLabel;
    CCLabelTTF * _player2ScoreLabel;
    
    CCSize _screenSize;
	
    int _player1Score;
    int _player2Score;
    
    void playerScore (int player);

    bool isMenuInitialized;
    bool isJoind;
//    CocosNetworkLogic *pPhotonNetwork;
    CCMenu *pMenu;
    CCMenu *pMenu2;
public:
    GameLayer();
    ~GameLayer();
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GameLayer);
    
    //THE OVERRIDES
    virtual void ccTouchesBegan(CCSet* pTouches, CCEvent* event);
	virtual void ccTouchesMoved(CCSet* pTouches, CCEvent* event);
	virtual void ccTouchesEnded(CCSet* pTouches, CCEvent* event);
	
	void update (float dt);
//    void PhotonUpdate (float dt);

    virtual void didUpdateState(State newState);
    virtual void ReceivedClientData(cocos2d::CCPoint NextPos, cocos2d::CCPoint Vector);
    virtual void ReceivedGameData(
                                  cocos2d::CCPoint HostPos,
                                  cocos2d::CCPoint HostNextPos,
                                  cocos2d::CCPoint HostVector,
                                  cocos2d::CCPoint ClientPos,
                                  cocos2d::CCPoint ClientNextPos,
                                  cocos2d::CCPoint ClientVector,
                                  cocos2d::CCPoint PuckPos,
                                  cocos2d::CCPoint PuckNextPos,
                                  cocos2d::CCPoint PuckVector
    );
    virtual void ReceivedScore(int HostScore, int ClientScore);
    virtual bool isHost();
    virtual cocos2d::CCPoint getPlayerPosition();
    virtual cocos2d::CCPoint getPlayerNextPosition();
    virtual cocos2d::CCPoint getPlayerVector();
    virtual cocos2d::CCPoint getClientPosition();
    virtual cocos2d::CCPoint getClientNextPosition();
    virtual cocos2d::CCPoint getClientVector();
    virtual cocos2d::CCPoint getPuckPosition();
    virtual cocos2d::CCPoint getPuckNextPosition();
    virtual cocos2d::CCPoint getPuckVector();
    virtual int getHostScore();
    virtual int getClientScore();
    
    void menuCloseCallback(CCObject* pSender);
    void menuCreateGameCallback(CCObject* pSender);
    void menuJoinGameCallback(CCObject* pSender);
    void menuLeaveGameCallback(CCObject* pSender);
    
    void initMenu();
    void initPosition();
    void updatePosition();

private:
    bool isHostOwner;
};

#endif // __GAMELAYER_H__

