#pragma once
class Video {
public:
	double fps = 0;
	int width = 0;
	int height = 0;
	string iformatName = "";
	int duration = 0;
	int start = 0;
	int bitrate = 0;
};
class Audio {
public:
	int sampleRate = 0;
	int channels = 0;
	string sampleFormat = "";
};

class VideoInfo {
public:
	Video video;
	Audio audio;
};