#include "player.h"
int main()
{
	Player p;
	p.init("D:\\Wildlife.wmv");
	p.readPacket();
	//p.decodeAudio();
	
	//thread readAudio(&Player::readAudioPacketThread, p);
	//thread readVideo(&Player::readVideoPacketThread, p);

	//readAudio.join();
	//readVideo.join();

	return 0;
}