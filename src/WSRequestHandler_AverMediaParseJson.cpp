#include "Utils.h"
#include "util/platform.h"
#include "WSRequestHandler.h"
#include <QString>
#include <QStringList>

#define DESKTOP_AUDIO_1 "DesktopAudioDevice1"
#define DESKTOP_AUDIO_2 "DesktopAudioDevice2"
#define AUX_AUDIO_1 "AuxAudioDevice1"
#define AUX_AUDIO_2 "AuxAudioDevice2"
#define AUX_AUDIO_3 "AuxAudioDevice3"
#define AUX_AUDIO_4 "AuxAudioDevice4"

static bool LoadData(obs_data_t* data, const char* file, OBSDataAutoRelease& scene_source_data)
{
	OBSDataAutoRelease modules = obs_data_get_obj(data, "modules");
	OBSDataAutoRelease desktop_audio1 = obs_data_get_obj(data, DESKTOP_AUDIO_1);
	OBSDataAutoRelease desktop_audio2 = obs_data_get_obj(data, DESKTOP_AUDIO_2);
	OBSDataAutoRelease aux_audio1 = obs_data_get_obj(data, AUX_AUDIO_1);
	OBSDataAutoRelease aux_audio2 = obs_data_get_obj(data, AUX_AUDIO_2);
	OBSDataAutoRelease aux_audio3 = obs_data_get_obj(data, AUX_AUDIO_3);
	OBSDataAutoRelease aux_audio4 = obs_data_get_obj(data, AUX_AUDIO_4);

	OBSDataArrayAutoRelease sceneOrder = obs_data_get_array(data, "scene_order");
	OBSDataArrayAutoRelease sources = obs_data_get_array(data, "sources");
	OBSDataArrayAutoRelease groups = obs_data_get_array(data, "groups");
	OBSDataArrayAutoRelease transitions = obs_data_get_array(data, "transitions");
	OBSDataArrayAutoRelease quickTransitionData = obs_data_get_array(data, "quick_transitions");
	OBSDataArrayAutoRelease savedProjectors = obs_data_get_array(data, "saved_projectors");

	QString sceneName = obs_data_get_string(data, "current_scene");
	QString programSceneName = obs_data_get_string(data, "current_program_scene");
	QString transitionName = obs_data_get_string(data, "current_transition");
	QString name = obs_data_get_string(data, "name");

	int newDuration = obs_data_get_int(data, "transition_duration");
	int scalingLevel = (int)obs_data_get_int(data, "scaling_level");
	float scrollOffX = (float)obs_data_get_double(data, "scaling_off_x");
	float scrollOffY = (float)obs_data_get_double(data, "scaling_off_y");
	bool fixedScaling = obs_data_get_bool(data, "scaling_enabled");
    bool previewLocked = obs_data_get_bool(data, "preview_locked");
	
	if (modules) {
		obs_data_set_obj(scene_source_data, "modules", modules);
		blog(LOG_INFO, "LoadData after set module");
	}

	if (desktop_audio1) {
		obs_data_set_obj(scene_source_data, DESKTOP_AUDIO_1, desktop_audio1);
		blog(LOG_INFO, "LoadData after set desktop audio1");
	}
	if (desktop_audio2) {
		obs_data_set_obj(scene_source_data, DESKTOP_AUDIO_2, desktop_audio2);
		blog(LOG_INFO, "LoadData after set desktop audio2");
	}
	if (aux_audio1) {
		obs_data_set_obj(scene_source_data, AUX_AUDIO_1, aux_audio1);
		blog(LOG_INFO, "LoadData after set audio1");
	}
	if (aux_audio2) {
		obs_data_set_obj(scene_source_data, AUX_AUDIO_2, aux_audio2);
		blog(LOG_INFO, "LoadData after set audio2");
	}
	if (aux_audio3) {
		obs_data_set_obj(scene_source_data, AUX_AUDIO_3, aux_audio3);
		blog(LOG_INFO, "LoadData after set audio3");
	}
	if (aux_audio4) {
		obs_data_set_obj(scene_source_data, AUX_AUDIO_4, aux_audio4);
		blog(LOG_INFO, "LoadData after set audio4");
	}

	obs_data_set_array(scene_source_data, "scene_order", sceneOrder);
	blog(LOG_INFO, "LoadData after set scene");
	
	obs_data_set_array(scene_source_data, "sources", sources);
	blog(LOG_INFO, "LoadData after set sources");

	obs_data_set_array(scene_source_data, "groups", groups);
	blog(LOG_INFO, "LoadData after set groups");
	
	obs_data_set_array(scene_source_data, "transitions", transitions);
	blog(LOG_INFO, "LoadData after set transitions");

	obs_data_set_array(scene_source_data, "quick_transitions", quickTransitionData);
	blog(LOG_INFO, "LoadData after set quick_transitions");

	obs_data_set_array(scene_source_data, "saved_projectors", savedProjectors);
	blog(LOG_INFO, "LoadData after set saved_projectors");

	obs_data_set_string(scene_source_data, "current_scene", sceneName.toUtf8());
	obs_data_set_string(scene_source_data, "current_program_scene", programSceneName.toUtf8());
	obs_data_set_string(scene_source_data, "current_transition", transitionName.toUtf8());
	obs_data_set_string(scene_source_data, "name", name.toUtf8());

	obs_data_set_int(scene_source_data, "transition_duration", newDuration);
	obs_data_set_bool(scene_source_data, "scaling_enabled", fixedScaling);
	obs_data_set_int(scene_source_data, "scaling_level", scalingLevel);
	obs_data_set_double(scene_source_data, "scaling_off_x", scrollOffX);
	obs_data_set_double(scene_source_data, "scaling_off_y", scrollOffY);
	obs_data_set_bool(scene_source_data, "preview_locked", previewLocked);
	
	blog(LOG_INFO, "LoadData load success");
	return true;
}

static obs_data_t* Load(const char* file)
{
	blog(LOG_INFO, "Load file: %s", file);
	obs_data_t* data = obs_data_create_from_json_file_safe(file, "bak");
	if (!data) {
		blog(LOG_INFO, "Load file fail");
		return 0;
	}

	return data;
}

static int GetConfigPath(char* path, size_t size, const char* name)
{
	return os_get_config_path(path, size, name);
}

static void CollectPathAndNames(QStringList& sceneCollectionConfigFilePaths, QStringList& sceneCollectionNames) {
	char path[512];
	os_glob_t* glob;

	int ret = GetConfigPath(path, sizeof(path),
		"obs-studio/basic/scenes/*.json");
	if (ret <= 0) {
		blog(LOG_WARNING, "Failed to get config path for scene "
			"collections");
		return;
	}

	if (os_glob(path, 0, &glob) != 0) {
		blog(LOG_WARNING, "Failed to glob scene collections");
		return;
	}
	blog(LOG_INFO, "CollectPathAndNames start for-loop");
	for (size_t i = 0; i < glob->gl_pathc; i++) {
		const char* filePath = glob->gl_pathv[i].path;
		if (glob->gl_pathv[i].directory)
			continue;

		obs_data_t* data =
			obs_data_create_from_json_file_safe(filePath, "bak");
		std::string name = obs_data_get_string(data, "name");

		/* if no name found, use the file name as the name
		 * (this only happens when switching to the new version) */
		if (name.empty()) {
			name = strrchr(filePath, '/') + 1;
			name.resize(name.size() - 5);
		}
		blog(LOG_INFO, "Name %s", name.c_str());
		blog(LOG_INFO, "File path %s", filePath);
		sceneCollectionConfigFilePaths.push_back(QString::fromUtf8(filePath));
		sceneCollectionNames.push_back(QString::fromStdString(name));

		obs_data_release(data);
	}
	blog(LOG_INFO, "CollectPathAndNames end for-loop");
	os_globfree(glob);
}

RpcResponse WSRequestHandler::ParseJson(const RpcRequest& request) {
	OBSDataAutoRelease response = obs_data_create();

	QStringList sceneCollectionFilePaths;
	QStringList sceneCollectionNames;

	CollectPathAndNames(sceneCollectionFilePaths, sceneCollectionNames);
	int listCount = sceneCollectionFilePaths.size();

	OBSDataArrayAutoRelease sceneCollectionlist = obs_data_array_create();
	for (int i = 0; i < listCount; i++) {
		OBSDataAutoRelease scene_source_data = obs_data_create();
		const auto jsonPath_stdstring = sceneCollectionFilePaths[i].toStdString();
		const auto jsonName_stdstring = sceneCollectionNames[i].toStdString();

		auto file = jsonPath_stdstring.c_str();
		obs_data_t* data = Load(file);
		if (data) {
			LoadData(data, file, scene_source_data);
			//obs_data_set_obj(response, jsonName_stdstring.c_str(), scene_source_data);
		}
		else {
			//obs_data_set_obj(response, jsonName_stdstring.c_str(), scene_source_data);
		}
		
		obs_data_array_push_back(sceneCollectionlist, scene_source_data);
		obs_data_release(data);
	}
	obs_data_set_array(response, "scenesList", sceneCollectionlist);
	obs_data_set_int(response, "count", obs_data_array_count(sceneCollectionlist));

	blog(LOG_INFO, "Before return");
	return request.success(response);
}
