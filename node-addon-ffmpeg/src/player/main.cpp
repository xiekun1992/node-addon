#include "player.h"
int main()
{
	Player p;
	//p.init("D:\\Wildlife.wmv");
	//p.init("E:\\Ѹ������\\����������3������ս��.720p.BD������Ӣ˫��[���µ�Ӱwww.66ys.tv].mp4");
	p.init("F:\\Ѹ������\\��Ʒ�.Bumblebee.2019.KORSUB.HD1080P.X264.AAC.English.CHS.mp4");
	p.freeMedia();
	//p.readPacket();

	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	//p.decodeAudio();
	
	//thread readAudio(&Player::readAudioPacketThread, p);
	//thread readVideo(&Player::readVideoPacketThread, p);

	//readAudio.join();
	//readVideo.join();

	return 0;
}