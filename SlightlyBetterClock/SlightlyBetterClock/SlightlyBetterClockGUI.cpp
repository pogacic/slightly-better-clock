#include "pch.h"
#include "SlightlyBetterClock.h"
#include <string>

// ImGUI manual/demo
// https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html

std::map<std::string, std::string> strftimeFormats{
	// https://en.cppreference.com/w/cpp/chrono/c/strftime
	// ^ Use as reference if you want to add your own 
	{ "time_date", "%X %Y-%m-%d" },
	{ "time_24h", "%X" },
	{ "time_12h", "%I:%M:%S %p" },
	{ "date", "%Y-%m-%d" },
};

// Kinda gross
std::map<std::string, int> clockLocations{
	{ "bottom_left", 0},
	{ "bottom_right", 1},
	{ "top_left", 2},
	{ "top_right", 3},
};

static std::tuple<float, float> GetClockLocation(std::string locationKey, Vector2F textSize) {
	float finalValX;
	float finalValY;

	// Calculate clock position using set location and clock size
	int location = clockLocations[locationKey];
	switch (location) {
	case 0:
		// bottom_left
		finalValX = 10;
		finalValY = (ImGui::GetIO().DisplaySize.y - textSize.Y - 10);
		break;
	case 1:
		// bottom_right
		finalValX = (ImGui::GetIO().DisplaySize.x - (textSize.X + 10));
		finalValY = (ImGui::GetIO().DisplaySize.y - textSize.Y - 10);
		break;
	case 2:
		// top_left
		finalValX = 10;
		finalValY = 10;
		break;
	case 3:
		// top_right
		finalValX = (ImGui::GetIO().DisplaySize.x - (textSize.X + 10));
		finalValY = 10;
		break;
	default:
		// default to bottom_right
		finalValX = (ImGui::GetIO().DisplaySize.x - (textSize.X + 10));
		finalValY = (ImGui::GetIO().DisplaySize.y - textSize.Y - 10);
		break;
	}

	return std::make_tuple(finalValX, finalValY);
}

static std::stringstream GenTime(std::string strftimeFormatsKey) {
	// Chrono to strftime
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;

	auto strftime = strftimeFormats[strftimeFormatsKey];

	ss << std::put_time(std::localtime(&in_time_t), strftime.c_str());
	return ss;
}

void SlightlyBetterClock::Render(CanvasWrapper canvas) {
	// Load show clock vals
	CVarWrapper showClock = cvarManager->getCvar("sbc_show");
	if (!showClock) { return; }
	if (!showClock.getBoolValue()) { return; }
	// Load clock format
	CVarWrapper sbcFormat = cvarManager->getCvar("sbc_format");
	if (!sbcFormat) { return; }
	// Load clock location
	CVarWrapper sbcLocation = cvarManager->getCvar("sbc_location");
	if (!sbcLocation) { return; }
	// Load color val
	CVarWrapper sbcTextColor = cvarManager->getCvar("sbc_color");
	if (!sbcTextColor) { return; }
	// Load fillbox alpha
	CVarWrapper sbcFBAlpha = cvarManager->getCvar("sbc_fb_alpha");
	if (!sbcFBAlpha) { return; }
	// Load clock scale
	CVarWrapper sbcScale = cvarManager->getCvar("sbc_scale");
	if (!sbcScale) { return; }

	// Get time
	auto ss = GenTime(sbcFormat.getStringValue());
	// Get scale float
	float scaleF = sbcScale.getFloatValue();
	// Get size of text
	Vector2F textSize = canvas.GetStringSize(ss.str(), scaleF, scaleF);

	// Set fillbox color
	auto fillboxColor = LinearColor(0.0, 0.0, 0.0, 0.0);
	fillboxColor.A = sbcFBAlpha.getFloatValue();

	// Set text color
	auto textColor = sbcTextColor.getColorValue();

	// Get xVal & yVal
	auto locationVal = GetClockLocation(sbcLocation.getStringValue(), textSize);
	float xVal = std::get<0>(locationVal);
	float yVal = std::get<1>(locationVal);

	// Set fillbox size, whatever
	auto fillBoxSize = Vector2(textSize.X + 10, textSize.Y + 9);

	// Draw fillbox
	canvas.SetPosition(Vector2F{ xVal - 5, yVal - 5 });
	canvas.SetColor(fillboxColor);
	canvas.FillBox(fillBoxSize);

	// Draw time
	canvas.SetPosition(Vector2F{ xVal, yVal });
	canvas.SetColor(textColor);
	canvas.DrawString(ss.str(), scaleF, scaleF, false);
}

void SlightlyBetterClock::RenderSettings() {
	// Clock: show
	CVarWrapper showClock = cvarManager->getCvar("sbc_show");
	if (!showClock) { return; }
	bool enabled = showClock.getBoolValue();
	if (ImGui::Checkbox("Show clock", &enabled)) {
		showClock.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle visibility of the clock");
	}

	// Text color
	CVarWrapper sbcTextColor = cvarManager->getCvar("sbc_color");
	if (!sbcTextColor) { return; }

	LinearColor textColor = sbcTextColor.getColorValue() / 255;
	if (ImGui::ColorEdit4("Set clock color", &textColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
		sbcTextColor.setValue(textColor * 255);
	}

	// Fillbox alpha
	CVarWrapper sbcFBAlpha = cvarManager->getCvar("sbc_fb_alpha");
	if (!sbcFBAlpha) { return; }

	float sbcFBAlphaValue = sbcFBAlpha.getFloatValue();
	if (ImGui::SliderFloat("Background opacity", &sbcFBAlphaValue, 0.0, 255.0, "%.1f")) {
		sbcFBAlpha.setValue(sbcFBAlphaValue);
	}

	// Load clock scale
	CVarWrapper sbcScale = cvarManager->getCvar("sbc_scale");
	if (!sbcScale) { return; }
	auto sbcScaleVal = sbcScale.getFloatValue();
	if (ImGui::SliderFloat("Clock scale (default: 1.0)", &sbcScaleVal, 0.0, 20.0, "%.1f")) {
		sbcScale.setValue(sbcScaleVal);
	}

	// Clock: format
	// Load current format
	CVarWrapper sbcFormat = cvarManager->getCvar("sbc_format");
	if (!sbcFormat) { return; }
	auto sbcFormatValue = sbcFormat.getStringValue();

	if (ImGui::BeginCombo("Clock format", sbcFormatValue.c_str()))
	{
		for (const auto& [key, value] : strftimeFormats) {

			const bool is_selected = (sbcFormatValue == key);
			if (ImGui::Selectable(key.c_str(), is_selected))
				sbcFormat.setValue(key);

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Clock: location
	CVarWrapper sbcLocation = cvarManager->getCvar("sbc_location");
	if (!sbcLocation) { return; }
	auto sbcLocationValue = sbcLocation.getStringValue();

	if (ImGui::BeginCombo("Clock location", sbcLocationValue.c_str()))
	{
		for (const auto& [key, value] : clockLocations) {

			const bool is_selected = (sbcLocationValue == key);
			if (ImGui::Selectable(key.c_str(), is_selected))
				sbcLocation.setValue(key);

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}