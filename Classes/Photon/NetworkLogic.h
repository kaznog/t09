#ifndef __NETWORK_LOGIC_H
#define __NETWORK_LOGIC_H

#include "OutputListener.h"

typedef enum _State
{
    // The client is ready to connect, having just either finished starting, or
    // finished disconnecting.
	STATE_INITIALIZED = 0,
    // The client has received the input INPUT_CONNECT, and is about to try
    // connecting.
	STATE_CONNECTING,
    // The client has attempted to connect, and is waiting for a response from
    // the server.
    STATE_WAITING,
    // The client has connected.
	STATE_CONNECTED,
    // The client has asked to join a game, but has not yet received a
    // response.
	STATE_JOINING,
    // The client has joined a game.
	STATE_JOINED,
    // The client has asked to join a game, but has not yet received a
    // response.
    STATE_READY,
    STATE_IN_GAME,
	STATE_LEAVING,
    // The client has just left a room.
	STATE_LEFT,
    // The client is attempting to disconnect.
	STATE_DISCONNECTING,
    // The client has just disconnected.
	STATE_DISCONNECTED
} State;

typedef enum _Input
{
	INPUT_NON = 0,
    INPUT_CONNECT,
	INPUT_CREATE_GAME,
	INPUT_JOIN_RANDOM_GAME,
    INPUT_DECLARE_READY,
	INPUT_LEAVE_GAME,
	INPUT_EXIT
} Input;

typedef enum _CustomEvents
{
	EVENT_MISC = 0,
    EVENT_CHAT,
    EVENT_DECLARE_READY,
} CustomEvents;

typedef enum _ChatEventBytes
{
	CHATBYTE_STRING = 0
} ChatEventBytes;


class NetworkLogicListener : public ExitGames::Common::ToString
{
public:
	using ExitGames::Common::ToString::toString;
	virtual void stateUpdate(State newState) = 0;
	virtual ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes=false) const;
};

class StateAccessor
{
public:
	State getState(void) const;
	void setState(State newState);
	void registerForStateUpdates(NetworkLogicListener* listener);

private:
	State mState;
	ExitGames::Common::JVector<NetworkLogicListener*> mStateUpdateListeners;
};



class NetworkLogic : ExitGames::LoadBalancing::Listener
{
public:
	NetworkLogic(OutputListener* listener, const wchar_t* appVersion, 
				ExitGames::LoadBalancing::AuthenticationValues authenticationValues = ExitGames::LoadBalancing::AuthenticationValues());
	void registerForStateUpdates(NetworkLogicListener* listener);
	virtual void run(void);
	virtual void connect(void);

	virtual void opCreateRoom(void);
	virtual void opJoinRandomRoom(void);
    virtual void opLeaveRoom(void);
    virtual void opJoinRoom(const ExitGames::Common::JString &gameID);
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
    virtual void opRaiseEvent(bool reliable, const ExitGames::Common::Hashtable &parameters, nByte eventCode);
	virtual void disconnect(void);
	virtual void sendEvent(void);
    virtual unsigned int countMyGames();
    virtual short getPeerId();

	Input getLastInput(void) const;
	void setLastInput(Input newInput);
	State getState(void) const;
    virtual const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*>& getRoomList();
    virtual const ExitGames::Common::JVector<ExitGames::Common::JString>& getRoomNameList(void);
    
    virtual ExitGames::LoadBalancing::Room* getRoomWithID( ExitGames::Common::JString gameID );
    virtual ExitGames::LoadBalancing::MutableRoom& getCurrentRoom();
    virtual ExitGames::LoadBalancing::MutableRoom& getCurrentlyJoinedRoom();
    virtual ExitGames::Common::JString getUserName() = 0;
    virtual bool allPlayersReady();
    virtual int getCountPlayersInGame();
    virtual bool getIsInGameRoom();
	ExitGames::LoadBalancing::Client mLoadBalancingClient;

protected:
	// receive and print out debug out here
	virtual void debugReturn(ExitGames::Common::DebugLevel::DebugLevel debugLevel, const ExitGames::Common::JString& string);

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);
    virtual void checkReadyPlayer( unsigned int readyPlayer );

	// events, triggered by certain operations of all players in the same room
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr);

	// callbacks for operations on PhotonLoadBalancing server
	virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
	virtual void disconnectReturn(void);
	virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinLobbyReturn(void);
	virtual void leaveLobbyReturn(void);

protected:
    unsigned int _readyPlayerChecklist;
    
    unsigned int _myGamesCount;

	ExitGames::Common::JString mGameID;
	StateAccessor mStateAccessor;
	Input mLastInput;
	OutputListener* mpOutputListener;

    ExitGames::Common::JString _gameID;
    bool _isGameMaster;
};

#endif