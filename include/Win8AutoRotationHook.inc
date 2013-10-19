typedef enum ORIENTATION_PREFERENCE {
	ORIENTATION_PREFERENCE_NONE 				= 0x0,
	ORIENTATION_PREFERENCE_LANDSCAPE 			= 0x1,
	ORIENTATION_PREFERENCE_PORTRAIT 			= 0x2,
	ORIENTATION_PREFERENCE_LANDSCAPE_FLIPPED 	= 0x4,
	ORIENTATION_PREFERENCE_PORTRAIT_FLIPPED 	= 0x8
} ORIENTATION_PREFERENCE;

typedef BOOL (WINAPI *pSetARP)(ORIENTATION_PREFERENCE orientation);
typedef BOOL (WINAPI *pGetARP)(ORIENTATION_PREFERENCE *pOrientation);
static ORIENTATION_PREFERENCE storedPref;
static pSetARP setARP =
		(pSetARP)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "SetDisplayAutoRotationPreferences");
static pGetARP getARP =
		(pGetARP)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "GetDisplayAutoRotationPreferences");

bool SaveCurrentARP() {
	if(getARP) {
		getARP(&storedPref);
		return true;
	}
	return false;
}
bool RestoreSavedARP() {
	if(setARP) {
		setARP(storedPref);
		return true;
	}
	return false;
}
bool SetARP(ORIENTATION_PREFERENCE orientation) {
	assert(
		orientation == 0x0 ||
		orientation == 0x1 ||
		orientation == 0x2 ||
		orientation == 0x4 ||
		orientation == 0x8); // Orientation invalid if assert fails
	if(setARP) {
		setARP(orientation);
		return true;
	}
	return false;
}