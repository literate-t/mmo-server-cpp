
#include "Session.h"
#include <CorePch.h>

class ServerSession : public PacketSession
{
public:
	ServerSession();
	~ServerSession();

	void OnConnectCompleted() override;
	void OnDisconnectCompleted() override;
	void OnRecvPacketProcessed(BYTE* buffer, int32 length) override;
	void OnSendCompleted(int32 length) override;

	int32 GetId() const { return _dummy_id; }

private:
	int32 _dummy_id;
};