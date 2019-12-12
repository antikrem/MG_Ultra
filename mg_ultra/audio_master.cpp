#include "audio_master.h"

#include <shared_mutex>

shared_mutex dispositionLock;
AudioMaster* globalAudioMaster = nullptr;

void g_audio::setAudioMaster(AudioMaster* audioMaster) {
	globalAudioMaster = audioMaster;
}

void g_audio::closeAudioPipeLine() {
	unique_lock<shared_mutex> lck(dispositionLock);
	globalAudioMaster = nullptr;
}

void g_audio::disposeBuffer(ALint buffer) {
	shared_lock<shared_mutex> lck(dispositionLock);
	if (globalAudioMaster) {
		globalAudioMaster->disposeBuffer(buffer);
	}
}

void g_audio::addAudioFile(const string& fileName, const string& fileLocation) {
	shared_lock<shared_mutex> lck(dispositionLock);
	if (globalAudioMaster) {
		globalAudioMaster->queueAssetLoad(fileName, fileLocation, AuFiLi_Global);
	}
}

void g_audio::print() {
	if (globalAudioMaster) {
		globalAudioMaster->printReport();
	}
}