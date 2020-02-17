/*Associated with UI aspect of graphics, such as border*/
#ifndef __UI__
#define __UI__


namespace g_ui {

	//update ui value
	void updateUI();

	//gets border value
	float getBorderCutoff();

	//set target
	void setTrend(float current, float rate);

	//set direct
	void set(float current);
}

#endif