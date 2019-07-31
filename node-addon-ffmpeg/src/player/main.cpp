#include "player.h"
int main()
{
	Player p;
	//p.init("D:\\Wildlife.wmv");
	p.init("E:\\迅雷下载\\复仇者联盟3：无限战争.720p.BD国语中英双字[最新电影www.66ys.tv].mp4");
	
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