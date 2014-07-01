//
//  CocosNetworkLogic.h
//  AirHockey
//
//  Created by noguchika on 2014/02/04.
//
//

#ifndef __AirHockey__CocosNetworkLogic__
#define __AirHockey__CocosNetworkLogic__

#include "cocos2d.h"
#include "NetworkLogic.h"
#include "OutputListener.h"
#include "Constants.h"

class CocosNetworkDelegate
{
public:
    virtual void didUpdateState(State newState) = 0;
    virtual void ReceivedClientData(cocos2d::CCPoint NextPos, cocos2d::CCPoint Vector) = 0;
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
    ) = 0;
    virtual void ReceivedScore(int HostScore, int ClientScore) = 0;
    virtual bool isHost() = 0;
    virtual cocos2d::CCPoint getPlayerPosition() = 0;
    virtual cocos2d::CCPoint getPlayerNextPosition() = 0;
    virtual cocos2d::CCPoint getPlayerVector() = 0;
    virtual cocos2d::CCPoint getClientPosition() = 0;
    virtual cocos2d::CCPoint getClientNextPosition() = 0;
    virtual cocos2d::CCPoint getClientVector() = 0;
    virtual cocos2d::CCPoint getPuckPosition() = 0;
    virtual cocos2d::CCPoint getPuckNextPosition() = 0;
    virtual cocos2d::CCPoint getPuckVector() = 0;
    virtual int getHostScore() = 0;
    virtual int getClientScore() = 0;
};

class CocosNetworkLogic: public NetworkLogic, public OutputListener, public NetworkLogicListener, public cocos2d::CCObject
{
    CC_SYNTHESIZE_READONLY( cocos2d::CCString, _output, Output );
    CC_PROPERTY_READONLY( cocos2d::CCString, _ccGameID, GameID );
    CC_SYNTHESIZE( bool, _didCreateGame, DidCreateGame );

public:
	CocosNetworkLogic();
    virtual ~CocosNetworkLogic();
    
    virtual ExitGames::Common::JString getUserName();
    virtual void setUserName(ExitGames::Common::JString& str);

	virtual void write(const ExitGames::Common::JString& str);
	virtual void writeLine(const ExitGames::Common::JString& str);
    virtual void stateUpdate(State newState);
    
    virtual cocos2d::CCString getStateString( State state );
    
    virtual void update(float dt);
    virtual void notify( int note );
    
    virtual cocos2d::CCString getRoomPropertyString( int key );
    virtual cocos2d::CCString getRoomPropertyString( const char* key );
    virtual int getRoomPropertyInt( int key );
    virtual int getRoomPropertyInt( const char* key );

    void run(void);
    void sendEvent();
    void sendEvent(cocos2d::CCPoint pos, cocos2d::CCPoint vector);
    void sendGameData(cocos2d::CCPoint HostPos, cocos2d::CCPoint HostNextPos, cocos2d::CCPoint HostVector, cocos2d::CCPoint ClientPos, cocos2d::CCPoint ClientNextPos, cocos2d::CCPoint ClientVector, cocos2d::CCPoint PuckPos, cocos2d::CCPoint PuckNextPos, cocos2d::CCPoint PuckVector);
    void sendScore(int HostScore, int ClientScore);
   
    void setDelegate(CocosNetworkDelegate *pDelegate);
    static CocosNetworkLogic* getInstance();
    static void dispose();

    CC_SYNTHESIZE(cocos2d::CCPoint, m_PlayerNextPos, PlayerNextPos);
    CC_SYNTHESIZE(cocos2d::CCPoint, m_PlayerVector, PlayerVector);
    CC_SYNTHESIZE(cocos2d::CCPoint, m_remotePlayerNextPos, RemotePlayerNextPos);
    CC_SYNTHESIZE(cocos2d::CCPoint, m_remotePlayerVector, RemotePlayerVector);
    CC_SYNTHESIZE(State, m_PhotonState, PhotonState);
    
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
private:
    static CocosNetworkLogic *_instance;
    CocosNetworkDelegate *cocosNetworkDelegate;
    
};

#endif /* defined(__AirHockey__CocosNetworkLogic__) */
