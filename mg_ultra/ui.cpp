#include "ui.h"

#include "trending_value.h"

TrendingValue<float> borderWidth(1.0f);

void g_ui::updateUI() {
	borderWidth.update();
}

float g_ui::getBorderCutoff() {
	return borderWidth.get();
}

void g_ui::setTrend(float current, float rate) {
	borderWidth.setTarget(current);
	borderWidth.setRate(rate);
}

void g_ui::set(float current) {
	borderWidth.setCurrent(current);
	borderWidth.setTarget(current);
}