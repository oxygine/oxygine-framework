#Sounds

[**oxygine-sound**](https://bitbucket.org/oxygine/oxygine-sound) is extension for oxygine-framework. It is based on OpenAL library and could be used for playing ogg files.

It supports streaming for long sound tracks and using separate thread for decoding and reading ogg files.
 
##Usage

Include headers:

	#include "SoundPlayer.h"
	#include "SoundSystem.h"
	#include "SoundInstance.h"

Initialize sound system with 16 channels:
	
	SoundSystem::instance = SoundSystem::create();
	SoundSystem::instance->init(16);

	SoundPlayer::initialize();

*SoundPlayer::initialize* registers new type of resource "sound".
	
Add your ogg tracks to resources file, lets call it "sounds.xml":

	<?xml version="1.0"?>
	<resources>
		<set path = "sounds" />	
	
	 	<sound file="sfx1.ogg" />
		<sound file="sfx2.ogg" />
		<sound file="voice1.ogg"/>
		<sound file="music.ogg" streaming="true"/>
	
	</resources>

> you could	have multiple resources files

* If sound is too short if would be automaticaly decoded to buffer when loading. 
* If sound is longer than about 5 seconds it would be decoded step by step from memory when playing.
* **streaming=true** attribute means that sound would be loaded from file and decoded on the fly when playing.   


Load resources:

	Resources resources;
	resources.loadXML("sounds.xml");

Next step is declaring SoundPlayer instance:

	SoundPlayer sfxPlayer;

SoundPlayer is a container for group of sounds playing right now. You could declare multiple instances of SoundPlayer and control them individually:

	SoundPlayer sfxPlayer;
	SoundPlayer musicPlayer;

	...

	sfxPlayer.setVolume(1.0f);
	musicPlayer.setVolume(0.5f);


Play sounds:
	
	sfxPlayer.play(resources.get("sfx1"));
	musicPlayer.play(resources.get("music"));

You could save handle to playing sound instance and use it for control:

	spSoundInstance music = musicPlayer.play(resources.get("music"));	

	void levelCompleted()
	{
		music->fadeOut();
	}

Or you could control group of sounds belonging to SoundPlayer:

	sfxPlayer.pause();
	...
	sfxPlayer.stop();


You could subscribe to events:

	spSoundInstance music = musicPlayer.play(resources.get("music"));
	music->addEventListener(SoundInstance::SoundEvent::EVENT_DONE, CLOSURE(this, &SomeClass::musicDone));


	void SomeClass::musicDone(Event*)
	{
		musicPlayer.play(resources.get("music2"));
	} 








	 