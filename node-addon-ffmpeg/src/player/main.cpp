#include "player.h"
int main()
{
	Player p;
	//p.init("D:\\Wildlife.wmv");
	p.init("E:\\Ѹ������\\����������3������ս��.720p.BD������Ӣ˫��[���µ�Ӱwww.66ys.tv].mp4");
	
	p.readPacket();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	p.decodeAudio();
	
	//thread readAudio(&Player::readAudioPacketThread, p);
	//thread readVideo(&Player::readVideoPacketThread, p);

	//readAudio.join();
	//readVideo.join();

	return 0;
}