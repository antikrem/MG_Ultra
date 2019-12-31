#include "audio_master.h"

#include <shared_mutex>

shared_mutex audioPtrLock;
AudioMaster* globalAudioMaster = nullptr;

void g_audio::setAudioMaster(AudioMaster* audioMaster) {
	unique_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster = audioMaster;
}

void g_audio::closeAudioPipeLine() {
	unique_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster = nullptr;
}

void g_audio::disposeBuffer(ALint buffer) {
	shared_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster->disposeBuffer(buffer);
}

void g_audio::addAudioFile(const string& fileName, const string& fileLocation) {
	shared_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster->queueAssetLoad(fileName, fileLocation, AuFiLi_Global);
}

void g_audio::flushAudioLoadRequests() {
	shared_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster->flushAssetLoadRequests();
}


void g_audio::print() {
	shared_lock<shared_mutex> lck(audioPtrLock);
	globalAudioMaster->printReport();
}