//
// Created by Adrien BLANCHET on 22/06/2020.
//

#include "tab_mod_options.h"
#include <GlobalObjects.h>

tab_mod_options::tab_mod_options() {

  _tabModBrowser_ = nullptr;
  _preSelection_ = 0;
  _inheritedTitle_ = "Inherited from the main menu";
  _itemFolderInstallPreset_ = nullptr;

}

void tab_mod_options::buildFolderInstallPresetItem() {

  _itemFolderInstallPreset_ = new brls::ListItem(
    "Attribute a config preset",
    "Specify on which install folder mods from this subfolder (game) will be installed.\n",
    ""
  );
  _itemFolderInstallPreset_->setValue(_inheritedTitle_);

  // Find the current selection
  std::string folderConfigFilePath = GlobalObjects::get_mod_browser().get_current_directory() + "/this_folder_config.txt";
  if(toolbox::do_path_is_file(folderConfigFilePath)){
    _preSelection_ = 1 + GlobalObjects::get_mod_browser().get_parameters_handler().get_current_config_preset_id();
    _itemFolderInstallPreset_->setValue(GlobalObjects::get_mod_browser().get_parameters_handler().get_current_config_preset_name());
  }

  // On click : show scrolling up menu
  _itemFolderInstallPreset_->getClickEvent()->subscribe([this](View* view) {

    // build the choice list
    std::vector<std::string> config_presets_list;
    config_presets_list.emplace_back(this->_inheritedTitle_);
    for(const auto& preset_name: GlobalObjects::get_mod_browser().get_parameters_handler().get_presets_list()){
      config_presets_list.emplace_back(preset_name);
    }

    // function that will set the config preset from the Dropdown menu selection (int result)
    brls::ValueSelectedEvent::Callback valueCallback = [this](int result) {
      if (result == -1)
        return;

      this->_preSelection_ = result;

      // overwriting
      std::string this_folder_config_file_path = GlobalObjects::get_mod_browser().get_current_directory() + "/this_folder_config.txt";
      toolbox::delete_file(this_folder_config_file_path);
      if(result > 0){
        // then a preset has been specified
        toolbox::dump_string_in_file(
          GlobalObjects::get_mod_browser().get_parameters_handler().get_presets_list()[result-1],
          this_folder_config_file_path
        );
        GlobalObjects::get_mod_browser().change_config_preset(
          GlobalObjects::get_mod_browser().get_parameters_handler().get_presets_list()[result-1]
        );
        this->_itemFolderInstallPreset_->setValue(GlobalObjects::get_mod_browser().get_parameters_handler().get_current_config_preset_name());
      }
      else{
        // restore the config preset from the main menu
        GlobalObjects::get_mod_browser().change_config_preset(
          GlobalObjects::get_mod_browser().get_main_config_preset()
        );
        this->_itemFolderInstallPreset_->setValue(_inheritedTitle_);
      }
      if(this->_tabModBrowser_ != nullptr) this->_tabModBrowser_->updateModsStatus();

    }; // Callback sequence

    brls::Dropdown::open(
      "Please select the config preset you want for this folder",
      config_presets_list,
      valueCallback,
      this->_preSelection_
    );

  });

}

void tab_mod_options::buildResetModsCacheItem() {

  _itemResetModsCache_ = new brls::ListItem(
    "Recheck all mods",
    "\tThis option resets all mods cache status and recheck if each files is properly applied.\n"
              "\tWhen files where managed without SimpleModManager, the displayed mod status can be wrong. "
              "This typically happens when you modified some mod files, or other programs override some of the applied files.",
    ""
  );

  _itemResetModsCache_->getClickEvent()->subscribe([this](View* view){

    auto* dialog = new brls::Dialog("Do you want to reset mods cache status and recheck all mod files ?");

    dialog->addButton("Yes", [this, dialog](brls::View* view) {
      dialog->close();
      brls::Application::notify("Resetting mods cache status...");
      GlobalObjects::get_mod_browser().get_mod_manager().reset_all_mods_cache_status();
      if(this->_tabModBrowser_ != nullptr) this->_tabModBrowser_->updateModsStatus();
    });
    dialog->addButton("No", [dialog](brls::View* view) {
      dialog->close();
    });

    dialog->setCancelable(true);
    dialog->open();

  });

}

void tab_mod_options::buildDisableAllMods() {

  _itemDisableAllMods_ = new brls::ListItem(
    "Disable All Mods",
    "This option will remove all installed mods files.\n"
              "Note that each installed mod file need to be identical to the one present in this folder.",
    ""
  );

  _itemDisableAllMods_->getClickEvent()->subscribe([this](View* view){

    auto* dialog = new brls::Dialog("Do you want to disable all mods ?");

    dialog->addButton("Yes", [this, dialog](brls::View* view) {
      dialog->close();
      brls::Application::notify("Disabling all mods of this folder...");
      GlobalObjects::get_mod_browser().remove_all_mods(true);
      GlobalObjects::get_mod_browser().get_mod_manager().reset_all_mods_cache_status();
      if(this->_tabModBrowser_ != nullptr) this->_tabModBrowser_->updateModsStatus();
    });
    dialog->addButton("No", [dialog](brls::View* view) {
      dialog->close();
    });

    dialog->setCancelable(true);
    dialog->open();

  });

}

void tab_mod_options::initialize() {

  this->buildFolderInstallPresetItem();
  this->buildResetModsCacheItem();
  this->buildDisableAllMods();

  // finally add to view
  this->addView(_itemDisableAllMods_);
  this->addView(_itemFolderInstallPreset_);
  this->addView(_itemResetModsCache_);

}



