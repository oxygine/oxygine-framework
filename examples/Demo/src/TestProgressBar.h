#pragma once
#include "test.h"
#include "ProgressBar.h"

class TestProgressBar: public Test
{
public:
	spProgressBar bar;
	TestProgressBar()
	{
		addButton("0", "dir: 0");
		addButton("90", "dir: 90");
		addButton("180", "dir: 180");
		addButton("270", "dir: 270");
		addButton("radial_cw", "dir: radial_cw");
		//addButton("radial_ccw", "dir: radial_ccw");

		bar = new ProgressBar(); 
		bar->setResAnim(resources.getResAnim("bg"));
		bar->setAnchor(Vector2(0.5f, 0.5f));
		bar->setPosition(getSize()/2);
		bar->setScale(0.5f);
		content->addChild(bar);
	}

	void clicked(string id)
	{		
		bar->setProgress(0);
		if (id == "0")
		{			
			bar->setDirection(ProgressBar::dir_0);			
		}
		if (id == "90")
		{			
			bar->setDirection(ProgressBar::dir_90);
		}
		if (id == "180")
		{			
			bar->setDirection(ProgressBar::dir_180);
		}
		if (id == "270")
		{			
			bar->setDirection(ProgressBar::dir_270);
		}
		/*
		if (id == "radial_ccw")
		{			
			bar->setDirection(ProgressBar::dir_radial_ccw);
		}
		*/
		if (id == "radial_cw")
		{			
			bar->setDirection(ProgressBar::dir_radial_cw);
		}

		bar->addTween(ProgressBar::TweenProgress(1.0f), 1000);
	}
};
